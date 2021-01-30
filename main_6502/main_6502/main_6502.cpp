#include <stdio.h>
#include <stdlib.h>

using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Mem {
	static constexpr u32 MAX_MEM = 1024 * 64;
	Byte Data[MAX_MEM];
	
	void Initialise() {
		for (u32 i = 0; i < MAX_MEM; i++) {
			Data[i] = 0;
		}
	}

	/** read 1 byte */
	Byte operator[](u32 Address) const {
		//asset here Address is < MAX_MEM 
		return Data[Address];
	}

	/** write 1 byte */
	Byte& operator[](u32 Address) {
		//assert here Address is < MAX_MEM
		return Data[Address];
	}

};

struct CPU {
	Word ProgramCounter;			//program counter
	Word StackPointer;	//stack pointer

	Byte A, X, Y;		//registers

	Byte C : 1;			//status flag
	Byte Z : 1;			//status flag
	Byte I : 1;			//status flag
	Byte D : 1;			//status flag
	Byte B : 1;			//status flag
	Byte V : 1;			//status flag
	Byte N : 1;			//status flag

	void Reset(Mem& memory) {
		ProgramCounter = 0xFFFC;
		StackPointer = 0x0100;
		C= D = Z = I = B = V = N = 0;
		A = X = Y = 0;
		memory.Initialise();
	}

	Byte FetchByte(u32& Cycles, Mem& memory) {
		Byte Data = memory[ProgramCounter];
		ProgramCounter++;
		Cycles--;
		return Data;
	}

	Byte ReadByte(u32& Cycles, Byte Address, Mem& memory) {
		Byte Data = memory[Address];
		Cycles--;
		return Data;
	}

	//opcodes
	static constexpr Byte
		INS_LDA_IM = 0xA9;
		INS_LDA_ZP = 0xA5; 

	void LDASetStatus() {
			Z = (A == 0);
			N = (A & 0b10000000) > 0;
	}

	void Execute(u32 Cycles, Mem& memory) {
		while (Cycles > 0) {
			Byte Instruction = FetchByte(Cycles, memory);
			switch (Instruction) {
				case INS_LDA_IM: {
					Byte value =
						FetchByte(Cycles, memory);
					A = value;
					LDASetStatus();
				}break;
				case INS_LDA_ZP: {
					Byte ZeroPageAddress =
						FetchByte(Cycles, memory);
					A = ReadByte(Cycles, ZeroPageAddress, memory);
					LDASetStatus();
				}break;
				default: {
					printf("Instruction not handled %d", Instruction);
				}break;
			}
		}
	}
};

int main(){
	Mem mem;
	CPU cpu;
	cpu.Reset( mem );
	//inline a little program
	mem[0xFFFC] = CPU::INS_LDA_IM;
	mem[0xFFFD] = 0x42;
	//end - ionline a little program
	cpu.Execute(2, mem);
	return 0;
}