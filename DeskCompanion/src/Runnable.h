#ifndef RUNNABLE_H
#define RUNNABLE_H

#include "../debug.h"

class Runnable {
  public:
    virtual void setup() = 0;
    virtual void loop() = 0;
};

#endif
