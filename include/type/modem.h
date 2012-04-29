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

#ifndef __TYPE_MODEM_H__
#define __TYPE_MODEM_H__

__BEGIN_DECLS

enum modem_state {
	MODEM_STATE_ONLINE,
	MODEM_STATE_OFFLINE,
	MODEM_STATE_ERROR,
};

struct treq_modem_set_flightmode {
	int enable;
};

struct treq_modem_get_flightmode {
};

struct treq_modem_get_imei {
};

struct treq_modem_get_version {
};


struct tresp_modem_set_flightmode {
	TReturn result;
};

struct tresp_modem_get_flightmode {
	TReturn result;
	int enable;
};

struct tresp_modem_get_imei {
	TReturn result;
	char imei[17];
};

struct tresp_modem_get_version {
	TReturn result;
	char software[20];
	char hardware[20];
};

struct tnoti_modem_power {
	enum modem_state state;
};

struct tnoti_modem_flight_mode {
	int enable;
};

__END_DECLS

#endif
