//
// Created by aj on 3/8/16.
//

#ifndef TOMASULO_RS_H
#define TOMASULO_RS_H


class RS {
    public:
        bool busy;
        int Qj;
        int Qk;
        int Vj;
        int Vk;
        int lat;
        int op;
        int result;
        bool resultReady;

    //**** Methods
    public:
        RS();
        RS(int, int);
};


#endif //TOMASULO_RS_H
