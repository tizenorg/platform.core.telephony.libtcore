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

#ifndef __CO_SIM_H__
#define __CO_SIM_H__

#include "core_object.h"
#include <tel_sim.h>
#include <tel_return.h>

#ifdef __cplusplus
	extern "C" {
#endif

#define TEL_SIM_LANG_CNT_MAX 30 /**< Max language count */
#define TEL_SIM_XDN_ALPHA_ID_LEN_MAX 30 /**< Max alpha id length */
#define TEL_SIM_XDN_NUMBER_LEN_MAX 20 /**< Max XDN number length */
#define TEL_SIM_SST_LEN_MAX 20 /**< Telephony defined */
#define TEL_SIM_SST_SERVICE_CNT_MAX 56 /**< Max SST service count */
#define TEL_SIM_UST_SERVICE_CNT_MAX 64 /**< Max UST service count */
#define TEL_SIM_SPDI_PLMN_MAX 41 /**< Max SPDI PLMN */
#define TEL_SIM_UST_BYTE_LEN_MAX 8 /**< Max UST byte length */
#define TEL_SIM_ECC_BYTE_LEN_MAX 3 /**< Max ECC byte length */
#define TEL_SIM_MAIL_BOX_IDENTIFIER_LEN_MAX 5 /**< Max mail box identifier length */
#define TEL_SIM_CPHS_VMWF_LEN_MAX 2 /**< Max CPHS VMWF length */
#define TEL_SIM_CPHS_OPERATOR_NAME_LEN_MAX 25 /**< Max CPHS operator name length */
#define TEL_SIM_CPHS_OPERATOR_NAME_SHORT_FORM_LEN_MAX 10 /**< Max CPHS operator name short form length */

typedef enum {
	// service 1 ~ 8
	TEL_SIM_SST_CHV1_DISABLE_FUNC = 0,	/**< CHV1 disable function */
	TEL_SIM_SST_ADN,					/**< abbreviated Dialing number */
	TEL_SIM_SST_FDN,					/**< fixed Dialing number */
	TEL_SIM_SST_SMS,					/**< short message storage */
	TEL_SIM_SST_AOC,					/**< advice of charge */
	TEL_SIM_SST_CCP,					/**< capability configuration parameters */
	TEL_SIM_SST_PLMN_SELECTOR,			/**< plmn selector */
	TEL_SIM_SST_RFU1,					/**< rfu */

	// service 9 ~ 16
	TEL_SIM_SST_MSISDN = 8,				/**< msisdn */
	TEL_SIM_SST_EXT1,					/**< extension1	*/
	TEL_SIM_SST_EXT2,					/**< extension2 */
	TEL_SIM_SST_SMS_PARAMS,				/**< sms parameteres */
	TEL_SIM_SST_LND,					/**< last number dialed */
	TEL_SIM_SST_CELL_BROADCAST_MSG_ID,	/**< cell broadcast message identifier */
	TEL_SIM_SST_GID_LV1,				/**< group identifier level 1 */
	TEL_SIM_SST_GID_LV2,				/**< group identifier level 2 */

	// service 17 ~ 24
	TEL_SIM_SST_SPN = 16,				/**< service provider name */
	TEL_SIM_SST_SDN,					/**< service Dialing number */
	TEL_SIM_SST_EXT3,					/**< extension3 */
	TEL_SIM_SST_RFU2,					/**< rfu */
	TEL_SIM_SST_VGCS_GID_LIST,			/**< vgcs group identifier (EF-VGCS, EF-VGCSS) */
	TEL_SIM_SST_VBS_GID_LIST,			/**< vbs group identifier (EF-VBS, EF-VBSS) */
	TEL_SIM_SST_ENHANCED_MULTI_LV_PRECEDENCE_PREEMPTION_SRVC,	/**< enhanced multi-level precedence and pre-emption service */
	TEL_SIM_SST_AUTO_ANSWER_FOR_EMLPP,	/**< automatic answer fro eMLPP */

	// service 25 ~ 32,
	TEL_SIM_SST_DATA_DOWNLOAD_VIA_SMS_CB = 24,	/**< data download via sms-cb */
	TEL_SIM_SST_DATA_DOWNLOAD_VIA_SMS_PP,		/**< data download via sms-pp */
	TEL_SIM_SST_MENU_SELECTION,					/**< menu selection */
	TEL_SIM_SST_CALL_CTRL,						/**< call control */
	TEL_SIM_SST_PROACTIVE_SIM,					/**< proactive sim command */
	TEL_SIM_SST_CELL_BROADCAST_MSG_ID_RANGES,	/**< cell broadcast message identifier ranges */
	TEL_SIM_SST_BDN,							/**< barred Dialing numbers */
	TEL_SIM_SST_EXT4,							/**< extension 4 */

	// service 33 ~ 40
	TEL_SIM_SST_DEPERSONALIZATION_CTRL_KEYS = 32,	/**< de-personalization control keys */
	TEL_SIM_SST_COOPERATIVE_NETWORK_LIST,			/**< co-operative network list */
	TEL_SIM_SST_SMS_STATUS_REPORTS,					/**< short message status reports */
	TEL_SIM_SST_NIA,					/**< network's indication of alerting in the MS (NIA) */
	TEL_SIM_SST_MO_SMS_CTRL_BY_SIM,		/**< mobile-originated short message control by sim */
	TEL_SIM_SST_GPRS,					/**< gprs */
	TEL_SIM_SST_IMG,					/**< image */
	TEL_SIM_SST_SOLSA,					/**< support of local service area */

	// service 41 ~ 48
	TEL_SIM_SST_USSD_STR_DATA_OBJECT_SUPPORTED_IN_CALL_CTRL = 40,	/**< ussd string data object supported in call control */
	TEL_SIM_SST_RUN_AT_CMD_CMD,							/**< RUN AT COMMAND command */
	TEL_SIM_SST_USER_CTRLED_PLMN_SELECTOR_WACT,			/**< user controlled PLMN selector with Access technology */
	TEL_SIM_SST_OPERATOR_CTRLED_PLMN_SELECTOR_WACT,		/**< operator controlled PLMN selector with Access technology */
	TEL_SIM_SST_HPLMN_SELECTOR_WACT,			/**< HPLMN selector with access technology */
	TEL_SIM_SST_CPBCCH_INFO,					/**< CPBCCH information */
	TEL_SIM_SST_INVESTIGATION_SCAN,				/**< investigation scan */
	TEL_SIM_SST_EXTENDED_CAPA_CONF_PARAMS,		/**< extended capability configuration parameters */

	//	service 49 ~ 56
	TEL_SIM_SST_MEXE = 48,				/**< MExE */
	TEL_SIM_SST_RPLMN_LAST_USED_ACCESS_TECH,	/**< RPLMN last used access technology */
	TEL_SIM_SST_PLMN_NETWORK_NAME,		/*PLMN Network Name*/
	TEL_SIM_SST_OPERATOR_PLMN_LIST,		/*Operator PLMN List*/
	TEL_SIM_SST_MBDN,					/*Mailbox Dialling Numbers*/
	TEL_SIM_SST_MWIS,					/*Message Waiting Indication Status*/
	TEL_SIM_SST_CFIS,					/*Call Forwarding Indication Status*/
	TEL_SIM_SST_SPDI					/*Service Provider Display Information*/
} TelSimSstService;

typedef enum {
	// service 1 ~ 8
	TEL_SIM_UST_LOCAL_PB = 0,		/**< local phone book */
	TEL_SIM_UST_FDN,				/**< fixed Dialing number */
	TEL_SIM_UST_EXT2,				/**< extension2 */
	TEL_SIM_UST_SDN,				/**< service Dialing number */
	TEL_SIM_UST_EXT3,				/**< extension3 */
	TEL_SIM_UST_BDN,				/**< barred Dialing numbers */
	TEL_SIM_UST_EXT4,				/**< extension 4 */
	TEL_SIM_UST_OUTGOING_CALL_INFO,	/**< outgoing call information */

	// service 9 ~ 16
	TEL_SIM_UST_INCOMING_CALL_INFO = 8,		/**< incoming call information */
	TEL_SIM_UST_SMS,						/**< short message storage */
	TEL_SIM_UST_SMS_STATUS_REPORTS,			/**< short message status reports */
	TEL_SIM_UST_SMS_PARAMS,					/**< sms parameteres */
	TEL_SIM_UST_AOC,						/**< advice of charge */
	TEL_SIM_UST_CCP,						/**< capability configuration parameters */
	TEL_SIM_UST_CELL_BROADCAST_MSG_ID,		/**< cell broadcast message identifier */
	TEL_SIM_UST_CELL_BROADCAST_MSG_ID_RANGES,	/**< cell broadcast message identifier ranges */

	// service 17 ~ 24
	TEL_SIM_UST_GID_LV1 = 16,						/**< group identifier level 1 */
	TEL_SIM_UST_GID_LV2,							/**< group identifier level 2 */
	TEL_SIM_UST_SPN,								/**< service provider name */
	TEL_SIM_UST_USER_CTRLED_PLMN_SELECTOR_WACT,		/**< user controlled PLMN selector with Access technology */
	TEL_SIM_UST_MSISDN,											/**< msisdn */
	TEL_SIM_UST_IMG,											/**< image */
	TEL_SIM_UST_SOLSA,											/**< support of local service area */
	TEL_SIM_UST_ENHANCED_MULTI_LV_PRECEDENCE_PREEMPTION_SRVC,	/**< enhanced multi-level precedence and pre-emption service */

	// service 25 ~ 32
	TEL_SIM_UST_AUTO_ANSWER_FOR_EMLPP = 24,	/**< automatic answer fro eMLPP */
	TEL_SIM_UST_RFU1,						/**< rfu */
	TEL_SIM_UST_GSM_ACCESS,					/**< gsm access */
	TEL_SIM_UST_DATA_DOWNLOAD_VIA_SMS_PP,	/**< data download via sms-pp */
	TEL_SIM_UST_DATA_DOWNLOAD_VIA_SMS_CB,	/**< data download via sms-cb */
	TEL_SIM_UST_CALL_CTRL,					/**< call control by usim*/
	TEL_SIM_UST_MO_SMS_CTRL,				/**< mobile-originated short message control by usim */
	TEL_SIM_UST_RUN_AT_CMD_CMD,				/**< RUN AT COMMAND command */

	// service 33 ~ 40
	TEL_SIM_UST_SHALL_BE_SET_TO_ONE = 32,	/**< shall be set to 1 */
	TEL_SIM_UST_ENABLED_SRVC_TABLE,			/**< enabled service table */
	TEL_SIM_UST_ACL,						/**< APN control list */
	TEL_SIM_UST_DEPERSONALIZATION_CTRL_KEYS,	/**< de-personalization control keys */
	TEL_SIM_UST_COOPERATIVE_NETWORK_LIST,		/**< co-operative network list */
	TEL_SIM_UST_GSM_SEC_CONTEXT,				/**< gsm security context */
	TEL_SIM_UST_CPBCCH_INFO,					/**< CPBCCH information */
	TEL_SIM_UST_INVESTIGATION_SCAN,				/**< investigation scan */

	// service 41 ~ 48
	TEL_SIM_UST_MEXE = 40,								/**< MExE */
	TEL_SIM_UST_OPERATOR_CTRLED_PLMN_SELECTOR_WACT,		/**< operator controlled PLMN selector with Access technology */
	TEL_SIM_UST_HPLMN_SELECTOR_WACT,	/**< HPLMN selector with access technology */
	TEL_SIM_UST_EXT5,					/**< extension 5 */
	TEL_SIM_UST_PLMN_NETWORK_NAME,		/*PLMN Network Name*/
	TEL_SIM_UST_OPERATOR_PLMN_LIST,		/*Operator PLMN List*/
	TEL_SIM_UST_MBDN,					/*Mailbox Dialling Numbers*/
	TEL_SIM_UST_MWIS,					/*Message Waiting Indication Status*/

	// service 49 ~ 56
	TEL_SIM_UST_CFIS = 48,						/*Call Forwarding Indication Status*/
	TEL_SIM_UST_RPLMN_LAST_USED_ACCESS_TECH,	/**< RPLMN last used access technology */
	TEL_SIM_UST_SPDI,							/*Service Provider Display Information*/
	TEL_SIM_UST_MMS,							/**< multi media messaging service */
	TEL_SIM_UST_EXT8,							/**< extension 8 */
	TEL_SIM_UST_CALL_CTRL_ON_GPRS,				/**< call control on gprs by usim */
	TEL_SIM_UST_MMS_USER_CONNECTIVITY_PARAMS,	/**< mms user connectivity parameters */
	TEL_SIM_UST_NIA,							/**< network's indication of alerting in the MS (NIA) */

	// service 57 ~ 64
	TEL_SIM_UST_VGCS_GID_LIST,			/**< vgcs group identifier List (EF-VGCS, EF-VGCSS) */
	TEL_SIM_UST_VBS_GID_LIST,			/**< vbs group identifier List (EF-VBS, EF-VBSS) */
	TEL_SIM_UST_PSEUDONYM,
	TEL_SIM_UST_USER_CTRLED_PLMN_SELECTOR_IWLAN,		/**< user controlled PLMN selector for I-WLAN access */
	TEL_SIM_UST_OPERATOR_CTRLED_PLMN_SELECTOR_IWLAN,		/**< operator controlled PLMN selector for I-WLAN access */
	TEL_SIM_UST_USER_CTRLED_WSID_LIST,
	TEL_SIM_UST_OPERATOR_CTRLED_WSID_LIST,
	TEL_SIM_UST_VGCS_SEC
} TelSimUstService;

typedef enum {
	TEL_SIM_EF_DIR = 0x2F00, /**< Root Directory for the USIM */
	TEL_SIM_EF_ICCID = 0x2FE2, /**< the ICC Identification file	*/
	TEL_SIM_EF_IMSI = 0x6F07, /**< the IMSI file */
	TEL_SIM_EF_SST = 0x6F38, /**< the SIM Service Table file. EF UST has same address */
	TEL_SIM_EF_EST = 0x6F56, /**< the Enabled Service Table file   */
	TEL_SIM_EF_OPLMN_ACT = 0x6F61, /**< the OPLMN List file*/
	TEL_SIM_EF_GID1 = 0x6F3E, /**< the Group Identifier Level 1 */
	TEL_SIM_EF_GID2 = 0x6F3F, /**< the Group Identifier Level 2 */

	TEL_SIM_EF_ELP = 0x2F05, /**< the Extended Language Preference file */
	TEL_SIM_EF_LP = 0x6F05, /**< SIM: Language preference */
	TEL_SIM_EF_ECC = 0x6FB7, /**< the Emergency Call Codes     */
	TEL_SIM_EF_SPN = 0x6F46, /**< the Service Provider Name    */
	TEL_SIM_EF_SPDI = 0x6FCD, /**< the Service provider display information*/
	TEL_SIM_EF_PNN = 0x6FC5, /**< the PLMN Network Name File*/
	TEL_SIM_EF_OPL = 0x6FC6, /**< the Operator PLMN List File*/
	TEL_SIM_EF_MBDN = 0x6FC7, /** < SIM Mail Box Dialing Number file */
	TEL_SIM_EF_MSISDN = 0x6F40, /**< MSISDN */
	TEL_SIM_EF_EXT1 = 0x6F4A, /**< Extension File-1 */
	TEL_SIM_EF_EXT5 = 0x6F4E, /**< Extension File-5 */
	TEL_SIM_EF_IMG = 0x4F20, /**Image file*/

	TEL_SIM_EF_USIM_LI = 0x6A05, /**< USIM: Language Indication */
	TEL_SIM_EF_USIM_PL = 0x2A05, /**< the Language Preference file of USIM */
	TEL_SIM_EF_USIM_MBI = 0x6FC9, /** < Mailbox Identifier -linear fixed*/
	TEL_SIM_EF_USIM_MWIS = 0x6FCA, /** < Message Waiting Indication Status -linear fixed*/
	TEL_SIM_EF_USIM_CFIS = 0x6FCB, /** < Call forward indication status -linear fixed*/

	/* CPHS FILE ID */
	TEL_SIM_EF_CPHS_VOICE_MSG_WAITING = 0x6F11, /** < CPHS voice MSG waiting indication  */
	TEL_SIM_EF_CPHS_SERVICE_STRING_TABLE = 0x6F12, /** < CPHS service string table  */
	TEL_SIM_EF_CPHS_CALL_FORWARD_FLAGS = 0x6F13, /** < CPHS call forward flags  */
	TEL_SIM_EF_CPHS_OPERATOR_NAME_STRING = 0x6F14, /** < CPHS operator name string  */
	TEL_SIM_EF_CPHS_CUSTOMER_SERVICE_PROFILE = 0x6F15, /** < CPHS customer service profile  */
	TEL_SIM_EF_CPHS_CPHS_INFO = 0x6F16, /** < CPHS information  */
	TEL_SIM_EF_CPHS_MAILBOX_NUMBERS = 0x6F17, /** < CPHS mail box numbers  */
	TEL_SIM_EF_CPHS_OPERATOR_NAME_SHORT_FORM_STRING = 0x6F18, /** < CPHS operator name short form string  */
	TEL_SIM_EF_CPHS_INFORMATION_NUMBERS = 0x6F19, /** < CPHS information numbers  */
	/*  CPHS ALS FILE ID */
	TEL_SIM_EF_CPHS_DYNAMICFLAGS = 0x6F9F, /** < CPHS Dynamics flags  */
	TEL_SIM_EF_CPHS_DYNAMIC2FLAG = 0x6F92, /** < CPHS Dynamics2 flags  */
	TEL_SIM_EF_CPHS_CUSTOMER_SERVICE_PROFILE_LINE2 = 0x6F98, /** < CPHS CSP2  */
	TEL_SIM_EF_PBR = 0x4F30,

	/*  SMS FILE ID */
	TEL_SIM_EF_SMSP = 0x6F42, /** < SMSP File  */

	/* Invalid File ID, All the file ID are less than this Value*/
	TEL_SIM_EF_INVALID = 0xFFFF, /**< Invalid file.*/
	TEL_SIM_EF_OTHERS, /**< Element to indicate an unknown file.*/
} TelSimFileId;

typedef struct {
	TelSimCardType sim_type;
	union {
		char sst_service[TEL_SIM_SST_SERVICE_CNT_MAX];	// should accessed with 'enum tel_sim_sst_service' as index
		char ust_service[TEL_SIM_UST_SERVICE_CNT_MAX];	// should accessed with 'enum tel_sim_ust_service' as index
	} table;
} TelSimServiceTable;

/* OEM operations */
typedef struct {
	TelReturn (*get_imsi)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_ecc)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_iccid)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_language)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_language)(CoreObject *co, TelSimLanguagePreferenceCode language, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_callforwarding_info)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_messagewaiting_info)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_messagewaiting_info)(CoreObject *co, const TelSimMwis *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_mailbox_info)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*set_mailbox_info)(CoreObject *co, const TelSimMailBoxNumber *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_msisdn)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_spn)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_cphs_netname)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_sp_display_info)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*req_authentication)(CoreObject *co, const TelSimAuthenticationData *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*verify_pins)(CoreObject *co, const TelSimSecPinPw *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*verify_puks)(CoreObject *co, const TelSimSecPukPw *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*change_pins)(CoreObject *co, const TelSimSecChangePinPw *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*disable_facility)(CoreObject *co, const TelSimFacilityPw *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*enable_facility)(CoreObject *co, const TelSimFacilityPw *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_facility)(CoreObject *co, TelSimLockType lock_type, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*get_lock_info)(CoreObject *co, TelSimLockType lock_type, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*req_apdu)(CoreObject *co, const TelSimApdu *request, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*req_atr)(CoreObject *co, TcoreObjectResponseCallback cb, void *cb_data);
} TcoreSimOps;

CoreObject *tcore_sim_new(TcorePlugin *plugin, TcoreSimOps *sim_ops, TcoreHal *hal);
void tcore_sim_free(CoreObject *co);

gboolean tcore_sim_set_ops(CoreObject *co, TcoreSimOps *ops);
void tcore_sim_override_ops(CoreObject *co,
			TcoreSimOps *sim_ops);

gboolean tcore_sim_get_type(CoreObject *co, TelSimCardType *type);
gboolean tcore_sim_set_type(CoreObject *co, TelSimCardType  type);

gboolean tcore_sim_get_imsi(CoreObject *co, TelSimImsiInfo **imsi);
gboolean tcore_sim_set_imsi(CoreObject *co, TelSimImsiInfo *imsi);

gboolean tcore_sim_get_status(CoreObject *co, TelSimCardStatus *status);
gboolean tcore_sim_set_status(CoreObject *co, TelSimCardStatus status);

gboolean tcore_sim_get_identification(CoreObject *co, TelSimCardChangeStatus *change_status);
gboolean tcore_sim_set_identification(CoreObject *co, TelSimCardChangeStatus change_status);

gboolean tcore_sim_get_service_table(CoreObject *co, TelSimServiceTable **svct);
gboolean tcore_sim_set_service_table(CoreObject *co, TelSimServiceTable *svct);

gboolean tcore_sim_get_cphs_status(CoreObject *co, gboolean *is_supported);
gboolean tcore_sim_set_cphs_status(CoreObject *co, gboolean is_support);

gboolean tcore_sim_get_spn(CoreObject *co, gchar **spn);
gboolean tcore_sim_set_spn(CoreObject *co, gchar *spn);
gboolean tcore_sim_get_disp_condition(CoreObject *co, TelSimSpnDispCondition *disp_condition);
gboolean tcore_sim_set_disp_condition(CoreObject *co, TelSimSpnDispCondition disp_condition);

gboolean tcore_sim_link_userdata(CoreObject *co, void *userdata);
void *tcore_sim_ref_userdata(CoreObject *co);

gboolean tcore_sim_decode_iccid(unsigned char *enc_iccid, int enc_iccid_len, char *dec_iccid);
gboolean tcore_sim_decode_lp(unsigned char *enc_lang, int enc_lang_len, TelSimLanguagePreferenceCode *dec_lang);
gboolean tcore_sim_encode_lp(TelSimLanguagePreferenceCode dec_lang, char **enc_lang, int *enc_lang_len);
gboolean tcore_sim_decode_li(unsigned char *enc_lang, int enc_lang_len, TelSimFileId file_id, TelSimLanguagePreferenceCode *dec_lang);
gboolean tcore_sim_encode_li(TelSimLanguagePreferenceCode dec_lang, char **en_lang, int *en_lang_len);
gboolean tcore_sim_decode_imsi(unsigned char *en_imsi, int en_imsi_len, TelSimImsiInfo *dec_imsi);
gboolean tcore_sim_decode_sst(unsigned char *enc_sst, int enc_sst_len, char *dec_sst);
gboolean tcore_sim_decode_spn(unsigned char *enc_spn, int enc_spn_len, TelSimSpn *dec_spn);
gboolean tcore_sim_decode_spdi(unsigned char *enc_spdi, int enc_spdi_len, TelSimSpPlmnList *dec_spdi);
gboolean tcore_sim_decode_msisdn(unsigned char *enc_msisdn, int enc_msisdn_len, TelSimSubscriberInfo *dec_msisdn);
gboolean tcore_sim_decode_xdn(unsigned char *enc_xdn, int enc_xdn_len,
	char *alpha_id, unsigned int *alpha_id_len, char *num);
gboolean tcore_sim_encode_xdn(char *alpha_id, char *num, char *enc_xdn, int enc_xdn_len);
gboolean tcore_sim_decode_ecc(unsigned char *enc_ecc, int enc_ecc_len, TelSimEccList *dec_ecc);
gboolean tcore_sim_decode_ust(unsigned char *enc_ust, int enc_ust_len, char *dec_ust);
gboolean tcore_sim_decode_uecc(unsigned char* enc_uecc, int enc_uecc_len, TelSimEcc *dec_ecc);
gboolean tcore_sim_decode_mbi(unsigned char *enc_mbi, int enc_mbi_len, TelSimMbi *dec_mbi);
gboolean tcore_sim_decode_cff(unsigned char *enc_cff, int enc_cff_len, TelSimMwis *dec_mwis);
gboolean tcore_sim_decode_mwis(unsigned char *enc_mwis, int enc_mwis_len, TelSimMwis *dec_mwis);
gboolean tcore_sim_encode_mwis(const TelSimMwis *mwis, guint field_count,
	gchar **encoded_mwis, guint *encoded_mwis_len);
gboolean tcore_sim_decode_vmwf(unsigned char* enc_vmwf, unsigned long enc_vmwf_len, TelSimMwis *dec_vmwf);
gboolean tcore_sim_decode_ons(unsigned  char* enc_ons, int enc_ons_len, unsigned char *dec_ons);
gboolean tcore_sim_decode_cfis(unsigned char *enc_cfis, int enc_cfis_len, TelSimCfis *dec_cfis);
gboolean tcore_sim_decode_short_ons(unsigned char *enc_short_ons, int enc_short_ons_len, unsigned char *dec_short_ons);
gboolean tcore_sim_decode_opl(unsigned char *enc_opl, int enc_opl_len, TelSimOpl *dec_opl);
gboolean tcore_sim_decode_pnn(unsigned char* enc_pnn, int enc_pnn_len, TelSimPnn *dec_pnn);

#ifdef __cplusplus
}
#endif

#endif	/* __CO_SIM_H__ */
