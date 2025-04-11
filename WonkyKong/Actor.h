#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:

	Actor(StudentWorld* world, int imageID, int startX, int startY, int dir, double size);

	virtual void doSomething() = 0;
	bool isAlive() const;
	void setDead();
	StudentWorld* getWorld() const;

	int getFreeze() const;
	void setFreeze(int num);

	int getMTick() const;
	void setMTick(int num);

	virtual bool isFloor() const;
	virtual bool canFall() const;

	virtual bool destructable() const { return false; }
	virtual bool dropGarlic() const { return false; }
	virtual bool dropLife() const { return false; }
	virtual bool bondFire() const { return false; }

	bool onIt() const;
	virtual void attack(Actor* actor);



private:
	bool m_alive;
	int m_freezeTick;
	int m_tickCount;
	StudentWorld* m_world;
};

class Player : public Actor
{
public:
	Player(StudentWorld* world, int startX, int startY);

	virtual void doSomething();
	int getnumBurps() const;

	void increaseBurps();


private:
	void jump(int direction);

	bool m_jumping;
	int m_jumpTick;
	int m_burps;
};

class StationaryObject : public Actor
{
public:
	StationaryObject(StudentWorld* world, int imageID, int startX, int startY);

	virtual void doSomething();
	virtual bool canFall() const;
};

class Floor : public StationaryObject
{
public:
	Floor(StudentWorld* world, int startX, int startY);
	virtual bool isFloor() const;
};

class Ladder : public StationaryObject
{
public:
	Ladder(StudentWorld* world, int startX, int startY);
};


class Burp : public Actor
{
public:
	Burp(StudentWorld* world, int startX, int startY, int dir);
	virtual void doSomething();
private:
	int m_burpTick;
};

class BonFire : public Actor
{
public: 
	BonFire(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
};



class Consumables : public Actor
{
public: 
	Consumables(StudentWorld* world, int imageID, int startX, int startY, int m_points);
	virtual void doSomething();

private:
	virtual void specialEffect() = 0;
	int m_points;
};

class ExtraLife : public Consumables
{
public: 
	ExtraLife(StudentWorld* world, int startX, int startY);
	virtual void specialEffect();
};

class Garlic : public Consumables
{
public:
	Garlic(StudentWorld* world, int startX, int startY);
	virtual void specialEffect();
};



class Destructables : public Actor
{
public: 
	Destructables(StudentWorld* world, int imageID, int startX, int startY, int dir);
	virtual void doSomething();
	virtual bool doUnique();


	virtual bool destructable() const { return true; }

};


class FireBall : public Destructables 
{
public: 
	FireBall(StudentWorld* world, int startX, int startY);
	virtual bool doUnique();

	virtual bool dropGarlic() const { return randInt(1, 3) == 1; }

private:
	int m_climbing; 
};

class Koopa : public Destructables
{
public:
	Koopa(StudentWorld* world, int startX, int startY);
	virtual void attack(Actor* actor);
	virtual bool dropLife() const { return randInt(1, 3) == 1; }
private:

};


class Barrel : public Destructables
{
public:
	Barrel(StudentWorld* world, int startX, int startY, int dir);
	virtual bool bondFire() const { return true; }
};



class Kong : public Actor
{
public:
	Kong(StudentWorld* world, int startX, int startY, int dir);
	virtual void doSomething();
private:
	bool isFleeing;
};
#endif // ACTOR_H_
