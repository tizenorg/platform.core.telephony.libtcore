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

#ifndef __TCORE_CO_SMS_H__
#define __TCORE_CO_SMS_H__

#include <core_object.h>

__BEGIN_DECLS


#define nDefaultSMSPWithoutAlphaId		28

#define SMSPValidDestAddr			0x01
#define SMSPValidSvcAddr			0x02
#define SMSPValidPID				0x04
#define SMSPValidDCS				0x08
#define SMSPValidVP				0x10

#define nDestAddrOffset				1
#define nSCAAddrOffset				13
#define nPIDOffset				25
#define nDCSOffset				26
#define nVPOffset					27

struct tcore_sms_operations {
	TReturn (*send_umts_msg)(CoreObject *o, UserRequest *ur);
	TReturn (*read_msg)(CoreObject *o, UserRequest *ur);
	TReturn (*save_msg)(CoreObject *o, UserRequest *ur);
	TReturn (*delete_msg)(CoreObject *o, UserRequest *ur);
	TReturn (*get_storedMsgCnt)(CoreObject *o, UserRequest *ur);
	TReturn (*get_sca)(CoreObject *o, UserRequest *ur);
	TReturn (*set_sca)(CoreObject *o, UserRequest *ur);
	TReturn (*get_cb_config)(CoreObject *o, UserRequest *ur);
	TReturn (*set_cb_config)(CoreObject *o, UserRequest *ur);
	TReturn (*set_mem_status)(CoreObject *o, UserRequest *ur);
	TReturn (*get_pref_brearer)(CoreObject *o, UserRequest *ur);
	TReturn (*set_pref_brearer)(CoreObject *o, UserRequest *ur);
	TReturn (*set_delivery_report)(CoreObject *o, UserRequest *ur);
	TReturn (*set_msg_status)(CoreObject *o, UserRequest *ur);
	TReturn (*get_sms_params)(CoreObject *o, UserRequest *ur);
	TReturn (*set_sms_params)(CoreObject *o, UserRequest *ur);
	TReturn (*get_paramcnt)(CoreObject *o, UserRequest *ur);
	TReturn (*send_cdma_msg)(CoreObject *o, UserRequest *ur);
};

/**
 * This function is used to encode SMS Parameters to TPDU on EFsmsp
 *
 * @return		length of string
 * @param[in]		incoming - telephony_sms_Params_t
 * @param[in]		data - TPDU data
 * @Interface		Synchronous.
 * @remark
 * @Refer
 */
int  _tcore_util_sms_encode_smsParameters(const struct telephony_sms_Params *incoming,
	unsigned char *data, int SMSPRecordLen);
int  tcore_util_sms_decode_smsParameters(uint8_t *incoming, uint32_t length,
	struct telephony_sms_Params *params);

void tcore_util_sms_semioctet_to_octect(int* nScLength);
enum telephony_sms_ready_status  tcore_sms_get_ready_status(CoreObject *o);
gboolean  tcore_sms_set_ready_status(CoreObject *o, enum telephony_sms_ready_status status);
CoreObject *tcore_sms_new(TcorePlugin *p, const char *name, struct tcore_sms_operations *ops, TcoreHal *hal);
void  tcore_sms_free(CoreObject * n);

void tcore_sms_set_ops(CoreObject *o, struct tcore_sms_operations *ops, enum tcore_ops_type ops_type);

__END_DECLS

#endif /* __TCORE_CO_SMS_H__ */
