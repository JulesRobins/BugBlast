#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <vector>
#include "Actor.h"
#include "Level.h"

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Player;
class Exit;

class StudentWorld : public GameWorld
{
public:

	StudentWorld();
    
    virtual ~StudentWorld();
    
    virtual int init();

	virtual int move();

	virtual void cleanUp();
    
    Actor* getActorAt(int x, int y);
    
    void incNumZumi();
    
    void decNumZumi();
    
    Level& myLev();
    
    unsigned int getSprayerCount();
    
    void incSprayerCount() {m_numSprayers++;}
    
    void decSprayerCount() {m_numSprayers--;}
    
    void addActor(Actor* newbie);
    
    Player* getPlayer() {return m_player;}

private:
    vector <Actor*> m_actors;
    Level m_lev;
    Player* m_player;
    Exit* m_exit;
    unsigned int m_bonus;
    unsigned int m_numZumi;
    unsigned int m_numSprayers;
};

#endif // STUDENTWORLD_H_
