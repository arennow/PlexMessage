//
//  Callbacks.h
//  Now Plexing
//
//  Created by Aaron Rennow on 1/24/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#ifndef Callbacks_h
#define Callbacks_h

#include "PlexMessage-Public.h"

/// Set this to an appropriate value to receive callbacks when a new Plex Message is received.
/// `message` is a heap-allocated value. You must call `plexMessage_free` on it when you are finished.
extern void(*plexMessageReceivedCallback)(PlexMessage* message);

#endif /* Callbacks_h */
