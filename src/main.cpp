#include "AbilityData.h"
#include "Charlie2D.h"
#include "ExtendedComponent.h"
#include "GameManager.h"

#include "InputManager.h"
#include "Player.h"

void loadGameScene();
void loadMenuScene();

const int TILE_SIZE = 16;

class Kill : public ExtendedComponent {
public:
  void start() override {}

  void update() override {
    Entity *player = GameManager::getEntities("Player")[0];

    if (entity->box->getBox().checkCollision(player->box->getBox())) {
      player->get<PlayerLevelChange>()->resetPlayer();
      GameManager::playSound("img/sound/52_Dive_02.wav");
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
      GameManager::playSound("img/sound/win sound 1-2.wav");
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
      entity->box->changePosition(direction * speed * deltaTime);

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

class PinToCamera : public Component {
  void update(float deltaTime) {
    entity->box->setWithCenter(Camera::getPosition());
  }
};

void loadGameScene() {
  GameManager::destroyAll();
  Camera::resetCamera();
  Camera::scale = 4;

  LDTK::loadJson("img/ldtk/ldtk.ldtk");

  LDTK::onLoadLevel = []() {
    // Removes previeus background and maybe more if something weird happened
    for (Entity *entity : GameManager::getEntities("Background")) {
      entity->toDestroy = true;
    }

    Entity *background = GameManager::createEntity("Background");
    background->add<Sprite>()->loadTexture("img/background.png");

    background->get<entityBox>()->setScale(LDTK::worldBox.size);
    background->get<entityBox>()->setPosition(LDTK::worldBox.position);

    background->useLayer = true;
    background->layer = -10;
    background->add<PinToCamera>();

    for (Entity *entity : GameManager::getAllObjects()) {
      if (entity->tag == "Ground") {
        entity->add<Collider>()->solid = true;
      }

      else if (entity->tag == "Kill") {
        entity->add<Kill>();
      }

      else if (entity->tag == "Lava") {
        if (entity->checkComponent<Sprite>()) {
          if (!playerCreated ||
              (playerCreated && !GameManager::getEntities("Player")[0]
                                     ->checkComponent<StanleyCup>())) {
            entity->add<Kill>();
          }

          entity->get<Sprite>()->setAlpha(150);
          entity->get<Sprite>()->preventWeirdBorder = false;

        } else { // Is layer object
          entity->useLayer = true;
          entity->layer = 30;
        }
      }

      else if (entity->tag == "PlayerSpawn") {
        if (!playerCreated)
          createPlayer(entity);
      }

      else if (entity->tag == "Star") {
        entity->add<Sprite>()->loadTexture("img/star.png");
        entity->get<entityBox>()->setScale({14, 14});
        entity->add<Kill>();

        json fields = entity->get<LDTKEntity>()->entityJson["fieldInstances"];
        if (fields.size() <= 0)
          return;
        for (json field : fields) {
          if (field["__identifier"] == "movePoint" &&
              !field["__value"].is_null()) {
            entity->add<MovePoint>()->startPoint =
                entity->get<entityBox>()->getBox().getCenter();

            Vector2f end = {field["__value"]["cx"], field["__value"]["cy"]};
            entity->add<MovePoint>()->endPoint = end * TILE_SIZE +
                                                 (float)TILE_SIZE / 2 +
                                                 LDTK::worldBox.position;
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

        entity->add<Sprite>()->loadTexture(abilityData.image, false);
        entity->add<Ability>();
        entity->get<Ability>()->abilityData = abilityData;
      }
    }
  };

  LDTK::loadLevel("78b44ad0-b0a0-11ee-b472-39fdb7fc8f8c");
}

class SpaceToStart : public ExtendedComponent {
public:
  void update() {
    if (InputManager::checkInput("jumpTrigger")) {
      loadGameScene();
    }
  }
};

void loadMenuScene() {
  GameManager::destroyAll();
  Camera::resetCamera();

  Entity *background = GameManager::createEntity("Background");
  background->add<Sprite>()->loadTexture("img/title.png");

  background->get<entityBox>()->setScale(GameManager::gameWindowSize);
  background->get<entityBox>()->setWithCenter({0, 0});

  background->add<SpaceToStart>();
}

int main(int, char **) {
  GameManager::init();

  GameManager::playSound("img/sound/08_Step_rock_02.wav");
  Mix_Music *music = Mix_LoadMUS("img/sound/watch_the_sand.wav");
  Mix_PlayMusic(music, -1);

  // loadGameScene();
  loadMenuScene();

  GameManager::doUpdateLoop();

  Mix_FreeMusic(music);

  return 0;
}
