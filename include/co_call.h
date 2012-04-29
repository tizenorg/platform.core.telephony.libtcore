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

#ifndef __TCORE_CO_CALL_H__
#define __TCORE_CO_CALL_H__

#include <core_object.h>

__BEGIN_DECLS

#define MAX_CALL_NUMBER_LEN			83
#define MAX_CALL_NAME_LEN			83

enum tcore_call_type {
	TCORE_CALL_TYPE_VOICE,
	TCORE_CALL_TYPE_VIDEO,
	TCORE_CALL_TYPE_E911,
};

enum tcore_call_direction {
	TCORE_CALL_DIRECTION_NONE,
	TCORE_CALL_DIRECTION_OUTGOING,
	TCORE_CALL_DIRECTION_INCOMING,
};

enum tcore_call_status {
	TCORE_CALL_STATUS_IDLE,
	TCORE_CALL_STATUS_ACTIVE,
	TCORE_CALL_STATUS_HELD,
	TCORE_CALL_STATUS_DIALING,
	TCORE_CALL_STATUS_ALERT,
	TCORE_CALL_STATUS_INCOMING,
	TCORE_CALL_STATUS_WAITING,
};

enum tcore_call_cli_mode {
	TCORE_CALL_CLI_MODE_DEFAULT,
	TCORE_CALL_CLI_MODE_PRESENT,
	TCORE_CALL_CLI_MODE_RESTRICT,
};

enum tcore_call_cna_mode {
	TCORE_CALL_CNA_MODE_DEFAULT,
	TCORE_CALL_CNA_MODE_PRESENT,
	TCORE_CALL_CNA_MODE_RESTRICT,
};

enum tcore_call_answer_type {
	TCORE_CALL_ANSWER_ACCEPT,
	TCORE_CALL_ANSWER_REJECT,
	TCORE_CALL_ANSWER_REPLACE,
	TCORE_CALL_ANSWER_HOLD_AND_ACCEPT,
};

enum tcore_call_end_type {
	TCORE_CALL_END_DEFAULT,
	TCORE_CALL_END_ALL,
	TCORE_CALL_END_ALL_ACTIVE,
	TCORE_CALL_END_ALL_HELD,
};

typedef struct call_object CallObject;

struct tcore_call_operations {
	TReturn (*dial)(CoreObject *o, UserRequest *ur);
	TReturn (*answer)(CoreObject *o, UserRequest *ur);
	TReturn (*end)(CoreObject *o, UserRequest *ur);
	TReturn (*hold)(CoreObject *o, UserRequest *ur);
	TReturn (*active)(CoreObject *o, UserRequest *ur);
	TReturn (*swap)(CoreObject *o, UserRequest *ur);
	TReturn (*join)(CoreObject *o, UserRequest *ur);
	TReturn (*split)(CoreObject *o, UserRequest *ur);
	TReturn (*deflect)(CoreObject* o, UserRequest *ur);
	TReturn (*transfer)(CoreObject* o, UserRequest *ur);
	TReturn (*send_dtmf)(CoreObject *o, UserRequest *ur);
	TReturn (*set_sound_path)(CoreObject *o, UserRequest *ur);
	TReturn (*set_sound_volume_level)(CoreObject *o, UserRequest *ur);
	TReturn (*get_sound_volume_level)(CoreObject *o, UserRequest *ur);
	TReturn (*mute)(CoreObject *o, UserRequest *ur);
	TReturn (*unmute)(CoreObject *o, UserRequest *ur);
	TReturn (*get_mute_status)(CoreObject *o, UserRequest *ur);
	TReturn (*set_active_line)(CoreObject *o, UserRequest *ur);
	TReturn (*get_active_line)(CoreObject *o, UserRequest *ur);
	TReturn (*activate_ccbs)(CoreObject *o, UserRequest *ur);
};

struct tcore_call_information_operations {
	void (*mo_call_col)(CoreObject *o, char* number);
	void (*mo_call_waiting)(CoreObject *o);
	void (*mo_call_cug)(CoreObject *o, int cug_index);
	void (*mo_call_forwarded)(CoreObject *o);
	void (*mo_call_barred_incoming)(CoreObject *o);
	void (*mo_call_barred_outgoing)(CoreObject *o);
	void (*mo_call_deflected)(CoreObject *o);
	void (*mo_call_clir_suppression_reject)(CoreObject *o);
	void (*mo_call_cfu)(CoreObject *o);
	void (*mo_call_cfc)(CoreObject *o);
	void (*mt_call_cli)(CoreObject *o, enum tcore_call_cli_mode mode, char *number);
	void (*mt_call_cna)(CoreObject *o, enum tcore_call_cna_mode mode, char *name, int dcs);
	void (*mt_call_forwarded_call)(CoreObject *o, char* number);
	void (*mt_call_cug_call)(CoreObject *o, int cug_index, char* number);
	void (*mt_call_deflected_call)(CoreObject *o, char* number);
	void (*mt_call_transfered)(CoreObject *o, char* number);
	void (*call_held)(CoreObject *o, char* number);
	void (*call_active)(CoreObject *o, char* number);
	void (*call_joined)(CoreObject *o, char* number);
	void (*call_released_on_hold)(CoreObject *o, char* number);
	void (*call_transfer_alert)(CoreObject *o, char* number);
	void (*call_transfered)(CoreObject *o, char* number);
	void (*call_cf_check_message)(CoreObject *o, char* number);
};

typedef void(*ConfirmCallback)(TcorePending *p, int data_len, const void *data, void *user_data);

struct tcore_call_control_operations {
	TReturn (*answer_hold_and_accept)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*answer_replace)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*answer_reject)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*end_specific)( CoreObject* o, UserRequest* ur, const int id, ConfirmCallback cb, void* user_data );
	TReturn (*end_all_active)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*end_all_held)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*active)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*hold)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*swap)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*join)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*split)( CoreObject* o, UserRequest* ur, const int id, ConfirmCallback cb, void* user_data );
	TReturn (*transfer)( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
	TReturn (*deflect)( CoreObject* o, UserRequest* ur, const char* number, ConfirmCallback cb, void* user_data );
};

// Call Core API
CoreObject*				tcore_call_new(TcorePlugin *p, const char *name, struct tcore_call_operations *ops);
void					tcore_call_free( CoreObject *o);


// Call Object API
CallObject*				tcore_call_object_new( CoreObject *o, int id );
gboolean				tcore_call_object_free( CoreObject *o, CallObject *co );

CallObject*				tcore_call_object_current_on_mt_processing( CoreObject *o );
CallObject*				tcore_call_object_current_on_mo_processing( CoreObject *o );
CallObject*				tcore_call_object_find_by_id( CoreObject *o, int id );
CallObject*				tcore_call_object_find_by_number( CoreObject *o, char *num );
GSList*					tcore_call_object_find_by_status( CoreObject *o, enum tcore_call_status cs );

int						tcore_call_object_get_id( CallObject *co );

gboolean				tcore_call_object_set_type( CallObject *co, enum tcore_call_type ct );
enum tcore_call_type	tcore_call_object_get_type( CallObject *co );

gboolean				tcore_call_object_set_direction( CallObject *co, enum tcore_call_direction cd );
enum tcore_call_direction tcore_call_object_get_direction( CallObject *co );

gboolean				tcore_call_object_set_status( CallObject *co, enum tcore_call_status cs );
enum tcore_call_status  tcore_call_object_get_status( CallObject *co );

gboolean				tcore_call_object_set_cli_info( CallObject *co, enum tcore_call_cli_mode mode, char *num );
enum tcore_call_cli_mode tcore_call_object_get_cli_mode( CallObject *co );
int						tcore_call_object_get_number( CallObject *co, char *num );

gboolean				tcore_call_object_set_cna_info( CallObject *co, enum tcore_call_cna_mode mode, char *name, int dcs );
enum tcore_call_cna_mode tcore_call_object_get_cna_mode( CallObject *co );
int						tcore_call_object_get_name( CallObject *co, char *name );

gboolean				tcore_call_object_set_multiparty_state ( CallObject *co,  gboolean is );
gboolean				tcore_call_object_get_multiparty_state ( CallObject *co );

gboolean				tcore_call_object_set_active_line( CallObject *co, unsigned int line );
int						tcore_call_object_get_active_line( CallObject *co );


// Call Control API
gboolean				tcore_call_control_new( CoreObject *o, struct tcore_call_control_operations *ops );
void					tcore_call_control_free( CoreObject *o );

TReturn					tcore_call_control_answer_hold_and_accept( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_answer_replace( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_answer_reject( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );

TReturn					tcore_call_control_end_specific( CoreObject* o, UserRequest* ur, const int id, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_end_all_active( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_end_all_held( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );

TReturn					tcore_call_control_active( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_hold( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_swap( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_join( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_split( CoreObject* o, UserRequest* ur, const int id, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_transfer( CoreObject* o, UserRequest* ur, ConfirmCallback cb, void* user_data );
TReturn					tcore_call_control_deflect( CoreObject* o, UserRequest* ur, const char* number, ConfirmCallback cb, void* user_data );

void					tcore_call_control_set_operations( CoreObject* o, struct tcore_call_control_operations *ops );


void					tcore_call_information_mo_col( CoreObject *o, char* number );
void					tcore_call_information_mo_waiting( CoreObject *o );
void					tcore_call_information_mo_cug( CoreObject *o, int cug_index );
void					tcore_call_information_mo_forwarded( CoreObject *o );
void					tcore_call_information_mo_barred_incoming( CoreObject *o );
void					tcore_call_information_mo_barred_outgoing( CoreObject *o );
void					tcore_call_information_mo_deflected( CoreObject *o );
void					tcore_call_information_mo_clir_suppression_reject( CoreObject *o );
void					tcore_call_information_mo_cfu( CoreObject *o );
void					tcore_call_information_mo_cfc( CoreObject *o );

void					tcore_call_information_mt_cli( CoreObject *o, enum tcore_call_cli_mode mode, char* number );
void					tcore_call_information_mt_cna( CoreObject *o, enum tcore_call_cna_mode mode, char* name, int dcs );
void					tcore_call_information_mt_forwarded_call( CoreObject *o, char* number );
void					tcore_call_information_mt_cug_call( CoreObject *o, int cug_index, char* number );
void					tcore_call_information_mt_deflected_call( CoreObject *o, char* number );
void					tcore_call_information_mt_transfered( CoreObject *o, char* number );

void					tcore_call_information_held( CoreObject *o, char* number );
void					tcore_call_information_active( CoreObject *o, char* number );
void					tcore_call_information_joined( CoreObject *o, char* number );
void					tcore_call_information_released_on_hold( CoreObject *o, char* number );
void					tcore_call_information_transfer_alert( CoreObject *o, char* number );
void					tcore_call_information_transfered( CoreObject *o, char* number );
void					tcore_call_information_cf_check_ss_message( CoreObject *o, char* number );

void					tcore_call_information_set_operations( CoreObject *o, struct tcore_call_information_operations *ops );

__END_DECLS

#endif
