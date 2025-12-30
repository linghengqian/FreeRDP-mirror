/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * RDPETXT server-side definitions (Text Input Virtual Channel)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FREERDP_CHANNEL_RDPETXT_SERVER_H
#define FREERDP_CHANNEL_RDPETXT_SERVER_H

#include <freerdp/channels/wtsvc.h>
#include <freerdp/channels/rdpetxt.h>

typedef struct _rdpetxt_server_context RdpetxtServerContext;
typedef struct _rdpetxt_server_private RdpetxtServerPrivate;

struct _rdpetxt_server_context
{
	HANDLE vcm;
	RdpetxtServerPrivate* priv;

	/* Optional callback for incoming data; raw PDU bytes are provided as-is. */
	UINT (*Receive)(RdpetxtServerContext* context, const BYTE* data, UINT32 length);

	void* user_data;
};

#ifdef __cplusplus
extern "C"
{
#endif

FREERDP_API RdpetxtServerContext* rdpetxt_server_context_new(HANDLE vcm);
FREERDP_API void rdpetxt_server_context_free(RdpetxtServerContext* context);
FREERDP_API UINT rdpetxt_server_init(RdpetxtServerContext* context);
FREERDP_API HANDLE rdpetxt_server_get_event_handle(RdpetxtServerContext* context);
FREERDP_API UINT rdpetxt_server_handle_messages(RdpetxtServerContext* context);
FREERDP_API UINT rdpetxt_server_send(RdpetxtServerContext* context, const BYTE* data, UINT32 length);

#ifdef __cplusplus
}
#endif

#endif /* FREERDP_CHANNEL_RDPETXT_SERVER_H */
