cmake_minimum_required(VERSION 3.17)
project(ImagineRobot
	VERSION 1.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Playground.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Simulation.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Map.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Common/Tile.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Render/PlaygroundView.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Agent/Agent.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Utils/JSONParser.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/vendor
)
