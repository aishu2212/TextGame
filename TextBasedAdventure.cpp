#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
// Function to load game data from a JSON file
json loadMapData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    json jsonData;
    file >> jsonData;
    file.close();
    return jsonData;
}


void displayRoom(const json& roomData, const json& playerInventory, const json& gameData) {
    std::cout << "Room: " << roomData["id"] << std::endl;
    std::cout << "Description: " << roomData["desc"] << std::endl;

    // Display objects in the room
    if (gameData.find("objects") != gameData.end()) {
        const json& objects = gameData["objects"];
        int objectCount = 0;

        for (const auto& obj : objects) {
            if (obj["initialroom"] == roomData["id"]) {
                std::string objId = obj["id"];
                std::cout << "- " << objId << ": " << obj["desc"];
                if (playerInventory.find(objId) != playerInventory.end()) {
                    std::cout << " (in your inventory)";
                }
                std::cout << std::endl;
                ++objectCount;
            }
        }

        std::cout << "Objects in the room: " << objectCount << std::endl;
    }

    // Display enemies in the room
    if (gameData.find("enemies") != gameData.end()) {
        int enemyCount = 0;

        for (const auto& enemy : gameData["enemies"]) {
            if (enemy["initialroom"] == roomData["id"]) {
                std::string enemyId = enemy["id"];
                std::cout << "- " << enemyId << ": " << enemy["desc"] << " Aggressiveness: " << enemy["aggressiveness"] << std::endl;
                if (playerInventory.find(enemyId) != playerInventory.end()) {
                    std::cout << " (in your inventory)";
                }
                ++enemyCount;
            }
        }

        std::cout << "Enemies in the room: " << enemyCount << std::endl;
    }
}

// Function to handle the "take" command
void takeObject(const std::string& objectId, json& playerInventory, const json& roomData, json& gameData) {
    if (gameData.find("objects") != gameData.end()) {
        json& objects = gameData["objects"];
        auto it = std::find_if(objects.begin(), objects.end(), [&](const json& obj) {
            return obj["id"] == objectId && obj["initialroom"] == roomData["id"];
        });
        if (it != objects.end()) {
            playerInventory[objectId] = *it;
            objects.erase(it);
            std::cout << "You took the " << objectId << "." << std::endl;
        } else {
            std::cout << "Error: Object not found in this room." << std::endl;
        }
    } else {
        std::cout << "Error: No objects in this room." << std::endl;
    }
}

bool canKillEnemy(const std::string& enemyId, const json& playerInventory, const json& gameData) {
    if (gameData.find("enemies") != gameData.end()) {
        const auto& enemies = gameData["enemies"];
        auto it = std::find_if(enemies.begin(), enemies.end(), [&](const auto& enemy) {
            return enemy["id"] == enemyId;
        });

        if (it != enemies.end()) {
            const auto& requiredObjects = it->value("killedby", json::array());

            std::cout << "Required objects to kill " << enemyId << ": " << requiredObjects << std::endl;

            if (requiredObjects.empty()) {
                return true;
            }

            bool hasAllObjects = std::all_of(requiredObjects.begin(), requiredObjects.end(), [&](const auto& objectId) {
                bool hasObject = playerInventory.find(objectId) != playerInventory.end();
                std::cout << "Has " << objectId << "? " << (hasObject ? "Yes" : "No") << std::endl;
                return hasObject;
            });

            return hasAllObjects;
        }
    }
    return false;
}

void killEnemy(const std::string& enemyId, json& playerInventory, json& roomData, json& gameData) {
    auto& enemies = gameData["enemies"];

    // Find the enemy in the list
    auto it = std::find_if(enemies.begin(), enemies.end(), [&](const json& enemy) {
        return enemy["id"] == enemyId;
    });

    bool enemyKilled = false;

    if (it != enemies.end()) {
        if (canKillEnemy(enemyId, playerInventory, gameData)) {
            std::cout << "You killed the " << enemyId << "." << std::endl;
            enemyKilled = true;
            // Erase the enemy from the list
            enemies.erase(it);
            if (enemies.empty()) {
                std::cout << "Enjoy! All enemies in this room are defeated." << std::endl;
            }
        } else {
            std::cout << "Error: You cannot kill the " << enemyId << ". Missing required objects." << std::endl;
            std::cout << "You have:";

            for (auto it = playerInventory.begin(); it != playerInventory.end(); ++it) {
                std::cout << " " << it.key();
            }
            std::cout << std::endl;
            std::cout << "You died." << std::endl;
            exit(EXIT_FAILURE);
        }
    } else {
        std::cout << "Error: Enemy not found in this room." << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Issue with loading map: " << argv[0] << " <map_file.json>" << std::endl;
        return EXIT_FAILURE;
    }
    const std::string mapFilename(argv[1]);
    json gameData = loadMapData(mapFilename);
    // Initialize game state
    std::string currentRoom = gameData["player"]["initialroom"];
    json playerInventory;
    // Game loop
    while (true) {
        json currentRoomData;
        // Find the current room in the list of rooms
        for (const auto& room : gameData["rooms"]) {
            if (room["id"] == currentRoom) {
                currentRoomData = room;
                break;
            }
        }
        if (currentRoomData.is_null()) {
            std::cerr << "Error: Current room not found in the map data." << std::endl;
            return EXIT_FAILURE;
        }
        displayRoom(currentRoomData, playerInventory, gameData);
        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);
        if (command == "look") {
            // Display room information again
        } else if (command.find("go ") == 0 || command.find("door") == 0) {
            std::string exit = (command.find("go ") == 0) ? command.substr(3) : command;
    // Check if there are enemies with aggressiveness greater than 0 in the current room
    if (gameData.find("enemies") != gameData.end()) {
        const auto& enemiesInCurrentRoom = gameData["enemies"];
        // Check if there's any enemy with aggressiveness greater than 0 in the current room
        if (std::any_of(enemiesInCurrentRoom.begin(), enemiesInCurrentRoom.end(), [&](const auto& enemy) {
                return enemy["initialroom"] == currentRoom && enemy["aggressiveness"] > 0;
            })) {
            std::cout << "Error: You cannot exit the room without defeating enemies with aggressiveness greater than 0." << std::endl;
            std::cout << "You died." << std::endl;
            return EXIT_FAILURE;
        }
    }
    // Check if the current room has valid exits
            if (!currentRoomData["exits"].empty()) {
                if (currentRoomData["exits"].is_object()) {
                    auto exits = currentRoomData["exits"];
                    if (exits.find(exit) != exits.end()) {
                        currentRoom = exits[exit];
                    } else {
                        std::cout << "Error: No exit in that direction." << std::endl;
                    }
                } 
            } else {
                // The current room has no valid exits
                std::cout << "Error: You entered a room with no exit." << std::endl;
                std::cout << "You died." << std::endl;
                return EXIT_FAILURE;
            }
        }
        else if (command.find("take ") == 0) {
            std::string objectId = command.substr(5);
            takeObject(objectId, playerInventory, currentRoomData, gameData);
        } else if (command.find("kill ") == 0) {
            std::string enemyId = command.substr(5);
            killEnemy(enemyId, playerInventory, currentRoomData, gameData);
        } else {
            std::cout << "Error: Command not recognized." << std::endl;
        }

       // Check if there are enemies in the current room
       if (gameData.find("enemies") != gameData.end()) {
           const auto& enemiesInCurrentRoom = gameData["enemies"];
            if (gameData["objective"]["type"] == "kill") {
            auto objectives = gameData["objective"]["what"];
            bool allEnemiesKilled = true;

            for (const auto& enemyId : objectives) {
                // Check if the enemy is still alive in any room
                if (gameData.find("enemies") != gameData.end()) {
                    const auto& allEnemies = gameData["enemies"];
                    if (std::any_of(allEnemies.begin(), allEnemies.end(), [&](const auto& enemy) {
                            return enemy["id"] == enemyId;
                        })) {
                        // Enemy from the objectives is still alive in some room
                        allEnemiesKilled = false;
                        break;
                    }
                }
            }

            if (allEnemiesKilled) {
                std::cout << "Congratulations! You've reached the objective of killing all specified enemies." << std::endl;
                break;
            }
        }
        }
        // Check win condition based on reaching a specific room
        if (gameData["objective"]["type"] == "room") {
            auto objectiveRooms = gameData["objective"]["what"];
            if (std::find(objectiveRooms.begin(), objectiveRooms.end(), currentRoom) != objectiveRooms.end()) {
                std::cout << "Congratulations! You've reached the objective room. You win!" << std::endl;
                break;
            }
        }
        if (gameData["objective"]["type"] == "collect") {
            auto requiredItems = gameData["objective"]["what"];
            bool allItemsCollected = std::all_of(requiredItems.begin(), requiredItems.end(), [&](const auto& itemId) {
                return playerInventory.find(itemId) != playerInventory.end();
            });

            if (allItemsCollected) {
                std::cout << "Congratulations! You've collected all required items." << std::endl;
                break;
            }
        }

    }

    return 0;

}

