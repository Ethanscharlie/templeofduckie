#pragma once
#include "AbilityComponents.h"
#include "ExtendedComponent.h"
#include "GameManager.h"
#include "physicsBody.h"
#include <Charlie2D.h>
#include <cstdlib>

bool playerCreated = false;

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

    if (direction != 0 && abs(get<physicsBody>()->velocity.y) <= 0.1) {
      Animation *runAnimation = get<Sprite>()->animations["run"];
      if (!runAnimation->playing) {
        runAnimation->play();
      }
    } else {
      get<Sprite>()->loadTexture("img/Player.png", false);
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

class PlayerSoundManager : public ExtendedComponent {
public:
  void checkLava() {
    for (Entity *lava : GameManager::getEntities("Lava")) {
      if (entity->box->getBox().checkCollision(lava->box->getBox())) {
        if (!touchingLava) {
          touchingLava = true;
          leftOrEnteredLava();
        }
        goto TOUCHING;
      }
    }

    if (touchingLava) {
      leftOrEnteredLava();
      touchingLava = false;
    }

  TOUCHING:;
  }

  void update() override {
    checkLava();
    if (InputManager::checkInput("jumpTrigger")) {
      GameManager::playSound("img/sound/30_Jump_03.wav");
    }

    if (InputManager::checkAxis().x != 0 &&
        abs(get<physicsBody>()->velocity.y) <= 0.1) {
      if (add<Scheduler>()->schedules.find("stepSound") ==
          add<Scheduler>()->schedules.end()) {

        GameManager::playSound("img/sound/08_Step_rock_02.wav");
        add<Scheduler>()->addSchedule("stepSound", 150, []() {
          GameManager::playSound("img/sound/08_Step_rock_02.wav");
        });
      }
    } else {
      add<Scheduler>()->removeSchedule("stepSound");
    }
  }

  void leftOrEnteredLava() {
    if (entity->checkComponent<StanleyCup>())
      GameManager::playSound("img/sound/lava.wav");
  }

  bool touchingLava = false;
};

Entity *createPlayer(Entity *spawn) {
  Entity *player = GameManager::createEntity("Player");
  player->box->setPosition(spawn->box->getPosition());
  player->box->setSize(spawn->box->getSize());

  Sprite *sprite = player->add<Sprite>();
  sprite->loadTexture("img/Player.png");
  player->box->setScale({12, 25.6f});

  sprite->addAnimation(
      "run",
      {
          "/home/ethanscharlie/Projects/Code/C++/CharlieGames/"
          "templeofduckie/img/Animations/Player/pixil-frame-0.png",
          "/home/ethanscharlie/Projects/Code/C++/CharlieGames/"
          "templeofduckie/img/Animations/Player/pixil-frame-1.png",
          "/home/ethanscharlie/Projects/Code/C++/CharlieGames/"
          "templeofduckie/img/Animations/Player/pixil-frame-2.png",
          "/home/ethanscharlie/Projects/Code/C++/CharlieGames/"
          "templeofduckie/img/Animations/Player/pixil-frame-3.png",
      },
      75.0f / 1000);

  player->useLayer = true;
  player->layer = 20;

  JumpMan *jumpMan = player->add<JumpMan>();
  jumpMan->gravity = 750.0f;
  jumpMan->speed = 20.0f;
  jumpMan->airSpeed = 20.0f;
  jumpMan->maxSpeed = 100.0f;
  jumpMan->tracktion = 500.0f;

  jumpMan->jumpPeak = 60.0f;
  // jumpMan->jumpPeak = 30.0f;
  jumpMan->jumpChange = 170.0f;

  player->add<PinCameraTo>();
  player->add<PlayerAnimator>();
  player->add<PlayerLevelChange>();
  player->add<PlayerSoundManager>();

  playerCreated = true;
  return player;
}
