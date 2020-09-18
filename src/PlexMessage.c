//
//  PlexMessage.c
//  Now Plexing
//
//  Created by Aaron Rennow on 1/23/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#include "PlexMessage.h"
#include <string.h>
#include <alloca.h>
#include <stdlib.h>
#include "json.h"
#include <assert.h>
#include "JSONUtils.h"
#include "asprintf.h"
#include <stdio.h>
#include "PreprocessorDeclarations.h"

static void plexMessage_private_partialInitFromJSONString(PlexMessage* message,
														  char const * jsonString,
														  size_t jsonStringLen,
														  char const * imageURLHost);
static char* plexMessage_private_allocatingConstructAbsoluteURLFromRequestAndFragment(char const * imageURLHost,
																					  char const * urlFragment,
																					  size_t urlFragmentLen);
static void* plexMessage_private_allocatingCopyFromBuffer(void const* src, size_t len);
static char* plexMessage_private_allocatingCopyFromStringNode(json_value* stringNode);
static PlexEvent plexMessage_private_eventFromStringNode(json_value* eventNode);
static PlexMediaType plexMessage_private_mediaTypeFromStringNode(json_value* mediaTypeNode);

PlexMessage* plexMessage_initFromHTTPMessage(struct http_message* hm, char const * imageURLHost) {
	PlexMessage* message = calloc(1, sizeof(PlexMessage));
	
	char var_name[100], file_name[100];
	char const * chunk;
	size_t chunk_len, n1, n2;
	
	n1 = n2 = 0;
	while ((n2 = mg_parse_multipart(hm->body.p + n1,
									hm->body.len - n1,
									var_name, sizeof(var_name),
									file_name, sizeof(file_name),
									&chunk, &chunk_len)) > 0) {
		
		if (strcmp(var_name, "payload") == 0) {
			plexMessage_private_partialInitFromJSONString(message, chunk, chunk_len, imageURLHost);
		} else if (strcmp(var_name, "thumb") == 0) {
			message->bundledImageData.ptr = plexMessage_private_allocatingCopyFromBuffer(chunk, chunk_len);
			message->bundledImageData.len = chunk_len;
		} else {
#if DEBUG
			printf("var: %s, file_name: %s, size: %d, chunk: [%.*s]\n",
				   var_name, file_name, (int) chunk_len, (int) chunk_len, chunk);
#endif
		}
		
		n1 += n2;
	}
	
	return message;
}

void plexMessage_free(PlexMessage* message) {
	nestedFunc(void, freeIfNonNull, void* ptr) {
		if (ptr) { free(ptr); }
	};
	
	freeIfNonNull(message->itemTitle);
	freeIfNonNull(message->grandparentTitle);
	freeIfNonNull(message->bundledImageData.ptr);
	freeIfNonNull(message->imageURL);
	
	free(message);
}

char* plexMessage_description(PlexMessage* message) {
	int const ptfSize = 5;
	void** pointersToFree = alloca(ptfSize * sizeof(void*));
	memset(pointersToFree, 0, ptfSize * sizeof(void*));
	int ptfCount = 0;
	
	char* humanReadableItemName;
	if (message->grandparentTitle) {
		asprintf(&humanReadableItemName, "%s (%s)", message->itemTitle, message->grandparentTitle);
		
		pointersToFree[ptfCount++] = humanReadableItemName;
	} else {
		humanReadableItemName = message->itemTitle;
	}
	
	char* outString;
	asprintf(&outString, "<PlexMessage %p: %s (%s) type: %s, account: %s>", message, humanReadableItemName, plexMediaType_string(message->mediaType), plexEvent_string(message->event), message->accountName ?: "");
	
	assert(ptfCount <= ptfSize);
	
	while (*pointersToFree) {
		free(*(pointersToFree++));
	}
	
	return outString;
}

char const * plexEvent_string(PlexEvent event) {
	switch (event) {
		case PlexEventPlay: return "play";
		case PlexEventPause: return "pause";
		case PlexEventResume: return "resume";
		case PlexEventStop: return "stop";
		case PlexEventScrobble: return "scrobble";
		case PlexEventRate: return "rate";
			
		case PlexEventUnknown: default: return "unknown";
	}
}

char const * plexMediaType_string(PlexMediaType type) {
	switch (type) {
		case PlexMediaTypeMovie: return "movie";
		case PlexMediaTypeEpisode: return "episode";
		
		case PlexMediaTypeUnknown: default: return "";
	}
}

// MARK: Private Definitions

static void plexMessage_private_partialInitFromJSONString(PlexMessage* message,
														  char const * jsonString,
														  size_t jsonStringLen,
														  char const * imageURLHost) {
	// printf("%.*s\n", jsonStringLen, jsonString);
	
	json_value* payloadDict = json_parse(jsonString, jsonStringLen);
	if (!json_checkValueIsObject(payloadDict)) { return; }
	
	json_value* accountDict = json_objectGetValueForKey(payloadDict, "Account");
	if (!json_checkValueIsObject(accountDict)) { return; }
	
	json_value* accountTitleNode = json_objectGetValueForKey(accountDict, "title");
	if (!json_checkValueIsString(accountTitleNode)) { return; }
	message->accountName = plexMessage_private_allocatingCopyFromStringNode(accountTitleNode);
		
	json_value* eventNode = json_objectGetValueForKey(payloadDict, "event");
	if (!json_checkValueIsString(eventNode)) { return; }
	message->event = plexMessage_private_eventFromStringNode(eventNode);
	
	json_value* metadataDict = json_objectGetValueForKey(payloadDict, "Metadata");
	if (!json_checkValueIsObject(metadataDict)) { return; }
	
	json_value* typeNode = json_objectGetValueForKey(metadataDict, "type");
	if (!json_checkValueIsString(typeNode)) { return; }
	message->mediaType = plexMessage_private_mediaTypeFromStringNode(typeNode);
	
	json_value* titleNode = json_objectGetValueForKey(metadataDict, "title");
	if (!json_checkValueIsString(titleNode)) { return; }
	message->itemTitle = plexMessage_private_allocatingCopyFromStringNode(titleNode);
	
	json_value* grandparentTitleNode = json_objectGetValueForKey(metadataDict, "grandparentTitle");
	if (json_checkValueIsString(grandparentTitleNode)) {
		message->grandparentTitle = plexMessage_private_allocatingCopyFromStringNode(grandparentTitleNode);
	}
	
	char const * imageKeys[] = {"grandparentThumb", "thumb", "grandparentArt", "art"};
	
	for (int i=0; i<length_of_fixed_length_array(imageKeys); i++) {
		char const * key = imageKeys[i];
		
		json_value* imageNode = json_objectGetValueForKey(metadataDict, key);
		if (json_checkValueIsString(imageNode)) {
			char* url = plexMessage_private_allocatingConstructAbsoluteURLFromRequestAndFragment(imageURLHost,
																								 imageNode->u.string.ptr,
																								 imageNode->u.string.length);
			message->imageURL = url;
			break;
		}
	}
	
	json_value_free(payloadDict);
}

static char* plexMessage_private_allocatingConstructAbsoluteURLFromRequestAndFragment(char const * imageURLHost,
																					  char const * urlFragment,
																					  size_t urlFragmentLen) {
	char* outString;
	asprintf(&outString, "http://%s:32400%.*s", imageURLHost, (int)urlFragmentLen, urlFragment);
	
	return outString;
}

static PlexEvent plexMessage_private_eventFromStringNode(json_value* eventNode) {
	char* uniquePortion = eventNode->u.string.ptr + 6; // Ignore the leading "media."
	
	if (strcmp(uniquePortion, "play") == 0) { return PlexEventPlay; }
	if (strcmp(uniquePortion, "pause") == 0) { return PlexEventPause; }
	if (strcmp(uniquePortion, "resume") == 0) { return PlexEventResume; }
	if (strcmp(uniquePortion, "stop") == 0) { return PlexEventStop; }
	if (strcmp(uniquePortion, "scrobble") == 0) { return PlexEventScrobble; }
	if (strcmp(uniquePortion, "rate") == 0) { return PlexEventRate; }
	
	return PlexEventUnknown;
}

static PlexMediaType plexMessage_private_mediaTypeFromStringNode(json_value* mediaTypeNode) {
	char* str = mediaTypeNode->u.string.ptr;
	
	if (strcmp(str, "episode") == 0) { return PlexMediaTypeEpisode; }
	if (strcmp(str, "movie") == 0) { return PlexMediaTypeMovie; }
	
	return PlexMediaTypeUnknown;
}

static char* plexMessage_private_allocatingCopyFromStringNode(json_value* stringNode) {
	if (!json_checkValueIsString(stringNode)) {
		fputs("Non-string found. Huh.", stderr);
		return calloc(1, 1);
	}
	
	if (stringNode->u.string.length == 0) {
		fputs("Zero-length string found. Huh.", stderr);
	}
	
	char* buffer = malloc(stringNode->u.string.length+1);
	strcpy(buffer, stringNode->u.string.ptr);
	return buffer;
}

static void* plexMessage_private_allocatingCopyFromBuffer(void const* src, size_t len) {
	return memcpy(malloc(len), src, len);
}
