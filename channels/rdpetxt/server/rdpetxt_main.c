/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * RDPETXT server-side implementation (skeleton)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <winpr/crt.h>
#include <winpr/stream.h>
#include <freerdp/channels/log.h>
#include <freerdp/channels/wtsvc.h>
#include <freerdp/dvc.h>

#include <freerdp/server/rdpetxt.h>

#define TAG CHANNELS_TAG("rdpetxt.server")

struct _rdpetxt_server_private
{
	HANDLE channelHandle;
	HANDLE eventHandle;
};

RdpetxtServerContext* rdpetxt_server_context_new(HANDLE vcm)
{
	RdpetxtServerContext* context = calloc(1, sizeof(RdpetxtServerContext));
	RdpetxtServerPrivate* priv = calloc(1, sizeof(RdpetxtServerPrivate));

	if (!context || !priv)
	{
		free(context);
		free(priv);
		return NULL;
	}

	priv->channelHandle = INVALID_HANDLE_VALUE;
	priv->eventHandle = NULL;

	context->vcm = vcm;
	context->priv = priv;
	return context;
}

void rdpetxt_server_context_free(RdpetxtServerContext* context)
{
	if (!context)
		return;

	if (context->priv)
	{
		if (context->priv->channelHandle && (context->priv->channelHandle != INVALID_HANDLE_VALUE))
			WTSVirtualChannelClose(context->priv->channelHandle);
		free(context->priv);
	}

	free(context);
}

UINT rdpetxt_server_init(RdpetxtServerContext* context)
{
	void* buffer = NULL;
	DWORD bytesReturned = 0;
	RdpetxtServerPrivate* priv;

	if (!context || !context->priv)
		return ERROR_INVALID_PARAMETER;

	priv = context->priv;
	priv->channelHandle =
	    WTSVirtualChannelOpenEx(WTS_CURRENT_SESSION, RDPETXT_DVC_CHANNEL_NAME,
	                            WTS_CHANNEL_OPTION_DYNAMIC);

	if (!priv->channelHandle)
	{
		WLog_ERR(TAG, "WTSVirtualChannelOpenEx failed for %s", RDPETXT_DVC_CHANNEL_NAME);
		return CHANNEL_RC_INITIALIZATION_ERROR;
	}

	if (!WTSVirtualChannelQuery(priv->channelHandle, WTSVirtualEventHandle, &buffer,
	                            &bytesReturned) ||
	    (bytesReturned != sizeof(HANDLE)))
	{
		WLog_ERR(TAG, "WTSVirtualChannelQuery failed or invalid size");
		if (buffer)
			WTSFreeMemory(buffer);
		WTSVirtualChannelClose(priv->channelHandle);
		priv->channelHandle = INVALID_HANDLE_VALUE;
		return CHANNEL_RC_INITIALIZATION_ERROR;
	}

	CopyMemory(&priv->eventHandle, buffer, sizeof(HANDLE));
	WTSFreeMemory(buffer);
	return CHANNEL_RC_OK;
}

HANDLE rdpetxt_server_get_event_handle(RdpetxtServerContext* context)
{
	if (!context || !context->priv)
		return NULL;
	return context->priv->eventHandle;
}

UINT rdpetxt_server_handle_messages(RdpetxtServerContext* context)
{
	RdpetxtServerPrivate* priv;
	BYTE buffer[4096];
	DWORD bytesRead = 0;
	UINT status = CHANNEL_RC_OK;

	if (!context || !context->priv)
		return ERROR_INVALID_PARAMETER;

	priv = context->priv;

	if (!priv->channelHandle || priv->channelHandle == INVALID_HANDLE_VALUE)
		return CHANNEL_RC_NOT_INITIALIZED;

	while (WTSVirtualChannelRead(priv->channelHandle, 0, (PCHAR)buffer, sizeof(buffer),
	                             &bytesRead))
	{
		if (bytesRead == 0)
			break;

		if (context->Receive)
		{
			status = context->Receive(context, buffer, bytesRead);
			if (status)
				break;
		}
	}

	return status;
}

UINT rdpetxt_server_send(RdpetxtServerContext* context, const BYTE* data, UINT32 length)
{
	DWORD bytesWritten = 0;
	RdpetxtServerPrivate* priv;

	if (!context || !context->priv || !data)
		return ERROR_INVALID_PARAMETER;

	priv = context->priv;

	if (!priv->channelHandle || priv->channelHandle == INVALID_HANDLE_VALUE)
		return CHANNEL_RC_NOT_INITIALIZED;

	if (!WTSVirtualChannelWrite(priv->channelHandle, (PCHAR)data, length, &bytesWritten))
		return ERROR_INTERNAL_ERROR;

	return (bytesWritten == length) ? CHANNEL_RC_OK : ERROR_INTERNAL_ERROR;
}

UINT DVCPluginEntry(IDRDYNVC_ENTRY_POINTS* pEntryPoints)
{
	WLog_INFO(TAG, "RDPETXT server plugin stub loaded");
	return CHANNEL_RC_OK;
}
