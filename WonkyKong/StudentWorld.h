#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>

#include "Actor.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  virtual ~StudentWorld();


  virtual int init();
  virtual int move();
  virtual void cleanUp();

  // Might not need
  bool isWithinBounds(int x, int y) const;
  bool isFloor(int x, int y) const;
  bool canFall(int x, int y) const;
  Actor* destructableType(int x, int y) const;

  void addActor(Actor* a);
  Player* getPlayer() const;

  void setFinishLevel(bool finish);

private:

	void updateDisplayText();
	void removeDeadGameObjects();
	bool finishLevel = false;

	Player* m_player;
	std::vector<Actor*> m_actors;
};

#endif // STUDENTWORLD_H_
