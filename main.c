#include "FLAC/stream_encoder.h"
#include "FLAC/metadata.h"

#include <math.h>
#include <assert.h>

#define ARRLEN(a) (sizeof(a) / sizeof(a[0]))

int SINE_PCM[2048];

int main()
{
	static const float tau = 6.28318530718;
	float inc = tau / (float)ARRLEN(SINE_PCM);
	float angle_rad = 0;
	for(int i = 0; i < ARRLEN(SINE_PCM); i++) {
		float sample = sinf(angle_rad);
		int i32 = (int16_t)(sample * INT16_MAX);
		SINE_PCM[i] = i32;
		angle_rad += inc;
	}
	FLAC__StreamEncoder *encoder = FLAC__stream_encoder_new();
	FLAC__bool ok = FLAC__stream_encoder_set_channels(encoder, 1);
	FLAC__StreamMetadata *metadata[2];

	assert(ok);
	ok = FLAC__stream_encoder_set_bits_per_sample(encoder, 16);
	assert(ok);
	ok = FLAC__stream_encoder_set_sample_rate(encoder, 44100);
	assert(ok);
	ok = FLAC__stream_encoder_set_total_samples_estimate(encoder, ARRLEN(SINE_PCM));
	assert(ok);
	ok = FLAC__stream_encoder_set_compression_level(encoder, 5);
	assert(ok);
	ok = FLAC__stream_encoder_set_verify(encoder, true);
	assert(ok);

	metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);
	assert(metadata[0] != NULL);
	metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING);
	assert(metadata[1] != NULL);

	FLAC__StreamMetadata_VorbisComment_Entry entry;
	ok = FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ARTIST", "Some Artist");
	assert(ok);
	ok = FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
	assert(ok);
	ok = FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "YEAR", "1984");
	assert(ok);
	ok = FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
	assert(ok);

	ok = FLAC__stream_encoder_set_metadata(encoder, metadata, ARRLEN(metadata));

	FLAC__StreamEncoderInitStatus status = FLAC__stream_encoder_init_file(encoder, "filename.flac", NULL, NULL);
	assert(status == FLAC__STREAM_ENCODER_INIT_STATUS_OK);
	ok = FLAC__stream_encoder_process_interleaved(encoder, SINE_PCM, ARRLEN(SINE_PCM));
	assert(ok);
	ok = FLAC__stream_encoder_finish(encoder);
	assert(ok);

	FLAC__stream_encoder_delete(encoder);
	return 0;
fail:
	FLAC__stream_encoder_delete(encoder);
	return 1;
}
