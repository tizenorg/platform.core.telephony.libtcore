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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tcore.h"
#include "plugin.h"
#include "user_request.h"
#include "co_sms.h"

struct private_object_data {
	struct tcore_sms_operations *ops;
	gboolean b_readyStatus;
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
int _tcore_util_sms_encode_smsParameters(const struct telephony_sms_Params *incoming, unsigned char *data, int SMSPRecordLen)
{
	struct telephony_sms_Params *smsParams =  (struct telephony_sms_Params *)incoming;
	unsigned int nPIDIndex = 0;
	unsigned char nOffset = 0;

	if(incoming == NULL || data == NULL)
		return FALSE;

	memset(data, 0xff, SMSPRecordLen);//pSmsParam->RecordLen);

	dbg(" Record index = %d", (int) smsParams->recordIndex);
	dbg(" Alpha ID Len = %d", (int) smsParams->alphaIdLen);
	dbg(" Record Length : %d", SMSPRecordLen);//pSmsParam->RecordLen);

	if (SMSPRecordLen/*pSmsParam->RecordLen*/>= nDefaultSMSPWithoutAlphaId) {
		nPIDIndex = SMSPRecordLen
				/*pSmsParam->RecordLen*/- nDefaultSMSPWithoutAlphaId;
	}

	//dongil01.park(2008/12/27) - Check Point
	memcpy(data, smsParams->szAlphaId, (int) nPIDIndex/*pSmsParam->AlphaIdLen*/);

	dbg(" Alpha ID : %s", smsParams->szAlphaId);
	dbg(" nPIDIndex = %d", nPIDIndex);

	data[nPIDIndex] = smsParams->paramIndicator;

	dbg(" Param Indicator = %02x",	smsParams->paramIndicator);

	if ((smsParams->paramIndicator & SMSPValidDestAddr) == 0x00) {
		nOffset = nDestAddrOffset;

		data[nPIDIndex + (nOffset)] = smsParams->tpDestAddr.dialNumLen + 1;
		data[nPIDIndex + (++nOffset)] = ((smsParams->tpDestAddr.typeOfNum << 4) | smsParams->tpDestAddr.numPlanId) | 0x80;

		memcpy(&data[nPIDIndex + (++nOffset)], &smsParams->tpDestAddr.diallingNum, smsParams->tpDestAddr.dialNumLen);

		dbg(" nextIndex = %d", nPIDIndex);
	}

	if( (smsParams->paramIndicator & SMSPValidSvcAddr) == 0x00 )
	{
		dbg("TON [%d] / NPI [%d]", smsParams->tpSvcCntrAddr.typeOfNum, smsParams->tpSvcCntrAddr.numPlanId);

		nOffset = nSCAAddrOffset;

		dbg("SCA Length : %d", smsParams->tpSvcCntrAddr.dialNumLen);

		data[nPIDIndex + (nOffset)] = smsParams->tpSvcCntrAddr.dialNumLen + 1;
		data[nPIDIndex + (++nOffset)] = ((smsParams->tpSvcCntrAddr.typeOfNum << 4) | smsParams->tpSvcCntrAddr.numPlanId) | 0x80;

		memcpy(&data[nPIDIndex + (++nOffset)], &smsParams->tpSvcCntrAddr.diallingNum, smsParams->tpSvcCntrAddr.dialNumLen);

		dbg(" nextIndex = %d", nPIDIndex);
	}

	if ((smsParams->paramIndicator & SMSPValidPID) == 0x00) {
		nOffset = nPIDOffset;

		data[nPIDIndex + nOffset] = smsParams->tpProtocolId;
		dbg(" PID = %02x", smsParams->tpProtocolId);
		dbg(" nextIndex = %d", nPIDIndex);
	}

	if ((smsParams->paramIndicator & SMSPValidDCS) == 0x00) {
		nOffset = nDCSOffset;

		data[nPIDIndex + nOffset] = smsParams->tpDataCodingScheme;
		dbg(" DCS = %02x", smsParams->tpDataCodingScheme);
		dbg(" nextIndex = %d", nPIDIndex);
	}

	if ((smsParams->paramIndicator & SMSPValidVP) == 0x00) {
		nOffset = nVPOffset;

		data[nPIDIndex + nOffset] = smsParams->tpValidityPeriod;
		dbg(" VP = %02x", smsParams->tpValidityPeriod);
		dbg(" nextIndex = %d", nPIDIndex);
	}

	return TRUE;

}

static void _clone_sms_operations(struct private_object_data *po, struct tcore_sms_operations *sms_ops)
{
	if(sms_ops->send_umts_msg) {
		po->ops->send_umts_msg = sms_ops->send_umts_msg;
	}
	if(sms_ops->read_msg) {
		po->ops->read_msg = sms_ops->read_msg;
	}
	if(sms_ops->save_msg) {
		po->ops->save_msg = sms_ops->save_msg;
	}
	if(sms_ops->delete_msg) {
		po->ops->delete_msg = sms_ops->delete_msg;
	}
	if(sms_ops->get_storedMsgCnt) {
		po->ops->get_storedMsgCnt = sms_ops->get_storedMsgCnt;
	}
	if(sms_ops->get_sca) {
		po->ops->get_sca = sms_ops->get_sca;
	}
	if(sms_ops->set_sca) {
		po->ops->set_sca = sms_ops->set_sca;
	}
	if(sms_ops->get_cb_config) {
		po->ops->get_cb_config = sms_ops->get_cb_config;
	}
	if(sms_ops->set_cb_config) {
		po->ops->set_cb_config = sms_ops->set_cb_config;
	}
	if(sms_ops->set_mem_status) {
		po->ops->set_mem_status = sms_ops->set_mem_status;
	}
	if(sms_ops->get_pref_brearer) {
		po->ops->get_pref_brearer = sms_ops->get_pref_brearer;
	}
	if(sms_ops->set_pref_brearer) {
		po->ops->set_pref_brearer = sms_ops->set_pref_brearer;
	}
	if(sms_ops->set_delivery_report) {
		po->ops->set_delivery_report = sms_ops->set_delivery_report;
	}
	if(sms_ops->set_msg_status) {
		po->ops->set_msg_status = sms_ops->set_msg_status;
	}
	if(sms_ops->get_sms_params) {
		po->ops->get_sms_params = sms_ops->get_sms_params;
	}
	if(sms_ops->set_sms_params) {
		po->ops->set_sms_params = sms_ops->set_sms_params;
	}
	if(sms_ops->get_paramcnt) {
		po->ops->get_paramcnt = sms_ops->get_paramcnt;
	}
	if(sms_ops->send_cdma_msg) {
		po->ops->send_cdma_msg = sms_ops->send_cdma_msg;
	}

	return;
}

static TReturn _dispatcher(CoreObject *o, UserRequest *ur)
{
	enum tcore_request_command command;
	struct private_object_data *po = NULL;
	TReturn rtn = TCORE_RETURN_SUCCESS;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SMS, TCORE_RETURN_EINVAL);

	po = tcore_object_ref_object(o);
	if (!po || !po->ops) {
		dbg("[tcore_SMS] ERR: private_object is NULL or ops is NULL");
		return TCORE_RETURN_ENOSYS;
	}

	if(po->b_readyStatus == FALSE) {
		dbg("[tcore_SMS] DEVICE_NOT_READY");
		return TCORE_RETURN_ENOSYS; /* TAPI_API_NETTEXT_DEVICE_NOT_READY */
	}

	command = tcore_user_request_get_command(ur);
	switch (command) {
		case TREQ_SMS_SEND_UMTS_MSG:
			if (!po->ops->send_umts_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->send_umts_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->send_umts_msg(o, ur);

			break;
		case TREQ_SMS_READ_MSG:
			if (!po->ops->read_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->read_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->read_msg(o, ur);

			break;
		case TREQ_SMS_SAVE_MSG:
			if (!po->ops->save_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->save_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->save_msg(o, ur);

			break;
		case TREQ_SMS_DELETE_MSG:
			if (!po->ops->delete_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->delete_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->delete_msg(o, ur);

			break;
		case TREQ_SMS_GET_COUNT:
			if (!po->ops->get_storedMsgCnt)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_storedMsgCnt is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_storedMsgCnt(o, ur);

			break;
		case TREQ_SMS_GET_SCA:
			if (!po->ops->get_sca)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_sca is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_sca(o, ur);

			break;
		case TREQ_SMS_SET_SCA:
			if (!po->ops->set_sca)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_sca is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_sca(o, ur);

			break;
		case TREQ_SMS_GET_CB_CONFIG:
			if (!po->ops->get_cb_config)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_cb_config is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_cb_config(o, ur);

			break;
		case TREQ_SMS_SET_CB_CONFIG:
			if (!po->ops->set_cb_config)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_cb_config is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_cb_config(o, ur);

			break;
		case TREQ_SMS_SET_MEM_STATUS:
			if (!po->ops->set_mem_status)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_mem_status is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_mem_status(o, ur);

			break;
		case TREQ_SMS_GET_PREF_BEARER:
			if (!po->ops->get_pref_brearer)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_pref_brearer is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_pref_brearer(o, ur);

			break;
		case TREQ_SMS_SET_PREF_BEARER:
			if (!po->ops->set_pref_brearer)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_pref_brearer is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_pref_brearer(o, ur);

			break;
		case TREQ_SMS_SET_DELIVERY_REPORT:
			if (!po->ops->set_delivery_report)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_delivery_report is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_delivery_report(o, ur);

			break;
		case TREQ_SMS_SET_MSG_STATUS:
			if (!po->ops->set_msg_status)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_msg_status is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_msg_status(o, ur);

			break;
		case TREQ_SMS_GET_PARAMS:
			if (!po->ops->get_sms_params)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_sms_params is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_sms_params(o, ur);

			break;
		case TREQ_SMS_SET_PARAMS:
			if (!po->ops->set_sms_params)
			{
				dbg("[tcore_SMS] ERR: po->ops->set_sms_params is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->set_sms_params(o, ur);

			break;
		case TREQ_SMS_GET_PARAMCNT:
			if (!po->ops->get_paramcnt)
			{
				dbg("[tcore_SMS] ERR: po->ops->get_paramcnt is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->get_paramcnt(o, ur);

			break;
		case TREQ_SMS_SEND_CDMA_MSG:
			if (!po->ops->send_cdma_msg)
			{
				dbg("[tcore_SMS] ERR: po->ops->send_cdma_msg is NULL");
				return TCORE_RETURN_ENOSYS;
			}

			rtn = po->ops->send_cdma_msg(o, ur);

			break;
		default:
			break;
	}

	dbg("[tcore_SMS] result = [0x%x], command = [0x%x]", rtn, command);

	return rtn;
}

static void _free_hook(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SMS);

	po = tcore_object_ref_object(o);
	if (po) {
		free(po);
		tcore_object_link_object(o, NULL);
	}
}

static void _clone_hook(CoreObject *src, CoreObject *dest)
{
	struct private_object_data *src_po = NULL;
	struct private_object_data *dest_po = NULL;

	if (!src || !dest)
		return;

	dest_po = calloc(sizeof(struct private_object_data), 1);
	if (!dest_po) {
		tcore_object_link_object(dest, NULL);
		return;
	}

	src_po = tcore_object_ref_object(src);
	dest_po->ops = src_po->ops;

	tcore_object_link_object(dest, dest_po);
}

gboolean tcore_sms_get_ready_status(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	return po->b_readyStatus;
}

gboolean tcore_sms_set_ready_status(CoreObject *o, int status)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	po->b_readyStatus = status;

	return TRUE;
}

void tcore_sms_override_ops(CoreObject *o, struct tcore_sms_operations *sms_ops)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SMS);
	
	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		return;
	}

	if(sms_ops) {
		_clone_sms_operations(po, sms_ops);
	}

	return;
}

CoreObject *tcore_sms_new(TcorePlugin *p, const char *name,
		struct tcore_sms_operations *ops, TcoreHal *hal)
{
	CoreObject *o = NULL;
	struct private_object_data *po = NULL;

	if (!p)
		return NULL;

	o = tcore_object_new(p, name, hal);
	if (!o)
		return NULL;

	po = calloc(sizeof(struct private_object_data), 1);
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	po->ops = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SMS);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_clone_hook(o, _clone_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

CoreObject *tcore_sms_clone(TcorePlugin *p, const char *name, TcoreHal *hal)
{
	CoreObject *o = NULL;

	if (!p)
		return NULL;

	o = tcore_object_clone_template_object(p, name, CORE_OBJECT_TYPE_SMS);
	if (!o)
		return NULL;

	tcore_object_set_hal(o, hal);
	
	return o;
}

void tcore_sms_free(CoreObject *o)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SMS);

	po = tcore_object_ref_object(o);
	if (!po)
		return;

	free(po);
	tcore_object_free(o);
}


