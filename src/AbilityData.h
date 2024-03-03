#pragma once
#include <Charlie2D.h>
#include <array>
#include <functional>
#include <iostream>
#include <string>

#include "AbilityComponents.h"

class Kill;
class FlyingHead;

// A struct for basic data for abilities
struct AbilityData {
  AbilityData(std::string _name, std::string _description, std::string _image,
              std::function<void()> _onPickup)
      : name(_name), description(_description), image(_image),
        onPickup(_onPickup) {}

  AbilityData() {
    name = "emptyAbility";
    description = "emptyAbility";
    image = "img/panel.png";
    onPickup = []() { std::cout << "Somebody forgor to give this an item\n"; };
  }

  std::string name;
  std::string description;
  std::string image;
  std::function<void()> onPickup;
};

// A constant array holding each abities data
const std::array<AbilityData, 3> ABILITY_DATA = {
    AbilityData(
        "HighJump", "Hold to jump higher", "img/HighJump.png",
        []() {
          GameManager::getEntities("Player")[0]->get<JumpMan>()->jumpPeak =
              60.0f;
        }),
    AbilityData("StanleyCup", "Lava protection", "img/StanleyCup.png",
                []() {
                  GameManager::getEntities("Player")[0]->add<StanleyCup>();

                  for (Entity *entity : GameManager::getEntities("Lava")) {
                    if (entity->checkComponent<Kill>())
                      entity->remove<Kill>();
                  }
                }),

    AbilityData(
        "FlyingHead", "Press E", "img/head1616.png",
        []() { GameManager::getEntities("Player")[0]->add<FlyingHead>(); }),
};
