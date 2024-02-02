# Brainfuck LLVM compiler

Brainfuck compiler written using the LLVM C++ API

## Build
1. Install [LLVM](https://releases.llvm.org/download.html) and [CMake](https://cmake.org/download/)
2. Clone the repo
```sh
git clone https://github.com/SkullMag/brainfuck.git
```
3. Build
```sh
cmake .
make
```

## Usage
```sh
# Compile .bf file to LLVM IR
./bfcompiler hello.bf 2> hello.ll

# Run using the LLVM interpreter
lli hello.ll
```