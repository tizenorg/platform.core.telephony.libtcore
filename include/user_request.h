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

#ifndef __TCORE_USER_REQUEST_H__
#define __TCORE_USER_REQUEST_H__

__BEGIN_DECLS

typedef void (*UserRequestFreeHook)(UserRequest *ur);
typedef void (*UserRequestResponseHook)(UserRequest *ur,
		enum tcore_response_command command,
		unsigned int data_len, const void *data, void *user_data);

UserRequest *tcore_user_request_new(Communicator *comm, const char *modem_name);
void tcore_user_request_free(UserRequest *ur);

UserRequest *tcore_user_request_ref(UserRequest *ur);
void tcore_user_request_unref(UserRequest *ur);

TReturn tcore_user_request_set_free_hook(UserRequest *ur,
                  UserRequestFreeHook free_hook);
TReturn tcore_user_request_set_response_hook(UserRequest *ur,
                  UserRequestResponseHook resp_hook, void *user_data);

Communicator *tcore_user_request_ref_communicator(UserRequest *ur);
char *tcore_user_request_get_modem_name(UserRequest *ur);

TReturn tcore_user_request_set_user_info(UserRequest *ur, void *user_info);
void *tcore_user_request_ref_user_info(UserRequest *ur);

TReturn tcore_user_request_send_response(UserRequest *ur,
	enum tcore_response_command command,
	unsigned int data_len, const void *data);

TReturn tcore_user_request_set_command(UserRequest *ur,
	enum tcore_request_command command);

enum tcore_request_command tcore_user_request_get_command(UserRequest *ur);

TReturn tcore_user_request_set_data(UserRequest *ur,
	unsigned int data_len, const void *data);
const void *tcore_user_request_ref_data(UserRequest *ur,
	unsigned int *data_len);

TReturn tcore_user_request_set_metainfo(UserRequest *ur,
	unsigned int metainfo_len, const void *metainfo);
const void *tcore_user_request_ref_metainfo(UserRequest *ur,
	unsigned int *metainfo_len);

__END_DECLS

#endif /* __TCORE_USER_REQUEST_H__ */
