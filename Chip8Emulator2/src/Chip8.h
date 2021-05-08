#pragma once

#include  <cstdint>
#include <array>

#define MEMORY_SIZE_BYTES 4096
#define GPU_BUFFER_WIDTH 64
#define GPU_BUFFER_HEIGHT 32
#define GPU_BUFFER_SIZE (GPU_BUFFER_WIDTH * GPU_BUFFER_HEIGHT)

class Chip8
{
public:
	Chip8();
	~Chip8();

	//load rom into memory
	void load(const char* romPath);

	//do one cpu cycle of emulation
	void cycle();

	//get draw data to render
	std::array<uint8_t, GPU_BUFFER_SIZE>& getDrawData();

	//if cpu is ready to draw a new frame
	bool canDraw();

private:

	//memory - 4096 bytes in size, lowest 512 reserved
	std::array<uint8_t, MEMORY_SIZE_BYTES> mMemory = {0x0};

	//registers - 16
	std::array<uint8_t, 16> mRegs = {0x0};

	//gpu mem
	std::array<uint8_t, GPU_BUFFER_SIZE> mGpu = {0x0};
	
	//stack - 16 levels nesting
	std::array<uint16_t, 16> mStack = {0x0};

	//keypad - 16 keys
	std::array<uint8_t, 16> mKeys = {0x0};

	uint16_t mIndex = 0;
	uint16_t mPc = 0;

	uint8_t mDelayTimer = 0;
	uint8_t mSoundTimer = 0;

	//current opcode being executed
	uint16_t mOpcode;
	
	uint16_t mStackP = 0;
	bool mDrawFlag = true;

	//fetch next opcode from memory
	void fetch();

	void invalidInst();
	inline void inst0(); //family of instructions starting with 0
	inline void inst1(); //family of instructions starting with 1
	inline void inst2(); //family of instructions starting with 2
	inline void inst3(); //family of instructions starting with 3
	inline void inst4(); //family of instructions starting with 4
	inline void inst5(); //family of instructions starting with 5
	inline void inst6(); //family of instructions starting with 6
	inline void inst7(); //family of instructions starting with 7
	inline void inst8(); //family of instructions starting with 8
	inline void inst9(); //family of instructions starting with 9
	inline void instA(); //family of instructions starting with A
	inline void instB(); //family of instructions starting with B
	inline void instC(); //family of instructions starting with C
	inline void instD(); //family of instructions starting with D
	inline void instE(); //family of instructions starting with E
	inline void instF(); //family of instructions starting with F

	//function prototype pointing to a void member function
	typedef void (Chip8::*FuncTable)();

	//array of function pointers to each instruction family
	FuncTable table[16] =
	{
		&Chip8::inst0, &Chip8::inst1, &Chip8::inst2, &Chip8::inst3,
		&Chip8::inst4, &Chip8::inst5, &Chip8::inst6, &Chip8::inst7,
		&Chip8::inst8, &Chip8::inst9, &Chip8::instA, &Chip8::instB,
		&Chip8::instC, &Chip8::instD, &Chip8::instE, &Chip8::instF
	};

	const std::array<uint8_t, 80> CHIP8_FONT_SET =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	    0x20, 0x60, 0x20, 0x20, 0x70, //1
	    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	    0x90, 0x90, 0xF0, 0x10, 0x10, //4
	    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	    0xF0, 0x10, 0x20, 0x40, 0x40, //7
	    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
	};
};

