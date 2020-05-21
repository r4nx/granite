# granite

Yet another CHIP-8 emulator, written in C++.

### Project structure
Currently, the whole interpreter is implemented in the `chipvm.cpp` and `instructions.cpp` files and it's 100% cross-platform. `chipvm.hpp` declares interfaces for drivers - modules, that do key scanning, rendering and other platform-dependent stuff. By the way about rendering - I am too lazy to learn any GUI library at the moment, so granite has proof-of-concept rendering via `std::cout`. I am familiar with Windows enviroment only, so you can run actual emulator only on Windows (at least for now).

### TO-DO
- Implement `LD Vx, K` (load pressed key to register) instruction (can be done with `SetWindowsHookEx` on Windows)
- Replace current console clearing (`cls` on Windows, `clear` on Linux) with something portable