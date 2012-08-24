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

#ifndef __TCORE_CO_SIM_H__
#define __TCORE_CO_SIM_H__

#include <core_object.h>

__BEGIN_DECLS

struct tcore_sim_operations {
	TReturn (*verify_pins)(CoreObject *o, UserRequest *ur);
	TReturn (*verify_puks)(CoreObject *o, UserRequest *ur);
	TReturn (*change_pins)(CoreObject *o, UserRequest *ur);
	TReturn (*get_facility_status)(CoreObject *o, UserRequest *ur);
	TReturn (*enable_facility)(CoreObject *o, UserRequest *ur);
	TReturn (*disable_facility)(CoreObject *o, UserRequest *ur);
	TReturn (*get_lock_info)(CoreObject *o, UserRequest *ur);
	TReturn (*read_file)(CoreObject *o, UserRequest *ur);
	TReturn (*update_file)(CoreObject *o, UserRequest *ur);
	TReturn (*transmit_apdu)(CoreObject *o, UserRequest *ur);
	TReturn (*get_atr)(CoreObject *o, UserRequest *ur);
	TReturn (*req_authentication)(CoreObject *o, UserRequest *ur);
};

CoreObject*          tcore_sim_new(TcorePlugin *p, const char *name, struct tcore_sim_operations *ops, TcoreHal *hal);
void                 tcore_sim_free(CoreObject *n);

enum tel_sim_type    tcore_sim_get_type(CoreObject *o);
gboolean             tcore_sim_set_type(CoreObject *o, enum tel_sim_type type);

struct tel_sim_imsi* tcore_sim_get_imsi(CoreObject *o);
gboolean             tcore_sim_set_imsi(CoreObject *o, struct tel_sim_imsi *imsi);

enum tel_sim_status  tcore_sim_get_status(CoreObject *o);
gboolean             tcore_sim_set_status(CoreObject *o, enum tel_sim_status status);

gboolean             tcore_sim_get_identification(CoreObject *o);
gboolean             tcore_sim_set_identification(CoreObject *o, gboolean b_changed);

gboolean             tcore_sim_get_cphs_status(CoreObject *o);
gboolean             tcore_sim_set_cphs_status(CoreObject *o, gboolean b_support);

gboolean             tcore_sim_link_userdata(CoreObject *o, void *userdata);
void*                tcore_sim_ref_userdata(CoreObject *o);

gboolean             tcore_sim_decode_iccid(struct tel_sim_iccid *p_out, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_lp(struct tel_sim_language *p_out, unsigned char *p_in, int in_length);
char*             tcore_sim_encode_lp(int *out_length, struct tel_sim_language *p_in);
gboolean             tcore_sim_decode_li(enum tel_sim_file_id file_id, struct tel_sim_language *p_out, unsigned char *p_in, int in_length);
char*             tcore_sim_encode_li(int *out_length, struct tel_sim_language *p_in);
gboolean             tcore_sim_decode_imsi(struct tel_sim_imsi *p_out, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_sst(struct tel_sim_sst *p_sst, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_spn(struct tel_sim_spn *p_spn, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_spdi(struct tel_sim_spdi *p_spdi, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_msisdn(struct tel_sim_msisdn *p_msisdn, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_xdn(struct tel_sim_dialing_number *p_xdn, unsigned char *p_in, int in_length);
gboolean             tcore_sim_encode_xdn(char *p_out, int out_length, struct tel_sim_dialing_number *p_xdn);
gboolean             tcore_sim_decode_ecc(struct tel_sim_ecc_list *p_ecc, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_ust(struct tel_sim_ust *p_ust, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_est(struct tel_sim_est *p_est, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_uecc(struct tel_sim_ecc *p_ecc, unsigned char* p_in, int in_length);
gboolean             tcore_sim_decode_gid( struct tel_sim_gid *p_gid, unsigned char* p_in, int in_length);
gboolean             tcore_sim_decode_mbi(struct tel_sim_mbi *p_mbi, unsigned char *p_in, int in_length);
gboolean             tcore_sim_encode_mbi(char *p_out, int out_length, struct tel_sim_mbi *p_mbi);
gboolean             tcore_sim_decode_cff(struct tel_sim_callforwarding *cfis, unsigned char *p_in, int in_length);
char*                tcore_sim_encode_cff(const struct tel_sim_callforwarding *cff);
gboolean             tcore_sim_decode_csp(struct tel_sim_cphs_csp *p_csp, unsigned char *p_in, int in_length);
gboolean             tcore_sim_encode_csp(unsigned char *p_out, int out_length, struct tel_sim_cphs_csp *p_csp);
gboolean             tcore_sim_decode_vmwf(struct tel_sim_cphs_mw *p_vmwf,	unsigned char* p_in, unsigned long in_length);
gboolean             tcore_sim_decode_mwis(struct tel_sim_mw *pMwis, unsigned char *p_in, int in_length);
gboolean             tcore_sim_encode_mwis( char *p_out, int out_length, struct tel_sim_mw *pMwis);
gboolean             tcore_sim_encode_vmwf(char *p_out, int out_length, struct tel_sim_cphs_mw *p_vmwf);
gboolean             tcore_sim_decode_ons(unsigned char *p_out, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_cfis(struct tel_sim_callforwarding *cfis, unsigned char *p_in, int in_length);
char*             tcore_sim_encode_cfis(int *out_length, const struct tel_sim_callforwarding *p_cfis);
gboolean             tcore_sim_decode_dynamic_flag(struct tel_sim_cphs_dflag *p_df, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_dynamic2_flag(struct tel_sim_cphs_dflag2 *p_d2f, unsigned char *p_in, int in_length);
gboolean             tcore_sim_encode_dynamic_flag(char *p_out, int out_length, struct tel_sim_cphs_dflag *p_df);
gboolean             tcore_sim_encode_dynamic2_flag(char *p_out, int out_length, struct tel_sim_cphs_dflag2 *p_d2f);
gboolean             tcore_sim_decode_cphs_info(struct tel_sim_cphs_info *pCphsInfo, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_short_ons(unsigned char *p_out, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_information_number(struct tel_sim_cphs_info_number *p_info, unsigned char* p_in, int in_length);
gboolean             tcore_sim_decode_opl(struct tel_sim_opl *p_opl, unsigned char *p_in, int in_length);
gboolean             tcore_sim_decode_pnn(struct tel_sim_pnn *p_pnn, unsigned char* p_in, int in_length);
gboolean             tcore_sim_decode_oplmnwact(struct tel_sim_oplmnwact_list *p_list, unsigned char *p_in, int in_length);

__END_DECLS

#endif
