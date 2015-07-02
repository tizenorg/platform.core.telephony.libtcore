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

#ifndef __TCORE_CO_GPS_H__
#define __TCORE_CO_GPS_H__

#include <core_object.h>

__BEGIN_DECLS

struct tcore_gps_operations {
	TReturn (*confirm_measure_pos)(CoreObject *o, UserRequest *ur);
	TReturn (*set_frequency_aiding)(CoreObject *o, UserRequest *ur);
	TReturn (*enable_smart_assistant)(CoreObject *o, UserRequest *ur);
	TReturn (*disable_smart_assistant)(CoreObject *o, UserRequest *ur);
	TReturn (*sync_smart_assistant_area_list)(CoreObject *o, UserRequest *ur);
	TReturn (*del_smart_assistant_area_list)(CoreObject *o, UserRequest *ur);
	TReturn (*add_smart_assistant_area)(CoreObject *o, UserRequest *ur);
	TReturn (*modify_smart_assistant_area)(CoreObject *o, UserRequest *ur);
	TReturn (*set_smart_assistant_info)(CoreObject *o, UserRequest *ur);
};

CoreObject *tcore_gps_new(TcorePlugin *p, const char *name, struct tcore_gps_operations *ops, TcoreHal *hal);
void tcore_gps_free(CoreObject *o);
void tcore_gps_set_ops(CoreObject *o, struct tcore_gps_operations *ops, enum tcore_ops_type ops_type);

__END_DECLS

#endif
