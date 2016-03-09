//
// Created by aj on 3/8/16.
//

#include "RS.h"

RS::RS(){
    busy = false;
    op = 0;
    lat = 0;
    result = 0;
    resultReady = false;
    Qj = 0;
    Qk = 0;
    Vj = 0;
    Vk = 0;
}
RS::RS(int OP){
    busy = false;
    op = OP;
    lat = 0;
    result = 0;
    resultReady = false;
    Qj = 0;
    Qk = 0;
    Vj = 0;
    Vk = 0;

}