#include "chip8.h"
uint8_t chip8font[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


bool keys[16] = {0};

void (*func[COUNT])(chip8* machine, uint16_t opcode) = { 
	chip8_CLS, 
	chip8_RET,

	chip8_1NNN,
	chip8_2NNN,
	chip8_3XNN,
	chip8_4XNN,
	chip8_5XY0,
	chip8_9XY0,
	chip8_BNNN,

	chip8_6XNN,
	chip8_7XNN,
	chip8_ANNN,

	chip8_8XY0,
	chip8_8XY1,
	chip8_8XY2,
	chip8_8XY3,
	chip8_8XY4,
	chip8_8XY5,
	chip8_8XY6,
	chip8_8XY7,
	chip8_8XYE,

	

	chip8_CXNN,
	chip8_DXYN,

	chip8_EX9E,
	chip8_EXA1,

	chip8_FX07,
	chip8_FX0A,
	chip8_FX15,
	chip8_FX18,
	chip8_FX1E,
	chip8_FX29,
	chip8_FX33,
	chip8_FX55,
	chip8_FX65,
				};

void chip8_init(chip8* machine)
{

	memset(machine, 0, sizeof(chip8));
	memcpy(&machine->memory[0x050], chip8font, sizeof(chip8font));
	machine->reg.PC = 0x200;
	//난수생성기
	srand((unsigned int)time(NULL));
}

bool chip8_loadRom(chip8* machine, const char* path)
{
	bool result = true;
	FILE* fp = 0;
	if (NULL == path)
	{
		result = false;
		goto functionend;
	}
	
	if (0 != fopen_s(&fp, path, "rb"))
	{
		
		result = false;
		goto functionend;
	}

	fseek(fp, 0, SEEK_END);
	machine->romsize = ftell(fp);
	rewind(fp);

	if (machine->romsize > 4096 - 0x200)
	{
		result = false;
		goto functionend;
	}
	unsigned int memoryAddr = 0x200;
	size_t readvalue = fread(&machine->memory[memoryAddr], sizeof(uint8_t), machine->romsize, fp);
	if (readvalue != machine->romsize)
	{
		result = false;
		goto functionend;
	}
	

	chip8_dump(machine);
functionend:
	if (NULL != fp)
	{
		fclose(fp);
	}
	return result;
}

void chip8_dump(const chip8* const machine)
{
	//올바르게 로딩되었는지 확인하기
	char* buffer = (char*)malloc(machine->romsize * (sizeof(uint8_t)));
	if (NULL != buffer)
	{
		memcpy(buffer, &machine->memory[0x200], machine->romsize);
		for (size_t i = 0; i < machine->romsize; i++) {
			if (i % 16 == 0)
				printf("\n%03X: ", (unsigned)(0x200 + i));   // 줄 시작에 주소 표시
			printf("%02X ", machine->memory[0x200 + i]);
		}
		printf("\n");
		free(buffer);
	}
}


OPINDEX chip8_CPUCycle(chip8* machine)
{
	uint16_t opcode = chip8_Fetch(machine);
	OPINDEX opIndex = chip8_decode(opcode);
	return chip8_execute(opIndex, machine, opcode);

}

uint16_t chip8_Fetch(chip8* machine)
{
	uint16_t opcode = 0;
	opcode |= machine->memory[machine->reg.PC] << 8 | machine->memory[machine->reg.PC + 1];
	machine->reg.PC += OPCODESIZE;
	return opcode;
}

OPINDEX chip8_execute(OPINDEX index, chip8* machine, uint16_t opcode)
{
	if (index > UNKNOWN && index < COUNT)
	{
		func[index](machine, opcode);
	}
	else
	{
		fprintf(stderr, "Unkown opcode %04x\n", opcode);
		index = UNKNOWN;
	}

	return index;
}

OPINDEX chip8_decode(uint16_t opcode)
{
	int16_t number = COUNT;
	switch (opcode)
	{
	case 0x00e0:
		number = CLS;
		break;

	case 0x00ee:
		number = RET;
		break;

	default:
		number = UNKNOWN;
	}

	switch (opcode >> 12)
	{
	case 0x0:
		break;

	case 0x1:
		number = JP1NNN;
		break;

	case 0x2:
		number = CALL2NNN;
		break;

	case 0x3:
		number = SE3XNN;
		break;

	case 0x4:
		number = SNE4XNN;
		break;

	case 0x5:
		number = SE5XY0;
		break;

	case 0x9:
		number = SNE9XY0;
		break;

	case 0xb:
		number = JPBNNN;
		break;

	case 0x6:
		number = LD6XNN;
		break;

	case 0x7:
		number = ADD7XNN;
		break;

	case 0x8:
		switch (opcode & 0xf)
		{
		case 0x0:
			number = LD8XY0;
			break;

		case 0x1:
			number = OR8XY1;
			break;

		case 0x2:
			number = AND8XY2;
			break;

		case 0x3:
			number = XOR8XY3;
			break;

		case 0x4:
			number = ADD8XY4;
			break;

		case 0x5:
			number = SUB8XY5;
			break;

		case 0x6:
			number = SHR8XY6;
			break;

		case 0x7:
			number = SUBN8XY7;
			break;

		case 0xE:
			number = SHL8XYE;
			break;

		}
		break;





	case 0xA:
		number = LDANNN;
		break;

	case 0xC:
		number = CXNN;
		break;

	case 0xD:
		number = DXYN;
		break;

	case 0xE:
		switch (opcode & 0xff)
		{
		case 0x9e:
			number = EX9E;
			break;
		case 0xa1:
			number = EXA1;
			break;

		}
		break;

	case 0xf:
	{
		switch (opcode & 0xff)
		{
		case 0x07:
			number = FX07;
			break;

		case 0x0a:
			number = FX0A;
			break;

		case 0x15:
			number = FX15;
			break;

		case 0x18:
			number = FX18;
			break;

		case 0x1e:
			number = FX1E;
			break;

		case 0x29:
			number = FX29;
			break;

		case 0x33:
			number = FX33;
			break;

		case 0x55:
			number = FX55;
			break;

		case 0x65:
			number = FX65;
			break;

		default:
			number = UNKNOWN;
		}
		break;
	}
	
	default:
		number = UNKNOWN;
		break;
	}


	return number;
}

void chip8_CLS(chip8* machine, uint16_t opcode)
{
	//비디오 버퍼를 지운다. 
	(void)opcode;
	memset(machine->display, 0, sizeof(machine->display[0]) * SCREENWIDTH * SCREENHEIGHT);
}

void chip8_RET(chip8* machine, uint16_t opcode)
{
	//서브루틴에서 복귀 : 스택에서 pop->'PC'
	(void)opcode;
	machine->reg.PC =  machine->stack[machine->reg.SP];
	machine->stack[machine->reg.SP] = 0x00;
	machine->reg.SP -= 1;
}

void chip8_1NNN(chip8* machine, uint16_t opcode)
{
	//PC에 타겟 주소를 설정한다. 
	uint16_t targetAddr = opcode & 0xfff;
	machine->reg.PC = targetAddr;
}

void chip8_2NNN(chip8* machine, uint16_t opcode)
{
	//현재의 'pc'를 스택에 push 한다. 
	uint16_t nextPC = (uint16_t)(opcode & 0xfff);
	uint16_t currentPC = machine->reg.PC;
	uint16_t currentSP = machine->reg.SP;

	machine->stack[currentSP + 1] = currentPC;
	machine->reg.PC = nextPC;
	machine->reg.SP += 1;
}

void chip8_3XNN(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	uint8_t Vx = machine->reg.V[X];
	uint8_t NN = (opcode & 0xff);
	if (Vx == NN)
		machine->reg.PC += OPCODESIZE;
}

void chip8_4XNN(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	uint8_t Vx = machine->reg.V[X];
	uint8_t NN = (opcode & 0xff);
	if(Vx != NN)
		machine->reg.PC += OPCODESIZE;
}

void chip8_5XY0(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	uint8_t Vx = machine->reg.V[X];
	uint8_t Y = (opcode & 0xf0) >> 4;
	uint8_t Vy = machine->reg.V[Y];

	if (Vx == Vy)
		machine->reg.PC += OPCODESIZE;
}

void chip8_9XY0(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	uint8_t Vx = machine->reg.V[X];
	uint8_t Y = (opcode & 0xf0) >> 4;
	uint8_t Vy = machine->reg.V[Y];

	if (Vx != Vy)
		machine->reg.PC += OPCODESIZE;
}

void chip8_BNNN(chip8* machine, uint16_t opcode)
{
	uint16_t NNN = opcode & 0xFFF;
	machine->reg.PC = machine->reg.V[0] + NNN;
}

void chip8_8XY0(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t Y = (opcode & (0xf0)) >> 4;
	machine->reg.V[X] = machine->reg.V[Y];
}

void chip8_8XY1(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t Y = (opcode & (0xf0)) >> 4;
	machine->reg.V[X] |= machine->reg.V[Y];
}

void chip8_8XY2(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t Y = (opcode & (0xf0)) >> 4;
	machine->reg.V[X] &= machine->reg.V[Y];
}

void chip8_8XY3(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t Y = (opcode & (0xf0)) >> 4;
	machine->reg.V[X] ^= machine->reg.V[Y];
}

void chip8_8XY4(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t Y = (opcode & (0xf0)) >> 4;
	uint16_t value = machine->reg.V[X] + machine->reg.V[Y];
	machine->reg.V[X] = (uint8_t)(value & 0xff);
	if (value > 0xff)
		machine->reg.V[0xF] = 1;
	else 
		machine->reg.V[0xF] = 0;


}

void chip8_8XY5(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t Y = (opcode & (0xf0)) >> 4;
	int16_t value = machine->reg.V[X] - machine->reg.V[Y];

	machine->reg.V[X] = (value & 0xff);
	if (value >= 0)
		machine->reg.V[0xf] = 1;
	else
		machine->reg.V[0xF] = 0;

	
		
}

void chip8_8XY6(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t LSB = machine->reg.V[X] & 0x1;
	uint8_t value =  machine->reg.V[X] >> 1;
	machine->reg.V[X] = value;
	machine->reg.V[0xf] = LSB;
}

void chip8_8XY7(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t Y = (opcode & (0xf0)) >> 4;

	int16_t value = machine->reg.V[Y] - machine->reg.V[X];
	machine->reg.V[X] = (uint8_t)value;

	if (value >= 0)
		machine->reg.V[0xf] = 1;
	else
		machine->reg.V[0xF] = 0;
}

void chip8_8XYE(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t MSB = machine->reg.V[X] & 0x80;
	uint8_t value = machine->reg.V[X] << 1;
	machine->reg.V[X] = value;
	machine->reg.V[0xF] = MSB >> 7;

}








void chip8_6XNN(chip8* machine, uint16_t opcode)
{
	//레지스터 Vx에 value값을 설정한다. 
	uint8_t targetVx = (opcode & 0xf00) >> 8;
	uint8_t value = opcode & 0xff;
	machine->reg.V[targetVx] = value;
}

void chip8_7XNN(chip8* machine, uint16_t opcode)
{
	//레지스터 Vx에 value 값을 더한다.  
	uint8_t targetVx = (opcode & 0xf00) >> 8;
	uint8_t value = opcode & 0xff;
	machine->reg.V[targetVx] += value;
}

void chip8_ANNN(chip8* machine, uint16_t opcode)
{
	//레지스터 I에 value를 설정한다. 
	machine->reg.I = opcode & 0xfff;
}

void chip8_CXNN(chip8* machine, uint16_t opcode)
{

	uint8_t X = (opcode & (0xf00)) >> 8;
	uint8_t NN = opcode & (0x0ff);
	uint8_t value = (rand() % 256) & NN;
	machine->reg.V[X] = value;
}

void chip8_DXYN(chip8* machine, uint16_t opcode)
{
	//스프라이트 그리기 명령
	// 레지스터 Vx, Vy에 저장된 값을 시작좌표로 하고 N바이트로 이루어진 스프라이트를
	// 그린다. 

	const uint8_t x0 =  machine->reg.V[((opcode & 0x0f00) >> 8)] % SCREENWIDTH;
	const uint8_t y0 =  machine->reg.V[((opcode & 0x00f0) >> 4)] % SCREENHEIGHT;
	const uint8_t byteCount = (opcode & 0x000f);
	uint8_t bits = 0;
	machine->reg.V[0xf] = 0;
	for (int y = 0; y < byteCount; ++y)
	{
		if (y0 + y >= SCREENHEIGHT)
			break;

		bits = machine->memory[machine->reg.I + y];
		for (int x = 0; x < SPRITEWIDTH; ++x)
		{
			if (x0 + x  >= SCREENWIDTH)
				break;

			if (bits & (0x80 >> x))
			{
				int pixelIndex = (y0 + y) * SCREENWIDTH + (x0 + x);
				if (machine->display[pixelIndex] == 1)
					machine->reg.V[0xf] = 1;

				machine->display[pixelIndex] ^= 1;
			}
		}
	}
}

void chip8_EX9E(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	if (keys[X] == true)
		machine->reg.PC += 2;
}

void chip8_EXA1(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	if (keys[X] != true)
		machine->reg.PC += 2;
}

void chip8_FX07(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	machine->reg.V[X] = machine->delayTimer;
}

void chip8_FX0A(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	int8_t KeyIndex = -1;

	for (int8_t i = 0; i < 16; ++i)
	{
		if (keys[i] == true)
			KeyIndex = i;
	}
	if (KeyIndex != -1)
		machine->reg.V[X] = KeyIndex;
	else
		machine->reg.PC -= OPCODESIZE;
}

void chip8_FX15(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	machine->delayTimer = machine->reg.V[X];
}

void chip8_FX18(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	machine->soundTimer = machine->reg.V[X];
}

void chip8_FX1E(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	machine->reg.I += machine->reg.V[X];
}

void chip8_FX29(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	machine->reg.I = 0x50 + machine->reg.V[X] * 5;
}

void chip8_FX33(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	uint8_t value = machine->reg.V[X];
	const uint16_t I = machine->reg.I;

	machine->memory[I] = value / 100;
	machine->memory[I+1] = ((value % 100) - (value % 10)) / 10 ;
	machine->memory[I+2] = value % 10;
}

void chip8_FX55(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	const uint16_t I = machine->reg.I;
	for (uint8_t i = 0; i <= X; ++i)
	{
		machine->memory[I + i] = machine->reg.V[i];
	}
}

void chip8_FX65(chip8* machine, uint16_t opcode)
{
	uint8_t X = (opcode & 0xf00) >> 8;
	const uint16_t I = machine->reg.I;
	for (uint8_t i = 0; i <= X; ++i)
	{
		machine->reg.V[i] = machine->memory[I + i];
	}
	
}



#pragma region Test
void chip8_Testdecode(const chip8* const machine)
{
	size_t i = 0;
	for (; i + 1 < machine->romsize; i += 2)
	{
		uint16_t opcode = 0;
		opcode |= machine->memory[0x200 + i] << 8 | machine->memory[0x200 + i + 1];
		//opcode |= machine->memory[0x200 + i];
		//opcode = opcode << 8;
		//opcode |= machine->memory[0x200 + i+1];
		chip8_showOpcode((unsigned)(0x200 + i), opcode);	
	}

	if (i < machine->romsize)
		printf_s("\n%03x : %x\n", (unsigned)(0x200 + i), machine->memory[0x200 + i]);
}

void chip8_showOpcode(unsigned int addr, uint16_t opcode)
{
	char mnemonic[20] = { 0 };

	switch (opcode)
	{
	case 0x00e0:
		strcpy_s(mnemonic, 20, "CLS");
		goto EndofFunction;

	case 0x00ee:
		strcpy_s(mnemonic, 20, "RET");
		goto EndofFunction;
	}

	switch (opcode >> 12)
	{
	case 0x0:
		sprintf_s(mnemonic, 20, "SYS %03x", opcode & 0xfff);
		goto EndofFunction;

	case 0x1:
		sprintf_s(mnemonic, 20, "JP %03x", opcode & 0xfff);
		goto EndofFunction;

	case 0x2:
		sprintf_s(mnemonic, 20, "CALL %03x", opcode & 0xfff);

		goto EndofFunction;

	case 0x3:
		sprintf_s(mnemonic, 20, "SE V%x %03x", (opcode & 0xf00) >> 8, (opcode & 0xff));
		goto EndofFunction;

	case 0x4:
		sprintf_s(mnemonic, 20, "SNE V%x %03x", (opcode & 0xf00) >> 8, (opcode & 0xff));
		goto EndofFunction;

	case 0x5:
		sprintf_s(mnemonic, 20, "SE V%x V%x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
		goto EndofFunction;

	case 0x9:
		sprintf_s(mnemonic, 20, "SNE V%x V%x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
		goto EndofFunction;

	case 0xb:
		sprintf_s(mnemonic, 20, "JP V0 %x", opcode & 0xfff);
		goto EndofFunction;

	case 0x6:
		sprintf_s(mnemonic, 20, "LD V%x %02x", (opcode & 0xf00) >> 8, (opcode & 0xff));
		goto EndofFunction;

	case 0x7:
		sprintf_s(mnemonic, 20, "ADD V%x %03x", (opcode & 0xf00) >> 8, (opcode & 0xff));
		goto EndofFunction;

	case 0x8:
		switch (opcode & 0xf)
		{
		case 0x0:
			sprintf_s(mnemonic, 20, "LD V%02x V%02x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
			goto EndofFunction;

		case 0x1:
			sprintf_s(mnemonic, 20, "OR V%02x V%02x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
			goto EndofFunction;

		case 0x2:
			sprintf_s(mnemonic, 20, "AND V%02x V%02x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
			goto EndofFunction;

		case 0x3:
			sprintf_s(mnemonic, 20, "XOR V%02x V%02x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
			goto EndofFunction;

		case 0x4:
			sprintf_s(mnemonic, 20, "ADD V%02x V%02x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
			goto EndofFunction;

		case 0x5:
			sprintf_s(mnemonic, 20, "SUB V%02x V%02x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
			goto EndofFunction;

		case 0x6:
			sprintf_s(mnemonic, 20, "SHR V%02x V%02x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
			goto EndofFunction;

		case 0x7:
			sprintf_s(mnemonic, 20, "SUBN V%02x V%02x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4);
			goto EndofFunction;

		case 0xE:
			sprintf_s(mnemonic, 20, "SHL V%02x", (opcode & 0xf00) >> 8);
			goto EndofFunction;

		}
		break;





	case 0xA:
		sprintf_s(mnemonic, 20, "LD %03x", (opcode & 0xfff));
		goto EndofFunction;

	case 0xC:
		sprintf_s(mnemonic, 20, "RND V%x %02x", (opcode & 0xf00) >> 8, (opcode & 0xff));
		goto EndofFunction;

	case 0xD:
		sprintf_s(mnemonic, 20, "DRW V%x V%x %x", (opcode & 0xf00) >> 8, (opcode & 0xf0) >> 4, (opcode & 0xf));
		goto EndofFunction;

	case 0xE:
		switch (opcode & 0xf)
		{
		case 0xe:
			sprintf_s(mnemonic, 20, "SKP V%x DT", (opcode & 0xf00) >> 8);
			goto EndofFunction;
		case 0x1:
			sprintf_s(mnemonic, 20, "SKNP V%x K", (opcode & 0xf00) >> 8);
			goto EndofFunction;

		}
		break;

		case 0xf:
		{
			switch (opcode & 0xff)
			{
			case 0x07:
				sprintf_s(mnemonic, 20, "LD V%x, DT", (opcode & 0xf00) >> 8);
				goto EndofFunction;

			case 0x0a:
				sprintf_s(mnemonic, 20, "LD V%x, K", (opcode & 0xf00) >> 8);
				goto EndofFunction;

			case 0x15:
				sprintf_s(mnemonic, 20, "LD DT, V%x", (opcode & 0xf00) >> 8);
				goto EndofFunction;

			case 0x18:
				sprintf_s(mnemonic, 20, "LD ST, V%x", (opcode & 0xf00) >> 8);
				goto EndofFunction;

			case 0x1e:
				sprintf_s(mnemonic, 20, "Add I, V%x", (opcode & 0xf00) >> 8);
				goto EndofFunction;

			case 0x29:
				sprintf_s(mnemonic, 20, "LD F, V%x", (opcode & 0xf00) >> 8);
				goto EndofFunction;

			case 0x33:
				sprintf_s(mnemonic, 20, "LD B, V%x", (opcode & 0xf00) >> 8);
				goto EndofFunction;

			case 0x55:
				sprintf_s(mnemonic, 20, "LD [I], V%x ", (opcode & 0xf00) >> 8);
				goto EndofFunction;

			case 0x65:
				sprintf_s(mnemonic, 20, "LD V%x, [I]", (opcode & 0xf00) >> 8);
				goto EndofFunction;
			}
		}

	}

EndofFunction:
	printf("\n%03x : %04x - %s", addr, opcode, mnemonic);

}
#pragma endregion

