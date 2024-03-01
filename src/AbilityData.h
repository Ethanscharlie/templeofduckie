#pragma once
#include <array>
#include <functional>
#include <iostream>
#include <string>

struct AbilityData {
  AbilityData(std::string _name, std::string _description,
              std::function<void()> _onPickup)
      : name(_name), description(_description), onPickup(_onPickup) {}

  std::string name;
  std::string description;
  std::function<void()> onPickup;
};

std::array<AbilityData, 1> ABILITY_DATA = {
  AbilityData(
    "HighJump", 
    "Hold to jump higher",
    []() { 
      std::cout << "Do a thing\n";
    }
  )
};
