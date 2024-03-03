#pragma once
#include "AbilityData.h"
#include <Charlie2D.h>

class ItemPanel : public Component {
public:
  void start() override {
    Entity *canvas = GameManager::createEntity("panel-canvas");
    canvas->add<UICanvas>();

    entity->setParent(canvas);
    entity->add<UISliceRenderer>();
    entity->get<UISliceRenderer>()->setColor({156, 39, 176});
    entity->get<UISliceRenderer>()->loadTexture("img/infoPanel.png");
    entity->require<entityBox>()->setScale({700, 300});
    entity->require<entityBox>()->anchor = 4;
    entity->require<entityBox>()->setLocalWithCenter({0, -900});
    entity->add<Text>();
    entity->get<Text>()->changeFont("img/fonts/prstart.ttf", 30);
  }

  void update(float deltaTime) override {
    if (opening) {
      entity->require<entityBox>()->changeLocalPosition({0, deltaTime * 1300});
      if (entity->require<entityBox>()->getLocalBox().getCenter().y >= 0) {
        opening = false;
      }
    } else {
      if (InputManager::checkInput("jump")) {
        closing = true;
      }

      if (closing) {
        entity->require<entityBox>()->changeLocalPosition(
            {0, -deltaTime * 1300});
        if (entity->require<entityBox>()->getLocalPosition().y <= -900) {
          entity->toDestroy = true;
        }
      }
    }
  }

  void onDestroy() override {}

  bool opening = true;
  bool closing = false;
  bool hover = false;
};

void createItemPanel(AbilityData abilityData) {
  Entity *panel = GameManager::createEntity("panel");
  panel->add<ItemPanel>();
  panel->get<Text>()->text = abilityData.name + "\n\n" +
                             abilityData.description +
                             "\n\nPress Space to close";
}
