cmake_minimum_required(VERSION 3.10.2)

project(DerbMBattle)

set(CMAKE_CXX_STANDARD 17)

add_library(native-lib SHARED
        GameEngine.cpp
        core/Shader.cpp
        core/MathUtils.cpp
        game/Player.cpp
        game/Bot.cpp
        game/Entity.cpp
        game/WeaponSystem.cpp
        game/EntityManager.cpp
        game/Map.cpp
)

find_library(log-lib log)
find_library(GLESv3-lib GLESv3)

target_link_libraries(
        native-lib
        ${GLESv3-lib}
        ${log-lib}
)
