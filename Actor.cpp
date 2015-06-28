#include "Actor.h"
#include "StudentWorld.h"
#include "GameWorld.h"
#include <vector>
#include <iostream>
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ACTOR IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Construct Actor base class
Actor::Actor(int m_ID, int startCol, int startRow, Direction d, StudentWorld* thisWorld)
	:GraphObject(m_ID, startCol, startRow, d)
{
	m_thisWorld = thisWorld;
	alive = true;
	visible = true;
}

void Actor::setDead()
{
	alive = false;
}

bool Actor::isDead()
{
	return alive == false;
}

// Move the actor once in its current direction
void Actor::moveOnce(Direction dir)
{
	switch (dir)
	{	
	case GraphObject::up:
		moveTo(getX(), getY() + 1);
		break;
	case GraphObject::down:
		moveTo(getX(), getY() - 1);
		break;
	case GraphObject::left:
		moveTo(getX() - 1, getY());
		break;
	case GraphObject::right:
		moveTo(getX() + 1, getY());
		break;
	}
}

// Return a pointer to the StudentWorld the game is in
StudentWorld* Actor::getWorld()
{
	return m_thisWorld;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXIT IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Check if the Jewels the player has collected equal to the amount of jewels in the level.
// If the Exit is visible (meaning all jewels have been collected), and if the player is on the exit,
// advance to the next level.
void Exit::doSomething()
{
	if (getWorld()->player()->getJewelsCollected() == getWorld()->getJewels())
	{
		setVisible(true);
		changeVisibility(true);
		getWorld()->playSound(SOUND_REVEAL_EXIT);
		getWorld()->player()->incNumJewelsCollected();
	}
	if (getVisibility() && getWorld()->hasA(IID_PLAYER, getX(), getY()))
	{
		getWorld()->increaseScore(2000);
		if (getWorld()->getBonus() > 0)
			getWorld()->increaseScore(getWorld()->getBonus());
		setDead();
		getWorld()->playSound(SOUND_FINISHED_LEVEL);
	//	getWorld()->advanceToNextLevel();			//must comment out
		getWorld()->changeCompletion(true);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MOVING OBJECT IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Returns the x-coordinate of the spot in front of the actor in their current direction.
int movingObj::getXInFrontOf(Direction dir, int howMany = 1)
{
	if(dir == left)
		return getX() - howMany;
	else if (dir == right)
		return getX() + howMany;
	else
		return getX();
}

// Returns the x-coordinate of the spot behind the actor in their current direction.
int movingObj::getXBehind(Direction dir, int howMany = 1)
{
	if (dir == left)
		return getX() + howMany;
	else if (dir == right)
		return getX() - howMany;
	else
		return getX();
}

// Returns the y-coordinate of the spot in front of the actor in their current direction.
int movingObj::getYInFrontOf(Direction dir, int howMany = 1)
{
	if (dir == up)
		return getY() + howMany;
	else if (dir == down)
		return getY() - howMany;
	else
		return getY();
}

// Returns the y-coordinate of the spot behind the actor in their current direction.
int movingObj::getYBehind(Direction dir, int howMany = 1)
{
	if (dir == up)
		return getY() - howMany;
	else if (dir == down)
		return getY() + howMany;
	else
		return getY();
}

void movingObj::decAmmo()
{
	m_ammo--;
}

int movingObj::getAmmo()
{
	return m_ammo;
}

void movingObj::restoreHealth()
{
	m_curhitpts = 20;
}

void movingObj::incAmmo()
{
	m_ammo += 20;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLAYER IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Check if the player is dead yet, and if not, set it dead if the HP is 0.
// If a bullet hit the player, decrease HP by 2.
// If the user presses the arrow keys, it will move the player the cooresponding direction.
// When moving a spot in its current direction, the player will not move if it is blocked by an obstacle.
// If the player wants the player to change direction, the player will change direction on screen.
// If the user wants to quit the level, they can press escape.
// If the user presses space the player will fire a bullet in its current direction.
void Player::doSomething()
{
	if (isDead())
		return;
	if (getHP() <= 0)
	{
		setDead();
		return;
	}
	if (getWorld()->hasA(IID_BULLET, getX(), getY()))
	{
		decHitPts();
	}
	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:							// left
			setDirection(left);
			if (getWorld()->hasObjectBlocking(left,this))
				break;
			moveOnce(left);
			break;
		case KEY_PRESS_RIGHT:							// right
			setDirection(right);
			if (getWorld()->hasObjectBlocking(right, this))
				break;
			moveOnce(right);
			break;
		case KEY_PRESS_UP:								// up
			setDirection(up);
			if (getWorld()->hasObjectBlocking(up, this))
				break;
			moveOnce(up);
			break;
		case KEY_PRESS_DOWN:							// down
			setDirection(down);
			if (getWorld()->hasObjectBlocking(down, this))
				break;
			moveOnce(down);
			break;
		case KEY_PRESS_ESCAPE:							// esc
			setDead();
			break;
		case KEY_PRESS_SPACE:							// space
			if (getAmmo() > 0)
			{
				getWorld()->addBullet(getDirection(), this);
				decAmmo();
				getWorld()->playSound(SOUND_PLAYER_FIRE);
			}
			break;
		}
	}
}

// If a boulder is in front of the player, if there is a hole in front of the boulder, 
// the player will push the boulder to fill the hole. If there is any other object in 
// front of the boulder, the player cannot push the boulder.
void Player::pushBoulder(Direction dir)
{
	if (getWorld()->hasA(IID_BOULDER,getXInFrontOf(dir), getYInFrontOf(dir)))	//if player want to move in direction where there is a boulder
	{
		if (getWorld()->hasA(IID_HOLE, getXInFrontOf(dir, 2), getYInFrontOf(dir, 2)))
		{
			getWorld()->getActorAtLocation(getXInFrontOf(dir), getYInFrontOf(dir))->moveOnce(dir);
			return;
		}
		if (getWorld()->getActorAtLocation(getXInFrontOf(dir, 2), getYInFrontOf(dir, 2)))
			return;
		getWorld()->getActorAtLocation(getXInFrontOf(dir), getYInFrontOf(dir))->moveOnce(dir);	//move normally
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BULLET IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

Bullet::Bullet(int startCol, int startRow, Direction d, StudentWorld* thisWorld)
	:Actor(IID_BULLET,startCol,startRow, d, thisWorld)
{
	setVisible(true);
	setDirection(d);
}

// Check if the bullet is on the same location as a:
//    1. Player
//    2. Robot
//    3. Wall
//    4. Boulder
//    5. Robot Factory
// If it is, it will self destruct.
void Bullet::ifHitSomething()
{
	if (getWorld()->hasA(IID_PLAYER,getX(),getY())) 
	{
		getWorld()->playSound(SOUND_PLAYER_IMPACT);
		setDead();
	}
	else if (getWorld()->hasA(IID_SNARLBOT, getX(), getY()) 
		|| getWorld()->hasA(IID_KLEPTOBOT, getX(), getY()) 
		|| getWorld()->hasA(IID_ANGRY_KLEPTOBOT, getX(), getY()))
	{
		getWorld()->playSound(SOUND_ROBOT_IMPACT);
		setDead();
	}
	else if (getWorld()->hasA(IID_WALL, getX(), getY()) 
		|| getWorld()->hasA(IID_BOULDER, getX(), getY()) 
		|| getWorld()->hasA(IID_ROBOT_FACTORY, getX(), getY()))
		setDead();
}

// Check if the bullet has hit something. Check if the bullet self-destructed.
// If not, move once in its current direction and check again if hit something.
void Bullet::doSomething()
{
	ifHitSomething();
	if (isDead())
		return;
	moveOnce(getDirection());
	ifHitSomething();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BOULDER IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Check if boulder has been destroyed.
// Check if a player is trying to push it, and if the player is, move it correspondingly.
// If is pushed into a hole, self-destruct and if a bullet hits it, decrease HP.
void Boulder::doSomething()
{
	if (getHP() <= 0) 
		setDead();
	push();
	if (getWorld()->hasA(IID_HOLE, getX(), getY()))
	{
		setDead();
		getWorld()->getActorAtLocation(getX(), getY())->setDead();
	}
	if (getWorld()->hasA(IID_BULLET, getX(), getY()))
		decHitPts();
}

// If there is a player next to the boulder wanting to move in its direction, only allow it to
// move if there is a hole or an empty space in the location the player wants to move the boulder.
void Boulder::push()
{
	Direction dir = getWorld()->player()->getDirection();
	if (getWorld()->hasA(IID_PLAYER, getXBehind(dir,0), getYBehind(dir,0)))	//if player want to move in direction where there is a boulder
	{
		if (getWorld()->hasA(IID_HOLE, getXInFrontOf(dir,1), getYInFrontOf(dir,1)))
		{
			moveOnce(dir);
			return;
		}
		if (getWorld()->getActorAtLocation(getXInFrontOf(dir,1), getYInFrontOf(dir,1)))
			return;
		moveOnce(dir);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// HOLE IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Check if hole has been filled up and set dead, and if not check if a boulder has just filled it up.
void Hole::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->hasA(IID_BOULDER, getX(), getY()))
	{
		setDead();
		getWorld()->getActorAtLocation(getX(), getY())->setDead();
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// JEWEL IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Check if a player is on the same location as a jewel, and if so, increase number of jewels player has collected
// and increase score by 50. Then remove the jewel from the screen.
void Jewel::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->hasA(IID_PLAYER, getX(), getY()))
	{
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(50);
		getWorld()->player()->incNumJewelsCollected();
		setDead();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GOODIE IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Base class Goodies checks if a player has landed on it. If the player has, it will respond with an
// action specific to that goodie. Then it will disappear from the screen.
void Goodies::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->hasA(IID_PLAYER, getX(), getY()))
	{
		action();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		setDead();
	}
}

// If player lands on an extra life goodie, score is increased by 1000 and the player gains 1 life.
void ExtraLifeGoodie::action()
{
	getWorld()->increaseScore(1000);
	getWorld()->incLives();
}

// If player lands on a restore health goodie, score is increased by 500 and player's health is retored to 100%.
void RestoreHealthGoodie::action()
{
	getWorld()->increaseScore(500);
	getWorld()->player()->restoreHealth();
}

// If player lands on ammo goodie, score is increased by 100 and the player will gain 20 ammo.
void ammoGoodie::action()
{
	getWorld()->increaseScore(100);
	getWorld()->player()->incAmmo();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ROBOT IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Set-up new tick frequency for robots.
int Robot::tickOutOf()
{
	int level = getWorld()->getLevel();
	int ticks = (28 - level)/4;
	if (ticks < 3)    
		ticks = 3; 
	return ticks;
}

// Check if the tick that robot is on is the tick that it can move.
bool Robot::checkTick()
{
	if (tickNum % tickOutOf() == 0)
		return true;
	return false;
}

// Increment ticks so that the tick number can restart when reaching maximum
void Robot::incTick()
{
	if (tickNum == tickOutOf())
		tickNum = 1;
	else
		tickNum++;
}

// If hit by bullet, decrease HP by 2. If HP has reached 0, the robot is dead. 
// Score will be increased by the respective amount each robot gives.
void Robot::damage()
{
	decHitPts();
	if (getHP() == 0)
	{
		setDead();
		getWorld()->increaseScore(raiseScore);
		getWorld()->playSound(SOUND_ROBOT_DIE);
	}
}

// Check if robot is facing the player in its current direction.
bool Robot::facingPlayer()
{
	movingObj* p = getWorld()->player();
	switch (getDirection())
	{
		case left:
			if (getX() > p->getX() && getY() == p->getY())
				return true;
			break;
		case right:
			if (getX() < p->getX() && getY() == p->getY())
				return true;
			break;
		case up:
			if (getX() == p->getX() && getY() < p->getY())
				return true;
			break;
		case down:
			if (getX() == p->getX() && getY() > p->getY())
				return true;
			break;
	}
	return false;
}

// Check if object is blocking robot from moving such as:
//    1. Boulder
//    2. Robot Factory
//    3. Wall
//    4. Robots
//    5. Player
//    6. Hole
bool Robot::blockingRobot()
{
	Direction d = getDirection();
	int x = getXInFrontOf(d);
	int y = getYInFrontOf(d);
	if (getWorld()->hasA(IID_BOULDER, x, y))	
		return true;
	else if (getWorld()->hasA(IID_ROBOT_FACTORY, x, y))	
		return true;
	else if (getWorld()->hasA(IID_WALL, x, y))	
		return true;
	else if (getWorld()->hasA(IID_SNARLBOT, x, y) || getWorld()->hasA(IID_KLEPTOBOT, x, y) || getWorld()->hasA(IID_ANGRY_KLEPTOBOT, x, y))	
		return true;
	else if (getWorld()->hasA(IID_PLAYER, x, y))
		return true;
	else if (getWorld()->hasA(IID_HOLE, x, y))
		return true;
	return false;
}

// If a player, robot, wall, boulder, or robot factory is in between the robot and player, return true.
bool Robot::willHitAnything(int x, int y)
{
	if (getWorld()->hasA(IID_PLAYER, x, y))
		return true;
	else if (getWorld()->hasA(IID_SNARLBOT, x, y) || getWorld()->hasA(IID_KLEPTOBOT, x, y))
		return true;
	else if (getWorld()->hasA(IID_WALL, x, y) || getWorld()->hasA(IID_BOULDER, x, y) || getWorld()->hasA(IID_ROBOT_FACTORY, x, y))
		return true;
	return false;
}

// Check if any object is in between player and robot to prevent robot from shooting at it 
// with respect to what direction the robot is facing and its distance to the player.
bool Robot::clearShot()
{
	Direction d = getDirection();
	int r, increment, p;
	switch (d)
	{
	case up:
		r = getY() + 1;
		p = getWorld()->player()->getY();
		increment = 1;
		break;
	case right:
		r = getX() + 1;
		increment = 1;
		p = getWorld()->player()->getX();
		break;
	case down:
		r = getY() - 1;
		increment = -1;
		p = getWorld()->player()->getY();
		break;
	case left:
		r = getX() - 1;
		increment = -1;
		p = getWorld()->player()->getX();
		break;
	}
	switch (d)
	{
	case up:
	case down:
		while (r != p)
		{
			if (willHitAnything(getX(), r))
				return false;
			r += increment;
		}
		break;
	case left:
	case right:
		while (r != p)
		{
			if (willHitAnything(r, getY()))
				return false;
			r += increment;
		}
	}
	return true;
}

// Turn robot the opposite direction it is currently facing.
void Robot::changeDirection()
{
	Direction d = getDirection();
	Direction newd;
	switch (d)
	{
	case GraphObject::up:
		newd = down;
		break;
	case GraphObject::down:
		newd = up;
		break;
	case GraphObject::left:
		newd = right;
		break;
	case GraphObject::right:
		newd = left;
		break;
	}
	setDirection(newd);
}

void Robot::decHitPts()
{
	if (getHP() > 1)
		movingObj::decHitPts();
	else
		setHP(0);
}

// If not dead, will perform actions. If hit by a bullet, will be damaged.
// The robot will only move on its single tick out of the number of ticks it waits 
// per level. If it can move on that tick, will shoot at the player if facing it
// and is a Snarlbot or Angry Kleptobot. If blocked by something, will randonly change direction.
void Robot::doSomething()
{
	if (isDead())
		return;
	if (getWorld()->hasA(IID_BULLET, getX(), getY()))
	{
		damage();
		return;
	}
	if (!checkTick())
	{
		incTick();
		return;
	}
	else
	{
		Direction d = getDirection();
		if (facingPlayer() && clearShot() && (m_typeBot == 's' || m_typeBot == 'a'))
		{
			getWorld()->playSound(SOUND_ENEMY_FIRE);
			getWorld()->addBullet(d, this);
			incTick();
			return;
		}
		if (blockingRobot())
		{
			changeDirection();
			incTick();
			return;
		}
		moveOnce(d);
		incTick();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KLEPTOBOT IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Check if Kleptobot landed on a goodie. If so, there is a 1/10 chance it will pick it up.
// If it does pick up the goodie, remove the goodie from the screen and disallow that Kleptobot
// from picking up any more goodies.
void KleptoBot::doSomething()
{
	// check for goodies on same spot
	if (!hasGoodie && (getWorld()->hasA(IID_EXTRA_LIFE, getX(), getY()) 
		|| getWorld()->hasA(IID_AMMO, getX(), getY()) 
		|| getWorld()->hasA(IID_RESTORE_HEALTH, getX(), getY())) 
		&& getWorld()->getActorAtLocation(getX(), getY())->getVisibility())
	{
		int luck = rand() % 10;
		if (luck == 5)
		{
			getWorld()->getActorAtLocation(getX(), getY())->setVisible(false);
			good = getWorld()->getActorAtLocation(getX(), getY());
			getWorld()->playSound(SOUND_ROBOT_MUNCH);
			hasGoodie = true;
			return;
		}
	}
	//check distancebeforeturning here
	if (curdist != distanceBeforeTurning)		//if not yet at max distanceBeforeTurning
	{
		Robot::doSomething();
	//	possibleDir--;
	}
	else										//if at max distanceBeforeTurning
	{
		distanceBeforeTurning = newDistRange();
		setDirection(randDirection(3));
		possibleDir = 4;
	}
	// check ifDead, then release goodies
	if (isDead())
	{
		if (hasGoodie)
		{
			good->moveTo(getX(), getY());
			good->setVisible(true);
		}
		return;
	}
	increaseCurDist();
}

//  Counter for distance before has to turn
void KleptoBot::increaseCurDist()
{
	if (curdist == distanceBeforeTurning)
		curdist = 1;
	else
		curdist++;
}

// Choose a random direction to turn when facing obstacle
GraphObject::Direction KleptoBot::randDirection(int out)
{
	int i = rand() % out;
	switch (i)
	{
	case 0:
		return left;
		break;
	case 1:
		return right;
		break;
	case 2:
		return up;
		break;
	case 3:
		return down;
		break;
	}
}

// Change direction if something blocking
void KleptoBot::changeDirection()				
{

	setDirection(randDirection(possibleDir));
	decreasePossibleDir();
}

// Decrement the remaining possible directions to turn if cannot be unblocked on first try
void KleptoBot::decreasePossibleDir()
{
	if (possibleDir == 1)
		possibleDir = 4;
	else
		possibleDir--;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ROBOT FACTORY IMPLEMENTATIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Return minimum x-coordinate of 6x6 box Robot Factory checks.
int RobotFactory::minXBound()
{
	if (getX() > 3)
		return getX() - 3;
	else
		return 0;
}

// Return minimum y-coordinate of 6x6 box Robot Factory checks.
int RobotFactory::minYBound()
{
	if (getY() > 3)
		return getY() - 3;
	else
		return 0;
}

// Return maximum x-coordinate of 6x6 box Robot Factory checks.
int RobotFactory::maxXBound()
{
	if (getX() < 11)
		return getX() + 3;
	else
		return 14;
}

// Return maximum y-coordinate of 6x6 box Robot Factory checks.
int RobotFactory::maxYBound()
{
	if (getY() < 11)
		return getY() + 3;
	else
		return 14;
}

// Count number of Kleptobots in the 6x6 region around the factory.
// If less than 3 KleptoBots are in the region, there is a 1/50 chance
// a new one will be added.
void RobotFactory::doSomething()
{
	for (int i = minXBound(); i <= maxXBound(); i++)
	{
		for (int j = minYBound(); j <= maxYBound(); j++)
		{
			if (getWorld()->hasA(IID_KLEPTOBOT, i, j))
				numBotsInRegion++;
		}
	}

	if (numBotsInRegion < 3 && !getWorld()->hasA(IID_KLEPTOBOT, getX(), getY()))
	{
		int r = rand() % 50;
		if (r == 1)
		{
			getWorld()->addBot(getX(), getY(), typeOfBot);
			getWorld()->playSound(SOUND_ROBOT_BORN);
		}
	}
	numBotsInRegion = 0;
}