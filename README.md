## CHIPS (A CHIP-8 Interpreter)
<p align=center><img src="https://github.com/kottesh/chips/assets/67730727/ec0f6175-4b01-4ed8-9caf-37778d639d7c"></p>

### compiling  
```bash
git clone https://github.com/kottesh/chips.git
mkdir chips/build
cd chips/build
cmake ..
make
```

If the above commands get executed without any errors you will end up with an executable file 'chips', execute it like below with the rom file(make sure to give the correct path to the rom file).

```bash
./chips <test-chip8-rom-file.ch8>
```

