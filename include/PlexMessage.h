//
//  PlexMessage.h
//  Now Plexing
//
//  Created by Aaron Rennow on 1/23/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#ifndef PlexMessage_h
#define PlexMessage_h

#include "PlexMessage-Public.h"
#include "mongoose.h"

/// The value in `imageURLHost` is copied.
PlexMessage* plexMessage_initFromHTTPMessage(struct http_message* hm, char const * imageURLHost);
void plexMessage_free(PlexMessage* message);

#endif /* PlexMessage_h */
