//
//  HTTPServer.c
//  Now Plexing
//
//  Created by Aaron Rennow on 1/23/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#include "HTTPServer.h"
#include "mongoose.h"
#include "PlexMessage.h"
#include "Callbacks.h"

static void ev_handler(struct mg_connection* c, int ev, void* ev_data) {
	switch(ev) {
		case MG_EV_HTTP_REQUEST: {
			char remoteHost[45]; // Max possible length of an IPv6 address (https://stackoverflow.com/a/7477384)
			mg_conn_addr_to_str(c, remoteHost, sizeof(remoteHost), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_REMOTE);
			
			struct http_message* hm = ev_data;
			
			PlexMessage* message = plexMessage_initFromHTTPMessage(hm, remoteHost);
			if (!message) { goto respond; }
			if (message->event == PlexEventUnknown) {
				free(message);
				goto respond;
			}
			
			plexMessageReceivedCallback(message);

			respond: {
				char const resp[] = "Thanks.";
				int const respLen = sizeof(resp)-1;

				mg_send_head(c, 200, respLen, NULL);
				mg_send(c, resp, respLen);
			}
		}
			break;
	}
}

void httpServer_start(void) {
	struct mg_mgr mgr;
	struct mg_connection *c;
	
	mg_mgr_init(&mgr, NULL);
	c = mg_bind(&mgr, "5555", ev_handler);
	mg_set_protocol_http_websocket(c);
	
	for (;;) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
}
