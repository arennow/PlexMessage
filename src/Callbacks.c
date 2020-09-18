//
//  Callbacks.c
//  Now Plexing
//
//  Created by Aaron Rennow on 1/24/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#include "Callbacks.h"
#include <stdlib.h>
#include <stdio.h>

#if DEBUG
static void debugCallbackHandler(PlexMessage* message) {
	char* description = plexMessage_description(message);
	puts(description);
	free(description);
	
	plexMessage_free(message);
}
#endif

void(*plexMessageReceivedCallback)(PlexMessage* message) =
#if DEBUG
debugCallbackHandler;
#else
plexMessage_free;
#endif
