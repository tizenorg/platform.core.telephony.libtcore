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

#define MODEM_DEVICE_SN_LEN_MAX 13
#define MODEM_DEVICE_MEID_LEN_MAX 17
#define MODEM_DEVICE_IMEI_LEN_MAX 17
#define MODEM_DEVICE_IMEISV_LEN_MAX 17
#define DONGLE_VENDOR_LEN_MAX 32
#define DONGLE_DEVICE_LEN_MAX 32

enum modem_state {
	MODEM_STATE_UNKNOWN = -1,
	MODEM_STATE_ONLINE = 0,
	MODEM_STATE_OFFLINE,
	MODEM_STATE_RESET,
	MODEM_STATE_LOW,
	MODEM_STATE_MAX = MODEM_STATE_LOW,

	/* internal states (not to be published) */
	MODEM_STATE_ERROR,
	MODEM_STATE_RESUME
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

struct treq_modem_power_low {
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

struct tresp_modem_power_low {
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
	char imei[MODEM_DEVICE_IMEI_LEN_MAX];
};

struct tresp_modem_get_device_info {
	TReturn result;
	char vendor_name[DONGLE_VENDOR_LEN_MAX];
	char device_name[DONGLE_DEVICE_LEN_MAX];
};

struct tresp_modem_get_version {
	TReturn result;
	char software[33];
	char hardware[33];
	char calibration[33];
	char product_code[33];
	char prl_version[18];
	char eri_version[18];
};

struct tresp_modem_get_sn {
	TReturn result;
	char sn[MODEM_DEVICE_SN_LEN_MAX];
	char meid[MODEM_DEVICE_MEID_LEN_MAX];
	char imei[MODEM_DEVICE_IMEI_LEN_MAX];
	char imeisv[MODEM_DEVICE_IMEISV_LEN_MAX];
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

struct tnoti_modem_bootup {
	void *plugin;
};

__END_DECLS

#endif
