/*
    This file is not used directly. This file was used with the google lyra project to build the library used by lyra.cc.

    Add to lyra directory this and header file, and edit BUILD with:

cc_binary(
    name = "liblyra_library.so",
    srcs = [
        "lyra_library.cc",
    ],
    deps = [
        "//lyra:lyra_config",
        "//lyra:lyra_encoder",
        "//lyra:no_op_preprocessor",
        "//lyra:wav_utils",
        "//lyra:fixed_packet_loss_model",
        "//lyra:gilbert_model",
        "//lyra:lyra_decoder",
        "//lyra:packet_loss_model_interface",
        "@com_google_absl//absl/flags:marshalling",
        "@com_google_absl//absl/random",
        "@com_google_absl//absl/random:bit_gen_ref",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:span",
        "@com_google_glog//:glog",
        "@gulrak_filesystem//:filesystem",
    ],
    linkshared = True,
)


    bazel build -c opt lyra/liblyra_library.so

    Need .so libraries for both mac and linux
*/

#include <string>
#include <vector>

#include "lyra/lyra_config.h"
#include "lyra/lyra_encoder.h"
#include "lyra/lyra_decoder.h"

void* LyraInitEncoder(const char* model_path) {
    return chromemedia::codec::LyraEncoder::Create(16000, 1, 3200, false, model_path).release();
}
void* LyraInitDecoder(const char* model_path) {
    return chromemedia::codec::LyraDecoder::Create(16000, 1, model_path).release();
}

void LyraEncode(void* encoder, int bitrate, const char* in, int length, char* out) {
    chromemedia::codec::LyraEncoder* lyraEncoder = static_cast<chromemedia::codec::LyraEncoder*>(encoder);
    lyraEncoder->set_bitrate(bitrate);

    int batchSize = 640;
    int outOffset = 0;

    std::vector<int16_t> samples_vector(batchSize >> 1);
    for (int i = 0; i + batchSize <= length; i += batchSize) {
        for (int j = 0; j < samples_vector.size(); j++) {
            samples_vector[j] = (in[i + j * 2] & 0xff) | ((in[i + j * 2 + 1] & 0xff) << 8);
        }
        std::vector<uint8_t> encoded = lyraEncoder->Encode(samples_vector).value();
        for (int j = 0; j < encoded.size(); j++) {
            out[outOffset++] = encoded[j];
        }
    }
}

void LyraDecode(void* decoder, int bitrate, const char* in, int length, char* out) {
    chromemedia::codec::LyraDecoder* lyraDecoder = static_cast<chromemedia::codec::LyraDecoder*>(decoder);
    int batchSize = bitrate / 400;
    int outOffset = 0;

    std::vector<uint8_t> samples_vector(batchSize);
    for (int i = 0; i + batchSize <= length; i += batchSize) {
        for (int j = 0; j < samples_vector.size(); j++) {
            samples_vector[j] = in[i + j];
        }
        lyraDecoder->SetEncodedPacket(samples_vector);
        std::vector<int16_t> decoded = lyraDecoder->DecodeSamples(320).value();
        for (int j = 0; j < decoded.size(); j++) {
            out[outOffset++] = (decoded[j] & 0xff);
            out[outOffset++] = (decoded[j] >> 8) & 0xff;
        }
    }
}

