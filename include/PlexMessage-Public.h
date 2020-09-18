//
//  PlexMessage-Public.h
//  PlexMessage
//
//  Created by Aaron Rennow on 1/24/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#ifndef PlexMessage_Public_h
#define PlexMessage_Public_h

#include <stddef.h>
#include "SwiftExportDeclarations.h"

typedef CF_ENUM(int, PlexEvent) {
	PlexEventUnknown = 0,
	PlexEventPlay,
	PlexEventPause,
	PlexEventResume,
	PlexEventStop,
	PlexEventScrobble,
	PlexEventRate
};

typedef CF_ENUM(int, PlexMediaType) {
	PlexMediaTypeUnknown = 0,
	PlexMediaTypeMovie,
	PlexMediaTypeEpisode
};

char const * plexEvent_string(PlexEvent event);
char const * plexMediaType_string(PlexMediaType type);

struct PlexMessage {
	PlexEvent event;
	PlexMediaType mediaType;
	char* itemTitle;
	char* grandparentTitle;
	struct {
		void* ptr;
		size_t len;
	} bundledImageData;
	char* imageURL;
	char* accountName;
};

typedef struct PlexMessage PlexMessage;

void plexMessage_free(PlexMessage* message) CF_SWIFT_NAME(PlexMessage.free(self:));

/// The caller is responsible for freeing the resultant string.
char* plexMessage_description(PlexMessage* message) CF_SWIFT_NAME(PlexMessage.description(self:));

#endif /* PlexMessage_Public_h */
