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

#ifndef __TYPE_GPS_H__
#define __TYPE_GPS_H__

__BEGIN_DECLS

#define SMART_ASSISTANT_AREA_LIST_MAX 10

struct tel_smart_assistant_area {
	unsigned short index;
	unsigned char mode_state;
};

struct tel_smart_assistant_area_list {
	unsigned char count;
	struct tel_smart_assistant_area area[SMART_ASSISTANT_AREA_LIST_MAX];
};

struct treq_gps_set_frequency_aiding {
	unsigned char enable;
};

struct treq_set_smart_assistant_info {
	unsigned short index;
	unsigned char lpp_state;
};

struct tresp_gps_set_frequency_aiding {
	TReturn result;
};

struct tresp_smart_assistant_result {
	TReturn result;
};

struct tnoti_gps_frequency_aiding {
	unsigned char lock_status;
	unsigned char afc_update;
};

struct tnoti_smart_assistant_area_status {
	unsigned char area_status;
	unsigned short index;
};

struct tnoti_smart_assistant_sync_status {
	unsigned char init_status;
	unsigned char init_fail_cause;
};

__END_DECLS

#endif
