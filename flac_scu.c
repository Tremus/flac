#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstatic-in-inline"
#define PACKAGE_VERSION "1.3.4"
#define HAVE_LROUND
#define FLAC__HAS_OGG 0

#include <stdint.h>
#ifndef PRIu64
#define PRIu64 "llu"
#endif

#ifdef __aarch64__
#define FLAC__CPU_ARM64
#define FLAC__HAS_NEONINTRIN 1
#define FLAC__HAS_A64NEONINTRIN 1
#endif

#ifdef __APPLE__
#define HAVE_FSEEKO
#endif

#ifdef _WIN32
#define FLAC__NO_DLL
#include "src/share/win_utf8_io/win_utf8_io.c"
#endif

#include "libFLAC/bitmath.c"
#include "libFLAC/bitreader.c"
#include "libFLAC/bitwriter.c"
#include "libFLAC/cpu.c"
#include "libFLAC/crc.c"
#include "libFLAC/fixed.c"
// #include "libFLAC/float.c"
#include "libFLAC/format.c"
#include "libFLAC/lpc.c"

// #include "libFLAC/lpc_intrin_fma.c"
#include "libFLAC/md5.c"
#include "libFLAC/memory.c"
// #include "libFLAC/metadata_iterators.c"
#include "libFLAC/metadata_object.c"
#include "libFLAC/stream_decoder.c"
#include "libFLAC/stream_encoder.c"
#include "libFLAC/stream_encoder_framing.c"
#include "libFLAC/window.c"

#if defined(__x86_64__) || defined(_M_X64)
#define FLAC__HAS_X86INTRIN 1
// #include "libFLAC/fixed_intrin_sse2.c"
// #include "libFLAC/fixed_intrin_ssse3.c"
#include "libFLAC/fixed_intrin_sse42.c"
// #include "libFLAC/fixed_intrin_avx2.c"
// #include "libFLAC/lpc_intrin_sse2.c"
#include "libFLAC/lpc_intrin_sse41.c"
// #include "libFLAC/lpc_intrin_avx2.c"
// #include "libFLAC/stream_encoder_intrin_sse2.c"
#include "libFLAC/stream_encoder_intrin_ssse3.c"
// #include "libFLAC/stream_encoder_intrin_avx2.c"
#elif __aarch64__
#include "libFLAC/lpc_intrin_neon.c"
#endif
#pragma clang diagnostic pop