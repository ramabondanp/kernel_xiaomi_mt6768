// SPDX-License-Identifier: GPL-2.0
/*
 *
 * (C) COPYRIGHT 2020 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 */

#include "mali_kbase_ipa_counter_common_csf.h"
#include "mali_kbase.h"

/* CSHW counter block offsets */
#define MESSAGES_RECEIVED   (9)
#define CEU_ACTIVE          (40)

/* MEMSYS counter block offsets */
#define L2_RD_MSG_IN        (16)
#define L2_WR_MSG_IN_STALL  (19)
#define L2_SNP_MSG_IN       (20)
#define L2_ANY_LOOKUP       (25)
#define L2_EXT_READ_BEATS   (32)
#define L2_EXT_AR_CNT_Q3    (36)
#define L2_EXT_AW_CNT_Q2    (50)

/* SC counter block offsets */
#define FRAG_FPK_ACTIVE     (7)
#define COMPUTE_ACTIVE      (22)
#define EXEC_CORE_ACTIVE    (26)
#define EXEC_STARVE_ARITH   (33)
#define TEX_FILT_NUM_OPS    (39)
#define BEATS_RD_TEX_EXT    (59)

/* Tiler counter block offsets */
#define PRIM_SAT_CULLED     (14)

#define COUNTER_DEF(cnt_name, coeff, cnt_idx, block_type)	\
	{							\
		.name = cnt_name,				\
		.coeff_default_value = coeff,			\
		.counter_block_offset = cnt_idx,		\
		.counter_block_type = block_type,		\
	}

#define CSHW_COUNTER_DEF(cnt_name, coeff, cnt_idx)	\
	COUNTER_DEF(cnt_name, coeff, cnt_idx, KBASE_IPA_CORE_TYPE_CSHW)

#define MEMSYS_COUNTER_DEF(cnt_name, coeff, cnt_idx)	\
	COUNTER_DEF(cnt_name, coeff, cnt_idx, KBASE_IPA_CORE_TYPE_MEMSYS)

#define SC_COUNTER_DEF(cnt_name, coeff, cnt_idx)	\
	COUNTER_DEF(cnt_name, coeff, cnt_idx, KBASE_IPA_CORE_TYPE_SHADER)

#define TILER_COUNTER_DEF(cnt_name, coeff, cnt_idx)	\
	COUNTER_DEF(cnt_name, coeff, cnt_idx, KBASE_IPA_CORE_TYPE_TILER)

/** Table of description of HW counters used by IPA counter model.
 *
 * This table provides a description of each performance counter
 * used by the top level counter model for energy estimation.
 */
static const struct kbase_ipa_counter ipa_top_level_cntrs_def_todx[] = {
	CSHW_COUNTER_DEF("messages_received", 925749, MESSAGES_RECEIVED),
	CSHW_COUNTER_DEF("ceu_active", 25611, CEU_ACTIVE),

	MEMSYS_COUNTER_DEF("l2_ext_read_beats", 3413, L2_EXT_READ_BEATS),
	MEMSYS_COUNTER_DEF("l2_ext_ar_cnt_q3", 8141, L2_EXT_AR_CNT_Q3),
	MEMSYS_COUNTER_DEF("l2_rd_msg_in", 3231, L2_RD_MSG_IN),
	MEMSYS_COUNTER_DEF("l2_ext_aw_cnt_q2", 21714, L2_EXT_AW_CNT_Q2),
	MEMSYS_COUNTER_DEF("l2_any_lookup", 110567, L2_ANY_LOOKUP),
	MEMSYS_COUNTER_DEF("l2_wr_msg_in_stall", -370971, L2_WR_MSG_IN_STALL),
	MEMSYS_COUNTER_DEF("l2_snp_msg_in", 270337, L2_SNP_MSG_IN),

	TILER_COUNTER_DEF("prim_sat_culled", -1094458, PRIM_SAT_CULLED),
};

 /* This table provides a description of each performance counter
  * used by the shader cores counter model for energy estimation.
  */
static const struct kbase_ipa_counter ipa_shader_core_cntrs_def_todx[] = {
	SC_COUNTER_DEF("frag_fpk_active", -91312, FRAG_FPK_ACTIVE),
	SC_COUNTER_DEF("exec_core_active", 485012, EXEC_CORE_ACTIVE),
	SC_COUNTER_DEF("beats_rd_tex_ext", 174174, BEATS_RD_TEX_EXT),
	SC_COUNTER_DEF("tex_filt_num_operations", 164419, TEX_FILT_NUM_OPS),
	SC_COUNTER_DEF("exec_starve_arith", -59107, EXEC_STARVE_ARITH),
	SC_COUNTER_DEF("compute_active", -277940, COMPUTE_ACTIVE),
};

#define IPA_POWER_MODEL_OPS(gpu, init_token) \
	const struct kbase_ipa_model_ops kbase_ ## gpu ## _ipa_model_ops = { \
		.name = "mali-" #gpu "-power-model", \
		.init = kbase_ ## init_token ## _power_model_init, \
		.term = kbase_ipa_counter_common_model_term, \
		.get_dynamic_coeff = kbase_ipa_counter_dynamic_coeff, \
		.reset_counter_data = kbase_ipa_counter_reset_data, \
	}; \
	KBASE_EXPORT_TEST_API(kbase_ ## gpu ## _ipa_model_ops)

#define STANDARD_POWER_MODEL(gpu, reference_voltage) \
	static int kbase_ ## gpu ## _power_model_init(\
			struct kbase_ipa_model *model) \
	{ \
		BUILD_BUG_ON((1 + \
			      ARRAY_SIZE(ipa_top_level_cntrs_def_ ## gpu) +\
			      ARRAY_SIZE(ipa_shader_core_cntrs_def_ ## gpu)) > \
			      KBASE_IPA_MAX_COUNTER_DEF_NUM); \
		return kbase_ipa_counter_common_model_init(model, \
			ipa_top_level_cntrs_def_ ## gpu, \
			ARRAY_SIZE(ipa_top_level_cntrs_def_ ## gpu), \
			ipa_shader_core_cntrs_def_ ## gpu, \
			ARRAY_SIZE(ipa_shader_core_cntrs_def_ ## gpu), \
			(reference_voltage)); \
	} \
	IPA_POWER_MODEL_OPS(gpu, gpu)


#define ALIAS_POWER_MODEL(gpu, as_gpu) \
	IPA_POWER_MODEL_OPS(gpu, as_gpu)

/* Reference voltage value is 750 mV.
 */
STANDARD_POWER_MODEL(todx, 750);

/* Assuming LODX is an alias of TODX for IPA */
ALIAS_POWER_MODEL(lodx, todx);

static const struct kbase_ipa_model_ops *ipa_counter_model_ops[] = {
	&kbase_todx_ipa_model_ops,
	&kbase_lodx_ipa_model_ops
};

const struct kbase_ipa_model_ops *kbase_ipa_counter_model_ops_find(
		struct kbase_device *kbdev, const char *name)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ipa_counter_model_ops); ++i) {
		const struct kbase_ipa_model_ops *ops =
			ipa_counter_model_ops[i];

		if (!strcmp(ops->name, name))
			return ops;
	}

	dev_err(kbdev->dev, "power model \'%s\' not found\n", name);

	return NULL;
}

const char *kbase_ipa_counter_model_name_from_id(u32 gpu_id)
{
	const u32 prod_id = (gpu_id & GPU_ID_VERSION_PRODUCT_ID) >>
			GPU_ID_VERSION_PRODUCT_ID_SHIFT;

	switch (GPU_ID2_MODEL_MATCH_VALUE(prod_id)) {
	case GPU_ID2_PRODUCT_TODX:
		return "mali-todx-power-model";
	case GPU_ID2_PRODUCT_LODX:
		return "mali-lodx-power-model";
	default:
		return NULL;
	}
}