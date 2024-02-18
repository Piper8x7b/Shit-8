#include "lib/Chip8.hpp"
#include "lib/Platform.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] <<
                  " <Video scale> <Cycle delay> <ROM>\n";
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const *romFilename = argv[3];

    Platform platform("SHIT-8 Emulator",
                      (int) VIDEO_WIDTH * videoScale,
                      (int) VIDEO_HEIGHT * videoScale,
                      VIDEO_WIDTH, VIDEO_HEIGHT);

    Chip8 chip8;
    chip8.LoadRom(romFilename);

    int videoPitch = sizeof(chip8.video[0] * VIDEO_WIDTH);

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit) {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime =
                std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float,
                std::chrono::milliseconds::period>
                (currentTime - lastCycleTime).count();

        if (dt > (float)cycleDelay){
            lastCycleTime = currentTime;

            chip8.Cycle();

            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0;
}
