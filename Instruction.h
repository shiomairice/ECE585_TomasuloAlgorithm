//
// Created by aj on 3/8/16.
//

#ifndef TOMASULO_INSTRUCTION_H
#define TOMASULO_INSTRUCTION_H


class Instruction {
    public:
        int rd; // rd <- rs + rt
        int rs;
        int rt;
        int op; // add,sub,mult,div
    //**** Class methods
    public:
        Instruction();
        Instruction(int,int,int,int);
};

#endif //TOMASULO_INSTRUCTION_H
