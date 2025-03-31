#include <iostream>
#include "lyra_library.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    if (true) {
        LyraEncode(16000, 1, 16000, false, "model.tflite", "input.wav", 16000, NULL);
    }
    if (true) {
        LyraDecode(16000, 1, "model.tflite", "input.wav", 16000, NULL);
    }
}
