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

#ifndef __TCORE_CO_MODEM_H__
#define __TCORE_CO_MODEM_H__

#include <core_object.h>

__BEGIN_DECLS


struct tcore_modem_operations {
	TReturn (*power_on)(CoreObject *o, UserRequest *ur);
	TReturn (*power_off)(CoreObject *o, UserRequest *ur);
	TReturn (*power_reset)(CoreObject *o, UserRequest *ur);
	TReturn (*set_flight_mode)(CoreObject *o, UserRequest *ur);
	TReturn (*get_imei)(CoreObject *o, UserRequest *ur);
	TReturn (*get_version)(CoreObject *o, UserRequest *ur);
	TReturn (*get_sn)(CoreObject *o, UserRequest *ur);
	TReturn (*dun_pin_ctrl)(CoreObject *o, UserRequest *ur);
	TReturn (*get_flight_mode)(CoreObject *o, UserRequest *ur);
};


CoreObject*      tcore_modem_new(TcorePlugin *p, const char *name, struct tcore_modem_operations *ops, TcoreHal *hal);
void             tcore_modem_free(CoreObject *o);

TReturn          tcore_modem_set_flight_mode_state(CoreObject *o, gboolean flag);
gboolean         tcore_modem_get_flight_mode_state(CoreObject *o);
TReturn          tcore_modem_set_powered(CoreObject *o, gboolean pwr);
gboolean         tcore_modem_get_powered(CoreObject *o);

__END_DECLS

#endif
