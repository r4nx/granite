# granite

Yet another CHIP-8 emulator, written in C++.

### Project structure
Currently, the whole interpreter is implemented in the `chipvm.cpp` and `instructions.cpp` files and it's 100% cross-platform. `chipvm.hpp` declares interfaces for drivers - modules, that do key scanning, rendering and other platform-dependent stuff. granite uses `SFML` library for rendering.


### TO-DO
- Implement `LD Vx, K` (load pressed key to register) instruction (can be done with `SetWindowsHookEx` on Windows) (**update:** see below)
- Implement all other drivers using `SFML`
- Generalize instruction decoding

### Code of conduct
I don't know what must happen to life that you want to contribute to this, but if you have absolutely nothing to do, I would be glad if somebody wrote drivers for Linux (not those drivers, drivers for granite).
