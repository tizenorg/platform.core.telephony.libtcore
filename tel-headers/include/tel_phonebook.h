/*
 * tel-headers
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

#ifndef __TEL_PHONEBOOK_H__
#define __TEL_PHONEBOOK_H__

#include <tel_if.h>

/**
*  @addtogroup TAPI_PHONEBOOK
*  @{
*
*  @file tel_phonebook.h
*  @brief TAPI Phonebook Interface
*/

#ifdef __cplusplus
extern "C"
{
#endif

#define TELEPHONY_PB_INTERFACE  TELEPHONY_SERVICE".Phonebook"

#define TEL_PB_TEXT_MAX_LEN	255 /**< Phone book text maximum length */
#define TEL_PB_NUMBER_MAX_LEN	82 /**< Phone book number maximum length */
#define TEL_PB_ANR_MAX_COUNT	3 /**< Phone book ANR maximum count */
#define TEL_PB_EMAIL_MAX_COUNT	4 /**< Phone book email maximum count */

/**
 * @enum TelPbResult
 * Response (Return) values.
 */
typedef enum {
	TEL_PB_RESULT_SUCCESS, /**< Operation completed successfully */
	TEL_PB_RESULT_FAILURE, /**< Operation Failed */
	TEL_PB_RESULT_INVALID_PARAMETER, /**< Invalid input parameters */
	TEL_PB_RESULT_MEMORY_FAILURE, /**< Memory allocation failed */
	TEL_PB_RESULT_OPERATION_NOT_SUPPORTED, /**< Operation not supported */
	TEL_PB_RESULT_UNKNOWN_FAILURE, /**< Unknown failure */
	TEL_PB_RESULT_INVALID_INDEX, /**< The index passed was not a valid index. */
	TEL_PB_RESULT_NOT_INITIALIZED, /**< Phonebook not initialized */
	TEL_PB_RESULT_PIN2_REQUIRED, /**< PIN2 Required */
	TEL_PB_RESULT_PUK2_REQUIRED, /**< PUK2 Required */
	TEL_PB_RESULT_ACCESS_RESTRICTED, /**< Access restricted */
	TEL_PB_RESULT_OPERATION_NOT_PERMITTED	/**< Operation not permitted */
} TelPbResult;

/**
 * @enum TelPbType
 * This enumeration defines the Phonebook type.
 */
typedef enum {
	TEL_PB_FDN, /**< Fixed Dialing Number */
	TEL_PB_ADN, /**< Abbreviated dialling numbers */
	TEL_PB_SDN, /**< Service Dialing Number */
	TEL_PB_USIM, /**< USIM - 3G phone book */
} TelPbType;

/**
 * This data structure gives the phone book availability of current SIM
 */
typedef struct {
	gboolean fdn; /**< Availability of Fixed Dialing Number */
	gboolean adn; /**< Availability of Abbreviated dialling numbers */
	gboolean sdn; /**< Availability of Service Dialing Number */
	gboolean usim; /**< Availability of USIM - 3G phone book */
} TelPbList;

/**
 * @enum TelPbInitInfo
 * This data structure gives the phonebook init information
 */
typedef struct {
	gboolean init_status; /**< Phonebook init status */
	TelPbList pb_list; /**< Phonebook availability of current SIM */
} TelPbInitInfo;

/**
 *This data structure gives the phone book entry information(SIM).
 */
typedef struct {
	unsigned int max_count; /**< Total number of locations in selected memory */
	unsigned int used_count; /**< Number of used locations in selected memory */
	unsigned int max_num_len; /**< maximum length of field "number" */
	unsigned int max_text_len; /**< maximum length of field "text" */
} TelPbSimInfo;

/**
 *This data structure gives the phone book entry information(USIM).
 */
typedef struct {
	unsigned int max_count; /**< Total number of locations in selected memory */
	unsigned int used_count; /**< Number of used locations in selected memory */
	unsigned int max_num_len; /**< maximum length of field "number" */
	unsigned int max_text_len; /**< maximum length of field "text" */

	unsigned char max_anr_count; /**< Total number of locations for Additional Number */
	unsigned int max_anr_len; /**< Maximun length for Additional Number */

	unsigned char max_email_count; /**< Total number of locations for email */
	unsigned int max_email_len; /**< Maximun length for email */

	unsigned int max_sne_len; /**< Maximun length for second name*/

	unsigned int max_gas_count; /**< Total number of locations for Grouping Alpha String */
	unsigned int max_gas_len; /**< Maximun length for Grouping Alpha String */

	unsigned int max_aas_count; /**< Total number of locations for Additional Alpha String */
	unsigned int max_aas_len; /**< Maximun length for Additional Alpha String */
} TelPbUsimInfo;

/**
 *This data structure gives SIM/USIM the phone book entry information.
 */
typedef struct {
	TelPbType pb_type; /**< Type of Phone book */
	union {
		TelPbSimInfo sim; /**< 2G sim phone book info */
		TelPbUsimInfo usim; /**< 3G sim phone book info */
	} info_u; /**< union */
} TelPbInfo;

/**
 *These data structures defines the phone book record fields.
 */
typedef char TelPbName[TEL_PB_TEXT_MAX_LEN + 1]; /**< Record of Phone book name */

typedef char TelPbNumber[TEL_PB_NUMBER_MAX_LEN + 1]; /**< Record of Phone book number */

typedef char TelPbEmail[TEL_PB_TEXT_MAX_LEN + 1]; /**< Record of Phone book email */

/**
 *These data structures defines the phone book Additional number information
 */
typedef struct {
	TelPbNumber number; /**< Additional number */
	gboolean description;	/**< TRUE if the ANR has an additional number description */
	TelPbName aas;		/**< Additional number description   */
} TelPbAnr;

/**
 *These data structures defines the USIM phone book records
 */
typedef struct {
	TelPbName name; /**< USIM Phone book name */
	TelPbNumber number; /**< USIM Phone book number */

	TelPbName sne; /**< USIM Phone book second name */
	TelPbName grp_name; /**< USIM Phone book group name */

	unsigned char anr_count; /**< USIM Phone book Additional number count */
	TelPbAnr anr[TEL_PB_ANR_MAX_COUNT]; /**< USIM Phone book Additional number description */

	unsigned char email_count; /**< USIM Phone book email count */
	TelPbEmail email[TEL_PB_EMAIL_MAX_COUNT]; /**< USIM Phone book email description */

	gboolean hidden; /**< indicates if the entry is hidden or not - only available, if a UICC with an active USIM application is present */
} TelPbUsimRecord;

/**
 *These data structures defines the SIM phone book records
 */
typedef struct {
	TelPbName name; /**< SIM Phone book name */
	TelPbNumber number; /**< SIM Phone book number */
} TelPbSimRecord;

/**
 *These data structures defines the read record informations
 */
typedef struct {
	unsigned int index; /**< Index of phonebook memory */
	unsigned int next_index; /**< Next index of phonebook memory */

	TelPbType pb_type; /**< Type of Phone book */
	union {
		TelPbSimRecord sim; /**< 2G sim phone book info */
		TelPbUsimRecord usim; /**< 3G sim phone book info */
	} rec_u; /**< union */
} TelPbReadRecord;

/**
 *These data structures defines the update record informations
 */
typedef struct {
	unsigned int index; /**< Index of phonebook memory */

	TelPbType pb_type; /**< Type of Phone book */
	union {
		TelPbSimRecord sim; /**< 2G sim phone book info */
		TelPbUsimRecord usim; /**< 3G sim phone book info */
	} rec_u; /**< union */
} TelPbUpdateRecord;

/**
 *These data structures defines the phone book record informations
 */
typedef struct {
	TelPbType pb_type; /**< Type of Phone book */
	unsigned int index; /**< Index of phonebook memory */
} TelPbRecordInfo;

/**
 *  \defgroup TAPI_EVENT TAPI Event IDs
 *  @{
 */

/**
 *  \defgroup TEL_EVENT_PHONEBOOK Phonebook Event IDs
 *  @{
 */

/**
 *  @hideinitializer
 *  @brief Phonebook init status
 *  @section subscription Event Subscription
 *  @details Event can be subscribed with event id TEL_NOTI_PB_STATUS
 *  @sa tapi_register_event_id
 *  @section handling Event Handling
 *  @details TAPI will invoke the registered callback (\ref TapiEvtCb) and return the following data:
 *  @retval handle #TelHandle used for registering the event
 *  @retval evt_id TEL_NOTI_PB_STATUS
 *  #retval data \ref TelPbInitInfo
 *  @retval user_data user data passed while registering the event
 */
#define TEL_NOTI_PB_STATUS				TELEPHONY_PB_INTERFACE":Status"

/** @}*/
/** @}*/

#ifdef __cplusplus
}
#endif

/** @}*/

#endif /* __TEL_PHONEBOOK_H__ */
