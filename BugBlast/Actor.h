#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <cstdlib>
#include <string>

using namespace std;

class StudentWorld;

// Return a uniformly distributed random int from lowest to highest, inclusive
int randInt(int lowest, int highest);

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject
{
public:
    Actor(int imageID, int startX, int startY);
    virtual ~Actor();
    bool isAlive();
    virtual void killMe();
    virtual void doSomething()=0;
    StudentWorld* getWorld();
    void setWorld(StudentWorld* home);
private:
    bool m_alive;
    StudentWorld* m_home;
};

class LoudActor: public Actor
{
public:
    LoudActor(int imageID, int startX, int startY);
    virtual ~LoudActor();
    virtual void killMe();
    virtual void deathCurse()=0;
private:
};

class Brick: public Actor
{
public:
    Brick(int imageID, int startX, int startY);
    virtual ~Brick();
    virtual void doSomething() {}
private:
};

class PermaBrick: public Brick
{
public:
    PermaBrick(int startX, int startY);
    virtual ~PermaBrick();
private:
};

class DestroyableBrick: public Brick
{
public:
    DestroyableBrick(int startX, int startY);
    virtual ~DestroyableBrick();
private:
};

class Player: public LoudActor
{
public:
    Player(int startX, int startY);
    virtual ~Player();
    virtual void doSomething();
    void tryMove(int x, int y);
    virtual void deathCurse();
    virtual void killMe();
    void setWalk(unsigned int t) {m_walk = t;}
    void setSpray(unsigned int t) {m_spray = t;}
private:
    unsigned int m_walk;
    unsigned int m_spray;
};

class Zumi: public LoudActor
{
public:
    Zumi(int imageID, int startX, int startY, int ticks);
    virtual ~Zumi()=0;
    virtual void killMe();
    virtual void deathCurse();
    void chooseDir();
    void setDir(unsigned int dir) {currentDirection = dir;}
    void restartCount();
    void decCount();
    unsigned int getDir();
    unsigned int getCount();
    virtual void doSomething();
    virtual void attemptMove();
    bool canMoveTo(int x, int y);
    void setScore(unsigned int score) {m_score=score;}
private:
    unsigned int currentDirection;
    unsigned int ticksPerMove;
    unsigned int countdown;
    unsigned int m_score;
};

class SimpleZumi: public Zumi
{
public:
    SimpleZumi(int startX, int startY, int ticks);
    virtual ~SimpleZumi();
private:
};

class ComplexZumi: public Zumi
{
public:
    ComplexZumi(int startX, int startY, int ticks, int smell);
    virtual ~ComplexZumi();
    virtual void attemptMove();
    bool isPath(Player* p, unsigned int& dir);
    void makeMaze();
private:
    unsigned int m_smell;
    vector<string> m_maze;
    class Coord
    {
    public:
        Coord(int rr, int cc, unsigned int dd) : m_r(rr), m_c(cc), m_d(dd) {}
        int r() const { return m_r; }
        int c() const { return m_c; }
        unsigned int d() const {return m_d;}
    private:
        int m_r;
        int m_c;
        unsigned int m_d;
    };
};

class Exit: public Actor
{
public:
    Exit(int startX, int startY);
    virtual ~Exit();
    virtual void doSomething(){}
private:
};

class TimedActor: public Actor
{
public:
    TimedActor(int imageID, int startX, int startY);
    virtual ~TimedActor()=0;
    void setTime(unsigned int time) {m_time=time;}
    virtual void doSomething();
private:
    unsigned int m_time;
};

class Goodie: public TimedActor
{
public:
    Goodie(int imageID, int startX, int startY, StudentWorld* world);
    virtual ~Goodie();
    virtual void doSomething();
    virtual void effect()=0;
private:
};

class ExtraLifeGoodie: public Goodie
{
public:
    ExtraLifeGoodie(int startX, int startY, StudentWorld* world);
    virtual ~ExtraLifeGoodie();
    virtual void effect();
private:
};

class WalkThroughWallsGoodie: public Goodie
{
public:
    WalkThroughWallsGoodie(int startX, int startY, StudentWorld* world);
    virtual ~WalkThroughWallsGoodie();
    virtual void effect();
private:
};

class IncreaseSimultaneousBugSprayerGoodie: public Goodie
{
public:
    IncreaseSimultaneousBugSprayerGoodie(int startX, int startY, StudentWorld* world);
    virtual ~IncreaseSimultaneousBugSprayerGoodie();
    virtual void effect();
private:
};

class BugSpray: public TimedActor
{
public:
    BugSpray(int startX, int startY);
    virtual ~BugSpray();
    virtual void doSomething();
private:
};

class BugSprayer: public TimedActor
{
public:
    BugSprayer(int startX, int startY);
    virtual ~BugSprayer();
    virtual void doSomething();
    void placeSpray(int LeftRight, int DownUp); //0 in unused direction, -1 means first option, +1 means second option
private:
};

#endif // ACTOR_H_
