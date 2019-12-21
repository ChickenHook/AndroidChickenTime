cmake_minimum_required(VERSION 3.4.1)

add_library(${PROJECT_NAME} SHARED
        src/main/cpp/interface.cpp
        src/main/cpp/dlopenBypass/DlOpenBypass.cpp
        src/main/cpp/invokeHook/ArtMethodInvokeHook.cpp
        src/main/cpp/invokeHook/ArtMethodPrettyNameHook.cpp
        src/main/cpp/artHelper/ArtPathFinder.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        src/main/cpp/
        )
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/artifacts/ChickenHook/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/include/
        ${CMAKE_SOURCE_DIR}/artifacts/BeaEngine/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/include/
        )

target_link_libraries(${PROJECT_NAME}
        # add chickenhook here
        ${CMAKE_SOURCE_DIR}/artifacts/ChickenHook/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/lib/libChickenHook.a
        ${CMAKE_SOURCE_DIR}/artifacts/BeaEngine/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/lib/libBeaEngine_s_d_l.a
        log
        dl
        )