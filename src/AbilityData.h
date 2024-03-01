#pragma once
#include <Charlie2D.h>
#include <array>
#include <functional>
#include <iostream>
#include <string>

#include "AbilityComponents.h"

class Kill;

struct AbilityData {
  AbilityData(std::string _name, std::string _description,
              std::function<void()> _onPickup)
      : name(_name), description(_description), onPickup(_onPickup) {}

  AbilityData() {
    name = "emptyAbility";
    description = "emptyAbility";
    onPickup = []() {
      std::cout << "Somebody forgor to give this an item\n";
    };
  }

  std::string name;
  std::string description;
  std::function<void()> onPickup;
};

const std::array<AbilityData, 2> ABILITY_DATA = {
  AbilityData(
    "HighJump", 
    "Hold to jump higher",
    []() { 
      GameManager::getEntities("Player")[0]->get<JumpMan>()->jumpPeak = 60.0f;
    }
  ),
  AbilityData(
    "StanleyCup", 
    "Lava protection",
    []() { 
      GameManager::getEntities("Player")[0]->add<StanleyCup>();

      for (Entity *entity : GameManager::getEntities("Lava")) {
        entity->remove<Kill>();
      }
    }
  ),
};
