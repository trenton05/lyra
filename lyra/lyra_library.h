/*
 * Copyright 2021 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LYRA_H_
#define LYRA_H_

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

void* LyraStartEncode(int hz, int channels, int bitrate, bool dtx, const char* model_path);
void LyraEncode(void* ptr, int bitrate, const char* input, int length, char* output);
void* LyraStartDecode(int hz, int channels, const char* model_path);
void LyraDecode(void* ptr, int bitrate, const char* input, int length, char* output);

#endif  // LYRA_LYRA_DECODER_H_

