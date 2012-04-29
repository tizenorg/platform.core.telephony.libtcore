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

#ifndef __TCORE_CO_SS_H__
#define __TCORE_CO_SS_H__

#include <core_object.h>

__BEGIN_DECLS

#define MAX_TCORE_SS_NUMBER_LEN 83

enum tcore_ss_class {
	TCORE_SS_CLATCORE_SS_NONE = 0x00,              /* 0x00 */

	/* TELESERVICE */
	TCORE_SS_CLATCORE_SS_ALL_TELE = 0x10,          /* 0x10 : All Teleservices */
	TCORE_SS_CLATCORE_SS_VOICE = 0x11,             /* 0x11 : All Voice (telephony) */
	TCORE_SS_CLATCORE_SS_ALL_DATA_TELE = 0x12,     /* 0x12 : All Data Teleservices */
	TCORE_SS_CLATCORE_SS_FAX = 0x13,               /* 0x13 : All Fax Service */
	TCORE_SS_CLATCORE_SS_SMS = 0x16,               /* 0x16 : SMS service */
	TCORE_SS_CLATCORE_SS_VGCS = 0x17,              /* 0x17 : Voice Group Call Service */
	TCORE_SS_CLATCORE_SS_VBS = 0x18,               /* 0x18 : Voice Broadcast */
	TCORE_SS_CLATCORE_SS_ALL_TELE_EXPT_SMS = 0x19, /* 0x19 : All teleservice except SMS */

	/* BEARER SERVICE */
	TCORE_SS_CLATCORE_SS_ALL_BEARER = 0x20,        /* 0X20 : all bearer services */
	TCORE_SS_CLATCORE_SS_ALL_ASYNC = 0x21,         /* 0x21 : All Async services */
	TCORE_SS_CLATCORE_SS_ALL_SYNC = 0x22,          /* 0x22 : All sync services */
	TCORE_SS_CLATCORE_SS_ALL_CS_SYNC = 0x24,       /* 0x24 : All Circuit switched sync */
	TCORE_SS_CLATCORE_SS_ALL_CS_ASYNC = 0x25,      /* 0x25 : All Circuit switched async */
	TCORE_SS_CLATCORE_SS_ALL_DEDI_PS = 0x26,       /* 0x26 : All Dedicated packet Access */
	TCORE_SS_CLATCORE_SS_ALL_DEDI_PAD = 0x27,      /* 0x27 : All Dedicated PAD Access */
	TCORE_SS_CLATCORE_SS_ALL_DATA_CDA = 0x28,      /* 0x28 : All Data CDA */

	/* PLMN SPECIFIC TELESERVICE */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_ALL = 0x50,     /* 0x50 : PLMN specific teleservices */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_1 = 0x51,       /* 0x51 : PLMN specific teleservice 1 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_2 = 0x52,       /* 0x52 : PLMN specific teleservice 2 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_3 = 0x53,       /* 0x53 : PLMN specific teleservice 3 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_4 = 0x54,       /* 0x54 : PLMN specific teleservice 4 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_5 = 0x55,       /* 0x55 : PLMN specific teleservice 5 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_6 = 0x56,       /* 0x56 : PLMN specific teleservice 6 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_7 = 0x57,       /* 0x57 : PLMN specific teleservice 7 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_8 = 0x58,       /* 0x58 : PLMN specific teleservice 8 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_9 = 0x59,       /* 0x59 : PLMN specific teleservice 9 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_A = 0x60,       /* 0x60 : PLMN specific teleservice 10 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_B = 0x61,       /* 0x61 : PLMN specific teleservice 11 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_C = 0x62,       /* 0x62 : PLMN specific teleservice 12 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_D = 0x63,       /* 0x63 : PLMN specific teleservice 13 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_E = 0x64,       /* 0x64 : PLMN specific teleservice 14 */
	TCORE_SS_CLATCORE_SS_PLMN_TELE_F = 0x65,       /* 0x65 : PLMN specific teleservice 15 */

	/* PLMN SPECIFIC BEARER SERVICE */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_ALL = 0x70,     /* 0x70 : All PLMN specific bearer services */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_1 = 0x71,       /* 0x71 : PLMN specific bearer service 1 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_2 = 0x72,       /* 0x72 : PLMN specific bearer service 2 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_3 = 0x73,       /* 0x73 : PLMN specific bearer service 3 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_4 = 0x74,       /* 0x74 : PLMN specific bearer service 4 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_5 = 0x75,       /* 0x75 : PLMN specific bearer service 5 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_6 = 0x76,       /* 0x76 : PLMN specific bearer service 6 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_7 = 0x77,       /* 0x77 : PLMN specific bearer service 7 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_8 = 0x78,       /* 0x78 : PLMN specific bearer service 8 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_9 = 0x79,       /* 0x79 : PLMN specific bearer service 9 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_A = 0x80,       /* 0x80 : PLMN specific bearer service 10 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_B = 0x81,       /* 0x81 : PLMN specific bearer service 11 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_C = 0x82,       /* 0x82 : PLMN specific bearer service 12 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_D = 0x83,       /* 0x83 : PLMN specific bearer service 13 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_E = 0x84,       /* 0x84 : PLMN specific bearer service 14 */
	TCORE_SS_CLATCORE_SS_PLMN_BEAR_F = 0x85,       /* 0x85 : PLMN specific bearer service 15 */

	/* CPHS - AUXILIARY SERVICE */
	TCORE_SS_CLATCORE_SS_AUX_VOICE = 0x89,         /* 0x89 : All Auxiliary Voice (Auxiliary telephony) */

	TCORE_SS_CLATCORE_SS_ALL_GPRS_BEARER = 0x99,   /* 0x99 : All GPRS bearer services */
	TCORE_SS_CLATCORE_SS_ALL_TELE_BEARER = 0xFF,   /* 0xFF : all tele and bearer services */
};

enum tcore_ss_status {
	TCORE_SS_STATUS_REG = 0x01, /* 0x01 : Registration */
	TCORE_SS_STATUS_DEREG,      /* 0x02 : De-registration( erase ) */
	TCORE_SS_STATUS_ACTIVATE,   /* 0x03 : Activation */
	TCORE_SS_STATUS_DEACTIVATE, /* 0x04 : De-activation */
	TCORE_SS_STATUS_MAX
};

enum tcore_ss_barring_mode {
	TCORE_SS_BARR_MODE_NONE,
	TCORE_SS_BARR_MODE_BAOC,        /* 0x01 : Barring All Outgoing Calls */
	TCORE_SS_BARR_MODE_BOIC,        /* 0x02 : Barring Outgoing International Calls */
	TCORE_SS_BARR_MODE_BOIC_NOT_HC, /* 0x03 : Barring Outgoing International Calls  except to Home Country */
	TCORE_SS_BARR_MODE_BAIC,        /* 0x04 : Barring All Incoming Calls */
	TCORE_SS_BARR_MODE_BIC_ROAM,    /* 0x05 : Barring Incoming Calls when roam,	outside of the Home Country */
	TCORE_SS_BARR_MODE_AB,          /* 0x06 : All Barring Services */
	TCORE_SS_BARR_MODE_AOB,         /* 0x07 : All Outgoing Barring Services */
	TCORE_SS_BARR_MODE_AIB,         /* 0x08 : All Incoming Barring Services */
	TCORE_SS_BARR_MODE_BIC_NOT_SIM, /* 0x09 : Barring Incoming Calls which is not stored in the SIM memory */
	TCORE_SS_BARR_MODE_MAX
};

enum tcore_ss_forwarding_mode {
	TCORE_SS_CF_MODE_CFU = 0x01, /* 0x01 : Call Forwarding Unconditional */
	TCORE_SS_CF_MODE_CFB,        /* 0x02 : Call Forwarding Mobile Busy */
	TCORE_SS_CF_MODE_CFNRy,      /* 0x03 : Call Forwarding No Reply */
	TCORE_SS_CF_MODE_CFNRc,      /* 0x04 : Call Forwarding Not Reachable */
	TCORE_SS_CF_MODE_CF_ALL,     /* 0x05 : All Call Forwarding */
	TCORE_SS_CF_MODE_CFC,        /* 0x06 : All Conditional Call Forwarding */
	TCORE_SS_CF_MODE_MAX         /* 0x07 : Max */
};

enum tcore_ss_ussd_type {
	TCORE_SS_USSD_TYPE_USER_INITIATED = 0x01, /* User Initiated USSD Message */
	TCORE_SS_USSD_TYPE_USER_RES,              /* User Response to Network Initiated Message */
	TCORE_SS_USSD_TYPE_USER_RELEASE,          /* SS Termination by user */
	TCORE_SS_USSD_TYPE_NETWORK_INITIATED,     /* Network Initiated USSD Message */
	TCORE_SS_USSD_TYPE_MAX,
};

enum tcore_ss_ussd_status {
	TCORE_SS_USSD_NO_ACTION_REQUIRE = 0x01, /* 0x01 : no further user action required  information needed after mobile initiated operation) */
	TCORE_SS_USSD_ACTION_REQUIRE,           /* 0x02 : further user action required(network initiated USSD Request, or further information needed after mobile initiated operation) */
	TCORE_SS_USSD_TERMINATED_BY_NET,        /* 0x03 : USSD terminated by network */
	TCORE_SS_USSD_OTHER_CLIENT,             /* 0x04 : other local client has responded */
	TCORE_SS_USSD_NOT_SUPPORT,              /* 0x05 : operation not supported */
	TCORE_SS_USSD_TIME_OUT,                 /* 0x06 : network time out */
	TCORE_SS_USSD_MAX
};

enum tcore_ss_aoc_type {
	TCORE_SS_AOC_TYPE_RESET  = 0x00, /* AoC Reset Message */
	TCORE_SS_AOC_TYPE_ACM    = 0x01, /* Accumulated call meter Message */
	TCORE_SS_AOC_TYPE_CCM    = 0x02, /* Current call meter Message */
	TCORE_SS_AOC_TYPE_MAXACM = 0x04, /* Max Accumulated call meter Message */
	TCORE_SS_AOC_TYPE_PUC    = 0x08, /* Price per unit and currency Message */
	TCORE_SS_AOC_TYPE_MAX    = 0x10
};

typedef struct ussd_session UssdSession;

struct tcore_ss_operations {
	TReturn (*barring_activate)(CoreObject *o, UserRequest *ur);
	TReturn (*barring_deactivate)(CoreObject *o, UserRequest *ur);
	TReturn (*barring_change_password)(CoreObject *o, UserRequest *ur);
	TReturn (*barring_get_status)(CoreObject *o, UserRequest *ur);
	TReturn (*forwarding_activate)(CoreObject *o, UserRequest *ur);
	TReturn (*forwarding_deactivate)(CoreObject *o, UserRequest *ur);
	TReturn (*forwarding_register)(CoreObject *o, UserRequest *ur);
	TReturn (*forwarding_deregister)(CoreObject *o, UserRequest *ur);
	TReturn (*forwarding_get_status)(CoreObject *o, UserRequest *ur);
	TReturn (*waiting_activate)(CoreObject *o, UserRequest *ur);
	TReturn (*waiting_deactivate)(CoreObject *o, UserRequest *ur);
	TReturn (*waiting_get_status)(CoreObject *o, UserRequest *ur);
	TReturn (*cli_activate)(CoreObject *o, UserRequest *ur);
	TReturn (*cli_deactivate)(CoreObject *o, UserRequest *ur);
	TReturn (*cli_get_status)(CoreObject *o, UserRequest *ur);
	TReturn (*send_ussd)(CoreObject *o, UserRequest *ur);
	TReturn (*set_aoc)(CoreObject *o, UserRequest *ur);
	TReturn (*get_aoc)(CoreObject *o, UserRequest *ur);
};


CoreObject* tcore_ss_new(TcorePlugin *p, const char *name, struct tcore_ss_operations *ops);
void        tcore_ss_free(CoreObject *o);

struct ussd_session*	tcore_ss_ussd_create_session( CoreObject *o, enum tcore_ss_ussd_type type, void *data, int data_len );
void					tcore_ss_ussd_destroy_session( struct ussd_session *ussd_s );
struct ussd_session*	tcore_ss_ussd_get_session( CoreObject *o );
enum tcore_ss_ussd_type tcore_ss_ussd_get_session_type( struct ussd_session* ussd_s );
void					tcore_ss_ussd_set_session_type( struct ussd_session* ussd_s, enum tcore_ss_ussd_type type );
int						tcore_ss_ussd_get_session_data( struct ussd_session* ussd_s, void** data );

__END_DECLS

#endif
