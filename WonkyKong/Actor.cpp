#include "Actor.h"
#include "StudentWorld.h"


StudentWorld* Actor::getWorld() const {
	return m_world;
}

Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, int dir, double size = 1.0) 
	: GraphObject(imageID, startX, startY, dir, size), m_alive(true), m_world(world), m_freezeTick(0), m_tickCount(0) {}


bool Actor::isAlive() const {
	return m_alive;
}

void Actor::setDead() {
	m_alive = false;
}

int Actor::getFreeze() const {
	return m_freezeTick;
}

void Actor::setFreeze(int num) {
	m_freezeTick = num;
}

int Actor::getMTick() const {
	return m_tickCount;
}

void Actor::setMTick(int num) {
	m_tickCount = num;
}



bool Actor::isFloor() const{
	return false;
}

bool Actor::canFall() const {
	return true;
}

bool Actor::onIt() const {
	Player* player = getWorld()->getPlayer();

	if (player->getX() == getX() && player->getY() == getY()) {
		return true;
	}
	else {
		return false;
	}
}

void Actor::attack(Actor* actor) {
	actor->setDead();
}



StationaryObject::StationaryObject(StudentWorld* world, int imageID, int startX, int startY)
	: Actor(world, imageID, startX, startY, none) {}

void StationaryObject::doSomething() {}

bool StationaryObject::canFall() const {
	return false;
}

Floor::Floor(StudentWorld* world, int startX, int startY) 
	: StationaryObject(world, IID_FLOOR, startX, startY) {}

bool Floor::isFloor() const {
	return true;
}

Ladder::Ladder(StudentWorld* world, int startX, int startY)
	: StationaryObject(world, IID_LADDER, startX, startY) {}





Player::Player(StudentWorld* world, int startX, int startY) 
	: Actor(world, IID_PLAYER, startX, startY, right), m_jumping(false), m_jumpTick(0), m_burps(0) {}

void Player::jump(int direction) {
	if (m_jumpTick == 0){

		if (!getWorld()->isFloor(getX(), getY() + 1) && getWorld()->isWithinBounds(getX(), getY() + 1)) {
			moveTo(getX(), getY() + 1);
			m_jumpTick++;
		}
		else {
			m_jumping = false;
		}
	}
	else if (m_jumpTick >= 1 && m_jumpTick <= 3) {
		if (!getWorld()->isFloor(getX() + direction, getY()) && getWorld()->isWithinBounds(getX() + direction, getY())) {
			moveTo(getX() + direction, getY());
			m_jumpTick++;
		}
		else {
			m_jumping = false;
		}
	}
	else if (m_jumpTick == 4) {
		if (!getWorld()->isFloor(getX(), getY() - 1) && getWorld()->isWithinBounds(getX(), getY() - 1)) {
			moveTo(getX(), getY() -1);
			m_jumpTick++;
		}
		m_jumping = false;
	}

	if (!getWorld()->canFall(getX(), getY()) && !getWorld()->isFloor(getX(), getY())) {
		m_jumping = false;
	}

}

int Player::getnumBurps() const{
	return m_burps;
}

void Player::increaseBurps() {
	m_burps += 5;
}

void Player::doSomething() {
	if (!isAlive()) {
		return;
	}

	if (m_jumping) {
		if (getDirection() == right) {
			jump(1);
		}
		else {
			jump(-1);
		}
		return;
	}

	if (getFreeze() != 0) {
		setFreeze(getFreeze() - 1);
		return;
	}

	if (getWorld()->canFall(getX(), getY() - 1) && getWorld()->isWithinBounds(getX(), getY() - 1)) {
		if (!(!getWorld()->canFall(getX(), getY()) && !getWorld()->isFloor(getX(), getY()))) {
			moveTo(getX(), getY() - 1);
			return;
		}
	}


	int ch;
	if (getWorld()->getKey(ch)) {
		switch (ch) {
			case KEY_PRESS_LEFT:
				if (getDirection() == right) {
					setDirection(left);
				}
				else if (!getWorld()->isFloor(getX() - 1, getY()) && getWorld()->isWithinBounds(getX() -1, getY())){
					moveTo(getX() - 1, getY());
					setDirection(left);
				}
				break;
			case KEY_PRESS_RIGHT:
				if (getDirection() == left) {
					setDirection(right);
				}
				else if (!getWorld()->isFloor(getX() + 1, getY()) && getWorld()->isWithinBounds(getX() + 1, getY())) {
					moveTo(getX() + 1, getY());
					setDirection(right);
				}
				break;
			case KEY_PRESS_UP:
				if (!getWorld()->canFall(getX(), getY()) && !getWorld()->isFloor(getX(), getY() + 1) && getWorld()->isWithinBounds(getX(), getY() + 1)) {
					moveTo(getX(), getY() + 1);
				}
				break;
			case KEY_PRESS_DOWN:
				if (!getWorld()->isFloor(getX(), getY() - 1) && getWorld()->isWithinBounds(getX(), getY() - 1)) {
					moveTo(getX(), getY() - 1);
				}
				break;
			case KEY_PRESS_SPACE:
				if (!m_jumping) {
					m_jumping = true;
					m_jumpTick = 0;
					if (getDirection() == right) {
						jump(1);
					}
					else {
						jump(-1);
					}
					getWorld()->playSound(SOUND_JUMP);
				}
				break;
			case KEY_PRESS_TAB:
				if (m_burps > 0) {
					getWorld()->playSound(SOUND_BURP);
					if (getDirection() == right) {
						getWorld()->addActor(new Burp(getWorld(), getX() + 1, getY(), right));
					}
					else if (getDirection() == left) {
						getWorld()->addActor(new Burp(getWorld(), getX() - 1, getY(), left));
					}
					m_burps--;
				}
				break;
		}


	}
}

Burp::Burp(StudentWorld* world, int startX, int startY, int dir)
	: Actor(world, IID_BURP, startX, startY, dir), m_burpTick(6) {}

void Burp::doSomething() {
	if (!isAlive()) {
		return;
	}
	m_burpTick--;

	if (m_burpTick == 0) {
		setDead();
		return;
	}

	//what if there are two things on it
	if (getWorld()->destructableType(getX(), getY()) != nullptr) {
		Actor* toDestroy = getWorld()->destructableType(getX(), getY());
		attack(toDestroy);
		if (toDestroy->dropGarlic()) {
			getWorld()->addActor(new Garlic(getWorld(), getX(), getY()));
		}
		else if (toDestroy->dropLife()) {
			getWorld()->addActor(new ExtraLife(getWorld(), getX(), getY()));
		}
		getWorld()->playSound(SOUND_ENEMY_DIE);
		getWorld()->increaseScore(100);
	}

}

BonFire::BonFire(StudentWorld* world, int startX, int startY) 
	: Actor(world, IID_BONFIRE, startX, startY, none) {}

void BonFire::doSomething() {
	increaseAnimationNumber();
	

	if (onIt()) {
		attack(getWorld()->getPlayer());
	}
	if (getWorld()->destructableType(getX(), getY()) != nullptr && getWorld()->destructableType(getX(), getY())->bondFire()) {
		attack(getWorld()->destructableType(getX(), getY()));
	}
}



Consumables::Consumables(StudentWorld* world, int imageID, int startX, int startY, int m_points) 
	: Actor(world, imageID, startX, startY, none), m_points(m_points) {}

void Consumables::doSomething() {
	if (!isAlive()) {
		return;
	}
	Player* player = getWorld()->getPlayer();
	if (onIt()) {
		getWorld()->increaseScore(m_points);
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);

		specialEffect();
		return;
	}


}

ExtraLife::ExtraLife(StudentWorld* world, int startX, int startY)
	: Consumables(world, IID_EXTRA_LIFE_GOODIE, startX, startY, 50) {}

void ExtraLife::specialEffect() {
	getWorld()->incLives();
}

Garlic::Garlic(StudentWorld* world, int startX, int startY)
	: Consumables(world, IID_GARLIC_GOODIE, startX, startY, 25) {}

void Garlic::specialEffect() {
	getWorld()->getPlayer()->increaseBurps();
	
}

Destructables::Destructables(StudentWorld* world, int imageID, int startX, int startY, int dir)
	: Actor(world, imageID, startX, startY, dir) {}

void Destructables::doSomething() {
	setMTick(getMTick() + 1);
	if (!isAlive()) {
		return;
	}
	if (onIt() && getFreeze() == 0) {
		attack(getWorld()->getPlayer());
		setMTick(getMTick() - 1);
		return;
	}
	if (getFreeze() != 0) {
		setFreeze(getFreeze() - 1);
	}
	if (bondFire()) {
		if (!getWorld()->isFloor(getX(), getY() - 1)) {
			moveTo(getX(), getY() - 1);
			if (getWorld()->isFloor(getX(), getY() - 1)) {
				if (getDirection() == right) {
					setDirection(left);
				}
				else{
					setDirection(right);
				}
			}
		}
	}
	if (getMTick() % 10 == 9) {
		if (bondFire()) {
			if (getDirection() == right) {
				if (getWorld()->isFloor(getX() + 1, getY())) {
					setDirection(left);
				}
				else {
					moveTo(getX() + 1, getY());
				}
			}
			else {
				if (getWorld()->isFloor(getX() - 1, getY())) {
					setDirection(right);
				}
				else {
					moveTo(getX() - 1, getY());
				}
			}
		}
		else if (!doUnique()) {
			if (getDirection() == right) {
				if (getWorld()->isFloor(getX() + 1, getY()) || (!getWorld()->isFloor(getX() + 1, getY() - 1) && getWorld()->canFall(getX() + 1, getY() - 1))) {
					setDirection(left);
				}
				else {
					moveTo(getX() + 1, getY());
				}
			}
			else {
				if (getWorld()->isFloor(getX() - 1, getY()) || (!getWorld()->isFloor(getX() - 1, getY() - 1) && getWorld()->canFall(getX() - 1, getY() - 1))) {
					setDirection(right);
				}
				else {
					moveTo(getX() - 1, getY());
				}
			}
		}
		if (onIt() && getFreeze() == 0) {
			attack(getWorld()->getPlayer());
		}
	}
}


bool Destructables::doUnique() {
	return false;
}


FireBall::FireBall(StudentWorld* world, int startX, int startY)
	: Destructables(world, IID_FIREBALL, startX, startY, randInt(0,1) == 0 ? left : right), m_climbing(0) {}

bool FireBall::doUnique() {
	if (m_climbing != -1 && !getWorld()->canFall(getX(), getY()) && !getWorld()->isFloor(getX(), getY() + 1) && getWorld()->isWithinBounds(getX(), getY() + 1)) {
		if (m_climbing == 1 || randInt(1, 3) == 1) {
			m_climbing = 1;
			moveTo(getX(), getY() + 1);
			return true;
		}
	}
	if (m_climbing != 1 && !getWorld()->canFall(getX(), getY() - 1) && !getWorld()->isFloor(getX(), getY() - 1) && getWorld()->isWithinBounds(getX(), getY() - 1)) {
		if (m_climbing == -1 || randInt(1, 3) == 1) {
			m_climbing = -1;
			moveTo(getX(), getY() - 1);
			return true;
			
		}
	}
	if (m_climbing == 1 && (getWorld()->isFloor(getX(), getY() + 1) || (getWorld()->canFall(getX(),getY() + 1) && !getWorld()->isFloor(getX(), getY() + 1)))) {

		m_climbing = 0;
	}
	else if (m_climbing == -1 && (getWorld()->isFloor(getX(), getY() - 1) || (getWorld()->canFall(getX(), getY() - 1) && !getWorld()->isFloor(getX(), getY() - 1)))) {
		m_climbing = 0;
	}
	return false;




}

Koopa::Koopa(StudentWorld* world, int startX, int startY)
	: Destructables(world, IID_KOOPA, startX, startY, randInt(0, 1) == 0 ? left : right) {}

void Koopa::attack(Actor* actor) {
	actor->setFreeze(50);
	this->setFreeze(49);
	return;
}

Barrel::Barrel(StudentWorld* world, int startX, int startY, int dir) 
	: Destructables(world, IID_BARREL, startX, startY, dir) {}



Kong::Kong(StudentWorld* world, int startX, int startY, int dir)
	: Actor(world, IID_KONG, startX, startY, dir), isFleeing(false) {}

void Kong::doSomething() {
	if (!isAlive()) {
		return;
	}
	increaseAnimationNumber();
	double dx = getX() - getWorld()->getPlayer()->getX();
	double dy = getY() - getWorld()->getPlayer()->getY();
	if (sqrt(dx * dx + dy * dy) <= 2.0) {
		isFleeing = true;
	}
	if (isFleeing == false) {
		int N = std::max(200 - 50 * getWorld()->getLevel(), 50);
		if (getFreeze() >= N) {
			if (getDirection() == left) {
				getWorld()->addActor(new Barrel(getWorld(), getX() - 1, getY(), getDirection()));
			}
			else {
				getWorld()->addActor(new Barrel(getWorld(), getX() + 1, getY(), getDirection()));
			}

			setFreeze(0);
		}
	}

	if (getMTick() % 5 == 4) {
		if (isFleeing) {
			if (getY() + 1 > VIEW_HEIGHT) {
				getWorld()->increaseScore(1000);
				getWorld()->playSound(SOUND_FINISHED_LEVEL);
				getWorld()->setFinishLevel(true);
			}
			else {
				moveTo(getX(), getY() + 1);
			}
		}

	}
	setFreeze(getFreeze() + 1);
	setMTick(getMTick() + 1);



}