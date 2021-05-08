#include "Chip8.h"

#include <iostream>
#include  <fstream>
#include <ctime>
#include <vector>

Chip8::Chip8()
{
	srand(time(nullptr));
}

Chip8::~Chip8()
{
}

void Chip8::load(const char* romPath)
{
	//reset stuffs
	mIndex = 0;
	mPc = 0x200;
	mOpcode = 0;
	mStackP = 0;
	mDelayTimer = 0;
	mSoundTimer = 0;

	mMemory.fill(0x0);
	mStack.fill(0x0);
	mKeys.fill(0x0);
	mRegs.fill(0x0);
	mGpu.fill(0x0);

	//load fontset into memory
	for(uint8_t i = 0; i < CHIP8_FONT_SET.size(); i++)
	{
		mMemory[i] = CHIP8_FONT_SET[i];
	}

	//load rom into mem
	std::ifstream file(romPath, std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if(file.read(buffer.data(), size))
	{
		int i = 0x200;
		for(const auto& data : buffer)
		{
			mMemory[i] = data;
			i++;
		}

	}else
	{
		std::cout << "ERROR READING ROM FILE: " << romPath << std::endl;
	}

}

void Chip8::cycle()
{
	fetch();

	(this->*(table[(mOpcode & 0xF000) >> 12]))();

	if(mDelayTimer > 0) mDelayTimer--;

	if(mSoundTimer > 0) mSoundTimer--;
}

std::array<uint8_t, 64 * 32>& Chip8::getDrawData()
{
	mDrawFlag = false;
	return mGpu;
}

bool Chip8::canDraw()
{
	return mDrawFlag;
}

void Chip8::invalidInst()
{
	std::cout << "Unknown opcode: " << std::hex << mOpcode << std::endl;
}

void Chip8::inst0()
{
	//0 family of instructions

	switch(mOpcode & 0x000F)
	{

	//00E0 - clear screen
	case 0x0:

		mGpu.fill(0x0);
		mDrawFlag = true;
		mPc += 2;
		break;

	//00EE - return from subroutine
	case 0xE:

		mStackP--;
		mPc = mStack[mStackP];
		mPc += 2;
		break;

	default:
		invalidInst();
	}
}

void Chip8::inst1()
{
	//1NNN - jump to address NNN
	mPc = mOpcode & 0x0FFF;
}

void Chip8::inst2()
{
	//2NNN - call subroutine NNN
	mStack[mStackP] = mPc;
	mStackP++;
	mPc = mOpcode & 0x0FFF;
}

void Chip8::inst3()
{
	//3XNN - skips to next inst if VX == NN
	if(mRegs[(mOpcode & 0x0F00) >> 8] == (mOpcode & 0x00FF))
	{
		mPc += 4;
	}else
	{
		mPc += 2;
	}
}

void Chip8::inst4()
{
	//4XNN - skips next instruction if VX != NN
	if(mRegs[(mOpcode & 0x0F00) >> 8] != (mOpcode & 0x00FF))
	{
		mPc += 4;
	}else
	{
		mPc += 2;
	}
}

void Chip8::inst5()
{
	//5XY0 - skips next instruction if VX == VY
	if(mRegs[(mOpcode & 0x0F00) >> 8] == mRegs[(mOpcode & 0x00F0) >> 4])
	{
		mPc += 4;
	}else
	{
		mPc += 2;
	}
}

void Chip8::inst6()
{
	//6XNN - sets VX to NN
	mRegs[(mOpcode & 0x0F00) >> 8] = mOpcode & 0x00FF;
	mPc += 2;
}

void Chip8::inst7()
{
	//7XNN - adds NN to VX. Carry flag not changed
	mRegs[(mOpcode & 0x0F00) >> 8] += mOpcode & 0x00FF;
	mPc += 2;
}

void Chip8::inst8()
{
	//8 family of instructions

	switch(mOpcode & 0x000F)
	{

	//8XY0 - set VX to val of VY
	case 0x0:

		mRegs[(mOpcode & 0x0F00) >> 8] = mRegs[(mOpcode & 0x00F0) >> 4];
		mPc += 2;
		break;

	//8XY1 - set VX to VX | VY (bitwise)
	case 0x1:

		mRegs[(mOpcode & 0x0F00) >> 8] |= mRegs[(mOpcode & 0x00F0) >> 4];
		mPc += 2;
		break;

	//8XY2 - set VX to VX & VY (bitwise)
	case 0x2:

		mRegs[(mOpcode & 0x0F00) >> 8] &= mRegs[(mOpcode & 0x00F0) >> 4];
		mPc += 2;
		break;

	//8XY3 - set VX to VX xor VY (bitwise)
	case 0x3:

		mRegs[(mOpcode & 0x0F00) >> 8] ^= mRegs[(mOpcode & 0x00F0) >> 4];
		mPc += 2;
		break;

	//8XY4 - adds VY to VX. VF set to 1 if carry, 0 otherwise
	case 0x4:

		mRegs[(mOpcode & 0x0F00) >> 8] += mRegs[(mOpcode & 0x00F0) >> 4];

		//if overflow has occured in the addition - set carry bit
		if(mRegs[(mOpcode & 0x00F0) >> 4] > 0xFF - mRegs[(mOpcode & 0x0F00) >> 8])
		{
			mRegs[0xF] = 1;
		}else
		{
			mRegs[0xF] = 0;
		}

		mPc += 2;

		break;

	//8XY5 - VY subtracted from VX. VF set to 0 when borrow occured, 1 otherwise
	case 0x5:

		
		//if borrow will occur - set carry bit to 0
		if(mRegs[(mOpcode & 0x00F0) >> 4] > mRegs[(mOpcode & 0x0F00) >> 8])
		{
			mRegs[0xF] = 0;
		}else
		{
			mRegs[0xF] = 1;
		}

		mRegs[(mOpcode & 0x0F00) >> 8] -= mRegs[(mOpcode & 0x00F0) >> 4];

		mPc += 2;

		break;

	//8XY6 - stores least significant bit of VX in VF and shifts VX >> 1
	case 0x6:

		//big endian - LSB last
		mRegs[0xF] = mRegs[(mOpcode & 0x0F00) >> 8] & 0x1;
		mRegs[(mOpcode & 0x0F00) >> 8] >>= 1;

		mPc += 2;

		break;

	//8XY7 - set VX to VY - VX. VF set to 0 when borrow occurs, 1 otherwise
	case 0x7:

		//if a borrow will occur - set carry bit to 0
		if(mRegs[(mOpcode & 0x0F00) >> 8] > mRegs[(mOpcode & 0x00F0) >> 4])
		{
			mRegs[0xF] = 0;
		}else
		{
			mRegs[0xF] = 1;
		}

		mRegs[(mOpcode & 0x0F00) >> 8] = mRegs[(mOpcode & 0x00F0) >> 4] - mRegs[(mOpcode & 0x0F00) >> 8];

		mPc += 2;

		break;

	//8XYE - store MSB of VX in VF and shifts VX << 1
	case 0xE:

		mRegs[0xF] = mRegs[(mOpcode & 0x0F00) >> 8] >> 7;
		mRegs[(mOpcode & 0x0F00) >> 8] <<= 1;

		mPc += 2;

		break;

	default:
		invalidInst();
	}
}

void Chip8::inst9()
{
	//9XY0 - skips next inst if VX != VY
	if(mRegs[(mOpcode & 0x0F00) >> 8] != mRegs[(mOpcode & 0x00F0) >> 4])
	{
		mPc += 4;
	}else
	{
		mPc += 2;
	}
}

void Chip8::instA()
{
	//ANNN - set index to address NNN
	mIndex = mOpcode & 0x0FFF;
	mPc += 2;
}

void Chip8::instB()
{
	//BNNN - jumps to address NNN + mRegs[0]
	mPc = (mOpcode & 0x0FFF) + mRegs[0x0];
}

void Chip8::instC()
{
	//CXNN - set VX to result of bitwise AND (&) on a random number - (0 - 255) & NN
	mRegs[(mOpcode & 0x0F00) >> 8] = (mOpcode & 0xFF) & rand() % 255;
	mPc += 2;
}

void Chip8::instD()
{
	//DXYN - draws sprite at VX, VY - width 8px, height Npx. Each row read bit coded starting from I
	//       VF set to 1 if any screen pixels flipped (collision occured)

	uint16_t x = mRegs[(mOpcode & 0x0F00) >> 8];
    uint16_t y = mRegs[(mOpcode & 0x00F0) >> 4];
    uint16_t height = mOpcode & 0x000F;
    uint16_t pixel;

    mRegs[0xF] = 0;
    for (int yline = 0; yline < height; yline++)
    {
        pixel = mMemory[mIndex + yline];
        for(int xline = 0; xline < 8; xline++)
        {
            if((pixel & (0x80 >> xline)) != 0)
            {
                if(mGpu[(x + xline + ((y + yline) * 64))] == 1)
                {
                    mRegs[0xF] = 1;
                }
                mGpu[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }

    mDrawFlag = true;
    mPc += 2;
}

void Chip8::instE()
{
	//E family of instructions

	switch(mOpcode & 0x00FF)
	{

	//EX9E - skips next instruction if key stored in VX is pressed
	case 0x9E:

		if(mKeys[mRegs[(mOpcode & 0x0F00) >> 8]] != 0)
		{
			mPc += 4;
		}else
		{
			mPc += 2;
		}

		break;

	//EXA1 - skips next inst if ket stored in VX not pressed
	case 0xA1:

		if(mKeys[mRegs[(mOpcode & 0x0F00) >> 8]] == 0)
		{
			mPc += 4;
		}else
		{
			mPc += 2;
		}

		break;

	default:
		invalidInst();
	}
}

void Chip8::instF()
{
	//F family of instructions

	switch(mOpcode & 0x00FF)
	{

	//FX07 - sets VX to value of delay timer
	case 0x07:

		mRegs[(mOpcode & 0x0F00) >> 8] = mDelayTimer;
		mPc += 2;
		break;

	//FX0A - key press awaited then stored in VX. Blocking instruction
	case 0x0A:
	{
		bool pressed = false;

		for(uint8_t i = 0; i < mKeys.size(); i++)
		{
			if(mKeys[i] != 0)
			{
				mRegs[(mOpcode & 0x0F00) >> 8] = i;
				pressed = true;
				break;
			}
		}

		//if key has not been pressed yet, return
		if(!pressed)
		{
			return;
		}

		mPc += 2;

		break;
	}

	//FX15 - set delay timer to VX
	case 0x15:

		mDelayTimer = mRegs[(mOpcode & 0x0F00) >> 8];

		mPc += 2;

		break;

	//FX18 - set sound timer to VX
	case 0x18:

		mSoundTimer = mRegs[(mOpcode & 0x0F00) >> 8];

		mPc += 2;

		break;

	//FX1E - adds VX to I
	case 0x1E:

		mIndex += mRegs[(mOpcode & 0x0F00) >> 8];

		mPc += 2;

		break;

	//FX29 - sets I to the location of the sprite for the character in VX
	case 0x29:

		mIndex = mRegs[(mOpcode & 0x0F00) >> 8] * 0x5;

		mPc += 2;

		break;

	//FX33 - stores the binary-coded decimal representation of VX
	case 0x33:

		mMemory[mIndex] = mRegs[(mOpcode & 0x0F00) >> 8] / 100;
		mMemory[mIndex + 1] = (mRegs[(mOpcode & 0x0F00) >> 8] / 10) % 10;
		mMemory[mIndex + 2] = (mRegs[(mOpcode & 0x0F00) >> 8] % 100) % 10;

		mPc += 2;

		break;

	//FX55 - stores V0 to VX in memory starting at address I
	case 0x55:

		for(int i = 0; i <= (mOpcode & 0x0F00) >> 8; i++)
		{
			mMemory[mIndex + i] = mRegs[i];
		}

		mIndex += 1 + ((mOpcode & 0x0F00) >> 8);

		mPc += 2;

		break;

	//FX65 - fills V0 to VX in memory starting at I
	case 0x65:

		for(int i = 0; i <= (mOpcode & 0x0F00) >> 8; i++)
		{
			mRegs[i] = mMemory[mIndex + i];
		}

		mIndex += 1 + ((mOpcode & 0x0F00) >> 8);

		mPc += 2;

		break;

	default:
		invalidInst();
	}
}


void Chip8::fetch()
{
	mOpcode = (mMemory[mPc] << 8) | mMemory[mPc + 1];
}
