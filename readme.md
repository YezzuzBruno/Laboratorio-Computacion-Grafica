# Guía de Configuración y Estructura del Proyecto (OpenGL + Laboratorio de Rasterización)

Esta guía documenta la estructura, configuración y propósito de cada archivo necesario para compilar y ejecutar el laboratorio de Computación Gráfica (algoritmos de punto medio para rectas, circunferencias y parábolas) en Windows, utilizando OpenGL moderno, GLFW, GLAD, CMake y MinGW-w64.

---

## 1. Requisitos Previos del Entorno

Antes de crear los archivos del proyecto, asegúrate de tener instalado:
*   **Visual Studio Code**: Con las extensiones `C/C++` (Microsoft) y `CMake Tools` (Microsoft).
*   **MinGW-w64 (WinLibs)**: Descargado de winlibs.com (UCRT, Win64, POSIX threads). Descomprimido en `C:\mingw64` y agregado al PATH del sistema. *No se debe usar Strawberry Perl.*
*   **CMake**: Instalado con la opción "Add to system PATH" marcada.
*   **GLFW**: Binarios de 64-bit descargados de glfw.org.
*   **GLAD**: Cargador generado en glad.dav1d.de (C/C++, OpenGL, Core, Version 3.3, "Generate a loader").

---

## 2. Estructura de Carpetas del Proyecto

El proyecto debe organizarse en la siguiente estructura de directorios y archivos:

*   **`.vscode/`**: Carpeta de configuración específica del editor.
    *   `settings.json`: Archivo de configuración del editor.
*   **`external/`**: Carpeta que contiene bibliotecas de terceros.
    *   **`include/`**: Cabeceras de GLAD.
    *   **`src/`**: Código fuente de GLAD.
    *   **`glfw/`**: Biblioteca GLFW.
        *   `include/`: Cabeceras de GLFW.
        *   `lib-mingw-w64/`: Librerías estáticas (.a) y dinámicas (.dll) de GLFW.
*   **`CMakeLists.txt`**: Archivo de configuración de construcción.
*   **`main.cpp`**: Código fuente principal en C++.

---

## 3. Explicación Detallada de Cada Archivo

### 3.1 Carpeta `external/include/` (GLAD)
Contiene los archivos de cabecera `glad.h` y `khrplatform.h`. 
*   **Propósito**: GLAD es un cargador de funciones de OpenGL. Estas cabeceras permiten a tu código C++ acceder a las funciones modernas de OpenGL (como `glGenVertexArrays`, `glDrawArrays`, etc.) de forma multiplataforma y segura. Sin ellas, el compilador no reconocería las funciones de OpenGL moderno.

### 3.2 Carpeta `external/src/` (GLAD)
Contiene el archivo `glad.c`.
*   **Propósito**: Es la implementación del cargador. CMake compilará este archivo junto con tu `main.cpp` para generar el ejecutable. Es el encargado de cargar dinámicamente los punteros a las funciones de OpenGL en tiempo de ejecución desde los drivers de la tarjeta gráfica.

### 3.3 Carpeta `external/glfw/` (GLFW)
Contiene la biblioteca GLFW, dividida en `include` (cabeceras) y `lib-mingw-w64` (archivos `.a` y `.dll`).
*   **Propósito**: GLFW es una biblioteca de gestión de ventanas. Se encarga de crear la ventana de la aplicación, gestionar el contexto de OpenGL, procesar eventos de teclado y ratón, y crear el bucle de renderizado. La carpeta `lib-mingw-w64` contiene la librería estática `libglfw3.a` (para enlazar en tiempo de compilación) y la librería dinámica `glfw3.dll` (necesaria en tiempo de ejecución).

### 3.4 Archivo `CMakeLists.txt`
Es el script de construcción del proyecto. Define las reglas para que CMake genere los archivos necesarios para compilar.
*   **Propósito**: 
    *   Indica la versión mínima de CMake y el nombre del proyecto.
    *   Configura el estándar de C++ (C++17).
    *   Añade los directorios de inclusión (`include_directories`) para que el compilador encuentre las cabeceras de GLAD y GLFW.
    *   Añade el directorio de librerías (`link_directories`) para que el enlazador encuentre `libglfw3.a`. Esta línea es crítica para evitar el error `cannot find -lglfw3`.
    *   Compila el archivo `glad.c` como una librería estática (`add_library`).
    *   Crea el ejecutable final (`add_executable`).
    *   Enlaza las bibliotecas necesarias (`target_link_libraries`): `glad`, `glfw3`, `opengl32` y `gdi32`.
    *   Ejecuta un comando posterior a la compilación (`add_custom_command`) para copiar automáticamente la DLL `glfw3.dll` a la carpeta `build`, evitando que el programa falle al ejecutarse por no encontrar la DLL.

### 3.5 Archivo `.vscode/settings.json`
Es el archivo de configuración del espacio de trabajo de VS Code.
*   **Propósito**:
    *   `cmake.generator`: Especifica que se use `MinGW Makefiles` como generador.
    *   `cmake.buildDirectory`: Define la carpeta `build` como el directorio de salida.
    *   `cmake.configureSettings`: Fuerza a CMake a usar las rutas absolutas de los compiladores `gcc.exe` y `g++.exe` de MinGW-w64 (ej. `C:/mingw64/bin/gcc.exe`). Esto es fundamental para evitar que VS Code use por error el compilador de Strawberry Perl u otro compilador incompatible que esté en el PATH del sistema.

### 3.6 Archivo `main.cpp`
Es el código fuente principal del laboratorio.
*   **Propósito**:
    *   **Inicialización**: Inicializa GLFW, configura la versión de OpenGL (3.3 Core), crea la ventana de 800x600 y carga las funciones de OpenGL con GLAD.
    *   **Shaders**: Define los shaders (Vertex y Fragment) en GLSL para renderizar puntos (`GL_POINTS`) con un tamaño específico y un color determinado.
    *   **Algoritmos de Rasterización**: Implementa las funciones del método del punto medio para:
        *   `drawLine`: Trazado de rectas.
        *   `drawCircle`: Trazado de circunferencias (usando simetría de orden 8).
        *   `drawParabola`: Trazado de parábolas (dividido en dos regiones según la pendiente).
    *   **Gestión de Datos**: Almacena los puntos generados en un vector (`std::vector`) y los envía a la GPU mediante un VBO (Vertex Buffer Object) y un VAO (Vertex Array Object).
    *   **Bucle de Renderizado**: Mantiene la ventana abierta, limpia el búfer, dibuja los puntos acumulados y procesa los eventos de la ventana.

---

## 4. Pasos para Compilar y Ejecutar

1.  Abre la carpeta del proyecto en VS Code.
2.  Abre la paleta de comandos (`Ctrl+Shift+P`) y ejecuta **`CMake: Select a Kit`**. Selecciona el compilador de MinGW-w64 (GCC x.x.x x86_64-w64-mingw32).
3.  Ejecuta **`CMake: Configure`**. Esto generará la carpeta `build` y los archivos de construcción.
4.  Ejecuta **`CMake: Build`** (o presiona `F7`). Esto compilará `glad.c` y `main.cpp`, enlazará las bibliotecas y copiará la DLL de GLFW.
5.  Ejecuta **`CMake: Run Without Debugging`** para lanzar la aplicación. Deberías ver una ventana con el trazado de la recta, la circunferencia y las parábolas.

---

## 5. Solución de Problemas Comunes (Troubleshooting)

*   **Error: `cmake: command not found`**
    *   *Causa*: CMake no está en el PATH del sistema.
    *   *Solución*: Reinstalar CMake marcando la opción "Add to system PATH".
*   **Error: `project() command is missing`**
    *   *Causa*: El archivo `CMakeLists.txt` está vacío o le falta la línea `project()`.
    *   *Solución*: Asegurarse de que el archivo `CMakeLists.txt` tenga el contenido completo.
*   **Error: `cannot find -lglfw3`**
    *   *Causa*: CMake no encuentra la librería `libglfw3.a`.
    *   *Solución*: Verificar que exista la línea `link_directories(external/glfw/lib-mingw-w64)` en el `CMakeLists.txt`.
*   **El compilador es Strawberry Perl**
    *   *Causa*: El PATH de Windows le da prioridad a Strawberry Perl.
    *   *Solución*: Usar el archivo `.vscode/settings.json` para forzar las rutas de los compiladores de `C:/mingw64/bin/`.
*   **Error: `glfw3.dll not found` al ejecutar**
    *   *Causa*: La DLL no está en la carpeta del ejecutable.
    *   *Solución*: Asegurarse de que el comando `add_custom_command` en el `CMakeLists.txt` esté copiando la DLL a la carpeta `build`.

---
*Documentación generada para el Laboratorio de Computación Gráfica.*