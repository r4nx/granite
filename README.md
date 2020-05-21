# granite

Yet another CHIP-8 emulator, written in C++.

### Project structure
Currently, the whole interpreter is implemented in the `chipvm.cpp` and `instructions.cpp` files and it's 100% cross-platform. `chipvm.hpp` declares interfaces for drivers - modules, that do key scanning, rendering and other platform-dependent stuff. By the way about rendering - I am too lazy to learn any GUI library at the moment, so granite has proof-of-concept rendering via `std::cout`. I am familiar with Windows enviroment only, so you can run actual emulator only on Windows (at least for now).

**[!] Huge warning**
This is not supposed to be a serious, "production ready" (lol), project, that I will support for a long time. There is a lot of commented stuff for debugging, though it generally works.

### TO-DO
- Fix the goddamn timers
- Implement `LD Vx, K` (load pressed key to register) instruction (can be done with `SetWindowsHookEx` on Windows)
- Replace current console clearing (`cls` on Windows, `clear` on Linux) with something portable

### Code of conduct
I don't know what must happen to life that you want to contribute to this, but if you have absolutely nothing to do, I would be glad if somebody wrote drivers for Linux (not those drivers, drivers for granite).
