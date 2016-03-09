//
// Created by aj on 3/8/16.
//

#include "Instruction.h"

Instruction::Instruction() {
    rd = 0;
    rs = 0;
    rt = 0;
    op = 0;
}

Instruction::Instruction(int RD,int RS, int RT, int OP) {
    rd = RD;
    rs = RS;
    rt = RT;
    op = OP;
}