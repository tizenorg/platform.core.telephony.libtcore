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

#ifndef __CO_MODEM_H__
#define __CO_MODEM_H__

#include "core_object.h"
#include <tel_modem.h>
#include <tel_return.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TelReturn (*set_power_status)(CoreObject *co, TelModemPowerStatus status, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_flight_mode)(CoreObject *co, gboolean enable, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_flight_mode)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_version)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_imei)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
} TcoreModemOps;

CoreObject *tcore_modem_new(TcorePlugin *plugin, TcoreModemOps *ops, TcoreHal *hal);
void tcore_modem_free(CoreObject *co);

gboolean tcore_modem_set_ops(CoreObject *co, TcoreModemOps *ops);
void tcore_modem_override_ops(CoreObject *co, TcoreModemOps *ops);

gboolean tcore_modem_set_flight_mode_state(CoreObject *co, gboolean state);
gboolean tcore_modem_get_flight_mode_state(CoreObject *co, gboolean *state);
gboolean tcore_modem_set_powered(CoreObject *co, gboolean pwr);
gboolean tcore_modem_get_powered(CoreObject *co, gboolean *pwr);

#ifdef __cplusplus
}
#endif

#endif /* __CO_MODEM_H__ */
