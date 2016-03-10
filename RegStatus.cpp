//
// Created by aj on 3/8/16.
//

#include "RegStatus.h"

RegStatus::RegStatus(){
    busy = false;
    Qi = 0;
}
RegStatus::RegStatus(int RegStatusEmpty) {
    Qi = RegStatusEmpty;
}
