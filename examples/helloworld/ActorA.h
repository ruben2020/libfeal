#ifndef _FEAL_ACTOR_A_H
#define _FEAL_ACTOR_A_H

#include "feal.h"

class EvtNotifyVege : public feal::Event
{
friend class ActorA;

public:
EvtNotifyVege() = default;
EvtNotifyVege( const EvtNotifyVege & ) = default;
EvtNotifyVege& operator= ( const EvtNotifyVege & ) = default;
virtual ~EvtNotifyVege() = default;
virtual feal::EventId_t getId(void);

int get_carrots(void);
int get_tomatoes(void);

private:
void set_carrots(const int& num);
void set_tomatoes(const int& num);
int carrots=0;
int tomatoes=0;

};

class ActorA : public feal::Actor
{

public:

ActorA() = default;
~ActorA() = default;


virtual void initActor(void);
virtual void startActor(void);
virtual void pauseActor(void);
virtual void shutdownActor(void);

private:

};


#endif // _FEAL_ACTOR_A_H
