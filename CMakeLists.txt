cmake_minimum_required(VERSION 3.20)
project(PruebaOpenGL)

set(CMAKE_CXX_STANDARD 17)

# Directorios de inclusión (GLAD y GLFW)
include_directories(external/include external/glfw/include)

# ⬇️ AÑADE ESTA LÍNEA NUEVA ⬇️
# Le dice al enlazador dónde encontrar libglfw3.a
link_directories(external/glfw/lib-mingw-w64)

# Añadir el archivo fuente de GLAD
add_library(glad external/src/glad.c)

# Crear el ejecutable
add_executable(mi_app main.cpp)

# Enlazar bibliotecas (glfw3, opengl32 y gdi32)
target_link_libraries(mi_app glad glfw3 opengl32 gdi32)

# Copiar la DLL de GLFW junto al ejecutable (esencial en Windows)
add_custom_command(TARGET mi_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/external/glfw/lib-mingw-w64/glfw3.dll"
    $<TARGET_FILE_DIR:mi_app>
)