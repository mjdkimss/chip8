#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



enum { SCREENWIDTH = 64, SCREENHEIGHT = 32,  SPRITEWIDTH = 8, 
	OPCODESIZE = 2, CPUCYCLECOUNT = 10};

enum eOPINDEX { UNKNOWN = -1, CLS = 0, JP1NNN, LD6XNN, ADD7XNN, LDANNN, DXYN, COUNT};


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
OPINDEX chip8_exccute(OPINDEX index, chip8* machine, uint16_t opcode);

void chip8_CLS(chip8* machine, uint16_t opcode);
void chip8_1NNN(chip8* machine, uint16_t opcode);
void chip8_6XNN(chip8* machine, uint16_t opcode);
void chip8_7XNN(chip8* machine, uint16_t opcode);
void chip8_ANNN(chip8* machine, uint16_t opcode);
void chip8_DXYN(chip8* machine, uint16_t opcode);

void chip8_Testdecode(const chip8* const machine);
void chip8_showOpcode(unsigned int addr, uint16_t opcode);