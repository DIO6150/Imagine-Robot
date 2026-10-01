cmake_minimum_required(VERSION 3.17)
project(ImagineRobot
	VERSION 1.0
	LANGUAGES CXX
)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Main.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Robot/Simulation/Simulation.cpp
		Robot/Simulation/Map.cpp
)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/vendor
)
