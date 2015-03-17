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

#ifndef __TYPE_SOUND_H__
#define __TYPE_SOUND_H__

__BEGIN_DECLS

#include <glib.h>

enum telephony_sound_path {
	XXX_SOUND_PATH_HANDSET		= 0x01,
	XXX_SOUND_PATH_HEADSET,
	XXX_SOUND_PATH_HANDFREE,
	XXX_SOUND_PATH_BLUETOOTH,
	XXX_SOUND_PATH_STEREO_BLUETOOTH,
	XXX_SOUND_PATH_SPEAKER,
	XXX_SOUND_PATH_HEADSET_3_5PI,
	XXX_SOUND_PATH_BT_NSEC_OFF,
	XXX_SOUND_PATH_MIC1,
	XXX_SOUND_PATH_MIC2,
	XXX_SOUND_PATH_HEADSET_HAC,
};

enum telephony_sound_device {
	XXX_SOUND_DEVICE_RECEIVER	= 0x10,
	XXX_SOUND_DEVICE_SPEAKER	= 0x20,
	XXX_SOUND_DEVICE_HFK		= 0x30,
	XXX_SOUND_DEVICE_BLUETOOTH	= 0x40,
	XXX_SOUND_DEVICE_ECHO_CANCELLER	= 0xA0,
};

enum telephony_sound_type {
	XXX_SOUND_TYPE_VOICE	= 0x1,
	XXX_SOUND_TYPE_KEYTONE,
	XXX_SOUND_TYPE_BELL,
	XXX_SOUND_TYPE_MESSAGE,
	XXX_SOUND_TYPE_ALARM,
	XXX_SOUND_TYPE_MISCELLANEOUS,
};

enum telephony_sound_volume_level {
	XXX_SOUND_MUTE,
	XXX_SOUND_VOLUME_LEVEL_1,
	XXX_SOUND_VOLUME_LEVEL_2,
	XXX_SOUND_VOLUME_LEVEL_3,
	XXX_SOUND_VOLUME_LEVEL_4,
	XXX_SOUND_VOLUME_LEVEL_5,
	XXX_SOUND_VOLUME_LEVEL_6,
	XXX_SOUND_VOLUME_LEVEL_7,
	XXX_SOUND_VOLUME_LEVEL_8,
	XXX_SOUND_VOLUME_LEVEL_9,
};


struct treq_sound_set_path {
	enum telephony_sound_path path;
};

struct treq_sound_set_volume_level {
	enum telephony_sound_type			sound;
	enum telephony_sound_device		device;
	enum telephony_sound_volume_level	volume;
};

struct treq_sound_get_volume_level {
	enum telephony_sound_type			sound;
	enum telephony_sound_device		device;
};

__END_DECLS

#endif
