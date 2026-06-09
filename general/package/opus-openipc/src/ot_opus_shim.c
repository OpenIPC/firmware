/*
 * HiSilicon ot_opus_* shim implementations.
 *
 * The Hi3516CV6xx (V5) vendor MPP audio adapter libss_mpi_audio_adp.so
 * is a closed binary that calls 6 ot_opus_* wrapper functions; vendor
 * ships their definitions only inside its prebuilt libopus.so, with no
 * accompanying source. Mirroring the wrapper here against upstream
 * Opus lets us drop the vendor blob and ship a single open libopus.so
 * that satisfies both the standard `libopus.so.0` SONAME path and the
 * vendor binary's unversioned `libopus.so` DT_NEEDED entry.
 *
 * The 6 entry points, their signatures and their error codes are
 * derived from the public headers (out/include/ot_opusenc.h,
 * out/include/ot_opusdec.h) and from the consumer source
 * (mpp/cbb/audio/adp/src/ot_audio_opus_adp.c) shipped in HiSilicon's
 * Hi3516CV610_SDK_V1.0.2.0.
 */

#include <stddef.h>
#include <opus.h>

/* Opus builds with -fvisibility=hidden — force the wrappers public. */
#define OT_OPUS_EXPORT __attribute__((visibility("default")))

enum {
	OT_ERR_OPUSENC_NONE                   =  0,
	OT_ERR_OPUSENC_NULL_POINTER           = -1,
	OT_ERR_OPUSENC_INVALID_PAYLOAD_LENGTH = -2,
	OT_ERR_OPUSENC_ENCODE_FAILED          = -3,
};

enum {
	OT_ERR_OPUSDEC_NONE                   =  0,
	OT_ERR_OPUSDEC_NOT_ENOUGH_DATA        = -1,
	OT_ERR_OPUSDEC_NULL_POINTER           = -2,
	OT_ERR_OPUSDEC_INVALID_PAYLOAD_LENGTH = -3,
	OT_ERR_OPUSDEC_RANGE_MISMATCH         = -4,
	OT_ERR_OPUSDEC_DECODE_FAILED          = -5,
};

#define OT_OPUSENC_MAX_PACKET      1500
#define OT_OPUSDEC_MAX_OUT_SAMPLES 2880

typedef struct {
	int sample_rate;
	int channels;
	int bit_rate;
	int app;
} ot_opusenc_config;

typedef struct {
	int sample_rate;
	int channels;
} ot_opusdec_config;

OT_OPUS_EXPORT void *ot_opusenc_create(const ot_opusenc_config *config)
{
	int err = 0;
	OpusEncoder *enc;

	if (config == NULL) {
		return NULL;
	}
	enc = opus_encoder_create(config->sample_rate, config->channels,
	                          config->app, &err);
	if (enc == NULL || err != OPUS_OK) {
		if (enc != NULL) {
			opus_encoder_destroy(enc);
		}
		return NULL;
	}
	if (opus_encoder_ctl(enc, OPUS_SET_BITRATE(config->bit_rate)) != OPUS_OK) {
		opus_encoder_destroy(enc);
		return NULL;
	}
	return enc;
}

OT_OPUS_EXPORT void ot_opusenc_destroy(void *encoder)
{
	if (encoder != NULL) {
		opus_encoder_destroy((OpusEncoder *)encoder);
	}
}

OT_OPUS_EXPORT int ot_opusenc_process_frame(void *encoder, short *in_buf, int samples,
                                            unsigned char *out_buf, unsigned int *out_valid_len)
{
	int n;

	if (encoder == NULL || in_buf == NULL || out_buf == NULL ||
	    out_valid_len == NULL) {
		return OT_ERR_OPUSENC_NULL_POINTER;
	}
	n = opus_encode((OpusEncoder *)encoder, in_buf, samples, out_buf,
	                OT_OPUSENC_MAX_PACKET);
	if (n <= 0) {
		return OT_ERR_OPUSENC_ENCODE_FAILED;
	}
	*out_valid_len = (unsigned int)n;
	return OT_ERR_OPUSENC_NONE;
}

OT_OPUS_EXPORT void *ot_opusdec_create(const ot_opusdec_config *config)
{
	int err = 0;
	OpusDecoder *dec;

	if (config == NULL) {
		return NULL;
	}
	dec = opus_decoder_create(config->sample_rate, config->channels, &err);
	if (dec == NULL || err != OPUS_OK) {
		if (dec != NULL) {
			opus_decoder_destroy(dec);
		}
		return NULL;
	}
	return dec;
}

OT_OPUS_EXPORT void ot_opusdec_destroy(void *decoder)
{
	if (decoder != NULL) {
		opus_decoder_destroy((OpusDecoder *)decoder);
	}
}

/*
 * The caller (ot_audio_opus_adp.c) passes one self-contained Opus packet
 * per call and uses the (in_buf, in_left_byte) update to learn how many
 * bytes were consumed. We treat the whole input slice as a single packet
 * and report it fully consumed on success.
 */
OT_OPUS_EXPORT int ot_opusdec_process_frame(void *decoder, unsigned char **in_buf,
                                            int *in_left_byte, short *out_buf,
                                            unsigned int *out_bytes_per_chn)
{
	int samples;

	if (decoder == NULL || in_buf == NULL || *in_buf == NULL ||
	    in_left_byte == NULL || out_buf == NULL || out_bytes_per_chn == NULL) {
		return OT_ERR_OPUSDEC_NULL_POINTER;
	}
	if (*in_left_byte <= 0) {
		return OT_ERR_OPUSDEC_NOT_ENOUGH_DATA;
	}

	samples = opus_decode((OpusDecoder *)decoder, *in_buf, *in_left_byte,
	                      out_buf, OT_OPUSDEC_MAX_OUT_SAMPLES, 0);
	if (samples < 0) {
		return OT_ERR_OPUSDEC_DECODE_FAILED;
	}

	*out_bytes_per_chn = (unsigned int)(samples * (int)sizeof(short));
	*in_buf += *in_left_byte;
	*in_left_byte = 0;
	return OT_ERR_OPUSDEC_NONE;
}
