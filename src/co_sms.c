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
#include "internal/tcore_types.h"
#include "plugin.h"
#include "user_request.h"
#include "co_sms.h"

struct private_object_data {
	struct tcore_sms_operations *ops[TCORE_OPS_TYPE_MAX];
	enum telephony_sms_ready_status SmsReadyStatus;
};

int _tcore_util_sms_encode_smsParameters(const struct telephony_sms_Params *incoming,
	unsigned char *data, int SMSPRecordLen)
{
	struct telephony_sms_Params *smsParams =  (struct telephony_sms_Params *)incoming;
	unsigned int nPIDIndex = 0;
	unsigned char nOffset = 0;

	if (incoming == NULL || data == NULL)
		return FALSE;

	memset(data, 0xff, SMSPRecordLen);

	dbg(" Record index = %d", (int) smsParams->recordIndex);
	dbg(" Alpha ID Len = %d", (int) smsParams->alphaIdLen);
	dbg(" Record Length : %d", SMSPRecordLen);

	if (SMSPRecordLen/*pSmsParam->RecordLen*/>= nDefaultSMSPWithoutAlphaId)
		nPIDIndex = SMSPRecordLen /*pSmsParam->RecordLen*/- nDefaultSMSPWithoutAlphaId;

	/* dongil01.park(2008/12/27) - Check Point */
	memcpy(data, smsParams->szAlphaId, (int)nPIDIndex/*pSmsParam->AlphaIdLen*/);

	dbg(" Alpha ID : %s", smsParams->szAlphaId);
	dbg(" nPIDIndex = %d", nPIDIndex);

	data[nPIDIndex] = smsParams->paramIndicator;

	dbg(" Param Indicator = %02x", smsParams->paramIndicator);

	if ((smsParams->paramIndicator & SMSPValidDestAddr) == 0x00) {
		nOffset = nDestAddrOffset;

		data[nPIDIndex + (nOffset)] = smsParams->tpDestAddr.dialNumLen + 1;
		data[nPIDIndex + (++nOffset)] = ((smsParams->tpDestAddr.typeOfNum << 4) | smsParams->tpDestAddr.numPlanId) | 0x80;

		memcpy(&data[nPIDIndex + (++nOffset)], &smsParams->tpDestAddr.diallingNum, smsParams->tpDestAddr.dialNumLen);

		dbg(" nextIndex = %d", nPIDIndex);
	}

	if ((smsParams->paramIndicator & SMSPValidSvcAddr) == 0x00) {
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

void tcore_util_sms_semioctet_to_octect(int *nScLength)
{
	if (*nScLength % 2)
		*nScLength = (*nScLength / 2) + 1;
	else
		*nScLength = *nScLength / 2;
}

int tcore_util_sms_decode_smsParameters(uint8_t *incoming, uint32_t length,
	struct telephony_sms_Params *params)
{
	int alpha_id_len = 0;
	int i = 0;
	int nOffset = 0;

	dbg(" RecordLen = %d", length);

	if (incoming == NULL || params == NULL)
		return FALSE;

	alpha_id_len = length - SMS_SMSP_PARAMS_MAX_LEN;

	if (alpha_id_len > 0) {
		if (alpha_id_len > SMS_SMSP_ALPHA_ID_LEN_MAX)
			alpha_id_len = SMS_SMSP_ALPHA_ID_LEN_MAX;

		for (i = 0 ; i < alpha_id_len ; i++) {
			if (0xff == incoming[i]) {
				dbg(" found");
				break;
			}
		}

		memcpy(params->szAlphaId, incoming, i);

		params->alphaIdLen = i;

		dbg(" Alpha id length = %d", i);

	} else {
		params->alphaIdLen = 0;
		dbg(" Alpha id length is zero");
	}

	/* dongil01.park - start parse from here. */
	params->paramIndicator = incoming[alpha_id_len];

	dbg(" Param Indicator = %02x", params->paramIndicator);

	/* dongil01.park(2008/12/26) - DestAddr */
	if ((params->paramIndicator & SMSPValidDestAddr) == 0) {
		nOffset = nDestAddrOffset;

		if (0x00 == incoming[alpha_id_len + nOffset] || 0xff == incoming[alpha_id_len + nOffset]) {
			params->tpDestAddr.dialNumLen = 0;

			dbg("DestAddr Length is 0");
		} else {
			if (0 < (int)incoming[alpha_id_len + nOffset]) {
				params->tpDestAddr.dialNumLen = (int)(incoming[alpha_id_len + nOffset] - 1);

				if (params->tpDestAddr.dialNumLen > SMS_SMSP_ADDRESS_LEN)
					params->tpDestAddr.dialNumLen = SMS_SMSP_ADDRESS_LEN;
			} else {
				params->tpDestAddr.dialNumLen = 0;
			}

			params->tpDestAddr.numPlanId = incoming[alpha_id_len + (++nOffset)] & 0x0f ;
			params->tpDestAddr.typeOfNum = (incoming[alpha_id_len + nOffset] & 0x70)>>4 ;

			memcpy(params->tpDestAddr.diallingNum, &incoming[alpha_id_len + (++nOffset)], (params->tpDestAddr.dialNumLen)) ;

			dbg("Dest TON is %d", params->tpDestAddr.typeOfNum);
			dbg("Dest NPI is %d", params->tpDestAddr.numPlanId);
			dbg("Dest Length = %d", params->tpDestAddr.dialNumLen);
			dbg("Dest Addr = %s", params->tpDestAddr.diallingNum);
		}
	}

	/* dongil01.park(2008/12/26) - SvcAddr */
	if ((params->paramIndicator & SMSPValidSvcAddr) == 0) {
		nOffset = nSCAAddrOffset;

		if (0x00 == (int)incoming[alpha_id_len + nOffset] || 0xff == (int)incoming[alpha_id_len + nOffset]) {
			params->tpSvcCntrAddr.dialNumLen = 0;

			dbg(" SCAddr Length is 0");
		} else {
			if (0 < (int)incoming[alpha_id_len + nOffset]) {
				params->tpSvcCntrAddr.dialNumLen = (int)(incoming[alpha_id_len + nOffset] - 1);

				if (params->tpSvcCntrAddr.dialNumLen > SMS_SMSP_ADDRESS_LEN)
					params->tpSvcCntrAddr.dialNumLen = SMS_SMSP_ADDRESS_LEN;

				params->tpSvcCntrAddr.numPlanId = incoming[alpha_id_len + (++nOffset)] & 0x0f ;
				params->tpSvcCntrAddr.typeOfNum = (incoming[alpha_id_len + nOffset] & 0x70) >> 4;

				memcpy(params->tpSvcCntrAddr.diallingNum, &incoming[alpha_id_len + (++nOffset)], (params->tpSvcCntrAddr.dialNumLen));

				dbg("SCAddr Length = %d ", params->tpSvcCntrAddr.dialNumLen);
				dbg("SCAddr TON is %d", params->tpSvcCntrAddr.typeOfNum);
				dbg("SCAddr NPI is %d", params->tpSvcCntrAddr.numPlanId);

				for (i = 0 ; i < (int)params->tpSvcCntrAddr.dialNumLen ; i++)
					dbg("SCAddr = %d [%02x]", i, params->tpSvcCntrAddr.diallingNum[i]);
			} else
				params->tpSvcCntrAddr.dialNumLen = 0;
		}
	} else if ((0x00 < (int)incoming[alpha_id_len + nSCAAddrOffset]
			&& (int)incoming[alpha_id_len + nSCAAddrOffset] <= 12)
			|| 0xff != (int)incoming[alpha_id_len + nSCAAddrOffset]) {
		nOffset = nSCAAddrOffset;

		if (0x00 == (int)incoming[alpha_id_len + nOffset] || 0xff == (int)incoming[alpha_id_len + nOffset]) {
			params->tpSvcCntrAddr.dialNumLen = 0;
			dbg("SCAddr Length is 0");
		} else {

			if (0 < (int)incoming[alpha_id_len + nOffset]) {
				params->tpSvcCntrAddr.dialNumLen = (int)(incoming[alpha_id_len + nOffset] - 1);

				params->tpSvcCntrAddr.dialNumLen = incoming[alpha_id_len + nOffset] - 1;

				if (params->tpSvcCntrAddr.dialNumLen > SMS_SMSP_ADDRESS_LEN)
					params->tpSvcCntrAddr.dialNumLen = SMS_SMSP_ADDRESS_LEN;

				params->tpSvcCntrAddr.numPlanId = incoming[alpha_id_len + (++nOffset)] & 0x0f ;
				params->tpSvcCntrAddr.typeOfNum = (incoming[alpha_id_len + nOffset] & 0x70) >> 4;

				memcpy(params->tpSvcCntrAddr.diallingNum, &incoming[alpha_id_len + (++nOffset)],
				(params->tpSvcCntrAddr.dialNumLen)) ;

				dbg("SCAddr Length = %d ", params->tpSvcCntrAddr.dialNumLen);
				dbg("SCAddr TON is %d", params->tpSvcCntrAddr.typeOfNum);
				dbg("SCAddr NPI is %d", params->tpSvcCntrAddr.numPlanId);

				for (i = 0 ; i < (int)params->tpSvcCntrAddr.dialNumLen ; i++)
					dbg("SCAddr = %d [%02x]", i, params->tpSvcCntrAddr.diallingNum[i]);
			} else
				params->tpSvcCntrAddr.dialNumLen = 0;
		}

	}

	if ((params->paramIndicator & SMSPValidPID) == 0
			&& (alpha_id_len + nPIDOffset) < SMS_MAX_EFSMSP_RECORD_LENGTH)
		params->tpProtocolId = incoming[alpha_id_len + nPIDOffset];

	if ((params->paramIndicator & SMSPValidDCS) == 0
			&& (alpha_id_len + nDCSOffset) < SMS_MAX_EFSMSP_RECORD_LENGTH)
		params->tpDataCodingScheme = incoming[alpha_id_len + nDCSOffset];

	if ((params->paramIndicator & SMSPValidVP) == 0
			&& (alpha_id_len + nVPOffset) < SMS_MAX_EFSMSP_RECORD_LENGTH)
		params->tpValidityPeriod = incoming[alpha_id_len + nVPOffset];

	dbg(" Alpha Id(Len) = %d", (int)params->alphaIdLen);

	for (i = 0; i < (int)params->alphaIdLen ; i++)
		dbg(" Alpha Id = [%d] [%c]", i, params->szAlphaId[i]);

	dbg(" PID = %d", params->tpProtocolId);
	dbg(" DCS = %d", params->tpDataCodingScheme);
	dbg(" VP = %d", params->tpValidityPeriod);

	return TRUE;
}


static TReturn _dispatcher(CoreObject *o, UserRequest *ur, enum tcore_ops_type ops_type)
{
	enum tcore_request_command command;
	struct private_object_data *po = tcore_object_ref_object(o);
	struct tcore_sms_operations *ops = NULL;
	TReturn rtn = TCORE_RETURN_SUCCESS;

	CORE_OBJECT_CHECK_RETURN(o, CORE_OBJECT_TYPE_SMS, TCORE_RETURN_EINVAL);
	CORE_OBJECT_VALIDATE_OPS_RETURN_VAL(ops_type, TCORE_RETURN_EINVAL);

	tcore_check_null_ret_err("po", po, TCORE_RETURN_EINVAL);
	tcore_check_null_ret_err("ur", ur, TCORE_RETURN_EINVAL);

	if (po->SmsReadyStatus == SMS_READY_STATUS_NONE) {
		dbg("[tcore_SMS] DEVICE_NOT_READY");
		return TCORE_RETURN_ENOSYS; /* TAPI_API_NETTEXT_DEVICE_NOT_READY */
	}

	ops = po->ops[ops_type];
	tcore_check_null_ret_err("ops", ops, TCORE_RETURN_FAILURE);

	command = tcore_user_request_get_command(ur);
	switch (command) {
	case TREQ_SMS_SEND_UMTS_MSG:
		tcore_check_null_ret_err("ops->send_umts_msg",
			ops->send_umts_msg, TCORE_RETURN_ENOSYS);

		rtn = ops->send_umts_msg(o, ur);
	break;

	case TREQ_SMS_READ_MSG:
		tcore_check_null_ret_err("ops->connect",
			ops->read_msg, TCORE_RETURN_ENOSYS);

		rtn = ops->read_msg(o, ur);
	break;

	case TREQ_SMS_SAVE_MSG:
		tcore_check_null_ret_err("ops->save_msg",
			ops->save_msg, TCORE_RETURN_ENOSYS);

		rtn = ops->save_msg(o, ur);
	break;

	case TREQ_SMS_DELETE_MSG:
		tcore_check_null_ret_err("ops->delete_msg",
			ops->delete_msg, TCORE_RETURN_ENOSYS);

		rtn = ops->delete_msg(o, ur);
	break;

	case TREQ_SMS_GET_COUNT:
		tcore_check_null_ret_err("ops->get_storedMsgCnt",
			ops->get_storedMsgCnt, TCORE_RETURN_ENOSYS);

		rtn = ops->get_storedMsgCnt(o, ur);
	break;

	case TREQ_SMS_GET_SCA:
		tcore_check_null_ret_err("ops->get_sca",
			ops->get_sca, TCORE_RETURN_ENOSYS);

		rtn = ops->get_sca(o, ur);
	break;

	case TREQ_SMS_SET_SCA:
		tcore_check_null_ret_err("ops->set_sca",
			ops->set_sca, TCORE_RETURN_ENOSYS);

		rtn = ops->set_sca(o, ur);
	break;

	case TREQ_SMS_GET_CB_CONFIG:
		tcore_check_null_ret_err("ops->get_cb_config",
			ops->get_cb_config, TCORE_RETURN_ENOSYS);

		rtn = ops->get_cb_config(o, ur);
	break;

	case TREQ_SMS_SET_CB_CONFIG:
		tcore_check_null_ret_err("ops->set_cb_config",
			ops->set_cb_config, TCORE_RETURN_ENOSYS);

		rtn = ops->set_cb_config(o, ur);
	break;

	case TREQ_SMS_SET_MEM_STATUS:
		tcore_check_null_ret_err("ops->set_mem_status",
			ops->set_mem_status, TCORE_RETURN_ENOSYS);

		rtn = ops->set_mem_status(o, ur);
	break;

	case TREQ_SMS_GET_PREF_BEARER:
		tcore_check_null_ret_err("ops->get_pref_brearer",
			ops->get_pref_brearer, TCORE_RETURN_ENOSYS);

		rtn = ops->get_pref_brearer(o, ur);
	break;

	case TREQ_SMS_SET_PREF_BEARER:
		tcore_check_null_ret_err("ops->set_pref_brearer",
			ops->set_pref_brearer, TCORE_RETURN_ENOSYS);

		rtn = ops->set_pref_brearer(o, ur);
	break;

	case TREQ_SMS_SET_DELIVERY_REPORT:
		tcore_check_null_ret_err("ops->set_delivery_report",
			ops->set_delivery_report, TCORE_RETURN_ENOSYS);

		rtn = ops->set_delivery_report(o, ur);
	break;

	case TREQ_SMS_SET_MSG_STATUS:
		tcore_check_null_ret_err("ops->set_msg_status",
			ops->set_msg_status, TCORE_RETURN_ENOSYS);

		rtn = ops->set_msg_status(o, ur);
	break;

	case TREQ_SMS_GET_PARAMS:
		tcore_check_null_ret_err("ops->get_sms_params",
			ops->get_sms_params, TCORE_RETURN_ENOSYS);

		rtn = ops->get_sms_params(o, ur);
	break;

	case TREQ_SMS_SET_PARAMS:
		tcore_check_null_ret_err("ops->set_sms_params",
			ops->set_sms_params, TCORE_RETURN_ENOSYS);

		rtn = ops->set_sms_params(o, ur);
	break;

	case TREQ_SMS_GET_PARAMCNT:
		tcore_check_null_ret_err("ops->get_paramcnt",
			ops->get_paramcnt, TCORE_RETURN_ENOSYS);

		rtn = ops->get_paramcnt(o, ur);
	break;

	case TREQ_SMS_SEND_CDMA_MSG:
		tcore_check_null_ret_err("ops->send_cdma_msg",
			ops->send_cdma_msg, TCORE_RETURN_ENOSYS);

		rtn = ops->send_cdma_msg(o, ur);
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

enum telephony_sms_ready_status tcore_sms_get_ready_status(CoreObject *o)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return SMS_READY_STATUS_NONE;
	}

	return po->SmsReadyStatus;
}

gboolean tcore_sms_set_ready_status(CoreObject *o, enum telephony_sms_ready_status status)
{
	struct private_object_data *po = NULL;
	po = tcore_object_ref_object(o);
	if (!po) {
		dbg("po access fail");
		return FALSE;
	}

	po->SmsReadyStatus = status;

	return TRUE;
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

	po = calloc(1, sizeof(struct private_object_data));
	if (!po) {
		tcore_object_free(o);
		return NULL;
	}

	/* set ops to default type when core object is created. */
	po->ops[TCORE_OPS_TYPE_CP] = ops;

	tcore_object_set_type(o, CORE_OBJECT_TYPE_SMS);
	tcore_object_link_object(o, po);
	tcore_object_set_free_hook(o, _free_hook);
	tcore_object_set_dispatcher(o, _dispatcher);

	return o;
}

void tcore_sms_free(CoreObject *o)
{
	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SMS);
	tcore_object_free(o);
}

void tcore_sms_set_ops(CoreObject *o, struct tcore_sms_operations *ops, enum tcore_ops_type ops_type)
{
	struct private_object_data *po = NULL;

	CORE_OBJECT_CHECK(o, CORE_OBJECT_TYPE_SMS);
	CORE_OBJECT_VALIDATE_OPS_RETURN(ops_type);

	po = (struct private_object_data *)tcore_object_ref_object(o);
	if (!po) {
		err("po is NULL");
		return;
	}

	po->ops[ops_type] = ops;
}
