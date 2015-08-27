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
	TReturn (*set_powerstate)(CoreObject *o, UserRequest *ur);
	TReturn (*set_provisioning)(CoreObject *o, UserRequest *ur);
};

enum tcore_sim_file_type_e {
	SIM_FTYPE_DEDICATED = 0x00, /**< Dedicated */
	SIM_FTYPE_TRANSPARENT = 0x01, /**< Transparent -binary type*/
	SIM_FTYPE_LINEAR_FIXED = 0x02, /**< Linear fixed - record type*/
	SIM_FTYPE_CYCLIC = 0x06, /**< Cyclic - record type*/
	SIM_FTYPE_INVALID_TYPE = 0xFF /**< Invalid type */
};

struct tcore_sim_ef_info {
	enum tcore_sim_file_type_e file_type;
	unsigned short file_id;
	unsigned int file_size;
	int record_length;
	int number_of_records;
};

CoreObject *tcore_sim_new(TcorePlugin *p, const char *name, struct tcore_sim_operations *ops, TcoreHal *hal);
void tcore_sim_free(CoreObject *n);

void tcore_sim_set_ops(CoreObject *o, struct tcore_sim_operations *ops, enum tcore_ops_type ops_type);

enum tel_sim_type tcore_sim_get_type(CoreObject *o);
gboolean tcore_sim_set_type(CoreObject *o, enum tel_sim_type type);

struct tel_sim_imsi *tcore_sim_get_imsi(CoreObject *o);
gboolean tcore_sim_set_imsi(CoreObject *o, const struct tel_sim_imsi *imsi);

enum tel_sim_status tcore_sim_get_status(CoreObject *o);
gboolean tcore_sim_set_status(CoreObject *o, enum tel_sim_status status);

gboolean tcore_sim_get_identification(CoreObject *o);
gboolean tcore_sim_set_identification(CoreObject *o, gboolean b_changed);

struct tel_sim_service_table *tcore_sim_get_service_table(CoreObject *o);
gboolean tcore_sim_set_service_table(CoreObject *o, const struct tel_sim_service_table *svct);
enum tcore_return tcore_sim_delete_service_table(CoreObject *o);

struct tel_sim_cphs_csp *tcore_sim_get_csp(CoreObject *o);
gboolean tcore_sim_set_csp(CoreObject *o, const struct tel_sim_cphs_csp *csp);
gboolean tcore_sim_delete_csp(CoreObject *o);


struct tel_sim_ecc_list *tcore_sim_get_ecc_list(CoreObject *o);
gboolean tcore_sim_set_ecc_list(CoreObject *o, const struct tel_sim_ecc_list *ecc_list);
enum tcore_return tcore_sim_delete_ecc_list(CoreObject *o);

struct tel_sim_msisdn_list *tcore_sim_get_msisdn_list(CoreObject *o);
gboolean tcore_sim_set_msisdn_list(CoreObject *o, const struct tel_sim_msisdn_list *msisdn_list);
enum tcore_return tcore_sim_delete_msisdn_list(CoreObject *o);

struct tel_sim_spn *tcore_sim_get_spn(CoreObject *o);
gboolean tcore_sim_set_spn(CoreObject *o, const struct tel_sim_spn *spn);
enum tcore_return tcore_sim_delete_spn(CoreObject *o);

struct tel_sim_cphs_netname *tcore_sim_get_cphs_netname(CoreObject *o);
gboolean tcore_sim_set_cphs_netname(CoreObject *o, const struct tel_sim_cphs_netname *cphs_netname);
enum tcore_return tcore_sim_delete_cphs_netname(CoreObject *o);

struct tel_sim_iccid *tcore_sim_get_iccid(CoreObject *o);
gboolean tcore_sim_set_iccid(CoreObject *o, const struct tel_sim_iccid *iccid);
enum tcore_return tcore_sim_delete_iccid(CoreObject *o);

gboolean tcore_sim_get_cphs_status(CoreObject *o);
gboolean tcore_sim_set_cphs_status(CoreObject *o, gboolean b_support);
unsigned char tcore_sim_get_app_list(CoreObject *o);
gboolean tcore_sim_set_app_list(CoreObject *o, unsigned char app_list);
struct tel_sim_ist *tcore_sim_get_isim_service_table(CoreObject *o);
gboolean tcore_sim_set_isim_service_table(CoreObject *o, struct tel_sim_ist *ist);
enum tcore_return tcore_sim_delete_isim_service_table(CoreObject *o);

gboolean tcore_sim_link_userdata(CoreObject *o, void *userdata);
void *tcore_sim_ref_userdata(CoreObject *o);

gboolean tcore_sim_decode_iccid(struct tel_sim_iccid *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_lp(struct tel_sim_language *p_out, unsigned char *p_in, int in_length);
char *tcore_sim_encode_lp(int *out_length, struct tel_sim_language *p_in);
gboolean tcore_sim_decode_li(enum tel_sim_file_id file_id, struct tel_sim_language *p_out, unsigned char *p_in, int in_length);
char *tcore_sim_encode_li(int *out_length, struct tel_sim_language *p_in);
gboolean tcore_sim_decode_imsi(struct tel_sim_imsi *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_cdma_imsi(struct tel_sim_imsi *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_sst(struct tel_sim_sst *p_sst, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_cdma_st(struct tel_sim_cst *p_cdma_st, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_csim_st(struct tel_sim_cst *p_csim_st, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_spn(struct tel_sim_spn *p_spn, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_cdma_spn(struct tel_sim_spn *p_spn, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_spdi(struct tel_sim_spdi *p_spdi, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_msisdn(struct tel_sim_msisdn *p_msisdn, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_mdn(struct tel_sim_msisdn *p_msisdn, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_xdn(struct tel_sim_dialing_number *p_xdn, unsigned char *p_in, int in_length);
char *tcore_sim_encode_xdn(int in_length, struct tel_sim_dialing_number *p_xdn);
gboolean tcore_sim_decode_ecc(struct tel_sim_ecc_list *p_ecc, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_ust(struct tel_sim_ust *p_ust, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_ist(struct tel_sim_ist *p_ist, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_est(struct tel_sim_est *p_est, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_uecc(struct tel_sim_ecc *p_ecc, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_gid( struct tel_sim_gid *p_gid, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_mbi(struct tel_sim_mbi *p_mbi, unsigned char *p_in, int in_length);
char *tcore_sim_encode_mbi(const struct tel_sim_mbi *p_mbi, int in_length);
gboolean tcore_sim_decode_mwis(struct tel_sim_mw *pMwis, unsigned char *p_in, int in_length);
char *tcore_sim_encode_mwis(int *out_length, const struct tel_sim_mw *pMwis, int in_length);
gboolean tcore_sim_decode_vmwf(struct tel_sim_cphs_mw *p_vmwf, unsigned char *p_in, unsigned long in_length);
char *tcore_sim_encode_vmwf(int *out_length, const struct tel_sim_cphs_mw *p_vmwf, int in_length);
gboolean tcore_sim_decode_cfis(struct tel_sim_cfis *cfis, unsigned char *p_in, int in_length);
char *tcore_sim_encode_cfis(int *out_length, const struct tel_sim_cfis *p_cfis);
gboolean tcore_sim_decode_cff(struct tel_sim_cphs_cf *cfis, unsigned char *p_in, int in_length);
char *tcore_sim_encode_cff(const struct tel_sim_cphs_cf *cff, int in_length);
gboolean tcore_sim_decode_csp(struct tel_sim_cphs_csp *p_csp, unsigned char *p_in, int in_length);
gboolean tcore_sim_encode_csp(unsigned char *p_out, int out_length, struct tel_sim_cphs_csp *p_csp);
gboolean tcore_sim_decode_dynamic_flag(struct tel_sim_cphs_dflag *p_df, unsigned char *p_in, int in_length);
gboolean tcore_sim_encode_dynamic_flag(char *p_out, int out_length, struct tel_sim_cphs_dflag *p_df);
gboolean tcore_sim_decode_dynamic2_flag(struct tel_sim_cphs_dflag2 *p_d2f, unsigned char *p_in, int in_length);
gboolean tcore_sim_encode_dynamic2_flag(char *p_out, int out_length, struct tel_sim_cphs_dflag2 *p_d2f);
gboolean tcore_sim_decode_cphs_info(struct tel_sim_cphs_info *pCphsInfo, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_short_ons(unsigned char *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_ons(unsigned char *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_information_number(struct tel_sim_cphs_info_number *p_info, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_opl(struct tel_sim_opl *p_opl, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_pnn(struct tel_sim_pnn *p_pnn, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_oplmnwact(struct tel_sim_oplmnwact_list *p_list, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_ext(struct tel_sim_ext *p_ext, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_img(struct tel_sim_img *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_check_plmn_having_3digits_mnc(char *plmn);
gboolean tcore_sim_decode_isim_impi(struct tel_sim_impi *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_isim_domain(struct tel_sim_domain *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_isim_impu(struct tel_sim_impu *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_isim_pcscf(struct tel_sim_pcscf *p_out, unsigned char *p_in, int in_length);
gboolean tcore_sim_decode_ef_info(struct tcore_sim_ef_info *p_ef_info, CoreObject *o, char *p_in, int in_length);

__END_DECLS

#endif /* __TCORE_CO_SIM_H__ */
