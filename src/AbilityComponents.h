#pragma once
#include <Charlie2D.h>

// Only needs to be used like a tag
class StanleyCup : public Component {};

// Allows for the player to change to the flying head
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

