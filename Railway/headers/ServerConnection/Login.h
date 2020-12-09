#pragma once
#include <string>
#include <optional>

struct Login {
    std::string name = "SVM";
    std::optional<std::string> parrword, game;
    std::optional<int> num_turns, num_players;
};