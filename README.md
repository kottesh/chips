# CHIPS (A CHIP-8 Interpreter)
<p align=center><img src="https://github.com/kottesh/chips/assets/67730727/5ea32192-fd8f-4d4b-b737-880ba8b0a985"></p>

## showcase
https://github.com/kottesh/chips/assets/67730727/823cc132-7c81-4a62-8ac1-3e5cabb495d1

## compiling  
You will need clang++, SDL2 for building from source and probably it should get compiled with below commands.  

```bash
$ git clone https://github.com/kottesh/chips.git
$ mkdir chips/build
$ cd chips/build
$ cmake ..
$ make
```

If the above commands get executed without any errors you will end up with an executable file 'chips', execute it like below with the rom file(make sure to give the correct path to the rom file).

**NOTE:** I have only tested with GNU/Linux(Arch)

```bash
$ ./chips /path/to/rom_file 
```
