#pragma once
#include <string>
#include <optional>

struct Login {
    std::string name = "SVM123";
    std::optional<std::string> parrword = "qwerty123", game;
    std::optional<int> num_turns = 500, num_players;
};