/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2021 Marvell.
 */

#ifndef _TEST_CRYPTODEV_SECURITY_IPSEC_H_
#define _TEST_CRYPTODEV_SECURITY_IPSEC_H_

#include <rte_cryptodev.h>
#include <rte_security.h>

#define IPSEC_TEST_PACKETS_MAX 32

struct ipsec_test_data {
	struct {
		uint8_t data[32];
	} key;
	struct {
		uint8_t data[64];
	} auth_key;

	struct {
		uint8_t data[1024];
		unsigned int len;
	} input_text;

	struct {
		uint8_t data[1024];
		unsigned int len;
	} output_text;

	struct {
		uint8_t data[4];
		unsigned int len;
	} salt;

	struct {
		uint8_t data[16];
	} iv;

	struct rte_security_ipsec_xform ipsec_xform;

	bool aead;

	union {
		struct {
			struct rte_crypto_sym_xform cipher;
			struct rte_crypto_sym_xform auth;
		} chain;
		struct rte_crypto_sym_xform aead;
	} xform;
};

enum df_flags {
	TEST_IPSEC_COPY_DF_INNER_0 = 1,
	TEST_IPSEC_COPY_DF_INNER_1,
	TEST_IPSEC_SET_DF_0_INNER_1,
	TEST_IPSEC_SET_DF_1_INNER_0,
};

struct ipsec_test_flags {
	bool display_alg;
	bool sa_expiry_pkts_soft;
	bool sa_expiry_pkts_hard;
	bool icv_corrupt;
	bool iv_gen;
	uint32_t tunnel_hdr_verify;
	bool udp_encap;
	bool udp_ports_verify;
	bool ip_csum;
	bool l4_csum;
	bool ipv6;
	bool tunnel_ipv6;
	bool transport;
	bool fragment;
	bool stats_success;
	enum df_flags df;
};

struct crypto_param {
	enum rte_crypto_sym_xform_type type;
	union {
		enum rte_crypto_cipher_algorithm cipher;
		enum rte_crypto_auth_algorithm auth;
		enum rte_crypto_aead_algorithm aead;
	} alg;
	uint16_t key_length;
	uint16_t iv_length;
	uint16_t digest_length;
};

static const struct crypto_param aead_list[] = {
	{
		.type = RTE_CRYPTO_SYM_XFORM_AEAD,
		.alg.aead =  RTE_CRYPTO_AEAD_AES_GCM,
		.key_length = 16,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_AEAD,
		.alg.aead = RTE_CRYPTO_AEAD_AES_GCM,
		.key_length = 24,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_AEAD,
		.alg.aead = RTE_CRYPTO_AEAD_AES_GCM,
		.key_length = 32
	},
};

static const struct crypto_param cipher_list[] = {
	{
		.type = RTE_CRYPTO_SYM_XFORM_CIPHER,
		.alg.cipher =  RTE_CRYPTO_CIPHER_NULL,
		.key_length = 0,
		.iv_length = 0,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_CIPHER,
		.alg.cipher =  RTE_CRYPTO_CIPHER_AES_CBC,
		.key_length = 16,
		.iv_length = 16,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_CIPHER,
		.alg.cipher =  RTE_CRYPTO_CIPHER_AES_CTR,
		.key_length = 16,
		.iv_length = 16,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_CIPHER,
		.alg.cipher =  RTE_CRYPTO_CIPHER_AES_CTR,
		.key_length = 24,
		.iv_length = 16,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_CIPHER,
		.alg.cipher =  RTE_CRYPTO_CIPHER_AES_CTR,
		.key_length = 32,
		.iv_length = 16,
	},
};

static const struct crypto_param auth_list[] = {
	{
		.type = RTE_CRYPTO_SYM_XFORM_AUTH,
		.alg.auth =  RTE_CRYPTO_AUTH_NULL,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_AUTH,
		.alg.auth =  RTE_CRYPTO_AUTH_SHA256_HMAC,
		.key_length = 32,
		.digest_length = 16,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_AUTH,
		.alg.auth =  RTE_CRYPTO_AUTH_SHA384_HMAC,
		.key_length = 48,
		.digest_length = 24,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_AUTH,
		.alg.auth =  RTE_CRYPTO_AUTH_SHA512_HMAC,
		.key_length = 64,
		.digest_length = 32,
	},
	{
		.type = RTE_CRYPTO_SYM_XFORM_AUTH,
		.alg.auth =  RTE_CRYPTO_AUTH_AES_XCBC_MAC,
		.key_length = 16,
		.digest_length = 12,
	},
};

struct crypto_param_comb {
	const struct crypto_param *param1;
	const struct crypto_param *param2;
};

extern struct ipsec_test_data pkt_aes_256_gcm;
extern struct ipsec_test_data pkt_aes_256_gcm_v6;
extern struct ipsec_test_data pkt_aes_128_cbc_hmac_sha256;
extern struct ipsec_test_data pkt_aes_128_cbc_hmac_sha256_v6;

extern struct crypto_param_comb alg_list[RTE_DIM(aead_list) +
					 (RTE_DIM(cipher_list) *
					  RTE_DIM(auth_list))];

void test_ipsec_alg_list_populate(void);

int test_ipsec_sec_caps_verify(struct rte_security_ipsec_xform *ipsec_xform,
			       const struct rte_security_capability *sec_cap,
			       bool silent);

int test_ipsec_crypto_caps_aead_verify(
		const struct rte_security_capability *sec_cap,
		struct rte_crypto_sym_xform *aead);

int test_ipsec_crypto_caps_cipher_verify(
		const struct rte_security_capability *sec_cap,
		struct rte_crypto_sym_xform *cipher);

int test_ipsec_crypto_caps_auth_verify(
		const struct rte_security_capability *sec_cap,
		struct rte_crypto_sym_xform *auth);

void test_ipsec_td_in_from_out(const struct ipsec_test_data *td_out,
			       struct ipsec_test_data *td_in);

void test_ipsec_td_prepare(const struct crypto_param *param1,
			   const struct crypto_param *param2,
			   const struct ipsec_test_flags *flags,
			   struct ipsec_test_data *td_array,
			   int nb_td);

void test_ipsec_td_update(struct ipsec_test_data td_inb[],
			  const struct ipsec_test_data td_outb[],
			  int nb_td,
			  const struct ipsec_test_flags *flags);

void test_ipsec_display_alg(const struct crypto_param *param1,
			    const struct crypto_param *param2);

int test_ipsec_post_process(struct rte_mbuf *m,
			    const struct ipsec_test_data *td,
			    struct ipsec_test_data *res_d, bool silent,
			    const struct ipsec_test_flags *flags);

int test_ipsec_status_check(struct rte_crypto_op *op,
			    const struct ipsec_test_flags *flags,
			    enum rte_security_ipsec_sa_direction dir,
			    int pkt_num);

int test_ipsec_stats_verify(struct rte_security_ctx *ctx,
			    struct rte_security_session *sess,
			    const struct ipsec_test_flags *flags,
			    enum rte_security_ipsec_sa_direction dir);

int test_ipsec_pkt_update(uint8_t *pkt, const struct ipsec_test_flags *flags);

#endif
