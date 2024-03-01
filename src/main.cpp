#include "AbilityData.h"
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
      std::cout << "Killed Player\n";
    }
  }
};

class Ability : public ExtendedComponent {
public:
  void start() override {}

  void update() override {
    Entity *player = GameManager::getEntities("Player")[0];

    if (entity->box->getBox().checkCollision(player->box->getBox())) {
      abilityData.onPickup();
      entity->toDestroy = true;
    }
  }

  AbilityData abilityData;
};

class MovePoint : public Component {
public:
  void update(float deltaTime) override {
    if (disable)
      return;
    Vector2f currentPosition = entity->get<entityBox>()->getBox().getCenter();
    if (movingToEnd) {
      // Check Past
      Vector2f direction = (endPoint - currentPosition).normalize();
      entity->get<entityBox>()->changePosition(direction * speed * deltaTime);

      if (currentPosition.dist(endPoint) < lowSpeed)
        movingToEnd = false;
    } else {

      // Check Past
      Vector2f direction = (startPoint - currentPosition).normalize();
      entity->get<entityBox>()->changePosition(direction * speed * deltaTime);

      if (currentPosition.dist(startPoint) < lowSpeed)
        movingToEnd = true;
    }
  }

  bool movingToEnd = true;
  bool disable = false;

  float speed = 60.0f;
  Vector2f startPoint;
  Vector2f endPoint;

  const float lowSpeed = 8;
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
      }

      else if (entity->tag == "Kill") {
        entity->add<Kill>();
      }

      else if (entity->tag == "Lava") {
        if (!playerCreated ||
            (playerCreated && !GameManager::getEntities("Player")[0]
                                   ->checkComponent<StanleyCup>())) {
          entity->add<Kill>();
        }
      }

      else if (entity->tag == "PlayerSpawn") {
        if (!playerCreated)
          createPlayer(entity);
      }

      else if (entity->tag == "Star") {
        entity->add<Sprite>()->loadTexture("img/star.png");
        entity->get<entityBox>()->setScale({14, 14});

        json fields = entity->get<LDTKEntity>()->entityJson["fieldInstances"];
        if (fields.size() <= 0)
          return;
        for (json field : fields) {
          if (field["__identifier"] == "movePoint" &&
              !field["__value"].is_null()) {
            entity->add<MovePoint>()->startPoint =
                entity->get<entityBox>()->getBox().getCenter();

            Vector2f end = {field["__value"]["cx"], field["__value"]["cy"]};
            entity->add<MovePoint>()->endPoint = end * 16 + 8;
          } else if (field["__identifier"] == "moveSpeed") {
            entity->add<MovePoint>()->speed = (float)field["__value"];
          }

          if (field["__identifier"] == "movePoint" &&
              field["__value"].is_null()) {
            entity->add<MovePoint>()->disable = true;
          }
        }
      }

      // Abilities
      for (AbilityData abilityData : ABILITY_DATA) {
        if (entity->tag != abilityData.name)
          continue;

        entity->add<Sprite>()->loadTexture("img/crocks.png", false);
        entity->add<Ability>();
        entity->get<Ability>()->abilityData = abilityData;
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
