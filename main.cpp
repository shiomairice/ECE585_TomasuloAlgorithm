/*
######################################################################################################################
  Author:   Arthur J. Miller
  FileName: main.cpp
  Full project can be found at github.com/milleraj66/ECE585_TomasuloAlgorithm
  Date:
            Start:  03-01-2016
            Update: 03-08-2016
            Update: 03-09-2016
            final:  03-10-2016

  Purpose:  ECE 585 Tomasulo Algorithm
            Driver file for Tomasulo algorithm
######################################################################################################################
This program follows the algorithm as stated by the following sources;
Computer Architecture : A Quantitative Approach (5th Edition) by Hennessy, John L., Patterson, David A.
https://en.wikipedia.org/wiki/Tomasulo_algorithm#Implementation_concepts
Dr. Chandra CPP

ALGORITHM:
	ISSUE:
		// DELAY: 1 Clock
		// determine reservation station destination for given instruction
	    // r is the given reservation station
	    Switch(INST.op):
		case(add):  r=add; 	goto ISSUE
		case(mult): r= mult; 	goto ISSUE
		case(div):  r = div;	goto ISSUE
		// check if spot in given reservation station is available
		if(RS[r].busy = false):
			// if operand rs is available -> set value of operand (Vj) to given register value
			// else point operand to the reservation station (Qj) that will give the operand value
			if(RegStatus[rs].Qi == 0):
				RS[r].Vj = Reg[rs]; RS[r].Qj = 0;
			else
				RS[r].Qj = RegStatus[r].Qi;
			// if operand rt is available -> set value of operand (Vk) to given register value
			// else point operand to the reservation station (Qk) that will give the operand value
			if(RegStatus[rt].Qi == 0):
				RS[r].Vj = Reg[rt]; RS[r].Qj = 0;
			else
				RS[r].Qj = RegStatus[r].Qi;
		// given reservation station is now busy until write back stage is completed.
		RS[r].busy = true; RegStatus[rd].Qi = true
	EXECUTE:
 		// check each reservation station to see if both operands are ready
		// Delay: Switch(INST.op)
		//		case(add): 	clock += 4;
		//		case(mult): 	clock += 12;
		//		case(div):	clock += 38;
		// wait for both operands to be available and execute given instructions operation
		if(RS[r].Qj == 0 && RS[r].Qk == 0):
			switch(INST.op):
				case(add):	temp = Vj +- Vk
				case(mult):	temp = Vj * Vk
				case(div):	temp = Vj / Vk
	WRITEBACK:
 		// Check each reservation station to see if operaional delay is done -> result is ready
		// Delay: 1 clock
		// for all x registers
		// if RegStatus points to the given reservation station r set that register[x] equal to executed result	
		if(RegStatus[x].Qi==r):
			Reg[x] = temp; RegStatus[x].Qi = false;
		// check if any reservation stations are waiting for the given result as an operand	
		if(RS[x].Qj==r):
			RS[x].Vi=temp; RS[x].Qj=false;
		if(RS[x].Qk==r):
			RS[x].Vk=temp; RS[x].Qk=false;
		// The given reservation station is can now be used again		
		RS[r].busy = false;
 */

#include <iostream>
#include <iomanip>          // Print Table Formatting
#include <vector>
#include "ReservationStation.h"
#include "Instruction.h"    // MIPS Style Instruction Class
#include "RegisterStatus.h"

using namespace std;

//####################################################################################################################
//*** Define Architecture
// RESERVATION STATION NUMBER
const int Num_ADD_RS = 4;
const int Num_MULT_RS = 2;
const int Num_DIV_RS = 3;
// RESERVATION STATION LATENCY
const int ADD_Lat = 4;
const int MULT_Lat = 12;
const int DIV_Lat = 38;
// Datapath Latency
const int ISSUE_Lat = 1;
const int WRITEBACK_Lat = 1;
// Global Clock
int Clock = 0;
bool Done = true;
// Opcode Values
const int AddOp = 0;
const int SubOp = 1;
const int MultOp = 2;
const int DivOp = 3;
// Counter for current instruction to issue
int currentInst_ISSUE = 0;
// Temporary fix for errors due to RS names being numbers -> errors with REG/RS/REGSTATUS == zero
const int ZERO_REG = 5000;
const int RegStatusEmpty = 1000;
const int OperandAvailable = 1001;
const int OperandInit = 1002;
//####################################################################################################################

//####################################################################################################################
// Driver Functions
int ISSUE(vector<Instruction>& Inst, vector<ReservationStation>& ResStat, vector<RegisterStatus>& RegStat, vector<int>& Register);
void EXECUTE(vector<Instruction>& Inst, vector<ReservationStation>& ResStat, vector<RegisterStatus>& RegStat, vector<int>& Register);
void WRITEBACK(vector<Instruction>& Inst, vector<ReservationStation>& ResStat, vector<RegisterStatus>& RegStat, vector<int>& Register);
// Helper functions
void printRegisterStatus(vector<RegisterStatus> RegisterStatusVector);
void printReservationStations(vector<ReservationStation> ReservationStationsVector);
void printRegisters(vector<int> RegistersVector);
void printInstructions(vector<Instruction> InstructionsVector);
void printTimingTable(vector<Instruction> INST);
//####################################################################################################################

//####################################################################################################################
// MAIN DRIVER
int main(){
    //**** START Define Architecture
    // Input program instructions
    Instruction
            //(rd,rs,rt,opcode)
            I0(1,2,3,AddOp),
            I1(4,1,5,AddOp),
            I2(6,7,8,SubOp),
            I3(9,4,10,MultOp),
            I4(11,12,6,DivOp),
            I5(8,1,5,MultOp),
            I6(7,2,3,MultOp);
    // Pack Instructions into vector
    vector<Instruction> Inst = {I0,I1,I2,I3,I4,I5,I6};

    //// Input reservation station architecture
    // DONT FORGET TO UPDATE ^
    // RESERVATION STATION NUMBERS
    //const int Num_ADD_RS = 4;
    //const int Num_MULT_RS = 2;
    //const int Num_DIV_RS = 3;
    ReservationStation
            ADD1(AddOp, OperandInit),
            ADD2(AddOp, OperandInit),
            ADD3(AddOp, OperandInit),
            ADD4(AddOp, OperandInit);
    ReservationStation
            MULT1(MultOp, OperandInit),
            MULT2(MultOp, OperandInit);
    ReservationStation
            DIV1(DivOp, OperandInit),
            DIV2(DivOp, OperandInit),
            DIV3(DivOp, OperandInit);
    // Pack reservation stations into vector
    vector<ReservationStation> ResStation = {ADD1, ADD2, ADD3, ADD4, MULT1, MULT2, DIV1, DIV2, DIV3};

    // Initialize register status objects
    RegisterStatus
            F0(RegStatusEmpty),F1(RegStatusEmpty),F2(RegStatusEmpty),F3(RegStatusEmpty),F4(RegStatusEmpty),
            F5(RegStatusEmpty),F6(RegStatusEmpty),F7(RegStatusEmpty),F8(RegStatusEmpty),F9(RegStatusEmpty),
            F10(RegStatusEmpty),F11(RegStatusEmpty),F12(RegStatusEmpty);
    // Pack register status objects into vector
    vector<RegisterStatus> RegisterStatus = {F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12};

    // Initialize register file vector
    vector<int> Register = {ZERO_REG,1,2,3,4,5,6,7,8,9,10,11,12};
    //**** END Define Architecture

    cout << "INITIAL VALUES:" << endl;
    printInstructions(Inst);
    printReservationStations(ResStation);
    printRegisters(Register);
    printRegisterStatus(RegisterStatus);
    cout << endl;

    //**** START functional loop
    do{
        // Datapath
        Clock++; // system clock

        ISSUE(Inst,ResStation,RegisterStatus,Register);
		EXECUTE(Inst,ResStation,RegisterStatus,Register);
		WRITEBACK(Inst,ResStation,RegisterStatus,Register);

        // PRINT
        printRegisters(Register);
        printTimingTable(Inst);
        cout << endl;

        // Check if all reservation stations are empty -> program done
        Done = true;
        for(int i=0;i<ResStation.size();i++){
            cout << ResStation[i].busy << " ";
            if(ResStation[i].busy == 1){
                Done = false;
                break;
            }
        }
        cout << endl;
	}while(Clock<50);//**** End functional loop

    return 0;
}//**** END MAIN DRIVER
//####################################################################################################################

//####################################################################################################################
// Datapath FUNCTIONS
int ISSUE(vector<Instruction>& INST, vector<ReservationStation>& RESSTATION, vector<RegisterStatus>& REGSTATUS, vector<int>& REG){
    // Latency of 1 if issued
    //**** check if spot in given reservation station is available
    int r = 0;
    bool rsFree = false;
    // r is the current instruction to be issued's operation code(add,sub,mult,div)
    // If all instructions have been issued then stop issueing for rest of program
    if(currentInst_ISSUE > INST.size())
            return 0;
    r = INST[currentInst_ISSUE].op;
    // determine if there is an open RS of r type. if yes -> r = that open spot.
    switch(r){
        case AddOp:
            for(int i=Num_ADD_RS-Num_ADD_RS;i<Num_ADD_RS;i++){
                if(!RESSTATION[i].busy){
                    r = i; // RS are definced 1 -> 10 rather than from 0
                    currentInst_ISSUE++;
                    RESSTATION[i].op = AddOp;
                    rsFree = true;
                    break;
                }
            }
            // if instruction is not issued because no resercation stations are free exit ISSUE
            // Initializaion is not necessary if instruction not issued
            if(!rsFree)
                return 1;
            break;
        case SubOp:
            for(int i=Num_ADD_RS-Num_ADD_RS;i<Num_ADD_RS;i++){
                if(!RESSTATION[i].busy){
                    r = i; // RS are definced 1 -> 10 rather than from 0
                    currentInst_ISSUE++;
                    RESSTATION[i].op = SubOp;
                    rsFree = true;
                    break;
                }
            }
            if(!rsFree)
                return 1;
            break;
        case MultOp:
            for(int i=Num_ADD_RS;i<Num_ADD_RS+Num_MULT_RS;i++){
                if(!RESSTATION[i].busy){
                    r = i; // RS are definced 1 -> 10 rather than from 0
                    currentInst_ISSUE++;
                    RESSTATION[i].op = MultOp;
                    rsFree = true;
                    break;
                }
            }
            if(!rsFree)
                return 1;
            break;
        case DivOp:
            for(int i=Num_ADD_RS+Num_MULT_RS;i<Num_ADD_RS+Num_MULT_RS+Num_DIV_RS;i++){
                if(!RESSTATION[i].busy){
                    r = i;// RS are definced 1 -> 10 rather than from 0
                    currentInst_ISSUE++;
                    RESSTATION[i].op = DivOp;
                    rsFree = true;
                    break;
                }
            }
            if(!rsFree)
                return 1;
            break;
        default:
            break;
    }
    //**** Initialize characteristics of issued instruction
    // if operand rs is available -> set value of operand (Vj) to given register value
    // else point operand to the reservation station (Qj) that will give the operand value
    // NOTE: since currentInst was in incremented we must do currentINST_ISSUE-1
    if(REGSTATUS[INST[currentInst_ISSUE-1].rs].Qi == RegStatusEmpty){
        RESSTATION[r].Vj = REG[INST[currentInst_ISSUE-1].rs];
        RESSTATION[r].Qj = OperandAvailable;
    }
    else{
        RESSTATION[r].Qj = REGSTATUS[INST[currentInst_ISSUE-1].rs].Qi;
    }
    // if operand rt is available -> set value of operand (Vk) to given register value
    // else point operand to the reservation station (Qk) that will give the operand value
    if(REGSTATUS[INST[currentInst_ISSUE-1].rt].Qi == RegStatusEmpty){
        RESSTATION[r].Vk = REG[INST[currentInst_ISSUE-1].rt];
        RESSTATION[r].Qk = OperandAvailable;
    }
    else{
        RESSTATION[r].Qk = REGSTATUS[INST[currentInst_ISSUE-1].rt].Qi;
    }
    // given reservation station is now busy until write back stage is completed.
    RESSTATION[r].busy = true;
    RESSTATION[r].ISSUE_Lat = 0;
    // set reservation station instuction number == current instruction
    RESSTATION[r].instNum = currentInst_ISSUE-1;
    // set clock cycle for issue time
    INST[currentInst_ISSUE-1].issueClock = Clock;
    // The register status Qi is set to the current instructions reservation station location r
    REGSTATUS[INST[currentInst_ISSUE-1].rd].Qi = r;
    return 2;
}//END ISSUE()
void EXECUTE(vector<Instruction>& INST, vector<ReservationStation>& RESSTATION, vector<RegisterStatus>& REGSTATUS, vector<int>& REG){
    // check each reservation station to see if both operands are ready
    // The current reservation station is r
    for (int r=0;r<RESSTATION.size();r++){
        // if both operands are available then execute given instructions operation
        // and set resultReady flag to true so that result can be written back to CDB
        // first check if instruction has been issued
        if(RESSTATION[r].busy == true){
            // second check if the ISSUE latency clock cycle has happened
            if(RESSTATION[r].ISSUE_Lat >= ISSUE_Lat){
                // third check if both operands are available
                if(RESSTATION[r].Qj == OperandAvailable && RESSTATION[r].Qk == OperandAvailable){
                    // Set clock cycle when execution begins
                    if(INST[RESSTATION[r].instNum].executeClockBegin == 0)
                        INST[RESSTATION[r].instNum].executeClockBegin = Clock;
                    // when execution starts we must wait the given latency number of clock cycles before making result
                    // available to WriteBack
                    // Delay: Switch(INST.op)
                    //		case(add): 	clock += 4;
                    //		case(mult): 	clock += 12;
                    //		case(div):	clock += 38;
                    RESSTATION[r].lat++;
                    switch(RESSTATION[r].op){
                        case(AddOp):
                            if(RESSTATION[r].lat == ADD_Lat){
                                RESSTATION[r].result = RESSTATION[r].Vj + RESSTATION[r].Vk;
                                RESSTATION[r].resultReady = true; // Result is ready to be writenback
                                RESSTATION[r].lat = 0;
                                // Set clock cycle when execution ends
                                INST[RESSTATION[r].instNum].executeClockEnd = Clock;
                                // reset ISSUE latency for RS
                                RESSTATION[r].ISSUE_Lat = 0;
                            }
                        case(SubOp):
                            if(RESSTATION[r].lat == ADD_Lat){
                                RESSTATION[r].result = RESSTATION[r].Vj - RESSTATION[r].Vk;
                                RESSTATION[r].resultReady = true; // Result is ready to be writenback
                                RESSTATION[r].lat = 0;
                                // Set clock cycle when execution ends
                                INST[RESSTATION[r].instNum].executeClockEnd = Clock;
                                // reset ISSUE latency for RS
                                RESSTATION[r].ISSUE_Lat = 0;
                            }
                        case(MultOp):
                            if(RESSTATION[r].lat == MULT_Lat){
                                RESSTATION[r].result = RESSTATION[r].Vj * RESSTATION[r].Vk;
                                RESSTATION[r].resultReady = true; // Result is ready to be writenback
                                RESSTATION[r].lat = 0;
                                // Set clock cycle when execution ends
                                INST[RESSTATION[r].instNum].executeClockEnd = Clock;
                                // reset ISSUE latency for RS
                                RESSTATION[r].ISSUE_Lat = 0;
                            }
                        case(DivOp):
                            if(RESSTATION[r].lat == DIV_Lat){
                                RESSTATION[r].result = RESSTATION[r].Vj / RESSTATION[r].Vk;
                                RESSTATION[r].resultReady = true; // Result is ready to be writenback
                                RESSTATION[r].lat = 0;
                                // Set clock cycle when execution ends
                                INST[RESSTATION[r].instNum].executeClockEnd = Clock;
                                // reset ISSUE latency for RS
                                RESSTATION[r].ISSUE_Lat = 0;
                            }
                        default:
                            break;
                    }
                }
            } else // Execute is not ready until one cycle latency of ISSUE
                RESSTATION[r].ISSUE_Lat++;
        }

    }

}//END EXECUTE()
void WRITEBACK(vector<Instruction>& INST, vector<ReservationStation>& RESSTATION, vector<RegisterStatus>& REGSTATUS, vector<int>& REG){
    // Check each reservation station to see if operational delay is done -> result is ready
    for(int r=0;r<RESSTATION.size();r++){
        // if result ready write back to CDB -> Register,and reservation stations
        if(RESSTATION[r].resultReady){
            // Before Writeback is available there must be a 1 cycle WB delay
            if(RESSTATION[r].WRITEBACK_Lat == WRITEBACK_Lat){
                // set clock cycle when write back occured. (Must add one because increment happens after loop)
                if(INST[RESSTATION[r].instNum].writebackClock == 0)
                    INST[RESSTATION[r].instNum].writebackClock = Clock;
                // Check if any registers (via the registerStatus) are waiting for current r result
                for(int x=0;x<REG.size();x++) {
                    // if RegisterStatus points to the given reservation station r set that register[x] equal to executed result
                    if (REGSTATUS[x].Qi == r) {
                        // Write back to Registers
                        REG[x] = RESSTATION[r].result;
                        REGSTATUS[x].Qi = RegStatusEmpty;
                    }
                }
                // Check if any reservation stations are waiting for current r result
                for(int y=0;y<RESSTATION.size();y++){
                    // check if any reservation stations are waiting for the given result as an operand
                    // Write back to reservation stations
                    // Given RS is not longer waiting for this operand value
                    if(RESSTATION[y].Qj==r){
                        RESSTATION[y].Vj=RESSTATION[r].result;
                        RESSTATION[y].Qj=OperandAvailable;
                    }
                    if(RESSTATION[y].Qk==r){
                        RESSTATION[y].Vk=RESSTATION[r].result;
                        RESSTATION[y].Qk=OperandAvailable;
                    }
                }
                // The given reservation station can now be used again
                RESSTATION[r].resultReady = false; // Set RS resultReady back to false
                RESSTATION[r].busy = false;
                RESSTATION[r].Qj = OperandInit;
                RESSTATION[r].Qk = OperandInit;
                RESSTATION[r].Vj = 0;
                RESSTATION[r].Vk = 0;
                RESSTATION[r].WRITEBACK_Lat = 0;
            }
            else
                RESSTATION[r].WRITEBACK_Lat++;
        }
    }

}//END WRITEBACK()
//####################################################################################################################

//####################################################################################################################
// Helper Functions
void printRegisterStatus(vector<RegisterStatus> RegisterStatusVector){
    cout << "Register Status: " << endl;
    for(int i=0; i<RegisterStatusVector.size(); i++)
        cout << RegisterStatusVector[i].Qi << ' ';
    cout << endl;
}
void printReservationStations(vector<ReservationStation> ReservationStationsVector){
    for(int i=0; i<ReservationStationsVector.size(); i++)
        cout << "RS #: " << i << "  Busy: " << ReservationStationsVector[i].busy << "  op: " <<
                ReservationStationsVector[i].op << "  Vj: " << ReservationStationsVector[i].Vj << "  Vk: " <<
                ReservationStationsVector[i].Vk << "  Qj: " << ReservationStationsVector[i].Qj << "  Qk: " <<
                ReservationStationsVector[i].Qk << endl;
}
void printRegisters(vector<int> RegistersVector){
    cout << "Register Content:" << endl;
    for(int i=0; i<RegistersVector.size(); i++)
        cout << RegistersVector[i] << ' ';
    cout << endl;
}
void printInstructions(vector<Instruction> InstructionsVector){
    for(int i=0; i<InstructionsVector.size(); i++)
        cout << "Instruction #: " << i << "  Operation: " << InstructionsVector[i].op << "  " <<
                InstructionsVector[i].rd << " <- " << InstructionsVector[i].rs << " op " <<
                InstructionsVector[i].rt << endl;
}
void printTimingTable(vector<Instruction> INST){
    char separator    = ' ';
    const int width     = 10;
    char lineSeperator = '-';
    const int lineWidth = 30;

    // Define column labels
    cout << left << setw(width) << setfill(separator) << "Inst";
    cout << left << setw(width) << setfill(separator) << "Issue";
    cout << left << setw(width) << setfill(separator) << "Execute";
    cout << left << setw(width) << setfill(separator) << "WB";
    cout << left << setw(width) << setfill(separator) << "SystemClock" << endl;
    cout << right << setw(width*5) << setfill(separator) << Clock;
    cout << endl;
    cout << left << setw(lineWidth) << setfill(lineSeperator);
    cout << endl;
    // Define Row Labels and values
    for(int i=0;i<INST.size();i++){
        cout << left  << setw(width) << setfill(separator) << i;
        cout << left << setw(width) << setfill(separator) << INST[i].issueClock;
        cout << INST[i].executeClockBegin <<  "-";
        cout << left << setw(width) << setfill(separator)  << INST[i].executeClockEnd;
        cout << left << setw(width) << setfill(separator) << INST[i].writebackClock;
        cout << endl;
    }

}
//####################################################################################################################
