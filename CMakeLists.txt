cmake_minimum_required(VERSION 3.28)
project(DATA_VISUALIZATION)

set(CMAKE_CXX_STANDARD 20)

# Ścieżki do bibliotek
set(SFML_ROOT "C:/libs/SFML-2.5.1")
set(GLEW_ROOT "C:/libs/glew-2.2.0")
set(GLM_ROOT "C:/libs/glm")  # Tutaj GLM

add_executable(DATA_VISUALIZATION main.cpp)

# Dodaj ścieżki include
target_include_directories(DATA_VISUALIZATION PRIVATE
    ${SFML_ROOT}/include
    ${GLEW_ROOT}/include
    ${GLM_ROOT}  # GLM jest header-only
)

# Dodaj ścieżki do bibliotek
target_link_directories(DATA_VISUALIZATION PRIVATE
    ${SFML_ROOT}/lib
    ${GLEW_ROOT}/lib/Release/x64
)

# Linkowanie bibliotek
target_link_libraries(DATA_VISUALIZATION
    sfml-system
    sfml-window
    glew32s
    opengl32
)

# Automatyczne kopiowanie DLL do folderu z programem
add_custom_command(TARGET DATA_VISUALIZATION POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SFML_ROOT}/bin/sfml-system-2.dll"
        "${SFML_ROOT}/bin/sfml-window-2.dll"
        $<TARGET_FILE_DIR:DATA_VISUALIZATION>
)