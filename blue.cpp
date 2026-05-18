#include <stdint.h>
#include <iostream>
#include <string.h>
#include <string>

#include <iostream>
#include <vector>
#define RAMLENGTH 4096

//TODO TEST ADD XOR AND IOR AND RAL
//IMPLEMENT UNDER AND OVERFLOW CHECKING FOR ADD
uint16_t ram[RAMLENGTH];	//4096 words of ram 16 bits per word

typedef uint16_t bluereg;
bluereg ACC;	//accumulator
bluereg DOT;	//data out
bluereg DIT; 	//data in
bluereg IR;	//instruction register
bluereg PC = 0x00;	//Program Counter
bluereg MAR;	//Memory address register
bluereg MBR;	//Memory buffer register
bluereg Z;	//Z register - auxillary register for calculations
bluereg SR;	//Console switch register for inputting data manually
bluereg DSL;	//Device selection register

typedef enum{FETCH,EXECUTE,} State;
State STATE = FETCH;
bool power = true;
uint8_t clock_pulse = 1;
void emulate_Cycle();
void process_tick(uint8_t tick);
uint8_t get_instruction();
void do_HLT(uint8_t tick){
	if(tick == 7){
		power = false;
	}
	else if(tick == 8){
		MAR = PC;
	}	
}
void do_ADD(uint8_t tick){
	if(STATE == FETCH){
                if(tick == 6){
                        Z = 0x0000;
                }
                else if(tick == 7){
                        Z = ACC;
                }
                else if(tick == 8){
			MAR = (IR & 0x0FFF);
                        STATE = EXECUTE;
                }
        }
        else if(STATE == EXECUTE){
                if(tick == 1){
                        //Initiate Read
                }
                else if(tick == 2){
                        ACC= 0x0000;
			MBR = 0x0000;
                }
		else if(tick == 3){
			MBR = ram[MAR];
			
		}
                else if(tick == 7){
		//Check for over and underflows
                        ACC = Z+MBR;
                        
                }
		else if(tick == 8){
			MAR = PC;
			STATE = FETCH;
		}
        }

}
void do_XOR(uint8_t tick){
	if(STATE == FETCH){
		if(tick == 6){
			Z=0x0000;
		}
		else if(tick == 7)
		{
			Z = ACC;
		}
		else if(tick == 8){
			MAR = IR & 0x0FFF;
			STATE = EXECUTE;
		}
	}
	else if(STATE == EXECUTE){
		if(tick == 1){
                	//Initiate Read
                }
		else if (tick == 2){
			ACC = 0x0000;
			MBR = 0x0000;
		}
		else if (tick == 3){
			MBR = ram[MAR];
		}
                else if(tick == 7)
                {
                	ACC = Z^MBR;
                }
                else if(tick == 8){
                	MAR = PC;
			STATE = FETCH;
                }
	}
}
void do_AND(uint8_t tick){
	 if(STATE == FETCH){
                if(tick == 6){
                        Z=0x0000;
                }
                else if(tick == 7)
                {
                        Z = ACC;
                }
                else if(tick == 8){
                        MAR = IR & 0x0FFF;
                        STATE = EXECUTE;
                }
        }
        else if(STATE == EXECUTE){
                if(tick == 1){
                        //Initiate Read
                }
                else if (tick == 2){
                        ACC = 0x0000;
                        MBR = 0x0000;
                }
                else if (tick == 3){
                        MBR = ram[MAR];
                }
                else if(tick == 7)
                {
                        ACC = Z&MBR;
                }
                else if(tick == 8){
                        MAR = PC;
                        STATE = FETCH;
                }
        }

}
void do_IOR(uint8_t tick){
	 if(STATE == FETCH){
                if(tick == 6){
                        Z=0x0000;
                }
                else if(tick == 7)
                {
                        Z = ACC;
                }
                else if(tick == 8){
                        MAR = IR & 0x0FFF;
                        STATE = EXECUTE;
                }
        }
        else if(STATE == EXECUTE){
                if(tick == 1){
                        //Initiate Read
                }
                else if (tick == 2){
                        ACC = 0x0000;
                        MBR = 0x0000;
                }
                else if (tick == 3){
                        MBR = ram[MAR];
                }
                else if(tick == 7)
                {
                        ACC = Z|MBR;
                }
                else if(tick == 8){
                        MAR = PC;
                        STATE = FETCH;
                }
        }

}
void do_NOT(uint8_t tick){}
void do_LDA(uint8_t tick){}
void do_STA(uint8_t tick){}
void do_SRJ(uint8_t tick){
	if(tick == 6){
		ACC = (ACC | ( PC & 0x0FFF));
	}
	else if (tick == 7){
		PC = 0x0000;
	}
	else if (tick == 8){
		MAR = (IR & 0x0FFF);
		PC = (IR & 0x0FFF);
	}
}
void do_JMA(uint8_t tick){
	if(tick == 6){
		if(ACC >> 15){
			PC = 0x0000;
		}
	}
	else if (tick == 7){
		if(ACC >> 15){
			PC = (IR & 0x0FFF);
		}
	}
	else if (tick == 8){
		MAR = PC;
	}
}
void do_JMP(uint8_t tick){
	if(tick == 6){
		PC = 0x0000;
	}
	else if (tick == 7){
		PC = (IR & 0x0FFF);
	}
	else if (tick == 8){
		MAR=PC;
	}
}
void do_INP(uint8_t tick){}
void do_OUT(uint8_t tick){}
void do_RAL(uint8_t tick){
	if(STATE == FETCH){
		if(tick == 6){
			Z = 0x0000;
		}
		else if(tick == 7){
			Z = ACC;
		}
		else if(tick == 8){
			STATE = EXECUTE;
		}
	}
	else if(STATE == EXECUTE){
		if(tick == 1){
			ACC = 0x0000;
		}
		else if(tick == 2){
			ACC = ((Z & 0x8000)  >> 15) | Z*2;	//rotation is not a left shift
		}
		else if(tick == 8){
			MAR = PC;
			STATE = FETCH;
		}
	}
}
void do_CSA(uint8_t tick){
	if(tick == 6){
		ACC=0x0000;
	}
	else if(tick == 7){
		ACC = SR;
	}
	else if(tick ==8){
		MAR = PC;
	}
}

void do_NOP(uint8_t tick){
	if(tick == 8)
		MAR = PC;
}
std::vector<void(*)(uint8_t tick)> instruction_callback{
	do_HLT,
	do_ADD,
	do_XOR,
	do_AND,
	do_IOR,
	do_NOT,
	do_LDA,
	do_STA,
	do_SRJ,
	do_JMA,
	do_JMP,
	do_INP,
	do_OUT,
	do_RAL,
	do_CSA,
	do_NOP,
	
};

void emulate_Cycle(){
	while(clock_pulse < 9){
		process_tick(clock_pulse);
		clock_pulse++;
	}
	clock_pulse=1;
}

void process_tick(uint8_t tick){
	switch(tick){
		case 1:
			break;
		case 2:
			if(STATE == FETCH){
				PC+=1;
			}
			break;
		case 3:
			if(STATE == FETCH){
				MBR=0x00;
			}
			break;
		case 4:
			if(STATE==FETCH){
				IR=0x00;
				MBR=ram[MAR];
			}
			break;
		case 5:
			if(STATE==FETCH){
				IR=MBR;
			}
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		default:
			break;
	}
	uint8_t INS = get_instruction();
	instruction_callback[INS](tick);
}

uint8_t get_instruction(){
	//instruction type is first 4 bits of IR
	return((IR & 0xF000) >> 12);
}

void dumpRegs()
{
	printf("PC: %04x, A %04x, IR: %04x, Z: %04x, MAR: %04x, MBR: %04x, DSL: %02x, DIT: %02x, DOT: %02x\n", PC,ACC,IR,Z,MAR,MBR,(DSL & 0x00FF),(DIT & 0x00FF),(DOT & 0x00FF));
}

void runProgram(const uint16_t* program){
	std::cout << "Copying program to ramADASLKJDLLS\n";
	
	memset(ram, 0x00, RAMLENGTH * sizeof(uint16_t));
	memmove(ram,program,  5*sizeof(uint16_t));
	for (;power;){
		emulate_Cycle();
		dumpRegs();
	}

}

// Sample program
uint16_t program0[6] = {
	0xF000, // NOP 000
	0xF003, // NOP 003
	0xA004,	// JUMP to instruction #4
	0xF005, // NOP 005
	0xF010,  // NOP 010
	0xA000	// JUMP to instruction #0
		
};
uint16_t program1[8]{
	0xF000,
	0xA004,	//Jumping instruction to instruction #4
	0xF007,
	0xF008,
	0xF010,
	0xD000,	//RAL left shift
	0x9000, //JMA to instruction #0
	0xA000	//JMP to instruction #0
};
uint16_t program2[5]{
	0x1004,
	0xF005,
	0xF004,
	0xD123,
	0x0005	

};

int main(int argc, char* argv[])
{
	runProgram(program2);
	return 0;
}
