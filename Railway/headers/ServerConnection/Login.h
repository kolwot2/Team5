#pragma once
#include <string>
#include <optional>

struct Login {
    std::string name = "SVM123";
    std::optional<std::string> parrword = "qwerty123", game = "1234554321QQ";
    std::optional<int> num_turns = 50, num_players;
};