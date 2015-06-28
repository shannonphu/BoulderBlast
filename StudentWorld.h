#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <list>
//#include <vector>
#include <string>
#include "Actor.h"
class GraphObject;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);						// constructor
	virtual int init();										// init
	bool isLevelWellFormed(std::string levelDataFile);
	int allocation(std::string levelData);					// allocation of memory
	virtual int move();										// move	
	// deals with checking is object is at location
	bool hasA(int m_ID, int x, int y);
	bool hasObjectBlocking(GraphObject::Direction dir, movingObj* j);
	Actor* getActorAtLocation(int x, int y) const;
	movingObj* player();
	// adds bullet or robot 
	void addBullet(GraphObject::Direction dir, movingObj* l);
	void addBot(int x, int y, char type);
	// deals with display
	int getBonus();
	void setDisplayText();
	std::string formatDisplay(int score, int level, int lives, int health, int ammo, unsigned int bonus);
	// deals with level completion
	int getJewels();
	// deals with ending level
	void changeCompletion(bool predicate);
	virtual void cleanUp();									// clean-up
	virtual ~StudentWorld();								// destructor
private:
	int numAllocated;
	//std::list<Actor*>& actor;								// list container of pointer to actors
	movingObj* p;
	list<Actor*>* actor;
	int m_bonus;
	int jewelsInThisLevel;
	int completedLevel;
	int totalLevels;
};

#endif // STUDENTWORLD_H_
