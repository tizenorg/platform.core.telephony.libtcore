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

#ifndef __TYPE_SS_H__
#define __TYPE_SS_H__

__BEGIN_DECLS

#include <glib.h>

enum telephony_ss_class {

	SS_CLASS_NONE=0x00,                               /* 0x00 */

	/* TELESERVICE */
	SS_CLASS_ALL_TELE=0x10,                        /* 0x10 : All Teleservices */
	SS_CLASS_VOICE=0x11,                              /* 0x11 : All Voice ( telephony ) */
	SS_CLASS_ALL_DATA_TELE=0x12,             /* 0x12 : All Data Teleservices */
	SS_CLASS_FAX=0x13,                                /* 0x13 : All Fax Service */
	SS_CLASS_SMS=0x16,                                /* 0x16 : SMS service	 */
	SS_CLASS_VGCS=0x17,                              /* 0x17 : Voice Group Call Service */
	SS_CLASS_VBS=0x18,                                /* 0x18 : Voice Broadcast */
	SS_CLASS_ALL_TELE_EXPT_SMS=0x19,    /* 0x19 : All teleservice except SMS */

	/* BEARER SERVICE */
	SS_CLASS_ALL_BEARER=0x20,                  /* 0X20 : all bearer services */
	SS_CLASS_ALL_ASYNC=0x21,                    /* 0x21 : All Async services */
	SS_CLASS_ALL_SYNC=0x22,                      /* 0x22 : All sync services*/
	SS_CLASS_ALL_CS_SYNC=0x24,                /* 0x24 : All Circuit switched sync */
	SS_CLASS_ALL_CS_ASYNC=0x25,              /* 0x25 : All Circuit switched async */
	SS_CLASS_ALL_DEDI_PS=0x26,                /* 0x26 : All Dedicated packet Access */
	SS_CLASS_ALL_DEDI_PAD=0x27,              /* 0x27 : All Dedicated PAD Access */
	SS_CLASS_ALL_DATA_CDA=0x28,		/*0x28 : All Data CDA*/

	/* PLMN SPECIFIC TELESERVICE */
	SS_CLASS_PLMN_TELE_ALL = 0x50,         /*0x50 : PLMN specific teleservices*/
	SS_CLASS_PLMN_TELE_1 = 0x51,              /*0x51 :PLMN specific teleservice 1*/
	SS_CLASS_PLMN_TELE_2 = 0x52,             /*0x52 : PLMN specific teleservice 2*/
	SS_CLASS_PLMN_TELE_3 = 0x53,             /*0x53 : PLMN specific teleservice 3*/
	SS_CLASS_PLMN_TELE_4 = 0x54,             /*0x54 : PLMN specific teleservice 4*/
	SS_CLASS_PLMN_TELE_5 = 0x55,             /*0x55 : PLMN specific teleservice 5*/
	SS_CLASS_PLMN_TELE_6 = 0x56,             /*0x56 : PLMN specific teleservice 6*/
	SS_CLASS_PLMN_TELE_7 = 0x57,             /*0x57 : PLMN specific teleservice 7*/
	SS_CLASS_PLMN_TELE_8 = 0x58,             /*0x58 : PLMN specific teleservice 8*/
	SS_CLASS_PLMN_TELE_9 = 0x59,             /*0x59 : PLMN specific teleservice 9*/
	SS_CLASS_PLMN_TELE_A = 0x60,           /*0x60 :PLMN specific teleservice 10*/
	SS_CLASS_PLMN_TELE_B = 0x61,           /*0x61 :PLMN specific teleservice 11*/
	SS_CLASS_PLMN_TELE_C = 0x62,             /*0x62 : PLMN specific teleservice 12*/
	SS_CLASS_PLMN_TELE_D = 0x63,             /*0x63 : PLMN specific teleservice 13*/
	SS_CLASS_PLMN_TELE_E = 0x64,             /*0x64 : PLMN specific teleservice 14*/
	SS_CLASS_PLMN_TELE_F = 0x65,             /*0x65 : PLMN specific teleservice 15*/

	/* PLMN SPECIFIC BEARER SERVICE */
	SS_CLASS_PLMN_BEAR_ALL = 0x70,         /*0x70 : All PLMN specific bearer services*/
	SS_CLASS_PLMN_BEAR_1 = 0x71,              /*0x71 :PLMN specific bearer service 1*/
	SS_CLASS_PLMN_BEAR_2 = 0x72,             /*0x72 : PLMN specific bearer service  2*/
	SS_CLASS_PLMN_BEAR_3 = 0x73,             /*0x73 : PLMN specific bearer service  3*/
	SS_CLASS_PLMN_BEAR_4 = 0x74,             /*0x74 : PLMN specific bearer service  4*/
	SS_CLASS_PLMN_BEAR_5 = 0x75,             /*0x75 : PLMN specific bearer service  5*/
	SS_CLASS_PLMN_BEAR_6 = 0x76,             /*0x76 : PLMN specific bearer service  6*/
	SS_CLASS_PLMN_BEAR_7 = 0x77,             /*0x77 : PLMN specific bearer service  7*/
	SS_CLASS_PLMN_BEAR_8 = 0x78,             /*0x78 : PLMN specific bearer service  8*/
	SS_CLASS_PLMN_BEAR_9 = 0x79,             /*0x79 : PLMN specific bearer service  9*/
	SS_CLASS_PLMN_BEAR_A = 0x80,            /*0x80 : PLMN specific bearer service  10*/
	SS_CLASS_PLMN_BEAR_B = 0x81,             /*0x81 : PLMN specific bearer service  11*/
	SS_CLASS_PLMN_BEAR_C = 0x82,            /*0x82 : PLMN specific bearer service  12*/
	SS_CLASS_PLMN_BEAR_D = 0x83,            /*0x83 : PLMN specific bearer service  13*/
	SS_CLASS_PLMN_BEAR_E = 0x84,             /*0x84 : PLMN specific bearer service  14*/
	SS_CLASS_PLMN_BEAR_F = 0x85,             /*0x85 : PLMN specific bearer service  15*/

	/* CPHS - AUXILIARY SERVICE */
	SS_CLASS_AUX_VOICE = 0x89,			/* 0x89 : All Auxiliary Voice ( Auxiliary telephony ) */

	SS_CLASS_ALL_GPRS_BEARER=0x99,       /* 0x99 : All GPRS bearer services */
	SS_CLASS_ALL_TELE_BEARER=0xFF,        /* 0xFF : all tele and bearer services */
};

enum telephony_ss_status {
	SS_STATUS_REG=0x01,         /* 0x01 : Registration */
	SS_STATUS_DEREG,              /* 0x02 : De-registration( erase ) */
	SS_STATUS_ACTIVATE,        /* 0x03 : Activation */
	SS_STATUS_DEACTIVATE,    /* 0x04 : De-activation */
	SS_STATUS_MAX
};

enum telephony_ss_barring_mode {
	SS_BARR_MODE_NONE,
	SS_BARR_MODE_BAOC,                /* 0x01 : Barring All Outgoing Calls */
	SS_BARR_MODE_BOIC,                /* 0x02 : Barring Outgoing International Calls */
	SS_BARR_MODE_BOIC_NOT_HC, /* 0x03 : Barring Outgoing International Calls
								 except to Home Country */
	SS_BARR_MODE_BAIC,                /* 0x04 : Barring All Incoming Calls */
	SS_BARR_MODE_BIC_ROAM,       /* 0x05 : Barring Incoming Calls when roam,
									outside of the Home Country */
	SS_BARR_MODE_AB,                   /* 0x06 : All Barring Services */
	SS_BARR_MODE_AOB,                 /* 0x07 : All Outgoing Barring Services */
	SS_BARR_MODE_AIB,                  /* 0x08 : All Incoming Barring Services */
	SS_BARR_MODE_BIC_NOT_SIM, /* 0x09 : Barring Incoming Calls which is
								 not stored in the SIM memory */
	SS_BARR_MODE_MAX
};

enum telephony_ss_forwarding_mode {
	SS_CF_MODE_CFU = 0x01, /* 0x01 : Call Forwarding Unconditional */
	SS_CF_MODE_CFB,        /* 0x02 : Call Forwarding Mobile Busy */
	SS_CF_MODE_CFNRy,      /* 0x03 : Call Forwarding No Reply */
	SS_CF_MODE_CFNRc,      /* 0x04 : Call Forwarding Not Reachable */
	SS_CF_MODE_CF_ALL,     /* 0x05 : All Call Forwarding */
	SS_CF_MODE_CFC,        /* 0x06 : All Conditional Call Forwarding */
	SS_CF_MODE_MAX         /* 0x07 : Max */
};

enum telephony_ss_forwarding_no_reply_time {
	SS_CF_NO_REPLY_5_SEC	= 5,
	SS_CF_NO_REPLY_10_SEC	= 10,
	SS_CF_NO_REPLY_15_SEC	= 15,
	SS_CF_NO_REPLY_20_SEC	= 20,
	SS_CF_NO_REPLY_25_SEC	= 25,
	SS_CF_NO_REPLY_30_SEC	= 30,
};

enum telephony_ss_forwarding_type_of_number {
	SS_CF_TON_UNKNOWN = 0, /**< unknown */
	SS_CF_TON_INTERNATIONAL = 1, /**< international number */
	SS_CF_TON_NATIONAL = 2, /**< national number */
	SS_CF_TON_NETWORK_SPECIFIC = 3, /**< network specific number */
	SS_CF_TON_DEDICATED_ACCESS = 4, /**< subscriber number */
	SS_CF_TON_ALPHA_NUMERIC = 5, /**< alphanumeric, GSM 7-bit default alphabet) */
	SS_CF_TON_ABBREVIATED_NUMBER = 6, /**< abbreviated number */
	SS_CF_TON_RESERVED_FOR_EXT = 7 /**< reserved for extension */
};

enum telephony_ss_forwarding_numbering_plan_identity {
	SS_CF_NPI_UNKNOWN = 0, /**< Unknown */
	SS_CF_NPI_ISDN_TEL = 1, /**< ISDN/Telephone numbering plan */
	SS_CF_NPI_DATA_NUMBERING_PLAN = 3, /**< Data numbering plan */
	SS_CF_NPI_TELEX = 4, /**< Telex numbering plan */
	SS_CF_NPI_SVC_CNTR_SPECIFIC_PLAN = 5, /**< Service Center Specific plan */
	SS_CF_NPI_SVC_CNTR_SPECIFIC_PLAN2 = 6, /**< Service Center Specific plan */
	SS_CF_NPI_NATIONAL = 8, /**< National numbering plan */
	SS_CF_NPI_PRIVATE = 9, /**< Private numbering plan */
	SS_CF_NPI_ERMES_NUMBERING_PLAN = 10, /**< ERMES numbering plan */
	SS_CF_NPI_RESERVED_FOR_EXT = 0xF /**< Reserved for extension */
};



enum telephony_ss_cli_type {
	SS_CLI_TYPE_NONE,
	SS_CLI_TYPE_CLIP,       /* 0x01 : Calling Line Identification Presentation */
	SS_CLI_TYPE_CLIR,       /* 0x02 : Calling Line Identification Restriction */
	SS_CLI_TYPE_COLP,      /* 0x03 : Connected Line Identification Presentation */
	SS_CLI_TYPE_COLR,      /* 0x04 : Connected Line Identification Restriction */
	SS_CLI_TYPE_CDIP,       /* 0x05 : Called Line Identification Presentation */
	SS_CLI_TYPE_CNAP,      /* 0x06 : Calling Name Presentation */
	SS_CLI_TYPE_MAX
};

enum telephony_ss_cli_status{
	SS_CLI_STATUS_NOT_PROVISONED = 0x01, /**<Service not provided by the service provider */
	SS_CLI_STATUS_PROVISIONED, /**<Service is provided by the service provider */
	SS_CLI_STATUS_ACTIVATED, /**<Service is activated at the network */
	SS_CLI_STATUS_UNKOWN, /**<Service status is unknown*/
	SS_CLI_STATUS_TEMP_RESTRICTED, /**<Service is temporarily restricted */
	SS_CLI_STATUS_TEMP_ALLOWED /**<Service is temporarily allowed */
};

enum telephony_ss_ussd_type {
	SS_USSD_TYPE_USER_INITIATED=0x01,   /* User Initiated USSD Message */
	SS_USSD_TYPE_USER_RES,                       /* User Response to Network Initiated Message */
	SS_USSD_TYPE_USER_RELEASE,               /* SS Termination by user */
	SS_USSD_TYPE_MAX,
};

enum telephony_ss_ussd_status {
	SS_USSD_NO_ACTION_REQUIRE = 0x01,  /* 0x01 : no further user action required
											  information needed after mobile initiated operation) */
	SS_USSD_ACTION_REQUIRE,                     /* 0x02 : further user action required
													   (network initiated USSD Request, or further
													   information needed after mobile initiated operation) */
	SS_USSD_TERMINATED_BY_NET,              /* 0x03 : USSD terminated by network */
	SS_USSD_OTHER_CLIENT,                         /* 0x04 : other local client has responded */
	SS_USSD_NOT_SUPPORT,                          /* 0x05 : operation not supported */
	SS_USSD_TIME_OUT,                                 /* 0x06 : network time out */
	SS_USSD_MAX
};

enum telephony_ss_info_type {
	SS_INFO_TYPE_BARRING = 0x00,
	SS_INFO_TYPE_FORWARDING,
	SS_INFO_TYPE_WAITING,
	SS_INFO_TYPE_CLI,
	SS_INFO_TYPE_SEND_USSD,
	SS_INFO_TYPE_MAX
};

enum telephony_ss_aoc_type {
	SS_AOC_TYPE_RESET		=0x00,		/* AoC Reset Message */
	SS_AOC_TYPE_ACM			=0x01,		/* Accumulated call meter Message */
	SS_AOC_TYPE_CCM			=0x02,		/* Current call meter Message */
	SS_AOC_TYPE_MAXACM	=0x04,		/* Max Accumulated call meter Message */
	SS_AOC_TYPE_PUC			=0x08,		/* Price per unit and currency Message */
	SS_AOC_TYPE_MAX		=0x10
};

enum telephony_ss_error {
	SS_ERROR_NONE =0x0, /**<  SS operation was successful */

	SS_ERROR_UNKNOWNSUBSCRIBER =0x01, /**< SS error indicating unknown/illegal subscriber.  */
	SS_ERROR_ILLEGALSUBSCRIBER= 0x09, /**<This error is returned when illegality of the access has been @n
	 established by use of authentication procedure. */
	SS_ERROR_BEARERSERVICENOTPROVISIONED = 0x0a, /**<The network returns this error when it is requested to  @n
	 perform an operation on a supplementary service  */
	SS_ERROR_TELESERVICENOTPROVISIONED= 0x0b, /**<The network returns this error when it is requested to perform  @n
	 an operation on a supplementary service  */
	SS_ERROR_ILLEGALEQUIPMENT= 0x0c, /**<This error is returned when the IMEI check procedure has shown that  @n
	 the IMEI is blacklisted or not whitelisted  */
	SS_ERROR_CALLBARRED= 0x0d, /**< This error is returned by the network to the MS when call independent subscriber control procedures are barred by the operator */

	SS_ERROR_ILLEGALSSOPERATION= 0x10, /**<This error is returned by the network when it is requested to perform an illegal operation @n
	 which is defined as not applicable for the relevant supplementary service */
	SS_ERROR_ERRORSTATUS= 0x11, /**<This error is returned by the network when it is requested to perform an operation @n
	 which is not compatible with the current status of the relevant supplementary service. */
	SS_ERROR_NOTAVAILABLE= 0x12, /**< SS not available in network */
	SS_ERROR_SUBSCRIPTIONVIOLATION= 0x13, /**< SS service subscription violation. */
	SS_ERROR_INCOMPATIBILITY= 0x14, /**< This error is returned by the network when it is requested for a supplementary service operation incompatible with the @n
	 status of another supplementary service or with the teleservice or bearer service for which the operation is requested */
	SS_ERROR_FACILITYNOTSUPPORTED= 0x15, /**< SS service facility not supported  */
	SS_ERROR_ABSENTSUBSCRIBER= 0x1b, /**< This error is returned when the subscriber has activated the detach service or the system detects the absence condition */

	SS_ERROR_SYSTEMFAILURE= 0x22, /**< This error is returned by the network, when it cannot perform an operation because of a failure in the network */
	SS_ERROR_DATAMISSING= 0x23, /**< This error is returned by the network when an optional parameter is missing in an invoke component @n
	 or an inner data structure, while it is required by the context of the request. */
	SS_ERROR_UNEXPECTEDDATAVALUE= 0x24, /**< SS error indicating unexpected data value on network side *//**< SS operation barred.  */
	SS_ERROR_PWREGISTRATIONFAILURE= 0x25, /**< SS error indicating change password failure. */
	SS_ERROR_NEGATIVEPWCHECK= 0x26, /**< SS error indicating negative password check.  */
	SS_ERROR_NUMBEROFPWATTEMPTSVIOLATION= 0x2b, /**< SS error indicating barring password attempts violated.  */

	SS_ERROR_UNKNOWNALPHABET= 0x47, /**< SS error indicating unknown SS data coding of alphabet */
	SS_ERROR_USSDBUSY= 0x48, /**< SS error indicating USSD Busy(Already SS / USSD is ongoing).  */

	SS_ERROR_FIXED_DIALING_NUMBER_ONLY = 0x5F, /**< SS error indicating Dialing number is not FDN */

	SS_ERROR_REJECTEDBYUSER= 0x79, /**< SS operation rejected by user.  */
	SS_ERROR_REJECTEDBYNETWORK= 0x7a, /**< SS operation rejected by network.  */
	SS_ERROR_DEFLECTIONTOSERVEDSUBSCRIBER= 0x7b, /**< This error is returned if a diversion to the served  @n
	 subscriber's number was requested.  */
	SS_ERROR_SPECIALSERVICECODE= 0x7c, /**< This error is returned if diversion to a special service code was requested.  */
	SS_ERROR_INVALIDDEFLECTEDTONUMBER= 0x7d, /**< SS error indicating the invalid deflected to number.  */
	SS_ERROR_MAXNOMPTYEXCEEDED= 0x7e, /**< SS error indicating Maximum MPTY is reached.  */
	SS_ERROR_RESOURCESNOTAVAILABLE= 0x7f, /**< SS error indicating resources not available in network.  */
	SS_ERROR_REJECTEDBYCALLCONTROL= 0x80, /**< SS operation rejected by call control.  */

	SS_ERROR_TIMEREXPIRE, /**< SS operation timer expired on network. */

	SS_ERROR_NET_NOT_ALLOWED_EMERGENCY_CALLS_ONLY, /**< SS operation is not allowed by network.  */
	SS_ERROR_OEM_NOT_SUPPORTED, /**< If oem do not support any of SS requests, then this error will be returned back */
	SS_ERROR_UNKNOWNERROR /**< SS error indicating unknown error  */
};


#define MAX_SS_BARRING_PASSWORD_LEN 4
struct treq_ss_barring {
	enum telephony_ss_class class;
	enum telephony_ss_barring_mode mode;
	char password[ MAX_SS_BARRING_PASSWORD_LEN + 1 ];
};

struct treq_ss_barring_change_password {
	char password_old[ MAX_SS_BARRING_PASSWORD_LEN + 1 ];
	char password_new[ MAX_SS_BARRING_PASSWORD_LEN + 1 ];
	char password_confirm[ MAX_SS_BARRING_PASSWORD_LEN + 1 ];
};

#define MAX_SS_FORWARDING_NUMBER_LEN 32
struct treq_ss_forwarding {
	enum telephony_ss_class class;
	enum telephony_ss_forwarding_mode mode;
	enum telephony_ss_forwarding_no_reply_time time;
	enum telephony_ss_forwarding_type_of_number ton;
	enum telephony_ss_forwarding_numbering_plan_identity npi;
	char number[ MAX_SS_FORWARDING_NUMBER_LEN ];
};

struct treq_ss_waiting {
	enum telephony_ss_class class;

};

struct treq_ss_cli {
	enum telephony_ss_cli_type type;
};

struct treq_ss_set_cli {
	enum telephony_ss_cli_type type;
	enum telephony_ss_cli_status status;
};

#define MAX_SS_USSD_LEN 208
struct treq_ss_ussd {
	enum telephony_ss_ussd_type type;
	unsigned char dcs;
	unsigned short len;
	unsigned char str[ MAX_SS_USSD_LEN ];
};

// response

struct tresp_ss_general {
	enum telephony_ss_error err;
};

struct tresp_ss_barring {
	int record_num;
	struct barring_info {
		enum telephony_ss_class class;
		enum telephony_ss_status status;
		enum telephony_ss_barring_mode mode;
	} *record;
	enum telephony_ss_error err;
};

struct tresp_ss_forwarding {
	int record_num;
	struct forwarding_info {
		enum telephony_ss_class class;
		enum telephony_ss_status status;
		enum telephony_ss_forwarding_mode mode;
		enum telephony_ss_forwarding_no_reply_time time;
		enum telephony_ss_forwarding_type_of_number ton;
		enum telephony_ss_forwarding_numbering_plan_identity npi;
		gboolean number_present;
		char	 number[ MAX_SS_FORWARDING_NUMBER_LEN ];
	} *record;
	enum telephony_ss_error err;
};

struct tresp_ss_waiting {
	int record_num;
	struct waiting_info {
		enum telephony_ss_class class;
		enum telephony_ss_status status;
	} *record;
	enum telephony_ss_error err;
};

struct tresp_ss_set_cli {
	enum telephony_ss_error err;
};

struct tresp_ss_cli {
	enum telephony_ss_cli_type type;
	enum telephony_ss_cli_status status;
	enum telephony_ss_error err;
};

struct tresp_ss_ussd {
	enum telephony_ss_ussd_type type;
	enum telephony_ss_ussd_status status;
	unsigned char dcs;
	unsigned short len;
	unsigned char str[ MAX_SS_USSD_LEN ];
	enum telephony_ss_error err;
};

// notification

struct tnoti_ss_ussd {
	enum telephony_ss_ussd_status status;
	unsigned char dcs;
	unsigned short len;
	unsigned char str[ MAX_SS_USSD_LEN ];
};

struct tnoti_ss_barring_status {
	int record_num;
	struct barring_info *record;
};

struct tnoti_ss_forwarding_status {
	int record_num;
	struct forwarding_info *record;
};

struct tnoti_ss_waiting_status {
	int record_num;
	struct waiting_info *record;
};

#define MAX_SS_RELEASE_COMPLETE_DATA_SIZE 260
struct tnoti_ss_release_complete {
	int data_len;
	unsigned char data[ MAX_SS_RELEASE_COMPLETE_DATA_SIZE ];
};

struct tnoti_ss_information {
	enum telephony_ss_error err;
	enum telephony_ss_info_type ss_type;
#if 0 /* To be expanded later with ss information */
	void *data;                                                       /* ( struct barring_info *,   struct forwarding_info *, struct waiting_info * ...etc...) */
#endif
};


__END_DECLS

#endif
