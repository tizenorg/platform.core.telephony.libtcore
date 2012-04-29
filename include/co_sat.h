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

#ifndef __TCORE_CO_SAT_H__
#define __TCORE_CO_SAT_H__

#include <core_object.h>

enum tcore_sat_result{
	TCORE_SAT_SUCCESS,
	TCORE_SAT_SUCCESS_PARTIAL_COMPREHENSION,
	TCORE_SAT_UNABLE_TO_PERFORM_CMD,
	TCORE_SAT_REQUIRED_VALUE_MISSING,
	TCORE_SAT_COMMAND_NOT_UNDERSTOOD,
	TCORE_SAT_BEYOND_ME_CAPABILITY,
	TCORE_SAT_COMMAND_TYPE_NOT_UNDERSTOOD,
	TCORE_SAT_ERROR_FATAL
};

struct tcore_sat_proactive_command {
	int cmd_num;
	enum tel_sat_proactive_cmd_type cmd_type;
	union {
		struct tel_sat_setup_event_list_tlv setup_event_list;
		struct tel_sat_display_text_tlv display_text;
		struct tel_sat_get_inkey_tlv get_inkey;
		struct tel_sat_get_input_tlv get_input;
		struct tel_sat_select_item_tlv select_item;
		struct tel_sat_setup_menu_tlv setup_menu;
/*		TelSatMoreTimeIndInfo_t moreTime;
		TelSatPlayToneIndInfo_t playTone;

		TelSatSendSmsIndInfo_t sendSms;
		TelSatSendSsIndInfo_t sendSs;
		TelSatSendUssdIndInfo_t sendUssd;
		TelSatSetupCallIndInfo_t setupCall;
		TelSatRefreshIndInfo_t refresh;
		TelSatProvideLocalInfoIndInfo_t provideLocInfo;
		TelSatLaunchBrowserIndInfo_t launchBrowser;
		TelSatSetupIdleModeTextIndInfo_t idleText;
		TelSatSendDtmfIndInfo_t sendDtmf;
		TelSatLanguageNotificationIndInfo_t languageNotification;
		TelSatSetupEventListIndInfo_t setupEventList;
		TelSatOpenChannelIndInfo_t openChannel;
		TelSatCloseChannelIndInfo_t closeChannel;
		TelSatReceiveDataIndInfo_t receiveData;
		TelSatSendDataIndInfo_t sendData;
		TelSatGetChannelStatusIndInfo_t getChannelStatus;*/
	} data;
};

struct tcore_sat_operations {
	TReturn (*envelope)(CoreObject *o, UserRequest *ur);
	TReturn (*terminal_response)(CoreObject *o, UserRequest *ur);
};

int tcore_sat_decode_proactive_command(unsigned char* tlv_origin, unsigned int tlv_length, struct tcore_sat_proactive_command* decoded_tlv);
int tcore_sat_encode_envelop_cmd(const struct treq_sat_envelop_cmd_data *src_envelop, char *dst_envelop);
int tcore_sat_encode_terminal_response(const struct treq_sat_terminal_rsp_data *src_tr, char *dst_tr);

CoreObject* tcore_sat_new(TcorePlugin *p, const char *name, struct tcore_sat_operations *ops);
void        tcore_sat_free(CoreObject *n);

#endif
