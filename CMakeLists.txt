cmake_minimum_required(VERSION 3.17)
project(ImagineRobot
	VERSION 1.0
	LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 26)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Agent/AgentController.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Agent/Brain.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Common/Request.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Common/Tile.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Parsers/MapParser.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Parsers/ScriptParser.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Parsers/DictionaryParser.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Parsers/ClosetParser.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Render/ShaderProgram.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Render/PlaygroundView.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Dictionary.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/ItemStash.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Map.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Playground.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Script.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Simulation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Utils/JSONParser.cpp

		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/glad.c
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/vendor
		${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

target_link_libraries(${PROJECT_NAME} PUBLIC
	glfw
	glm::glm
	stdc++exp
)
