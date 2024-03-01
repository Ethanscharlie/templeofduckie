#pragma once
#include "ExtendedComponent.h"
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

class Dash : public ExtendedComponent {
  void update() override {
    if (InputManager::checkInput("dash")) {
      if (abs(get<physicsBody>()->velocity.x) < 0.1)
        return;
      int dir = 0;
      if (InputManager::checkInput("right"))
        dir = 1;
      if (InputManager::checkInput("left"))
        dir = -1;

      if (canDash) {
        Vector2f slide = {InputManager::checkHorizontal() * dashAmount, 0};
        entity->require<entityBox>()->slide(
            slide, GameManager::getEntities("Ground"), true, true);
        canDash = false;
        add<Scheduler>()->addSchedule(
            "dash", 400, [&]() { canDash = true; }, []() {}, true);
      }
    }
  }

  const float dashAmount = 60.0f;
  bool canDash = true;
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

  // jumpMan->jumpPeak = 60.0f;
  jumpMan->jumpPeak = 30.0f;
  jumpMan->jumpChange = 170.0f;

  player->add<PinCameraTo>();
  player->add<PlayerAnimator>();
  player->add<PlayerLevelChange>();

  player->add<Dash>();

  playerCreated = true;
  return player;
}
