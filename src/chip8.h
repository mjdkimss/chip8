#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>



enum { SCREENWIDTH = 64, SCREENHEIGHT = 32,  SPRITEWIDTH = 8, 
	OPCODESIZE = 2, CPUCYCLECOUNT = 10};

//SYS Addr은 무시한다. 
enum eOPINDEX { UNKNOWN = -1, CLS = 0, RET 
	,JP1NNN,CALL2NNN ,SE3XNN,SNE4XNN, SE5XY0, SNE9XY0, JPBNNN
	,LD6XNN, ADD7XNN, LDANNN

	,LD8XY0, OR8XY1, AND8XY2, XOR8XY3, ADD8XY4, SUB8XY5, SHR8XY6
	,SUBN8XY7, SHL8XYE

	,CXNN,DXYN
	,EX9E,EXA1

	,FX07,FX0A,FX15,FX18,FX1E,FX29,FX33,FX55,FX65
	,COUNT};


typedef enum eOPINDEX OPINDEX;

struct tagRegister
{
	uint8_t  V[16];
	uint16_t I;
	uint8_t  SP;
	uint16_t PC;
};

typedef struct tagRegister Register;


typedef struct 
{
	uint8_t  memory[4096];
	uint16_t stack[16];
	uint8_t  display[64 * 32];
	uint8_t delayTimer;
	uint8_t soundTimer;
	Register reg;
	size_t   romsize;
	
} chip8;

extern uint8_t chip8font[80];
extern bool keys[16];

void chip8_init(chip8* machine);
bool chip8_loadRom(chip8* machine, const char* path);
void chip8_dump(const chip8* const machine);


OPINDEX chip8_CPUCycle(chip8* machine);
uint16_t chip8_Fetch(chip8* machine);
OPINDEX chip8_decode(uint16_t opcode);
OPINDEX chip8_execute(OPINDEX index, chip8* machine, uint16_t opcode);

void chip8_CLS(chip8* machine, uint16_t opcode);
void chip8_RET(chip8* machine, uint16_t opcode);

void chip8_1NNN(chip8* machine, uint16_t opcode);
void chip8_2NNN(chip8* machine, uint16_t opcode);
void chip8_3XNN(chip8* machine, uint16_t opcode);
void chip8_4XNN(chip8* machine, uint16_t opcode);
void chip8_5XY0(chip8* machine, uint16_t opcode); 
void chip8_9XY0(chip8* machine, uint16_t opcode); 
void chip8_BNNN(chip8* machine, uint16_t opcode);


void chip8_8XY0(chip8* machine, uint16_t opcode);
void chip8_8XY1(chip8* machine, uint16_t opcode);
void chip8_8XY2(chip8* machine, uint16_t opcode);
void chip8_8XY3(chip8* machine, uint16_t opcode);
void chip8_8XY4(chip8* machine, uint16_t opcode);
void chip8_8XY5(chip8* machine, uint16_t opcode);
void chip8_8XY6(chip8* machine, uint16_t opcode);
void chip8_8XY7(chip8* machine, uint16_t opcode);
void chip8_8XYE(chip8* machine, uint16_t opcode);

void chip8_6XNN(chip8* machine, uint16_t opcode);
void chip8_7XNN(chip8* machine, uint16_t opcode);
void chip8_ANNN(chip8* machine, uint16_t opcode);

void chip8_CXNN(chip8* machine, uint16_t opcode);
void chip8_DXYN(chip8* machine, uint16_t opcode);

void chip8_EX9E(chip8* machine, uint16_t opcode);
void chip8_EXA1(chip8* machine, uint16_t opcode);

void chip8_FX07(chip8* machine, uint16_t opcode);
void chip8_FX0A(chip8* machine, uint16_t opcode);
void chip8_FX15(chip8* machine, uint16_t opcode);
void chip8_FX18(chip8* machine, uint16_t opcode);
void chip8_FX1E(chip8* machine, uint16_t opcode);
void chip8_FX29(chip8* machine, uint16_t opcode);
void chip8_FX33(chip8* machine, uint16_t opcode);
void chip8_FX55(chip8* machine, uint16_t opcode);
void chip8_FX65(chip8* machine, uint16_t opcode);

void chip8_Testdecode(const chip8* const machine);
void chip8_showOpcode(unsigned int addr, uint16_t opcode);