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

#ifndef __TCORE_CO_SAP_H__
#define __TCORE_CO_SAP_H__

#include <core_object.h>

__BEGIN_DECLS

struct tcore_sap_operations {
	TReturn (*connect)(CoreObject *o, UserRequest *ur);
	TReturn (*disconnect)(CoreObject *o, UserRequest *ur);
	TReturn (*req_status)(CoreObject *o, UserRequest *ur);
	TReturn (*set_transport_protocol)(CoreObject *o, UserRequest *ur);
	TReturn (*set_power)(CoreObject *o, UserRequest *ur);
	TReturn (*get_atr)(CoreObject *o, UserRequest *ur);
	TReturn (*transfer_apdu)(CoreObject *o, UserRequest *ur);
	TReturn (*get_cardreader_status)(CoreObject *o, UserRequest *ur);
};

CoreObject *tcore_sap_new(TcorePlugin *p, const char *name, struct tcore_sap_operations *ops, TcoreHal *hal);
void tcore_sap_free(CoreObject *o);
void tcore_sap_set_ops(CoreObject *o, struct tcore_sap_operations *ops, enum tcore_ops_type ops_type);

__END_DECLS

#endif /* __TCORE_CO_SAP_H__ */
