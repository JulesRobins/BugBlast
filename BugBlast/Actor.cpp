#include "Actor.h"
#include "StudentWorld.h"
#include <string>
#include <queue>

using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

int randInt(int lowest, int highest)
{
    srand (time(NULL));
    if (highest < lowest)
        swap(highest, lowest);
    return lowest + (rand() % (highest - lowest + 1));
}

Actor::Actor(int imageID, int startX, int startY): GraphObject(imageID, startX, startY)
{
    setVisible(true);
    m_alive=true;
}

Actor::~Actor()
{
    //
}

bool Actor::isAlive()
{
    return m_alive;
}

void Actor::killMe()
{
    if (isAlive())
    {
        m_alive=false;
    }
}

StudentWorld* Actor::getWorld()
{
    return m_home;
}

void Actor::setWorld(StudentWorld* home)
{
    m_home=home;
}

LoudActor::LoudActor(int imageID, int startX, int startY): Actor(imageID, startX, startY)
{
    //
}

LoudActor::~LoudActor()
{
    //
}

void LoudActor::killMe()
{
    Actor::killMe();
    deathCurse();
}

Brick::Brick(int imageID, int startX, int startY): Actor(imageID, startX, startY)
{
    //
}

Brick::~Brick()
{
    //
}

PermaBrick::PermaBrick(int startX, int startY): Brick(IID_PERMA_BRICK, startX, startY)
{
    //
}

PermaBrick::~PermaBrick()
{
    //
}

DestroyableBrick::DestroyableBrick(int startX, int startY): Brick(IID_DESTROYABLE_BRICK, startX, startY)
{
    //
}

DestroyableBrick::~DestroyableBrick()
{
    //
}

Player::Player(int startX, int startY): LoudActor(IID_PLAYER, startX, startY)
{
    m_walk=0;
    m_spray=0;
}

Player::~Player()
{
    //
}

void Player::tryMove(int x, int y)
{
    Actor* a=getWorld()->getActorAt(x, y);
    if (a==NULL)
    {
        moveTo(x,y);
    }
    else
    {
        PermaBrick* pb=dynamic_cast<PermaBrick*>(a);
        DestroyableBrick* db=dynamic_cast<DestroyableBrick*>(a);
        if (pb==NULL && (db==NULL || m_walk>0))
        {
            moveTo(x,y);
        }
    }
}

void Player::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    Actor* cellmate = getWorld()->getActorAt(getX(), getY());
    if (cellmate!=NULL)
    {
        Zumi* z = dynamic_cast<Zumi*>(cellmate);
        Brick* b = dynamic_cast<Brick*>(cellmate);
        Goodie* g =dynamic_cast<Goodie*>(cellmate);
        BugSpray* bs = dynamic_cast<BugSpray*>(cellmate);
        if (z!=NULL || (b!=NULL && m_walk==0) || bs!=NULL)
        {
            killMe();
            return;
        }
        if (g!=NULL)
        {
            g->killMe();
            getWorld()->increaseScore(1000);
            getWorld()->playSound(SOUND_GOT_GOODIE);
            g->effect();
        }
    }
    if (m_spray>0) {m_spray--;}
    if (m_walk>0) {m_walk--;}
    int ch;
    if (getWorld()->getKey(ch))
    {
        // user hit a key this tick!
        switch (ch)
        {
            case KEY_PRESS_LEFT:
                tryMove(getX()-1, getY());
                break;
            case KEY_PRESS_RIGHT:
                tryMove(getX()+1, getY());
                break;
            case KEY_PRESS_UP:
                tryMove(getX(), getY()+1);
                break;
            case KEY_PRESS_DOWN:
                tryMove(getX(), getY()-1);
                break;
            case KEY_PRESS_SPACE:
                if ((getWorld()->getSprayerCount()<getWorld()->myLev().getOptionValue("maxBoostedSprayers") && m_spray>0) || (getWorld()->getSprayerCount()<2 && m_spray==0))
                {
                    Actor* a = getWorld()->getActorAt(getX(), getY());
                    if (a!=NULL)
                    {
                        BugSprayer* bsr = dynamic_cast<BugSprayer*>(a);
                        if (bsr!=NULL) {break;}
                    }
                    Actor* sprayer=new BugSprayer(getX(), getY());
                    getWorld()->addActor(sprayer);
                    getWorld()->incSprayerCount();
                }
                break;
        }
    }
}

void Player::deathCurse()
{
    getWorld()->playSound(SOUND_PLAYER_DIE);
}

void Player::killMe()
{
    LoudActor::killMe();
    getWorld()->decLives();
}

Zumi::Zumi(int imageID, int startX, int startY, int ticks): LoudActor(imageID, startX, startY)
{
    ticksPerMove=ticks;
    chooseDir();
    countdown=ticksPerMove;
}

Zumi::~Zumi()
{
    getWorld()->decNumZumi();
}

void Zumi::killMe()
{
    LoudActor::killMe();
    getWorld()->increaseScore(m_score);
    int goodieVal = randInt(0, 99);
    int tot = getWorld()->myLev().getOptionValue("probOfGoodieOverall");
    int life = (getWorld()->myLev().getOptionValue("probOfExtraLifeGoodie")*tot)/100;
    int walk = life + (getWorld()->myLev().getOptionValue("probOfWalkThruGoodie")*tot)/100;
    int more = walk + (getWorld()->myLev().getOptionValue("probOfMoreSprayersGoodie")*tot)/100;
    Actor* oldieButGoodie;
    if (goodieVal < life)
    {
        oldieButGoodie = new ExtraLifeGoodie(getX(), getY(), getWorld());
    }
    else if (goodieVal >= life && goodieVal < walk)
    {
        oldieButGoodie = new WalkThroughWallsGoodie(getX(), getY(), getWorld());
    }
    else if (goodieVal >= walk && goodieVal < more)
    {
        oldieButGoodie = new IncreaseSimultaneousBugSprayerGoodie(getX(), getY(), getWorld());
    }
    if (goodieVal < more)
    {
        getWorld()->addActor(oldieButGoodie);
    }
}

void Zumi::deathCurse()
{
    getWorld()->playSound(SOUND_ENEMY_DIE);
}

void Zumi::chooseDir()
{
    currentDirection=randInt(0, 3);
}

void Zumi::restartCount() {countdown=ticksPerMove;}

void Zumi::decCount() {countdown--;}

unsigned int Zumi::getCount() {return countdown;}

void Zumi::doSomething()
{
    if (!isAlive())
    {
        return;
    }
    Actor* a = getWorld()->getActorAt(getX(), getY());
    if (a!=NULL)
    {
        Player* p = dynamic_cast<Player*>(a);
        if (p!=NULL) {p->killMe();}
    }
    if (getCount()>1) {decCount();}
    else
    {
        restartCount();
        attemptMove();
    }
}

bool Zumi::canMoveTo(int x, int y)
{
    Actor* occupant = getWorld()->getActorAt(x, y);
    if (occupant==NULL) {return true;}
    else
    {
        Brick* b = dynamic_cast<Brick*>(occupant);
        BugSprayer* bsr = dynamic_cast<BugSprayer*>(occupant);
        if (b!=NULL || bsr!=NULL) {return false;}
    }
    return true;
}

unsigned int Zumi::getDir() {return currentDirection;}

void Zumi::attemptMove()
{
    int s=getDir();
    int x, y;
    switch (s)
    {
        case 0:
            x=getX();
            y=getY()+1;
            break;
        case 1:
            x=getX();
            y=getY()-1;
            break;
        case 2:
            x=getX()-1;
            y=getY();
            break;
        case 3:
            x=getX()+1;
            y=getY();
            break;
    }
    if (canMoveTo(x, y)) {moveTo(x, y);}
    else {chooseDir();}
}

SimpleZumi::SimpleZumi(int startX, int startY, int ticks): Zumi(IID_SIMPLE_ZUMI, startX, startY, ticks)
{
    setScore(100);
}

SimpleZumi::~SimpleZumi()
{
    //
}

ComplexZumi::ComplexZumi(int startX, int startY, int ticks, int smell): Zumi(IID_COMPLEX_ZUMI, startX, startY, ticks)
{
    m_smell=smell;
    setScore(500);
}

ComplexZumi::~ComplexZumi()
{
    //
}

void ComplexZumi::makeMaze()
{
    m_maze.clear();
    char hold;
    string col;
    Actor* a;
    for (int i=0; i<VIEW_WIDTH; i++)
    {
        col="";
        for (int j=0; j<VIEW_HEIGHT; j++)
        {
            a = getWorld()->getActorAt(i, j);
            if (a!=NULL)
            {
                Brick* b = dynamic_cast<Brick*>(a);
                BugSprayer* bsr = dynamic_cast<BugSprayer*>(a);
                if (b!=NULL || bsr!=NULL) {hold='#';}
                else {hold='.';}
            }
            else {hold='.';}
            col+=hold;
        }
        m_maze.push_back(col);
        //cerr << col << endl;
    }
}

bool ComplexZumi::isPath(Player *p, unsigned int &dir)
{
    makeMaze();
    int sr=getY();
    int sc=getX();
    int er=getWorld()->getPlayer()->getY();
    int ec=getWorld()->getPlayer()->getX();
    unsigned int currentDir;
    bool firstTime = 1;
    queue<Coord> coordQueue;
    coordQueue.push(Coord(sr, sc, 0));
    m_maze[sr][sc]='e';
    while (!coordQueue.empty())
    {
        Coord temp=coordQueue.front();
        coordQueue.pop();
        if (temp.r()==er && temp.c()==ec)
        {
            dir=temp.d();
            return true;
        }
        if (m_maze[temp.c()][temp.r()-1]=='.') //S
        {
            if (firstTime) {currentDir=1;}
            else {currentDir=temp.d();}
            coordQueue.push(Coord(temp.r()-1, temp.c(), currentDir));
            m_maze[temp.c()][temp.r()-1]='e';
        }
        if (m_maze[temp.c()+1][temp.r()]=='.') //E
        {
            if (firstTime) {currentDir=3;}
            else {currentDir=temp.d();}
            coordQueue.push(Coord(temp.r(), temp.c()+1, currentDir));
            m_maze[temp.c()+1][temp.r()]='e';
        }
        if (m_maze[temp.c()][temp.r()+1]=='.') //N
        {
            if (firstTime) {currentDir=0;}
            else {currentDir=temp.d();}
            coordQueue.push(Coord(temp.r()+1, temp.c(), currentDir));
            m_maze[temp.c()][temp.r()+1]='e';
        }
        if (m_maze[temp.c()-1][temp.r()]=='.') //W
        {
            if (firstTime) {currentDir=2;}
            else {currentDir=temp.d();}
            coordQueue.push(Coord(temp.r(), temp.c()-1, currentDir));
            m_maze[temp.c()-1][temp.r()]='e';
        }
        firstTime = 0;
    }
    return false;
}

void ComplexZumi::attemptMove()
{
    Player* p = getWorld()->getPlayer();
    unsigned int vert=abs(getY()-p->getY());
    unsigned int hori=abs(getX()-p->getX());
    unsigned int dir;
    if (vert<=m_smell && hori<=m_smell)
    {
        if (isPath(p, dir)) {setDir(dir);}
    }
    Zumi::attemptMove();
}


Exit::Exit(int startX, int startY): Actor(IID_EXIT, startX, startY)
{
    setVisible(false);
}

Exit::~Exit()
{
    //
}

TimedActor::TimedActor(int imageID, int startX, int startY): Actor(imageID, startX, startY)
{
    //
}

TimedActor::~TimedActor()
{
    //
}

void TimedActor::doSomething()
{
    Goodie* g = dynamic_cast<Goodie*>(this);
    if (g!=NULL)
    {
        //
    }
    
    if (!isAlive())
    {
        return;
    }
    if (m_time==0)
    {
        killMe();
    }
    else
    {
        m_time--;
    }
}

BugSprayer::BugSprayer(int startX, int startY): TimedActor(IID_BUGSPRAYER, startX, startY)
{
    setTime(40);
}

BugSprayer::~BugSprayer()
{
    getWorld()->decSprayerCount();
}

void BugSprayer::placeSpray(int LeftRight, int DownUp) //0 in unused direction, -1 means first option, +1 means second option
{
    int x=getX()+LeftRight;
    int y=getY()+DownUp;
    Actor* temp;
    PermaBrick* pb;
    DestroyableBrick* db=NULL;
    Actor* a=getWorld()->getActorAt(x, y);
    Actor* a2=getWorld()->getActorAt(x+LeftRight, y+DownUp);
    if (a!=NULL)
    {
        pb=dynamic_cast<PermaBrick*>(a);
        db=dynamic_cast<DestroyableBrick*>(a);
        if (pb!=NULL)
        {
            return;
        }
    }
    temp=new BugSpray(x,y);
    getWorld()->addActor(temp);
    if (db==NULL) //check a second brick
    {
        if (a2!=NULL)
        {
            pb=NULL;
            pb=dynamic_cast<PermaBrick*>(a2);
            if (pb!=NULL)
            {
                return;
            }
        }
        temp=new BugSpray(x+LeftRight,y+DownUp);
        getWorld()->addActor(temp);
    }
}

void BugSprayer::doSomething()
{
    TimedActor::doSomething();
    if (!isAlive())
    {
        getWorld()->playSound(SOUND_SPRAY);
        Actor* temp=new BugSpray(getX(),getY());
        getWorld()->addActor(temp);
        placeSpray(-1, 0);
        placeSpray(+1, 0);
        placeSpray(0, -1);
        placeSpray(0, +1);
    }
}

BugSpray::BugSpray(int startX, int startY): TimedActor(IID_BUGSPRAY, startX, startY)
{
    setTime(3);
}

BugSpray::~BugSpray()
{
    //
}

void BugSpray::doSomething()
{
    TimedActor::doSomething();
    if (isAlive())
    {
        Actor* overlap=getWorld()->getActorAt(getX(), getY());
        DestroyableBrick* db=dynamic_cast<DestroyableBrick*>(overlap);
        if (db!=NULL)
        {
            db->killMe();
        }
        LoudActor* la=dynamic_cast<LoudActor*>(overlap);
        if (la!=NULL)
        {
            la->killMe();
        }
        BugSprayer* bsr=dynamic_cast<BugSprayer*>(overlap);
        if (bsr!=NULL)
        {
            bsr->killMe();
            bsr->doSomething();
        }
    }
}

Goodie::Goodie(int imageID, int startX, int startY, StudentWorld* world): TimedActor(imageID, startX, startY)
{
    setTime(world->myLev().getOptionValue("goodieLifetimeInTicks"));
    setWorld(world);
}

Goodie::~Goodie()
{
    //
}

void Goodie::doSomething()
{
    TimedActor::doSomething();
    if (!isAlive()) {return;}
    Actor* a = getWorld()->getActorAt(getX(), getY());
    if (a!=NULL)
    {
        Player* p = dynamic_cast<Player*>(a);
        if (p!=NULL)
        {
            killMe();
            getWorld()->increaseScore(1000);
            getWorld()->playSound(SOUND_GOT_GOODIE);
            effect();
        }
    }
}

ExtraLifeGoodie::ExtraLifeGoodie(int startX, int startY, StudentWorld* world): Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, world)
{
    //
}

ExtraLifeGoodie::~ExtraLifeGoodie()
{
    //
}

void ExtraLifeGoodie::effect()
{
    getWorld()->incLives();
}

WalkThroughWallsGoodie::WalkThroughWallsGoodie(int startX, int startY, StudentWorld* world): Goodie(IID_WALK_THRU_GOODIE, startX, startY, world)
{
    //
}

WalkThroughWallsGoodie::~WalkThroughWallsGoodie()
{
    //
}

void WalkThroughWallsGoodie::effect()
{
    getWorld()->getPlayer()->setWalk(getWorld()->myLev().getOptionValue("walkThruLifetimeTicks"));
}

IncreaseSimultaneousBugSprayerGoodie::IncreaseSimultaneousBugSprayerGoodie(int startX, int startY, StudentWorld* world): Goodie(IID_INCREASE_SIMULTANEOUS_SPRAYER_GOODIE, startX, startY, world)
{
    //
}

IncreaseSimultaneousBugSprayerGoodie::~IncreaseSimultaneousBugSprayerGoodie()
{
    //
}

void IncreaseSimultaneousBugSprayerGoodie::effect()
{
    getWorld()->getPlayer()->setSpray(getWorld()->myLev().getOptionValue("boostedSprayerLifetimeTicks"));
}