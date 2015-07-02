/*
 * libtcore
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Ja-young Gu <jygu@samsung.com>
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

#ifndef __TCORE_HAL_H__
#define __TCORE_HAL_H__

__BEGIN_DECLS

typedef void (*TcoreHalReceiveCallback)(TcoreHal *hal, unsigned int data_len, const void *data, void *user_data);
typedef enum tcore_hook_return (*TcoreHalSendHook)(TcoreHal *hal, unsigned int data_len, void *data, void *user_data);
typedef void (*TcoreHalSetupNetifCallback)(CoreObject *co, int result, const char* devname, void *user_data);

enum tcore_hal_recv_data_type {
	TCORE_HAL_RECV_INDICATION,
	TCORE_HAL_RECV_RESPONSE,
	TCORE_HAL_RECV_NOTIFICATION,
	TCORE_HAL_RECV_UNKNOWN
};

enum tcore_hal_mode {
	TCORE_HAL_MODE_UNKNOWN,
	TCORE_HAL_MODE_AT,
	TCORE_HAL_MODE_CUSTOM,
    TCORE_HAL_MODE_TRANSPARENT
};

struct tcore_hal_operations {
	TReturn (*power)(TcoreHal *hal, gboolean flag);
	TReturn (*send)(TcoreHal *hal, unsigned int data_len, void *data);
	TReturn (*setup_netif)(CoreObject *co,
		TcoreHalSetupNetifCallback func, void *user_data,
		unsigned int cid, gboolean enable);
};

TcoreHal *tcore_hal_new(TcorePlugin *plugin, const char *name,
	struct tcore_hal_operations *hops, enum tcore_hal_mode mode);
void tcore_hal_free(TcoreHal *hal);

TReturn tcore_hal_set_name(TcoreHal *hal, const char *name);
char *tcore_hal_get_name(TcoreHal *hal);

TcoreAT *tcore_hal_get_at(TcoreHal *hal);
enum tcore_hal_mode tcore_hal_get_mode(TcoreHal *hal);
TReturn tcore_hal_set_mode(TcoreHal *hal, enum tcore_hal_mode mode);

TReturn tcore_hal_set_power(TcoreHal *hal, gboolean flag);

TReturn tcore_hal_link_user_data(TcoreHal *hal, void *user_data);
void *tcore_hal_ref_user_data(TcoreHal *hal);

TReturn tcore_hal_send_data(TcoreHal *hal, unsigned int data_len, void *data);
TReturn tcore_hal_send_request(TcoreHal *hal, TcorePending *pending);
TReturn tcore_hal_send_force(TcoreHal *hal);

TReturn tcore_hal_free_timeout_pending_request(TcoreHal *hal, TcorePending *p,
	unsigned int data_len, const void *data);
TReturn tcore_hal_dispatch_response_data(TcoreHal *hal, int id,
	unsigned int data_len, const void *data);

TReturn tcore_hal_add_recv_callback(TcoreHal *hal,
	TcoreHalReceiveCallback func, void *user_data);
TReturn tcore_hal_remove_recv_callback(TcoreHal *hal,
	TcoreHalReceiveCallback func);
TReturn tcore_hal_emit_recv_callback(TcoreHal *hal,
	unsigned int data_len, const void *data);

TReturn tcore_hal_add_send_hook(TcoreHal *hal,
	TcoreHalSendHook func, void *user_data);
TReturn tcore_hal_remove_send_hook(TcoreHal *hal, TcoreHalSendHook func);

TReturn tcore_hal_set_power_state(TcoreHal *hal, gboolean flag);
gboolean tcore_hal_get_power_state(TcoreHal *hal);

TcoreQueue *tcore_hal_ref_queue(TcoreHal *hal);
TcorePlugin *tcore_hal_ref_plugin(TcoreHal *hal);

TReturn tcore_hal_setup_netif(TcoreHal *hal, CoreObject *co,
	TcoreHalSetupNetifCallback func, void *user_data,
	unsigned int cid, gboolean enable);

__END_DECLS

#endif /* __TCORE_HAL_H__ */
