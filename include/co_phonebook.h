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

#ifndef __CO_PHONEBOOK_H__
#define __CO_PHONEBOOK_H__

#include "core_object.h"
#include <tel_phonebook.h>
#include <tel_return.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	TelReturn (*get_info)(CoreObject *co, TelPbType pb_type, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*read_record)(CoreObject *co, const TelPbRecordInfo *record, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*update_record)(CoreObject *co, const TelPbUpdateRecord *req_data, TcoreObjectResponseCallback cb, void *cb_data);
	TelReturn (*delete_record)(CoreObject *co, const TelPbRecordInfo *record, TcoreObjectResponseCallback cb, void *cb_data);
} TcorePbOps;

CoreObject *tcore_phonebook_new(TcorePlugin *p, TcorePbOps *ops, TcoreHal *hal);
void tcore_phonebook_free(CoreObject *co);

gboolean tcore_phonebook_set_ops(CoreObject *co, TcorePbOps *ops);
void tcore_phonebook_override_ops(CoreObject *co, TcorePbOps *phonebook_ops);

gboolean tcore_phonebook_get_status(CoreObject *co, gboolean *init_status);
gboolean tcore_phonebook_set_status(CoreObject *co, gboolean init_status);

gboolean tcore_phonebook_get_support_list(CoreObject *co, TelPbList **pb_list);
gboolean tcore_phonebook_set_support_list(CoreObject *co, TelPbList *pb_list);

gboolean tcore_phonebook_get_selected_type(CoreObject *co, TelPbType *pb_type);
gboolean tcore_phonebook_set_selected_type(CoreObject *co, TelPbType pb_type);

#ifdef __cplusplus
}
#endif

#endif	/*__CO_PHONEBOOK_H__*/
