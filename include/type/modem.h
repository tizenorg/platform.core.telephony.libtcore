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

enum modem_dun_pincontrol_signal {
	DUN_PINCONTROL_SIGNAL_DCD = 0x01,
	DUN_PINCONTROL_SIGNAL_DTR = 0x04,
	DUN_PINCONTROL_SIGNAL_DSR = 0x06,
	DUN_PINCONTROL_SIGNAL_RTS = 0x07,
	DUN_PINCONTROL_SIGNAL_CTS = 0x08,
	DUN_PINCONTROL_SIGNAL_RI = 0x09
};

enum modem_dun_pincontrol_status {
	DUN_PINCONTROL_STATUS_OFF = 0x00,
	DUN_PINCONTROL_STATUS_ON = 0x01,
};

struct treq_modem_power_on {
};

struct treq_modem_power_off {
};

struct treq_modem_power_reset {
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

struct treq_modem_get_sn {
};

struct treq_modem_set_dun_pin_control {
	enum modem_dun_pincontrol_signal signal;
	enum modem_dun_pincontrol_status status;
};


struct tresp_modem_power_on {
	TReturn result;
};

struct tresp_modem_power_off {
	TReturn result;
};

struct tresp_modem_power_reset {
	TReturn result;
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
	char software[33];
	char hardware[33];
	char calibration[33];
	char product_code[33];
};

struct tresp_modem_get_sn {
	TReturn result;
	char sn[13];
};

struct tresp_modem_set_dun_pin_control {
	int result;
};

struct tnoti_modem_power {
	enum modem_state state;
};

struct tnoti_modem_flight_mode {
	int enable;
};

struct tnoti_modem_dun_pin_control {
	enum modem_dun_pincontrol_signal signal;
	enum modem_dun_pincontrol_status status;
};

struct tnoti_modem_dun_external_call {
};

__END_DECLS

#endif
