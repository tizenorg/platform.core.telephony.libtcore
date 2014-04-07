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

#ifndef __CO_GPS_H__
#define __CO_GPS_H__

#include "core_object.h"
#include <tel_gps.h>
#include <tel_return.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    TelReturn (*confirm_measure_pos)(CoreObject *co, const TelGpsDataInfo *gps_data,
				TcoreObjectResponseCallback cb, void *cb_data);
    TelReturn (*set_frequency_aiding)(CoreObject *co, gboolean state,
				TcoreObjectResponseCallback cb, void *cb_data);
} TcoreGpsOps;


CoreObject *tcore_gps_new(TcorePlugin *plugin, TcoreGpsOps *ops, TcoreHal *hal);
void tcore_gps_free(CoreObject *co);

gboolean tcore_gps_set_ops(CoreObject *co, TcoreGpsOps *ops);
void tcore_gps_override_ops(CoreObject *co, TcoreGpsOps *ops);

#ifdef __cplusplus
}
#endif

#endif/* __CO_GPS_H__ */
