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

#ifndef __TYPE_NETWORK_H__
#define __TYPE_NETWORK_H__

__BEGIN_DECLS

#define NETWORK_GERAN_NEIGHBORING_CELL_LIST_MAX 6		/**<	max count of GERAN neighboring cell list	*/
#define NETWORK_UMTS_NEIGHBORING_CELL_LIST_MAX 24		/**<	max count of UMTS neighboring cell list		*/

#define NETWORK_MODE_AUTO  0x00
#define NETWORK_MODE_GSM   0x01
#define NETWORK_MODE_WCDMA 0x02
#define NETWORK_MODE_CDMA  0x04
#define NETWORK_MODE_LTE   0x08

#define NETWORK_RESTRICTED_STATE_NONE           0x00 /* No restriction at all including voice/SMS/USSD/SS/AV64 and packet data. */
#define NETWORK_RESTRICTED_STATE_CS_EMERGENCY   0x01 /* Block emergency call due to restriction. But allow all normal voice/SMS/USSD/SS/AV64. */
#define NETWORK_RESTRICTED_STATE_CS_NORMAL      0x02 /* Block all normal voice/SMS/USSD/SS/AV64 due to restriction. Only Emergency call allowed. */
#define NETWORK_RESTRICTED_STATE_CS_ALL         0x04 /* Block all voice/SMS/USSD/SS/AV64 including emergency call due to restriction.*/
#define NETWORK_RESTRICTED_STATE_PS_ALL         0x10 /* Block packet data access due to restriction. */

enum telephony_network_select_mode {
	NETWORK_SELECT_MODE_AUTOMATIC = 0x01,
	NETWORK_SELECT_MODE_MANUAL = 0x02
};

enum telephony_network_plmn_status {
	NETWORK_PLMN_UNKNOWN,
	NETWORK_PLMN_AVAILABLE,
	NETWORK_PLMN_CURRENT,
	NETWORK_PLMN_FORBIDDEN
};

enum telephony_network_service_domain {
	NETWORK_SERVICE_DOMAIN_COMBINED = 0x01,
	NETWORK_SERVICE_DOMAIN_CS,
	NETWORK_SERVICE_DOMAIN_PS,
	NETWORK_SERVICE_DOMAIN_AUTOMATIC
};

enum telephony_network_service_domain_status {
	NETWORK_SERVICE_DOMAIN_STATUS_NO = 0x0,
	NETWORK_SERVICE_DOMAIN_STATUS_EMERGENCY,
	NETWORK_SERVICE_DOMAIN_STATUS_FULL,
	NETWORK_SERVICE_DOMAIN_STATUS_SEARCH
};

enum telephony_network_service_type {
	NETWORK_SERVICE_TYPE_UNKNOWN = 0x0,
	NETWORK_SERVICE_TYPE_NO_SERVICE,
	NETWORK_SERVICE_TYPE_EMERGENCY,
	NETWORK_SERVICE_TYPE_SEARCH,
	NETWORK_SERVICE_TYPE_2G,
	NETWORK_SERVICE_TYPE_2_5G,
	NETWORK_SERVICE_TYPE_2_5G_EDGE,
	NETWORK_SERVICE_TYPE_3G,
	NETWORK_SERVICE_TYPE_HSDPA,
	NETWORK_SERVICE_TYPE_LTE
};

enum telephony_network_access_technology {
	NETWORK_ACT_UNKNOWN = 0x0,
	NETWORK_ACT_GSM = 0x1,
	NETWORK_ACT_GPRS,
	NETWORK_ACT_EGPRS,
	NETWORK_ACT_UMTS = 0x4,
	NETWORK_ACT_UTRAN = 0x4,
	NETWORK_ACT_GSM_UTRAN,
	NETWORK_ACT_IS95A = 0x11,
	NETWORK_ACT_IS95B,
	NETWORK_ACT_CDMA_1X,
	NETWORK_ACT_EVDO_REV0,
	NETWORK_ACT_CDMA_1X_EVDO_REV0,
	NETWORK_ACT_EVDO_REVA,
	NETWORK_ACT_CDMA_1X_EVDO_REVA,
	NETWORK_ACT_EVDV,
	NETWORK_ACT_LTE = 0x21,
	NETWORK_ACT_NOT_SPECIFIED = 0xFF
};

enum telephony_network_band_mode {
	NETWORK_BAND_MODE_PREFERRED = 0x01,
	NETWORK_BAND_MODE_ONLY = 0x02
};

enum telephony_network_band {
	NETWORK_BAND_TYPE_ANY = 0x00,
	NETWORK_BAND_TYPE_GSM850,
	NETWORK_BAND_TYPE_GSM_900_1800,
	NETWORK_BAND_TYPE_GSM1900,
	NETWORK_BAND_TYPE_GSM,
	NETWORK_BAND_TYPE_WCDMA,
	NETWORK_BAND_TYPE_WCDMA850,
	NETWORK_BAND_TYPE_WCDMA1900,
	NETWORK_BAND_TYPE_WCDMA2100,
	NETWORK_BAND_TYPE_GSM900,
	NETWORK_BAND_TYPE_GSM1800,
	NETWORK_BAND_TYPE_GSM_850_1900,
	NETWORK_BAND_TYPE_LTE_BAND_4,
	NETWORK_BAND_TYPE_LTE_BAND_17
};

enum telephony_network_preferred_plmn_operation {
	NETWORK_PREF_PLMN_ADD = 0x01,
	NETWORK_PREF_PLMN_EDIT,
	NETWORK_PREF_PLMN_DELETE
};

enum telephony_network_acquisition_order {
	NETWORK_ORDER_AUTO = 0x01,
	NETWORK_ORDER_GSM_FIRST = 0x02,
	NETWORK_ORDER_WCDMA_FIRST = 0x03,
	NETWORK_ORDER_NO_CHANGE = 0x04,
	NETWORK_ORDER_LTE_FIRST = 0x05
};

enum telephony_network_icon_info_type {
	NETWORK_ICON_INFO_RSSI = 0x01,
	NETWORK_ICON_INFO_BATTERY = 0x02,
	NETWORK_ICON_INFO_HDR_RSSI = 0x04,
	NETWORK_ICON_INFO_ALL = 0xFF
};

struct tel_network_geran_neighboring_cell_info{
	int cell_id;
	int lac;
	int bcch;
	int bsic;
	int rxlev;
};

struct tel_network_umts_neighboring_cell_info{
	int cell_id;
	int lac;
	int arfcn;
	int psc;
	int rscp;
};

struct tel_network_neighboring_cell_info{
	int geran_list_count;
	struct tel_network_geran_neighboring_cell_info geran_list[NETWORK_GERAN_NEIGHBORING_CELL_LIST_MAX];
	int umts_list_count;
	struct tel_network_umts_neighboring_cell_info umts_list[NETWORK_UMTS_NEIGHBORING_CELL_LIST_MAX];
};


struct treq_network_search { /* no data */
};
struct treq_network_set_plmn_selection_mode {
	enum telephony_network_select_mode mode;
	char plmn[7];
	enum telephony_network_access_technology act;
};
struct treq_network_get_plmn_selection_mode { /* no data */
};
struct treq_network_set_service_domain {
	enum telephony_network_service_domain domain;
};
struct treq_network_get_service_domain { /* no data */
};
struct treq_network_set_band {
	enum telephony_network_band_mode mode;
	enum telephony_network_band band;
};
struct treq_network_get_band { /* no data */
};
struct treq_network_set_preferred_plmn {
	enum telephony_network_preferred_plmn_operation operation;
	char plmn[7];
	enum telephony_network_access_technology act;
	int ef_index;
};
struct treq_network_get_preferred_plmn { /* no data */
};
struct treq_network_set_order {
	enum telephony_network_acquisition_order order;
};
struct treq_network_get_order { /* no data */
};
struct treq_network_set_power_on_attach {
	int enable;
};
struct treq_network_get_power_on_attach { /* no data */
};
struct treq_network_set_cancel_manual_search { /* no data */
};
struct treq_network_get_serving_network { /* no data */
};
struct treq_network_set_mode {
	int mode;
};
struct treq_network_get_mode { /* no data */
};
struct treq_network_set_neighboring_cell_info {
	int enable;
};
struct treq_network_get_neighboring_cell_info { /* no data */
};


struct tresp_network_search {
	TReturn result;
	int list_count;
	struct {
		enum telephony_network_plmn_status  status;
		char name[40];
		char plmn[7];
		enum telephony_network_access_technology act;
		unsigned int lac;
		unsigned int rac;
		unsigned int cell_id;
		unsigned int tac;
	} list[20];
};

struct tresp_network_set_plmn_selection_mode {
	TReturn result;
};

struct tresp_network_get_plmn_selection_mode {
	TReturn result;
	enum telephony_network_select_mode mode;
};

struct tresp_network_set_service_domain {
	TReturn result;
};

struct tresp_network_get_service_domain {
	TReturn result;
	enum telephony_network_service_domain domain;
};

struct tresp_network_set_band {
	TReturn result;
};

struct tresp_network_get_band {
	TReturn result;
	enum telephony_network_band_mode mode;
	enum telephony_network_band band;
};

struct tresp_network_set_preferred_plmn {
	TReturn result;
};

struct tresp_network_get_preferred_plmn {
	TReturn result;
	int list_count;
	struct {
		int ef_index;
		char plmn[7];
		enum telephony_network_access_technology act;
		unsigned int lac;
	} list[150];
};

struct tresp_network_set_order {
	TReturn result;
};

struct tresp_network_get_order {
	TReturn result;
	enum telephony_network_acquisition_order order;
};

struct tresp_network_set_power_on_attach {
	TReturn result;
};

struct tresp_network_get_power_on_attach {
	TReturn result;
	int enabled;
};

struct tresp_network_set_cancel_manual_search {
	TReturn result;
};

struct tresp_network_get_serving_network {
	TReturn result;
	char plmn[7];
	enum telephony_network_access_technology act;
	struct {
		unsigned int lac;
	} gsm;
	struct {
		int carrier;
		int sid;
		int nid;
		int bs_id;
		int bs_lat;
		int bs_long;
		int reg_zone;
		int pilot_pn;
	} cdma;
};

struct tresp_network_set_mode {
	TReturn result;
};

struct tresp_network_get_mode {
	TReturn result;
	int mode;
};

struct tresp_network_set_neighboring_cell_info {
	TReturn result;
};

struct tresp_network_get_neighboring_cell_info {
	TReturn result;
	struct tel_network_neighboring_cell_info info;
};


struct tnoti_network_registration_status {
	enum telephony_network_service_domain_status cs_domain_status;
	enum telephony_network_service_domain_status ps_domain_status;
	enum telephony_network_service_type service_type;
	int roaming_status;
};

struct tnoti_network_location_cellinfo {
	unsigned int lac;
	unsigned int cell_id;
};

struct tnoti_network_icon_info {
	enum telephony_network_icon_info_type type;
	int rssi;
	int battery;
	int hdr_rssi;
};

struct tnoti_network_change {
	char plmn[7];
	enum telephony_network_access_technology act;
	struct {
		unsigned int lac;
	} gsm;
	struct {
		int carrier;
		int sid;
		int nid;
		int bs_id;
		int bs_lat;
		int bs_long;
		int reg_zone;
		int pilot_pn;
	} cdma;
};

struct tnoti_network_timeinfo {
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int wday;
	int gmtoff;
	int dstoff;
	unsigned int isdst;
	char plmn[7];
};

struct tnoti_network_identity {
	char short_name[17];
	char full_name[33];
	char plmn[7];
};

struct tnoti_network_neighboring_cell_info {
	struct tel_network_neighboring_cell_info info;
};

struct tnoti_network_restricted_state {
	int restricted_state;
};

__END_DECLS

#endif
