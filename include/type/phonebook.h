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

#ifndef __TYPE_PHONEBOOK_H__
#define __TYPE_PHONEBOOK_H__

__BEGIN_DECLS

#include <glib.h>

enum tel_phonebook_type {
	PB_TYPE_FDN, /**< Fixed Dialing Number */
	PB_TYPE_ADN, /**< SIM - ADN	 */
	PB_TYPE_SDN, /**< Service Dialing Number  */
	PB_TYPE_USIM, /**< USIM - 3G phone book */
	PB_TYPE_AAS, /**< Additional number Alpha String */
	PB_TYPE_GAS, /**< Grouping identifier Alpha String */
	PB_TYPE_UNKNOWNN = 0xFF, /**< Unknown file type */
};

enum tel_phonebook_field_type {
	PB_FIELD_NAME = 0x01, /**< Name */
	PB_FIELD_NUMBER, /**< Number */
	PB_FIELD_ANR1, /**< First Another number*/
	PB_FIELD_ANR2, /**< Second Another number */
	PB_FIELD_ANR3, /**< Third Another number */
	PB_FIELD_EMAIL1, /**< First Email */
	PB_FIELD_EMAIL2, /**< Second Email */
	PB_FIELD_EMAIL3, /**< Third Email */
	PB_FIELD_EMAIL4, /**< Fourth Email */
	PB_FIELD_SNE, /**< Second name entry of main name*/
	PB_FIELD_GRP, /**< Group  */
	PB_FIELD_PBC, /** <1 byte control info and 1 byte hidden info*/
};

enum tel_phonebook_result {
	PB_SUCCESS, /**< SIM phonebook operation successful. */
	PB_FAIL, /**< SIM phonebook operation failure. */
	PB_INVALID_INDEX, /**< The index passed was not a valid index. */
	PB_INVALID_NUMBER_LENGTH, /**< The number length is exceeds the max length allowed (or 0). */
	PB_INVALID_NAME_LENGTH, /**< The name length is exceeds the max length allowed (or 0). */
	PB_ACCESS_CONDITION_NOT_SATISFIED, /**< Access condition for PB file is not satisfied. */
};

enum tel_phonebook_ton {
	PB_TON_UNKNOWN = 0, /**< unknown */
	PB_TON_INTERNATIONAL = 1, /**< international number */
	PB_TON_NATIONAL = 2, /**< national number */
	PB_TON_NETWORK_SPECIFIC = 3, /**< network specific number */
	PB_TON_DEDICATED_ACCESS = 4, /**< subscriber number */
	PB_TON_ALPHA_NUMERIC = 5, /**< alphanumeric, GSM 7-bit default alphabet) */
	PB_TON_ABBREVIATED_NUMBER = 6, /**< abbreviated number */
	PB_TON_RESERVED_FOR_EXT = 7 /**< reserved for extension */
};

enum tel_phonebook_dcs {
	PB_TEXT_ASCII, /**< ASCII Encoding */
	PB_TEXT_GSM7BIT, /**< GSM 7 Bit Encoding */
	PB_TEXT_UCS2, /**< UCS2 Encoding */
	PB_TEXT_HEX, /**< HEX Encoding */
};

struct tel_phonebook_support_list {
	gboolean b_fdn; /**< Fixed Dialing Number */
	gboolean b_adn; /**< SIM - ADN(2G phonebook	 */
	gboolean b_sdn; /**< Service Dialing Number  */
	gboolean b_usim; /**< USIM - 3G phonebook */
	gboolean b_aas; /**< Additional number Alpha String phonebook */
	gboolean b_gas; /**< Grouping information Alpha String phonebook */
};

struct treq_phonebook_get_count {
	enum tel_phonebook_type phonebook_type;
};

struct treq_phonebook_get_info {
	enum tel_phonebook_type phonebook_type;
};

struct treq_phonebook_get_usim_info {

};

struct treq_phonebook_read_record {
	enum tel_phonebook_type phonebook_type;
	unsigned short index;
};

struct treq_phonebook_update_record {
	enum tel_phonebook_type phonebook_type;
	unsigned short index;

	unsigned char name[60];
	enum tel_phonebook_dcs dcs;

	unsigned char number[40];
	enum tel_phonebook_ton ton;

	/* following field is valid in only USIM*/
	unsigned char anr1[40];
	enum tel_phonebook_ton anr1_ton;
	unsigned char anr2[40];
	enum tel_phonebook_ton anr2_ton;
	unsigned char anr3[40];
	enum tel_phonebook_ton anr3_ton;

	unsigned char email1[60];
	unsigned char email2[60];
	unsigned char email3[60];
	unsigned char email4[60];

	unsigned short group_index; //GRP
	unsigned short pb_control; //PBC
};

struct treq_phonebook_delete_record {
	enum tel_phonebook_type phonebook_type;
	unsigned short index;
};
/*WILL BE REMOVED - START*/
struct tresp_phonebook_select {
	enum tel_phonebook_result result;
};
/*WILL BE REMOVED - END*/
struct tresp_phonebook_get_count {
	enum tel_phonebook_result result;
	enum tel_phonebook_type type;
	unsigned short used_count;
	unsigned short total_count;
};

struct tresp_phonebook_get_info {
	enum tel_phonebook_result result;
	enum tel_phonebook_type type;
	unsigned short index_min;
	unsigned short index_max;
	unsigned short number_length_max;
	unsigned short text_length_max;
};

struct tel_phonebook_usim_meta {
	enum tel_phonebook_field_type field;
	unsigned short index_max;
	unsigned short text_max;
	unsigned short used_count;
};

struct tresp_phonebook_get_usim_info {
	enum tel_phonebook_result result;
	int field_count;
	struct tel_phonebook_usim_meta field_list[13]; //supported fields are 13 currently.
};

struct tresp_phonebook_read_record {
	enum tel_phonebook_result result;
	enum tel_phonebook_type phonebook_type;
	unsigned short index;
	unsigned short next_index;

	unsigned char name[60];
	enum tel_phonebook_dcs dcs;

	unsigned char number[40];
	enum tel_phonebook_ton ton;

	/* following field is valid in only USIM*/
	unsigned char anr1[40];
	enum tel_phonebook_ton anr1_ton;
	unsigned char anr2[40];
	enum tel_phonebook_ton anr2_ton;
	unsigned char anr3[40];
	enum tel_phonebook_ton anr3_ton;

	unsigned char email1[60];
	unsigned char email2[60];
	unsigned char email3[60];
	unsigned char email4[60];

	unsigned short group_index; //GRP
	unsigned short pb_control; //PBC
};

struct tresp_phonebook_update_record {
	enum tel_phonebook_result result;
};

struct tresp_phonebook_delete_record {
	enum tel_phonebook_result result;
};

struct tnoti_phonebook_status {
	gboolean b_init;
	struct tel_phonebook_support_list support_list;
};

__END_DECLS


#endif
