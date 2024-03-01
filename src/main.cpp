#include "Charlie2D.h"
#include "ExtendedComponent.h"
#include "GameManager.h"

#include "Player.h"

void loadGameScene();
void loadMenuScene();

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

class Ability : public ExtendedComponent {
public:
  void start() override {}

  void update() override {
    Entity *player = GameManager::getEntities("Player")[0];

    if (entity->box->getBox().checkCollision(player->box->getBox())) {
      onPickup();
      entity->toDestroy = true;
    }
  }

  std::function<void()> onPickup = []() {
    std::cout << "Somebody forgor to give this an item\n";
  };
};

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

      // Items
      else if (entity->tag == "HighJump") {
        entity->add<Sprite>()->loadTexture("img/crocks.png", false);
        entity->add<Ability>();
        entity->get<Ability>()->onPickup = []() {
          GameManager::getEntities("Player")[0]->get<JumpMan>()->jumpPeak =
              60.0f;
        };
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
