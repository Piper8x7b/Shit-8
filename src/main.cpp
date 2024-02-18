//
// Created by Piper8x7b on 2/17/24.
//

#include "lib/Chip8.hpp"
#include "lib/Platform.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char **argv) {
    // Check if the number of command line arguments is correct
    if (argc != 4) {
        // If not, print usage information and exit the program
        std::cerr << "Usage: " << argv[0] <<
                  " <Video scale> <Cycle delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    // Convert command line arguments to their respective types
    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const *romFilename = argv[3];

    // Initialize the platform with the given parameters
    Platform platform("SHIT-8 Emulator",
                      (int) VIDEO_WIDTH * videoScale,
                      (int) VIDEO_HEIGHT * videoScale,
                      VIDEO_WIDTH, VIDEO_HEIGHT);

    // Initialize the Chip8 system
    Chip8 chip8;
    // Load the ROM into the Chip8 system
    chip8.LoadRom(romFilename);

    // Calculate the pitch of the video (the length of a row in bytes)
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    // Main loop
    while (!quit) {
        // Process input and update the quit flag
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime =
                std::chrono::high_resolution_clock::now();
        // Calculate the delta time since the last cycle
        float dt = std::chrono::duration<float,
                std::chrono::milliseconds::period>
                (currentTime - lastCycleTime).count();

        // If it's time for a new frame
        if (dt > (float)cycleDelay){
            lastCycleTime = currentTime;

            // Perform a cycle of the Chip8 system
            chip8.Cycle();

            // Update the platform with the current video state
            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0;
}
