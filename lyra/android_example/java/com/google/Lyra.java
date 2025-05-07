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

package com.google;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * The main activity for the Lyra android example.
 * It features benchmarking to logcat and coding lyra from the mic.
 * See jni_lyra_benchmark_lib.cc for instructions on re-building the jniLibs .so file used by this
 */
public class Lyra {
  private static final String TAG = "MainActivity";

  static {
    System.loadLibrary("lyra_android_example");
  }

  public static Lyra instance;
  public static void init(Context context) {
    instance = new Lyra(context);
    instance.init();
  }
  private Context context;
  private Lyra(Context context) {
      this.context = context;
  }

  private String weightsDirectory;
  public void init() {
    weightsDirectory = context.getExternalFilesDir(null).getAbsolutePath();
    copyWeightsAssetsToDirectory(weightsDirectory);
    initLyra(weightsDirectory);
  }

  private void copyWeightsAssetsToDirectory(String targetDirectory) {
    try {
      AssetManager assetManager = context.getAssets();
      String[] files = {"lyra_config.binarypb", "lyragan.tflite",
        "quantizer.tflite", "soundstream_encoder.tflite"};
      byte[] buffer = new byte[1024];
      int amountRead;
      for (String file : files) {
        InputStream inputStream = assetManager.open(file);
        File outputFile = new File(targetDirectory, file);

        OutputStream outputStream = new FileOutputStream(outputFile);
        Log.i(TAG, "copying asset to " + outputFile.getPath());

        while ((amountRead = inputStream.read(buffer)) != -1) {
          outputStream.write(buffer, 0, amountRead);
        }
        inputStream.close();
        outputStream.close();
      }
    } catch (Exception e) {
      Log.e(TAG, "Error copying assets", e);
    }
  }

  public byte[] encode(int bitrate, short[] samples) {
      return encodeRaw(bitrate, samples, samples.length);
  }

  public short[] decode(int bitrate, byte[] encoded) {
      return decodeRaw(bitrate, encoded, encoded.length);
  }

    public native byte[] initLyra(String directory);
    public native byte[] encodeRaw(int bitrate, short[] samples, int sampleLength);
    public native short[] decodeRaw(int bitrate, byte[] encoded, int encodedLength);
}
