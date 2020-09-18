//
//  JSONUtils.c
//  Now Plexing
//
//  Created by Aaron Rennow on 1/23/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#include "JSONUtils.h"
#include <assert.h>
#include <string.h>

json_value* json_objectGetValueForKey(json_value* object, char const * key) {
	if (!json_checkValueIsObject(object)) { return NULL; }
	
	for (unsigned i=0; i<object->u.object.length; i++) {
		// ick
		if (strncmp(key, object->u.object.values[i].name, object->u.object.values[i].name_length) == 0) {
			return object->u.object.values[i].value;
		}
	}
	
	return NULL;
}

bool json_checkValueIsObject(json_value* value) {
	return value && value->type == json_object;
}

bool json_checkValueIsString(json_value* value) {
	return value && value->type == json_string;
}