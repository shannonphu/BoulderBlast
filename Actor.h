#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>
#include "GameWorld.h"
class StudentWorld;
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ACTOR DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Actor : public GraphObject
{
public:
	Actor::Actor(int m_ID, int startCol, int startRow, Direction d, StudentWorld* thisWorld);	// constructor
	virtual void doSomething() = 0;
	StudentWorld* getWorld();
	void setDead();
	bool isDead();
	void moveOnce(Direction dir);
	void changeVisibility(bool pred) { visible = pred; }
	virtual int returnID() = 0;
	bool getVisibility() { return visible; }
	virtual ~Actor() {}				// destructor
private:
	int start_x;
	int start_y;
	StudentWorld* m_thisWorld;
	bool alive;
	bool visible;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// nonMovingObj DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class nonMovingObj : public Actor
{
public:
	nonMovingObj(int m_ID, int startCol, int startRow, StudentWorld* thisWorld)			// constructor
		: Actor(m_ID, startCol, startRow, none, thisWorld)
	{
		setVisible(true);
	}
	virtual void doSomething() = 0;
	virtual ~nonMovingObj() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXIT DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Exit : public Actor
{
public:
	Exit(int startx, int starty, StudentWorld* thisWorld)
		: Actor(IID_EXIT,startx,starty,none,thisWorld)
	{
		setVisible(false);
		changeVisibility(false);
	}
	virtual void doSomething();
	virtual int returnID() { return IID_EXIT; }
	virtual ~Exit() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WALL DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Wall : public nonMovingObj
{
public:
	Wall::Wall(int startCol, int startRow, StudentWorld* thisWorld)
		: nonMovingObj(IID_WALL, startCol, startRow, thisWorld)
	{
		setVisible(true);
	}
	virtual void doSomething() {}
	virtual int returnID() { return IID_WALL; }
	virtual ~Wall() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HOLE DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Hole : public nonMovingObj
{
public:
	Hole(int startcol, int startrow, StudentWorld* thisWorld)
		: nonMovingObj(IID_HOLE, startcol, startrow, thisWorld)
	{}
	virtual void doSomething();
	virtual int returnID() { return IID_HOLE; }
	virtual ~Hole() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// JEWEL DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Jewel : public nonMovingObj
{
public:
	Jewel(int startx, int starty, StudentWorld* thisWorld)
		:nonMovingObj(IID_JEWEL, startx, starty, thisWorld)
	{}
	virtual void doSomething();
	virtual int returnID() { return IID_JEWEL; }
	virtual ~Jewel() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BULLET DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Bullet : public Actor
{
public:
	Bullet(int startCol, int startRow, Direction d, StudentWorld* thisWorld);
	virtual void doSomething();
	void ifHitSomething();
	virtual int returnID() { return IID_BULLET; }
	virtual ~Bullet() { }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GOODIES DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Goodies : public nonMovingObj
{
public:
	Goodies(int m_ID, int col, int row, StudentWorld* thisWorld)
		:nonMovingObj(m_ID, col, row, thisWorld)
	{}
	virtual void doSomething();
	virtual void action() = 0;
	virtual ~Goodies() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXTRA LIFE GOODIES DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ExtraLifeGoodie : public Goodies
{
public: 
	ExtraLifeGoodie(int col, int row, StudentWorld* thisWorld)
		: Goodies(IID_EXTRA_LIFE, col, row, thisWorld)
	{}
	virtual void action();
	virtual int returnID() { return IID_EXTRA_LIFE; }
	virtual ~ExtraLifeGoodie() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RESTORE HEALTH GOODIES DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RestoreHealthGoodie : public Goodies
{
public:
	RestoreHealthGoodie(int col, int row, StudentWorld* thisWorld)
		: Goodies(IID_RESTORE_HEALTH, col, row, thisWorld)
	{}
	virtual void action();
	virtual int returnID() { return IID_RESTORE_HEALTH; }
	virtual ~RestoreHealthGoodie() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AMMO GOODIE DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ammoGoodie : public Goodies
{
public:
	ammoGoodie(int col, int row, StudentWorld* thisWorld)
		: Goodies(IID_AMMO, col, row, thisWorld)
	{}
	virtual void action();
	virtual int returnID() { return IID_AMMO; }
	virtual ~ammoGoodie() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVING OBJECT DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class movingObj : public Actor
{
public:
	movingObj(int m_ID, int startC, int startR, Direction dir, StudentWorld* thisWorld, int numHitPts)
		:Actor(m_ID, startC, startR, dir, thisWorld)
	{
		setVisible(true);
		setDirection(dir);
		m_ammo = 20;
		m_curhitpts = numHitPts;
	}
	virtual void doSomething() = 0;
	// Deal with ammo
	void incAmmo();
	void decAmmo();
	int getAmmo();
	// Deal with location
	int getXInFrontOf(Direction dir, int howMany);
	int getXBehind(Direction dir, int howMany);
	int getYInFrontOf(Direction dir, int howMany);
	int getYBehind(Direction dir, int howMany);
	// Deal with HP
	void restoreHealth();
	virtual void decHitPts() {	m_curhitpts -= 2;	}
	void setHP(int input) { m_curhitpts = input; }
	int getHP() { return m_curhitpts; }
	// Dealing with Jewels:
	virtual void incNumJewelsCollected() {};
	virtual int getJewelsCollected() { return 0; };
	virtual ~movingObj() {}
private:
	int m_ammo;
	int m_curhitpts;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLAYER DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Player : public movingObj
{
public:
	Player(int startCol, int startRow, StudentWorld* thisWorld)
		: movingObj(IID_PLAYER, startCol, startRow, right, thisWorld, 20)
	{	
		numJewelsCollected = 0;
	}
	virtual void doSomething();
	void pushBoulder(Direction dir);
	virtual void incNumJewelsCollected() { numJewelsCollected++; }
	virtual int getJewelsCollected() { return numJewelsCollected; }
	virtual int returnID() { return IID_PLAYER; }
	virtual ~Player() {}
private:
	int numJewelsCollected;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BOULDER DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Boulder : public movingObj
{
public:
	Boulder(int startcol, int startRow, StudentWorld* thisworld)
		:movingObj(IID_BOULDER, startcol, startRow, none, thisworld, 10)
	{ }
	virtual void doSomething();
	void push();
	virtual void incNumJewelsCollected() { return; };
	virtual int getJewelsCollected() { return 0; };
	virtual int returnID() { return IID_BOULDER; }
	virtual ~Boulder() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ROBOT DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Robot : public movingObj
{
public:
	Robot(int ID, int startx, int starty, Direction d, StudentWorld* thisWorld, int numHPinit, int raiseScoreBy, char typeBot)
		: movingObj(ID, startx, starty, d, thisWorld, numHPinit)
	{
		tickNum = tickOutOf();
		raiseScore = raiseScoreBy;
		m_typeBot = typeBot;
	}
	virtual void damage();
	virtual void doSomething();			
	int tickOutOf();
	bool checkTick();
	void incTick();
	bool blockingRobot();
	bool willHitAnything(int x, int y);
	bool facingPlayer();
	virtual void changeDirection();
	virtual void decHitPts();
	bool clearShot();
	virtual int returnID() { return IID_SNARLBOT; }
	virtual ~Robot() {}
private:
	int tickNum;
	int raiseScore;
	char m_typeBot;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SNARLBOT DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SnarlBot : public Robot
{
public:
	SnarlBot(int startx, int starty, Direction d, StudentWorld* thisWorld)
		:Robot(IID_SNARLBOT, startx, starty, d, thisWorld, 10, 100, 's')
	{}
	virtual ~SnarlBot() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// kLEPTOBOT DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class KleptoBot : public Robot
{
public:
	KleptoBot(int id, int startx, int starty, StudentWorld* thisWorld, char type, int incpts)
		: Robot(id, startx, starty, right, thisWorld, 5, incpts, type)
	{
		distanceBeforeTurning = newDistRange();
		hasGoodie = false;
		curdist = 1;
		possibleDir = 4;
	}
	int newDistRange() { return rand() % 6 + 1; }
	virtual void doSomething();
	// Deals with direction
	virtual void changeDirection();
	void increaseCurDist();
	GraphObject::Direction randDirection(int out);
	void decreasePossibleDir();
	virtual int returnID() { return IID_KLEPTOBOT; }
	virtual ~KleptoBot() {}
private:
	int distanceBeforeTurning;
	int curdist;
	int possibleDir;
	bool hasGoodie;
	Actor* good;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ANGRY KLEPTOBOT DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AngryKleptoBot : public KleptoBot
{
public:
	AngryKleptoBot(int startx, int starty, StudentWorld* thisworld, char type)
		: KleptoBot(IID_ANGRY_KLEPTOBOT,startx, starty, thisworld, type, 20)
	{
		setHP(8);
	}
	virtual int returnID() { return IID_ANGRY_KLEPTOBOT; }
	virtual ~AngryKleptoBot() {}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ROBOT FACTORY DECLARATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RobotFactory : public nonMovingObj
{
public:
	RobotFactory(int startx, int starty, StudentWorld* thisworld, char typeBot)
		:nonMovingObj(IID_ROBOT_FACTORY, startx, starty, thisworld)
	{
		numBotsInRegion = 0;
		typeOfBot = typeBot;
	}
	int minXBound();
	int minYBound();
	int maxXBound();
	int maxYBound();
	virtual int returnID() { return IID_ROBOT_FACTORY; }
	virtual void doSomething();
private:
	int numBotsInRegion;
	char typeOfBot;
};

#endif // ACTOR_H_
