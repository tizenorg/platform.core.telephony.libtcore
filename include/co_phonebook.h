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

#ifndef __TCORE_CO_PHONEBOOK_H__
#define __TCORE_CO_PHONEBOOK_H__

#include <core_object.h>

__BEGIN_DECLS

struct tcore_phonebook_operations {
	/*WILL BE REMOVED - START*/
	TReturn (*select)(CoreObject *o, UserRequest *ur);
	/*WILL BE REMOVED - END*/
	TReturn (*get_count)(CoreObject *o, UserRequest *ur);
	TReturn (*get_info)(CoreObject *o, UserRequest *ur);
	TReturn (*get_usim_info)(CoreObject *o, UserRequest *ur);
	TReturn (*read_record)(CoreObject *o, UserRequest *ur);
	TReturn (*update_record)(CoreObject *o, UserRequest *ur);
	TReturn (*delete_record)(CoreObject *o, UserRequest *ur);
};

CoreObject* tcore_phonebook_new(TcorePlugin *p, const char *name, struct tcore_phonebook_operations *ops, TcoreHal *hal);
void        tcore_phonebook_free(CoreObject *n);

gboolean    tcore_phonebook_get_status(CoreObject *o);
gboolean    tcore_phonebook_set_status(CoreObject *o, gboolean b_init);

struct tel_phonebook_support_list* tcore_phonebook_get_support_list(CoreObject *o);
gboolean    tcore_phonebook_set_support_list(CoreObject *o, struct tel_phonebook_support_list *list);

enum tel_phonebook_type tcore_phonebook_get_selected_type(CoreObject *o);
gboolean tcore_phonebook_set_selected_type(CoreObject *o, enum tel_phonebook_type t);

__END_DECLS

#endif
