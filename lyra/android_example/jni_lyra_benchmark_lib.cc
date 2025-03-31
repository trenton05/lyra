// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
    File for reference only... this was ultimately compiled into liblyra_android_example.so via the following:
    * Take lyra source code and build android_example
    * Modify android_example to rename com.google.android_example.lyra.MainActivity to com.google.Lyra (file + manifest + BUILD)
    * Add to Lyra.java any native methods found in this file
    * Copy this file over the existing file in the example
    * Build the example, find the output apk
    * Unzip the apk and copy the two .so files into the jniLibs folder
*/

#include <jni.h>

#include <string>
#include <vector>

#include "lyra/cli_example/decoder_main_lib.h"
#include "lyra/cli_example/encoder_main_lib.h"
#include "lyra/lyra_config.h"
#include "lyra/lyra_encoder.h"
#include "lyra/lyra_decoder.h"

static chromemedia::codec::LyraDecoder* lyraDecoders[3] = {nullptr, nullptr, nullptr};
static chromemedia::codec::LyraEncoder* lyraEncoders[3] = {nullptr, nullptr, nullptr};

const char* cpp_model_base_path;

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_google_Lyra_encodeRaw(
    JNIEnv* env, jobject this_obj, jstring model_base_path, jint bitrate, jshortArray samples, jint sample_length) {
    int index = bitrate <= 3200 ? 0 : (bitrate <= 6400 ? 1 : 2);
    if (lyraEncoders[index] == nullptr) {
        cpp_model_base_path = env->GetStringUTFChars(model_base_path, nullptr);
        lyraEncoders[index] = chromemedia::codec::LyraEncoder::Create(
            16000, 1, bitrate, false, cpp_model_base_path).release();
    }
    
    std::vector<int16_t> samples_vector(sample_length);
    jbyteArray java_encoded = nullptr;
    env->GetShortArrayRegion(samples, jsize{0}, sample_length,
                           &samples_vector[0]);
    std::vector<uint8_t> encoded = lyraEncoders[index]->Encode(samples_vector).value();
    java_encoded = env->NewByteArray(encoded.size());
    env->SetByteArrayRegion(java_encoded, 0, encoded.size(),
                             (jbyte*) &encoded[0]);
    return java_encoded;
}

extern "C" JNIEXPORT jshortArray JNICALL
Java_com_google_Lyra_decodeRaw(
    JNIEnv* env, jobject this_obj, jstring model_base_path, jint bitrate, jbyteArray samples, jint sample_length) {
    int index = bitrate <= 3200 ? 0 : (bitrate <= 6400 ? 1 : 2);
    if (lyraDecoders[index] == nullptr) {
        cpp_model_base_path = env->GetStringUTFChars(model_base_path, nullptr);
        lyraDecoders[index] = chromemedia::codec::LyraDecoder::Create(
            16000, 1, cpp_model_base_path).release();
    }
    
    std::vector<uint8_t> samples_vector(sample_length);
    jshortArray java_decoded = nullptr;
    env->GetByteArrayRegion(samples, jsize{0}, sample_length,
                           (jbyte*) &samples_vector[0]);
    lyraDecoders[index]->SetEncodedPacket(samples_vector);
    std::vector<int16_t> decoded = lyraDecoders[index]->DecodeSamples(320).value();
    java_decoded = env->NewShortArray(decoded.size());
    env->SetShortArrayRegion(java_decoded, 0, decoded.size(),
                             &decoded[0]);
    return java_decoded;
}

