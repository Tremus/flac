#include "FLAC/stream_encoder.h"
#include "FLAC/metadata.h"

#include <math.h>
#include <assert.h>

#define ARRLEN(a) (sizeof(a) / sizeof(a[0]))

int SINE_PCM[2048];

static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data)
{
	printf("blocksize: %u\nsample rate: %u\nchannels: %u\nbits per sample: %u\n",
		   frame->header.blocksize,
		   frame->header.sample_rate,
		   frame->header.channels,
		   frame->header.bits_per_sample);

	if(frame->header.number_type != FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER)
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

	// frame->header.number.sample_number
	double div = 1;
	if(frame->header.bits_per_sample == 16) {
		div = 1 << 15;
	}
	else if(frame->header.bits_per_sample == 24) {
		div = 1 << 23;
	}
	else if(frame->header.bits_per_sample == 32) {
		div = 1 << 31;
	}

	for(int ch = 0; ch < frame->header.channels; ch++) {
		const int *buf = buffer[ch];
		for(int i = 0; i < frame->header.blocksize; i++) {
			double v = (double)buf[i] / div;
			printf("%d %d: %f\n", ch, i, v);
		}
	}
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
};

void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
	fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

int main()
{
	static const double tau = 6.28318530718;
	double inc = tau / (double)ARRLEN(SINE_PCM);
	double angle_rad = 0;
	for(int i = 0; i < ARRLEN(SINE_PCM); i++) {
		double sample = sin(angle_rad);
		int i32 = (int16_t)(sample * (1 << 15));
		SINE_PCM[i] = i32;
		angle_rad += inc;
	}
	FLAC__StreamEncoder *encoder = FLAC__stream_encoder_new();
	FLAC__StreamEncoderInitStatus encoder_status;
	FLAC__bool ok = FLAC__stream_encoder_set_channels(encoder, 1);
	FLAC__StreamMetadata *metadata[2];
	FLAC__StreamMetadata_VorbisComment_Entry entry;

	FLAC__StreamDecoder *decoder = 0;
	FLAC__StreamDecoderInitStatus decoder_status;

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

	ok = FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ARTIST", "Some Artist");
	assert(ok);
	ok = FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
	assert(ok);
	ok = FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "YEAR", "1984");
	assert(ok);
	ok = FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
	assert(ok);

	ok = FLAC__stream_encoder_set_metadata(encoder, metadata, ARRLEN(metadata));

	encoder_status = FLAC__stream_encoder_init_file(encoder, "filename.flac", NULL, NULL);
	assert(encoder_status == FLAC__STREAM_ENCODER_INIT_STATUS_OK);
	ok = FLAC__stream_encoder_process_interleaved(encoder, SINE_PCM, ARRLEN(SINE_PCM));
	assert(ok);
	ok = FLAC__stream_encoder_finish(encoder);
	assert(ok);

	FLAC__stream_encoder_delete(encoder);

	decoder = FLAC__stream_decoder_new();
	assert(decoder != NULL);
	FLAC__stream_decoder_set_md5_checking(decoder, true);

	decoder_status = FLAC__stream_decoder_init_file(decoder, "filename.flac", write_callback, NULL, error_callback, NULL);
	assert(decoder_status == FLAC__STREAM_DECODER_INIT_STATUS_OK);
	ok = FLAC__stream_decoder_process_until_end_of_stream(decoder);
	FLAC__stream_decoder_delete(decoder);

	return 0;
}
