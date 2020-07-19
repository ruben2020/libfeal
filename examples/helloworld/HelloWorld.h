#ifndef _FEAL_HELLO_WORLD_H
#define _FEAL_HELLO_WORLD_H

#include <memory>
#include "feal.h"

class HelloWorld : public feal::Actor
{

public:

HelloWorld() = default;
~HelloWorld() = default;


virtual void initActor(void);
virtual void startActor(void);
virtual void pauseActor(void);
virtual void shutdownActor(void);

private:
std::vector<std::shared_ptr<feal::Actor>> actors;

};


#endif // _FEAL_HELLO_WORLD_H
