#pragma once
#include "AbilityComponents.h"
#include "AbilityData.h"
#include "ExtendedComponent.h"
#include "GameManager.h"
#include "InputManager.h"
#include "Vector2f.h"
#include "physicsBody.h"
#include <Charlie2D.h>
#include <cstdlib>

bool playerCreated = false;
Vector2f playerSize = {12, 25.6f};

class PinCameraTo : public ExtendedComponent {
public:
  void update() override {
    Camera::setPosition(entity->box->getBox().getCenter());
  }
};

class FlyingHead : public ExtendedComponent {
public:
  void update() override {
    if (InputManager::keys[SDLK_e]) {
      flying = !flying;
    }

    if (flying) {
      get<physicsBody>()->velocity.y = -InputManager::checkVertical() * 100.0f;
    }
  }

  bool flying = false;
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

    if (entity->checkComponent<FlyingHead>() && get<FlyingHead>()->flying) {
      get<Sprite>()->loadTexture("img/head.png", false);
    } else {
      if (direction != 0 && abs(get<physicsBody>()->velocity.y) <= 0.1) {
        Animation *runAnimation = get<Sprite>()->animations["run"];
        if (!runAnimation->playing) {
          runAnimation->play();
        }
      } else {
        get<Sprite>()->loadTexture("img/Player.png", false);
      }
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

class PlayerAbilityCollector : public ExtendedComponent {
public:
  std::vector<AbilityData> abilities;
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

        return;
      }
    }

    if (touchingLava) {
      leftOrEnteredLava();
      touchingLava = false;
    }
  }

  void update() override {
    checkLava();

    bool flying =
        entity->checkComponent<FlyingHead>() && get<FlyingHead>()->flying;

    if (InputManager::checkInput("jumpTrigger") &&
        abs(get<physicsBody>()->velocity.y) <= 0.1 && !flying) {
      GameManager::playSound("img/sound/30_Jump_03.wav");
    }

    if (InputManager::checkAxis().x != 0 &&
        abs(get<physicsBody>()->velocity.y) <= 0.1 && !flying) {
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
  player->box->setScale(playerSize);

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

  jumpMan->jumpPeak = 30.0f;
  jumpMan->jumpChange = 170.0f;

  player->add<PinCameraTo>();
  player->add<PlayerAnimator>();
  player->add<PlayerLevelChange>();
  player->add<PlayerSoundManager>();
  player->add<PlayerAbilityCollector>();

  playerCreated = true;
  return player;
}
