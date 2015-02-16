#include "StudentWorld.h"
#include "Actor.h"
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>
#include <vector>
using namespace std;

GameWorld* createStudentWorld()
{
	return new StudentWorld();
}

StudentWorld::StudentWorld()
{
    m_numZumi=0;
    m_numSprayers=0;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    unsigned int levNum = getLevel();
    ostringstream levFile;
    levFile.fill('0');
    levFile << "level";
	levFile << setw(2) << levNum;
    levFile << ".dat";
    string levString = levFile.str();
    Level::LoadResult result = m_lev.loadLevel(levString,
            "/Users/julesrobins/Dropbox/Code/CS32/Project3/BugBlast/DerivedData/BugBlast/Build/Products/Debug");
    if (levNum!=0 && result==Level::load_fail_file_not_found)
    {
        return GWSTATUS_PLAYER_WON;
    }
    if (levNum==0 && result==Level::load_fail_file_not_found)
    {
        return GWSTATUS_NO_FIRST_LEVEL;
    }
    if (result==Level::load_fail_bad_format)
    {
        return GWSTATUS_LEVEL_ERROR;
    }
    m_bonus=m_lev.getOptionValue("levelBonus");
    Actor* adder;
    for (int i=0; i<VIEW_WIDTH; i++)
    {
        for (int j=0; j<VIEW_HEIGHT; j++)
        {
            Level::MazeEntry me = m_lev.getContentsOf(i, j);
            switch(me)
            {
                case Level::empty:                                                  break;
                case Level::simple_zumi: adder=new SimpleZumi(i,j,m_lev.getOptionValue("ticksPerSimpleZumiMove")); incNumZumi();   break;
                case Level::complex_zumi: adder=new ComplexZumi(i,j,m_lev.getOptionValue("ticksPerComplexZumiMove"),m_lev.getOptionValue("complexZumiSearchDistance")); incNumZumi(); break;
                case Level::perma_brick: adder=new PermaBrick(i,j);                 break;
                case Level::destroyable_brick: adder=new DestroyableBrick(i,j);     break;
                case Level::player: m_player=new Player(i,j); m_player->setWorld(this); break;
                case Level::exit: m_exit=new Exit(i,j); m_exit->setWorld(this);         break;
            }
            if (me!=Level::empty && me!=Level::player && me!=Level::exit)
            {
                adder->setWorld(this);
                m_actors.push_back(adder);
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after hitting enter a few times
    if (m_exit->isVisible() && m_exit->getX()==m_player->getX() && m_exit->getY()==m_player->getY())
    {
        increaseScore(m_bonus);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    m_player->doSomething();
    if (!m_player->isAlive()) {return GWSTATUS_PLAYER_DIED;}
    for(int i = 0; i<m_actors.size() ; i++)
    {
        m_actors[i]->doSomething();
        if (!m_player->isAlive()) {return GWSTATUS_PLAYER_DIED;}
    }
    vector<Actor*> holder;
    for(int i = 0; i<m_actors.size() ; i++)
    {
        if(m_actors[i]->isAlive())
        {
            holder.push_back(m_actors[i]);
        }
        else
        {
            delete m_actors[i];
        }
    }
    m_actors=holder;
    if (m_bonus>0) {m_bonus--;}
    if (m_numZumi==0 && !m_exit->isVisible())
    {
        m_exit->setVisible(true);
        playSound(SOUND_REVEAL_EXIT);
    }
    string header;
    ostringstream top;
    top.fill('0');
    top << "Score: ";
	top << setw(8) << getScore();
    top << "  Level: ";
    top << setw(2) << getLevel();
    top << "  Lives: ";
    top << setw(3) << getLives();
    top << "  Bonus: ";
    top.fill(' ');
    top << setw(6) << m_bonus;
    header=top.str();
    setGameStatText(header);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_player;
    delete m_exit;
    vector<Actor*>::iterator it;
    for(it = m_actors.begin() ; it != m_actors.end() ; ++it)
    {
        delete *it;
    }
    m_actors.clear();
}

void StudentWorld::incNumZumi() {m_numZumi++;}

void StudentWorld::decNumZumi() {m_numZumi--;}

Level& StudentWorld::myLev() {return m_lev;}

unsigned int StudentWorld::getSprayerCount() {return m_numSprayers;}

void StudentWorld::addActor(Actor* newbie)
{
    newbie->setWorld(this);
    m_actors.push_back(newbie);
}

Actor* StudentWorld::getActorAt(int x, int y)
{
    for (int i=0; i<m_actors.size(); i++)
    {
        if (m_actors[i]->getX()==x && m_actors[i]->getY()==y && m_actors[i]->isAlive())
        {
            return m_actors[i];
        }
    }
    if (m_player->getX()==x && m_player->getY()==y)
    {
        return dynamic_cast<Actor*>(m_player);
    }
    if (m_exit->getX()==x && m_exit->getY()==y)
    {
        return dynamic_cast<Actor*>(m_exit);
    }
    return NULL;
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
