#ifndef TOMASULO_H
#define TOMASULO_H

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;

#define LOAD 	(10)
#define STORE 	(11)
#define ADD 	(12)
#define SUB 	(13)
#define MUL 	(14)
#define DIV 	(15)

class Instruction
{
public:
	int OP, Rd, Rs, Rt, addr;
    std::string ins_str;

	int shoot_time, finish_time;

    int getOP(std::string str)
	{
		#define RETURN(XXX, YYY) if (str == #XXX) return YYY;
		RETURN(ADDD, ADD)
		RETURN(SUBD, SUB)
		RETURN(MULD, MUL)
		RETURN(DIVD, DIV)
		RETURN(LD, LOAD)
		RETURN(ST, STORE)
		#undef RETURN
        throw 0;
		return 0;
	}

    Instruction(std::string str)
	{
		ins_str = str;
		char buf[32], opr[32];
		for (int i = 0; i < str.length(); i++) buf[i] = str[i];
		sscanf(buf, "%s", opr);
        OP = getOP((std::string)opr);
		if (OP == LOAD)
		{
			sscanf(buf + strlen(opr) + 1, "F%d %d", &Rd, &addr); // LOAD Rd addr
		}
		else if (OP == STORE)
		{
			sscanf(buf + strlen(opr) + 1, "F%d %d", &Rd, &addr); // STORE Rd addr
		}
		else if (OP == ADD || OP == SUB || OP == MUL || OP == DIV)
		{
			sscanf(buf + strlen(opr) + 1, "F%d F%d F%d", &Rd, &Rs, &Rt); // OP Rd Rs Rt
		}
		else assert(0);
		init();
	}

	void set_finish_time(int clock)
	{
		finish_time = clock;
	}

	void init()
	{
		shoot_time = finish_time = -1;
	}
};

class ReservationStation
{
public:
	int instruction_number;
	int OP;
	double Vj, Vk, result;
	ReservationStation *Qi, *Qj, *Qk;
	int addr;
	bool busy, ready;
	int remain_time;

	void set_arithm_result()
	{
		if (OP == ADD) result = Vj + Vk;
		else if (OP == SUB) result = Vj - Vk;
		else if (OP == MUL) result = Vj * Vk;
		else if (OP == DIV) result = Vj / Vk;
		else assert(0);
	}

	void release()
	{
		busy = false;
		Qj = Qk = NULL;
	}

	bool is_ready()
	{
		return busy && Qj == NULL && Qk == NULL;
	}

};

class Register
{
public:
	ReservationStation *Qi;
	double value;
};

class Memory
{
public:
	double value;
};

class Tomasulo
{
public:

	#define ADDSUB_BEGIN	(0)
	#define ADDSUB_END 		(3)
	#define MULDIV_BEGIN	(3)
	#define MULDIV_END		(5)
	#define ARITHM_BEGIN	(0)
	#define ARITHM_END		(5)
	#define LOAD_BEGIN		(5)
	#define LOAD_END		(8)
	#define STORE_BEGIN		(8)
	#define STORE_END		(11)

	#define STATION_N		(11)
	#define REGISTER_N		(16)

	int pc, clock;
	std::vector<Instruction> instruction;
	ReservationStation station[STATION_N];
	Register reg[REGISTER_N];
	Memory memory[1 << 12];

	Tomasulo()
	{
		pc = clock = 0;
        instruction.clear();
	}

	void addInstruction(Instruction __instruction)
	{
		instruction.push_back(__instruction);
	}

	void set_memory(int index, double value)
	{
		memory[index].value = value;
	}

	double get_register(int index)
	{
        if (reg[index].Qi != NULL) return 1e100;
		return reg[index].value;
	}

	double get_memory(int index)
	{
		return memory[index].value;
	}

	bool is_finish()
	{
		for (int i = 0; i < instruction.size(); i++)
			if (instruction[i].finish_time < 0) return false;
		return true;
	}

	void broadcast(ReservationStation *__station, double result)
	{
		for (int i = 0; i < STATION_N; i++)
		{
			if (station[i].Qj == __station) station[i].Qj = NULL, station[i].Vj = result;
			if (station[i].Qk == __station) station[i].Qk = NULL, station[i].Vk = result;
		}
		for (int i = 0; i < REGISTER_N; i++)
			if (reg[i].Qi == __station) reg[i].Qi = NULL, reg[i].value = result;
	}

	ReservationStation *get_free_station(int begin, int end)
	{
		for (int i = begin; i < end; i++)
			if (!station[i].busy) return &station[i];
		return NULL;
	}

	int get_remain_time(int OP)
	{
		if (OP == MUL) return 10;
		if (OP == DIV) return 40;
		return 2;
	}


	void issue_instruction(ReservationStation *free_station)
	{
		Instruction &__ins = instruction[pc];
		__ins.shoot_time = clock;
		int OP = __ins.OP;

		free_station->busy = true;
		free_station->OP = OP;
		free_station->instruction_number = pc;
		free_station->remain_time = get_remain_time(OP);
		int Rd = __ins.Rd, Rs = __ins.Rs, Rt = __ins.Rt;
		int addr = __ins.addr;

		if (OP == ADD || OP == SUB || OP == MUL || OP == DIV) // ADD SUB MUL DIV
		{
			// cout << Rd << ' ' << Rs << ' ' << Rt << endl;
			free_station->Qj = reg[Rs].Qi;
			free_station->Qk = reg[Rt].Qi;
			if (reg[Rs].Qi == NULL) free_station->Vj = reg[Rs].value;
			if (reg[Rt].Qi == NULL) free_station->Vk = reg[Rt].value;
			// cout << free_station->Vj << ' ' << free_station->Vk << endl;
			// cout << free_station->Qj << ' ' << free_station->Qk << endl;
			// cout << reg[Rs].value << ' ' << reg[Rt].value << endl;
			// cout << reg[Rs].Qi << ' ' << reg[Rt].Qi << endl;
			
			reg[Rd].Qi = free_station;
		}
		if (OP == LOAD) // LOAD
		{
			free_station->Qj = NULL;
			free_station->Qk = NULL;
			free_station->addr = addr;
			reg[Rd].Qi = free_station;
			// cout << "Rd: " << addr << endl;
		}
		if (OP == STORE)
		{
			free_station->Qj = reg[Rd].Qi;
			free_station->Qk = NULL;
			free_station->addr= addr;
			if (reg[Rd].Qi == NULL) free_station->Vj = reg[Rd].value, free_station->Qi = NULL;
			else free_station->Qi = reg[Rd].Qi;

		}
	}

	void caculate()
	{		
		for (int i = ARITHM_BEGIN; i < ARITHM_END; i++) // ADD SUB MUL DIV 
		{
			if (!station[i].is_ready()) continue;
			station[i].remain_time--;
			if (station[i].remain_time <= 0)
			{
				instruction[station[i].instruction_number].set_finish_time(clock);
				station[i].set_arithm_result();
			}
		}
		for (int i = LOAD_BEGIN; i < LOAD_END; i++) // LOAD
		{
			if (!station[i].is_ready()) continue;
			station[i].remain_time--;
			if (station[i].remain_time <= 0)
			{
				instruction[station[i].instruction_number].set_finish_time(clock);
				station[i].result = memory[station[i].addr].value;
			}
		}
		bool flag = false;
		for (int i = STORE_BEGIN; i < STORE_END; i++) // STORE
		{
			if (!station[i].is_ready()) continue;
			station[i].remain_time--;
			if (station[i].remain_time <= 0 && !flag)
			{
				flag = true; // 每个周期只允许写一次
				instruction[station[i].instruction_number].set_finish_time(clock);
				memory[station[i].addr].value = station[i].Vj;
				station[i].release();
			}
		}
		flag = false;
		for (int i = ARITHM_BEGIN; i < LOAD_END; i++) // ADD SUB MUL DIV LOAD
		{
			if (!station[i].busy || station[i].remain_time > 0 || flag) continue;
			flag = true; // 每个周期只允许广播一次
			broadcast(&station[i], station[i].result);
			station[i].release();
		}
	}

	void step()
	{
		if (is_finish()) return;
		clock++;
		if (pc == instruction.size())
		{
			caculate();
			return;
		}
		// issue new instruction
		int OP = instruction[pc].OP;
		ReservationStation *free_station = NULL;
		if (OP == LOAD) free_station = get_free_station(LOAD_BEGIN, LOAD_END);
		else if (OP == STORE) free_station = get_free_station(STORE_BEGIN, STORE_END);
		else if (OP == ADD || OP == SUB) free_station = get_free_station(ADDSUB_BEGIN, ADDSUB_END);
		else if (OP == MUL || OP == DIV) free_station = get_free_station(MULDIV_BEGIN, MULDIV_END);
		else assert(0);
		if (free_station != NULL)
		{
			issue_instruction(free_station);
			pc++;
		}
		// caculate and access memory
		caculate();
	}

	void work()
	{
		while (!is_finish()) step();
        std::cout << "clock: " << clock << std::endl;
	}

};

//Tomasulo tomasulo;
//int main()
//{
//	// tomasulo.addInstruction(Instruction("LOAD R6 34"));
//	// tomasulo.addInstruction(Instruction("LOAD R2 45"));
//	// tomasulo.addInstruction(Instruction("MUL R0 R2 R4"));
//	// tomasulo.addInstruction(Instruction("SUB R8 R6 R2"));
//	// tomasulo.addInstruction(Instruction("DIV R9 R0 R6"));
//	// tomasulo.addInstruction(Instruction("ADD R6 R8 R2"));

//	// Testcase 1:
//	tomasulo.set_memory(1000, 1);
//	tomasulo.set_memory(1001, 1);
//	tomasulo.addInstruction(Instruction("LD F1 1000")); // R1 = m[1000] = 1
//	tomasulo.addInstruction(Instruction("LD F2 1001")); // R2 = m[1001] = 1
	
//	for (int i = 0; i < 7; i++)
//	{
//		tomasulo.addInstruction(Instruction("ADDD F1 F1 F2")); // R1 = R1 + R2
//		tomasulo.addInstruction(Instruction("ST F1 777")); // swap(R1, R2)
//		tomasulo.addInstruction(Instruction("ST F2 888"));
//		tomasulo.addInstruction(Instruction("LD F1 888"));
//		tomasulo.addInstruction(Instruction("LD F2 777"));
//	}

//	// Testcase 2:
//	// tomasulo.set_memory(1000, 20);
//	// tomasulo.set_memory(1001, 3);
//	// tomasulo.set_memory(1002, 12345);
	
//	// tomasulo.addInstruction(Instruction("LOAD R1 1000")); // R1 = m[1000] = 20
//	// tomasulo.addInstruction(Instruction("LOAD R2 1001")); // R2 = m[1001] = 3
//	// tomasulo.addInstruction(Instruction("DIV R1 R1 R2")); // R1 = R1 / R2 = 20 / 3 = 6.6667
//	// tomasulo.addInstruction(Instruction("LOAD R1 1002")); // R1 = m[1002] = 12345
	
//	tomasulo.work();
//	cout << tomasulo.get_register(1) << endl;
//	cout << "done" << endl;
//	return 0;
//}
 
#endif









