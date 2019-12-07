cmake_minimum_required(VERSION 3.4.1)

add_library(${PROJECT_NAME} SHARED
        src/main/cpp/interface.cpp
        src/main/cpp/dlopenBypass/DlOpenBypass.cpp
        src/main/cpp/invokeHook/ArtMethodInvokeHook.cpp
        src/main/cpp/artHelper/ArtPathFinder.cpp
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        src/main/cpp/
        )
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/artifacts/ChickenHook/${CMAKE_LIBRARY_ARCHITECTURE}/include/
        )

target_link_libraries(${PROJECT_NAME}
        # add chickenhook here
        ${CMAKE_SOURCE_DIR}/artifacts/ChickenHook/${CMAKE_LIBRARY_ARCHITECTURE}/lib/libChickenHook.a
        log
        dl
        )