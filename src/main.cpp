#include "Charlie2D.h"
#include "ExtendedComponent.h"
#include "GameManager.h"
#include "SDL_render.h"
#include "Vector2f.h"

bool playerCreated = false;

void loadGameScene();
void loadMenuScene();

class PinCameraTo : public ExtendedComponent {
public:
  void update() override {
    Camera::setPosition(entity->box->getBox().getCenter());
  }
};

class PlayerAnimator : public ExtendedComponent {
public:
  void update() override {
    int direction = InputManager::checkHorizontal();
    bool facingRight = direction > 0;
    bool facingLeft = direction < 0;

    if (facingRight) {
      get<Sprite>()->flip = SDL_FLIP_NONE;
    } else if (facingLeft) {
      get<Sprite>()->flip = SDL_FLIP_HORIZONTAL;
    }
  }
};

class PlayerLevelChange : public ExtendedComponent {
public:
  void update() override {
    if (LDTK::checkOutsideBounds(entity)) {
      level = LDTK::findTraveledLevel(entity);
      lastEnterPosition = entity->box->getPosition();
      LDTK::loadLevel(level);
    }
  }

  void resetPlayer() { entity->box->setPosition(lastEnterPosition); }

  std::string level;
  Vector2f lastEnterPosition = {0, 0};
};

class Kill : public ExtendedComponent {
public:
  void start() override {}

  void update() override {
    Entity *player = GameManager::getEntities("Player")[0];

    if (entity->box->getBox().checkCollision(player->box->getBox())) {
      player->get<PlayerLevelChange>()->resetPlayer();
    }
  }
};

Entity *createPlayer(Entity *spawn) {
  Entity *player = GameManager::createEntity("Player");
  player->box->setPosition(spawn->box->getPosition());
  player->box->setSize(spawn->box->getSize());

  Sprite *sprite = player->add<Sprite>();
  sprite->loadTexture("img/Player.png", false);

  player->useLayer = true;
  player->layer = 20;

  JumpMan *jumpMan = player->add<JumpMan>();
  jumpMan->gravity = (float)1500 / 2;
  jumpMan->speed = (float)40 / 2;
  jumpMan->airSpeed = (float)40 / 2;
  jumpMan->maxSpeed = (float)200 / 2;
  jumpMan->tracktion = (float)1000 / 2;
  jumpMan->jumpPeak = (float)80 / 2;
  jumpMan->jumpChange = (float)300 / 2;

  player->add<PinCameraTo>();
  player->add<PlayerAnimator>();
  player->add<PlayerLevelChange>();

  playerCreated = true;
  return player;
}

void loadGameScene() {
  GameManager::destroyAll();
  Camera::resetCamera();
  Camera::scale = 4;

  LDTK::loadJson("img/ldtk/ldtk.ldtk");

  LDTK::onLoadLevel = []() {
    for (Entity *entity : GameManager::getAllObjects()) {
      if (entity->tag == "Ground") {
        entity->add<Collider>()->solid = true;
      } else if (entity->tag == "Kill") {
        entity->add<Kill>();
      } else if (entity->tag == "PlayerSpawn") {
        if (!playerCreated)
          createPlayer(entity);
      }
    }
  };

  LDTK::loadLevel("78b44ad0-b0a0-11ee-b472-39fdb7fc8f8c");
}

int main(int, char **) {
  GameManager::init();

  loadGameScene();

  GameManager::doUpdateLoop();

  return 0;
}
