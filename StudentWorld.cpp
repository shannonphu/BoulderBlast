#include "StudentWorld.h"
#include <string>
#include "GameWorld.h"
#include "GraphObject.h"
#include "Actor.h"
#include "Level.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Construct StudentWorld:
//	- start bonus at 1000
//  - have not completed level yet
//  - total levels discovered = 0
//  - jewels made = 0
StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	numAllocated = 0;
	p = nullptr;
	m_bonus = 1000;
	jewelsInThisLevel = 0;
	completedLevel = false;
	totalLevels = 0;
	actor = new list<Actor*>;
}

// Add a bullet in front of the Robot or Player
void StudentWorld::addBullet(GraphObject::Direction dir, movingObj* l)
{
	actor->push_front(new Bullet(l->getXInFrontOf(dir,1), l->getYInFrontOf(dir,1), l->getDirection(), this));
}

// Add either KleptoBot or Angry KleptoBot to actor list
void StudentWorld::addBot(int x, int y, char type)
{
	if (type == 'k')
		actor->push_back(new KleptoBot(IID_KLEPTOBOT, x, y, this, 'k', 10));
	else if (type == 'a')
		actor->push_back(new AngryKleptoBot(x, y, this, 'a'));
}

// Check if a boulder, hole, wall, robot, or other combination of the 4 will prevent movement
bool StudentWorld::hasObjectBlocking(GraphObject::Direction dir, movingObj* j)
{
	if (hasA(IID_WALL,j->getXInFrontOf(dir,1), j->getYInFrontOf(dir,1)))
		return true;
	else if (hasA(IID_BOULDER, j->getXInFrontOf(dir, 1), j->getYInFrontOf(dir, 1)) && hasA(IID_BOULDER, j->getXInFrontOf(dir, 2), j->getYInFrontOf(dir, 2)))
		return true;
	else if (hasA(IID_BOULDER, j->getXInFrontOf(dir, 1), j->getYInFrontOf(dir, 1)) && hasA(IID_HOLE, j->getXInFrontOf(dir, 2), j->getYInFrontOf(dir, 2)))
		return false;
	else if (hasA(IID_BOULDER, j->getXInFrontOf(dir, 1), j->getYInFrontOf(dir, 1)) && getActorAtLocation(j->getXInFrontOf(dir, 2), j->getYInFrontOf(dir, 2)))
		return true;
	else if (hasA(IID_HOLE, j->getXInFrontOf(dir, 1), j->getYInFrontOf(dir, 1)) || hasA(IID_ROBOT_FACTORY, j->getXInFrontOf(dir, 1), j->getYInFrontOf(dir, 1)))
		return true;
	else if (hasA(IID_SNARLBOT, j->getXInFrontOf(dir, 1), j->getYInFrontOf(dir, 1)) || hasA(IID_KLEPTOBOT, j->getXInFrontOf(dir, 1), j->getYInFrontOf(dir, 1)))
		return true;
	return false;
}

int StudentWorld::getBonus()
{
	return m_bonus;
}

int StudentWorld::getJewels()
{
	return jewelsInThisLevel;
}

// Has completed level
void StudentWorld::changeCompletion(bool predicate)
{
	if (predicate)
		completedLevel = true;
}

// -set up data file name with ostringstream
// -check if there are any errors in forming the level
// -check if all the levels have been completed
// -allocate and place all game actors on screen
int StudentWorld::init()
{
	ostringstream l;
	l.fill('0');
	l << "level" << setw(2) << getLevel() << ".dat";
	string input = l.str();
	bool anyErrors = isLevelWellFormed(input);	
	if (anyErrors)
		return GWSTATUS_LEVEL_ERROR;
	if (getLevel() > 99 || getLevel() == totalLevels)
		return GWSTATUS_PLAYER_WON;
	jewelsInThisLevel = 0;
	allocation(input);
	return GWSTATUS_CONTINUE_GAME;
}

 // Check if level well formed and if it produces any errors on loading
bool StudentWorld::isLevelWellFormed(string levelDataFile)
{
	Level lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel(levelDataFile);
	if (result == Level::load_fail_file_not_found)
	{
		if (getLevel() == totalLevels)
			return false;
		return true;
	}
	else if (result == Level::load_fail_bad_format)
		return true;
	totalLevels++;
	return false;  				// means can get contents of level //success
}

// Add onto the list every game actor appearing on start-up 
//of game at their proper locations using dynamic allocation
int StudentWorld::allocation(string levelData)
{
	Level lev(assetDirectory());
	Level::LoadResult result = lev.loadLevel(levelData);
	for (int i = 0; i < VIEW_HEIGHT; i++)
	{
		for (int j = 0; j < VIEW_WIDTH; j++)
		{
			Level::MazeEntry he = lev.getContentsOf(i, j);
			switch (he)
			{
			case Level::player:
				p = new Player(i, j, this);
				numAllocated++;
				break;
			case Level::wall:
				actor->push_back(new Wall(i, j, this));
				break;
			case Level::boulder:
				actor->push_back(new Boulder(i, j, this));
				break;
			case Level::hole:
				actor->push_back(new Hole(i, j, this));
				break;
			case Level::jewel:
				actor->push_back(new Jewel(i, j, this));
				jewelsInThisLevel++;
				break;
			case Level::extra_life:
				actor->push_back(new ExtraLifeGoodie(i, j, this));
				break;
			case Level::restore_health:
				actor->push_back(new RestoreHealthGoodie(i, j, this));
				break;
			case Level::ammo:
				actor->push_back(new ammoGoodie(i, j, this));
				break;
			case::Level::exit:
				actor->push_back(new Exit(i, j, this));
				break;
			case::Level::horiz_snarlbot:
				actor->push_back(new SnarlBot(i, j, GraphObject::Direction::right, this));
				break;
			case::Level::vert_snarlbot:
				actor->push_back(new SnarlBot(i, j, GraphObject::Direction::down, this));
				break;
			case::Level::kleptobot_factory:
				actor->push_back(new RobotFactory(i, j, this, 'k'));
				break;
			case::Level::angry_kleptobot_factory:
				actor->push_back(new RobotFactory(i, j, this, 'a'));
				break;
			}
		}
	}
	return 1;
}

// -set-up stats display at the beginning of every tick
// -check if player is dead
// -iterate through list checking if actor has died or not
//      if actor is dead, erase it from the list and deallocate memory
//      if actor is not dead, tell it to do something
// -decrement the bonus if its above 0
// -check if player is dead again
// -check if completed level at end of tick
int StudentWorld::move()
{
	setDisplayText();
	if (p->isDead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	else
		p->doSomething();
	list<Actor*>::iterator itr = actor->begin();
	while (itr != actor->end())
	{
		if (!(*itr)->isDead())	//if still alive
		{
			(*itr)->doSomething();
			itr++;
		}
		else					// if died
		{
			list<Actor*>::iterator itr2 = itr;
			itr2++;
			(*itr)->setVisible(false);
			delete (*itr);
			actor->erase(itr);
			itr = itr2;
		}
	}
	if (m_bonus > 0)
		m_bonus--;
	if (p->isDead())
	{
		decLives();
		playSound(SOUND_PLAYER_DIE);
		return GWSTATUS_PLAYER_DIED;
	}
	if (completedLevel)
	{
		completedLevel = false;
		jewelsInThisLevel = 0;
		return GWSTATUS_FINISHED_LEVEL;
	}
	return GWSTATUS_CONTINUE_GAME;
}

// Get values for score, level, lives, HP, ammo, and bonus, and format the properly for stats display
void StudentWorld::setDisplayText()
{
	int score = getScore();
	int level = getLevel();
	int livesLeft = getLives();
	int hitpts = p->getHP();
	int ammo = p->getAmmo();
	unsigned int bonus = getBonus();
	string formatted = formatDisplay(score, level, livesLeft, hitpts, ammo, bonus);
	setGameStatText(formatted);
}

// Format stats display properly
string StudentWorld::formatDisplay(int score, int level, int lives, int health, int ammo, unsigned int bonus)
{
// score
	ostringstream o_stats;
	ostringstream o2;
	//o_stats << "Score: ";
	o_stats.fill('0');
	o_stats << "Score: " << setw(7) << score << "  ";
// level
	o_stats.fill('0');
	o_stats << "Level: " << setw(2) << level << "  ";
// # lives
	o_stats << "Lives: " << setw(2) << lives << "  ";
// health
	int percent = health * 100 / 20;
	o2 << "Health: " << setw(3) << percent << "%  ";
// ammo
	o2 << "Ammo: " << setw(3) << ammo << "  ";
// bonus
	o2 << "Bonus: " << setw(4) << bonus;
	
// form overall string
	string stats = o_stats.str() + o2.str();
	return stats;
}

// Deallocate all dynamically allocated memory
void StudentWorld::cleanUp()
{
	list<Actor*>::iterator itr = actor->begin();
	while (itr != actor->end())
	{
		list<Actor*>::iterator itr2 = itr;
		itr2++;
		(*itr)->setVisible(false);
		delete (*itr);
		actor->erase(itr);
		itr = itr2;
	}
	delete p;
}

// Check if there has been a certain type of game actor at a certain location on the game map
bool StudentWorld::hasA(int m_ID, int x, int y)
{
	for (list<Actor*>::iterator itr = actor->begin(); itr != actor->end(); itr++)
	{
		if ((*itr)->getX() == x && (*itr)->getY() == y && (*itr)->returnID() == m_ID)
		{
			return true;
		}
	}
	if (player()->getX() == x && player()->getY() == y && m_ID == IID_PLAYER)
		return true;
	return false;
}

// Return a pointer to the player
movingObj* StudentWorld::player()
{
	return p;
}

// Return a pointer to the game acter at the desired location
Actor* StudentWorld::getActorAtLocation(int x, int y) const
{
	list<Actor*>::const_iterator itr = actor->begin();
	while (itr != actor->end())
	{
		if ((*itr)->getX() == x && (*itr)->getY() == y)
		{
			Actor* p = *itr;
			return p;
		}
		itr++;
	}
	return nullptr;
}

StudentWorld::~StudentWorld()
{
	list<Actor*>::iterator itr = actor->begin();
	while (itr != actor->end())
	{
		list<Actor*>::iterator itr2 = itr;
		itr2++;
		(*itr)->setVisible(false);
		delete (*itr);
		actor->erase(itr);
		itr = itr2;
	}
	delete p;
}