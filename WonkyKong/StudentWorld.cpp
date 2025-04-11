#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;


GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

bool StudentWorld::isWithinBounds(int x, int y) const {
    return x >= 0 && x < VIEW_WIDTH && y >= 0 && y < VIEW_HEIGHT;
}

bool StudentWorld::isFloor(int x, int y) const {
    for (std::vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->isFloor()) {
            return true;
        }
        
    }
    return false;
}

bool StudentWorld::canFall(int x, int y) const {
    for (std::vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getX() == x && (*it)->getY() == y && !(*it)->canFall()) {
            return false;
        }
        
    }
    return true;
}

Actor* StudentWorld::destructableType(int x, int y) const {
    for (std::vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->destructable()) {
            return *it;
        }

    }
    return nullptr;
}


void StudentWorld::addActor(Actor* a) {
    m_actors.push_back(a);
}

Player* StudentWorld::getPlayer() const {
    return m_player;
}

void StudentWorld::updateDisplayText() {
    ostringstream oss;
    oss.setf(ios::fixed);
    oss << "Score: " << setw(7) << setfill('0') << getScore()
        << "  Level: " << setw(2) << setfill('0') << getLevel()
        << "  Lives: " << setw(2) << setfill('0') << getLives()
        << "  Burps: " << setw(2) << setfill('0') << m_player->getnumBurps();
    setGameStatText(oss.str());
}

void StudentWorld::removeDeadGameObjects() {
    for (std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
        if (!(*it)->isAlive()) {
            delete* it;
            it = m_actors.erase(it);
        }
        else {
            it++;
        }
    }
}

void StudentWorld::setFinishLevel(bool finish) {
    finishLevel = finish;
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_player(nullptr)
{
}

int StudentWorld::init()
{
    Level lev(assetPath());

    ostringstream levelFile;
    levelFile << "level" << setw(2) << setfill('0') << getLevel() << ".txt";

    Level::LoadResult result = lev.loadLevel(levelFile.str());

    if (getLevel() > 99) {
        return GWSTATUS_PLAYER_WON;
    }

    if (result == Level::load_fail_file_not_found) {
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format) {
        cerr << "Error loading level file." << endl;
        return GWSTATUS_LEVEL_ERROR;
    }

    for (int x = 0; x < VIEW_WIDTH; x++) {
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            Level::MazeEntry item = lev.getContentsOf(x, y);

            switch (item) {
            case Level::player:
                m_player = new Player(this, x, y);
                break;
            case Level::floor:
                m_actors.push_back(new Floor(this, x, y));
                break;
            case Level:: ladder:
                m_actors.push_back(new Ladder(this, x, y));
                break;
            case Level::left_kong:
                m_actors.push_back(new Kong(this, x, y, GraphObject::left));
                break;
            case Level::right_kong:
                m_actors.push_back(new Kong(this, x, y, GraphObject::right));
                break;
            case Level::fireball:
                m_actors.push_back(new FireBall(this, x, y));
                break;
            case Level::koopa:
                m_actors.push_back(new Koopa(this, x, y));
                break;
            case Level::bonfire:
                m_actors.push_back(new BonFire(this, x, y));
                break;
            case Level::extra_life:
                m_actors.push_back(new ExtraLife(this, x, y));
                break;
            case Level::garlic:
                m_actors.push_back(new Garlic(this, x, y));
                break;
            default:
                break;
            }
        }
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

    updateDisplayText();

    if (m_player->isAlive()) {
        m_player->doSomething();
    }

    for (std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        if ((*it)->isAlive()) {
            (*it)->doSomething();
            if (!m_player->isAlive()) {
                decLives();
                playSound(SOUND_PLAYER_DIE);
                return GWSTATUS_PLAYER_DIED;

            }
        }

    }

    removeDeadGameObjects();
    


    if (finishLevel) {
        finishLevel = false;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
    delete m_player;
    m_player = nullptr;

    for (std::vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        delete* it;
    }
    m_actors.clear();
}
