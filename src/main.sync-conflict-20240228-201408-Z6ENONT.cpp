#include "Charlie2D.h"

void loadGameScene();
void loadMenuScene();

void loadGameScene() {
  GameManager::destroyAll();
  Camera::resetCamera();
  Camera::scale = 2;

  LDTK::loadJson("img/ldtk/ldtk.ldtk");

  LDTK::onLoadLevel = []() {
    for (Entity *entity : GameManager::getAllObjects()) {
      if (entity->tag == "Ground") {
        entity->add<Collider>()->solid = true;
      }
    }
  };

  // Load a specific level using its ID
  LDTK::loadLevel("c4217370-b0a0-11ee-9036-7938393bcf38");
}

int main(int, char **) {
  GameManager::init();

  loadGameScene();

  GameManager::doUpdateLoop();

  return 0;
}
