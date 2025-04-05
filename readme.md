This is an implementation of chip-8 emulator (see [wikipedia](https://en.m.wikipedia.org/wiki/CHIP-8))

![image](https://upload.wikimedia.org/wikipedia/commons/thumb/5/54/Space_intercept.png/960px-Space_intercept.png)
Image: [Telmac-1800 running CHIP-8 game Space Intercept](https://commons.m.wikimedia.org/wiki/File:Space_intercept.png
) (Joseph Weisbecker, 1978) [CC-BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/deed.en)

# Compile
```bash
git clone https://github.com/YeaminSarder/chip-8
cd chip-8
make
```

# Usage
```
Usage: chip8 [OPTIONS] ROM
Emulate the chip8 ROM

  -d, --debug-level=LEVEL    set debug level to LEVEL. default 1.
                             0  -> disable
                             >0 -> show registers
                             >2 -> show callstack on return
  -f, --fps=NUM              limits the framerate of emulation to NUM; default 60.
                             set to 0 for no framerate limit.
  -h, --help                 show this help
```

Note: this repo does not provide chip8 roms. you can find them in [this](https://github.com/kripod/chip8-roms) repo.

## V 0.0.1
# status
* all chip8 opcodes are working
* runs any rom

# feature
* ctrl-c to cleanly exit (completed in feat/ctrl-c_clean_exit)
* help message (completed in feat/help)
* inputmap option (working in feat/map_option)

# known issuses
* ~~cannot create the temp.ch8 file autometically~~ (fixed in fix/cannot_create_temp) 
* needs better error handling (working in feat/logs_n_errors)
  * [X] file errors



