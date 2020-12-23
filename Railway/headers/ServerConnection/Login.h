#pragma once
#include <string>
#include <optional>

struct Login {
    std::string name = "SVM1";
    std::optional<std::string> parrword = "qwerty123", game;
    std::optional<int> num_turns = 200, num_players;
};