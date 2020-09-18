//
//  JSONUtils.h
//  Now Plexing
//
//  Created by Aaron Rennow on 1/23/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#ifndef JSONUtils_h
#define JSONUtils_h

#include "json.h"
#include <stdbool.h>

json_value* json_objectGetValueForKey(json_value* object, char const * key);

bool json_checkValueIsObject(json_value* value);
bool json_checkValueIsString(json_value* value);

#endif /* JSONUtils_h */
