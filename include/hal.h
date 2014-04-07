/*
 * libtcore
 *
 * Copyright (c) 2013 Samsung Electronics Co. Ltd. All rights reserved.
 * Copyright (c) 2013 Intel Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __HAL_H__
#define __HAL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TcoreHalReceiveCallback)(TcoreHal *hal,
	guint data_len, const void *data, void *user_data);

typedef TcoreHookReturn (*TcoreHalSendHook)(TcoreHal *hal,
	guint data_len, void *data, void *user_data);

typedef void (*TcoreHalSetupNetifCallback)(CoreObject *co,
	gint result, const gchar* devname, void *user_data);

typedef enum {
	TCORE_HAL_RECV_INDICATION,
	TCORE_HAL_RECV_RESPONSE,
	TCORE_HAL_RECV_NOTIFICATION,
	TCORE_HAL_RECV_UNKNOWN
} TcoreHalRecvDataType;

typedef enum {
	TCORE_HAL_MODE_UNKNOWN,
	TCORE_HAL_MODE_AT,
	TCORE_HAL_MODE_CUSTOM,
	TCORE_HAL_MODE_TRANSPARENT
} TcoreHalMode;

typedef struct {
	TelReturn (*power)(TcoreHal *hal, gboolean flag);
	TelReturn (*send)(TcoreHal *hal, guint data_len, void *data);
	TelReturn (*setup_netif)(CoreObject *co,
		TcoreHalSetupNetifCallback func, void *user_data,
		guint cid, gboolean enable);
} TcoreHalOperations;

TcoreHal *tcore_hal_new(TcorePlugin *plugin, const gchar *name,
		TcoreHalOperations *hops, TcoreHalMode mode);
void tcore_hal_free(TcoreHal *hal);

TelReturn tcore_hal_set_name(TcoreHal *hal, const gchar *name);
gchar *tcore_hal_get_name(TcoreHal *hal);

TcoreAT *tcore_hal_get_at(TcoreHal *hal);
TcoreHalMode tcore_hal_get_mode(TcoreHal *hal);
TelReturn tcore_hal_set_mode(TcoreHal *hal, TcoreHalMode mode);

TelReturn tcore_hal_set_power(TcoreHal *hal, gboolean flag);

TelReturn tcore_hal_link_user_data(TcoreHal *hal, void *user_data);
void *tcore_hal_ref_user_data(TcoreHal *hal);

TelReturn tcore_hal_send_data(TcoreHal *hal, guint data_len, void *data);
TelReturn tcore_hal_send_request(TcoreHal *hal, TcorePending *pending);
TelReturn tcore_hal_send_force(TcoreHal *hal);

TelReturn tcore_hal_dispatch_response_data(TcoreHal *hal, guint id,
		guint data_len, const void *data);

TelReturn tcore_hal_add_recv_callback(TcoreHal *hal,
	TcoreHalReceiveCallback func, void *user_data);
TelReturn tcore_hal_remove_recv_callback(TcoreHal *hal,
	TcoreHalReceiveCallback func);
TelReturn tcore_hal_emit_recv_callback(TcoreHal *hal,
	guint data_len, const void *data);
TelReturn tcore_hal_add_send_hook(TcoreHal *hal,
	TcoreHalSendHook func, void *user_data);
TelReturn tcore_hal_remove_send_hook(TcoreHal *hal,
	TcoreHalSendHook func);

TelReturn tcore_hal_set_power_state(TcoreHal *hal, gboolean flag);
gboolean tcore_hal_get_power_state(TcoreHal *hal);

TcoreQueue *tcore_hal_ref_queue(TcoreHal *hal);
TcorePlugin *tcore_hal_ref_plugin(TcoreHal *hal);

TelReturn tcore_hal_setup_netif(TcoreHal *hal, CoreObject *co,
	TcoreHalSetupNetifCallback func, void *user_data,
	guint cid, gboolean enable);

#ifdef __cplusplus
}
#endif

#endif	/* __HAL_H__ */
