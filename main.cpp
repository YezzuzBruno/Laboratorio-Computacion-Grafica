#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

// --- Estructura para guardar los puntos ---
struct Point {
    int x, y;
};

// --- Shaders (Adaptados para 2D) ---
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
void main() {
    // Mapeamos el sistema de coordenadas de -400 a 400 (X) y -300 a 300 (Y)
    // a Normalized Device Coordinates (-1.0 a 1.0)
    float nx = aPos.x / 400.0f;
    float ny = aPos.y / 300.0f;
    gl_Position = vec4(nx, ny, 0.0, 1.0);
    gl_PointSize = 2.0; // Tamaño del punto en píxeles
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 ourColor; // <-- Añade esta línea
void main() {
    FragColor = ourColor; // <-- Usa la variable
}
)";

// --- Variables Globales para OpenGL ---
unsigned int shaderProgram;
unsigned int VAO, VBO;
std::vector<Point> points; // Aquí guardaremos todos los puntos a dibujar

// --- Función para inicializar OpenGL (Shaders, VAO, VBO) ---
void initOpenGL() {
    // Compilar Shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Crear VAO y VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Configurar atributos de posición (x, y)
    glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, sizeof(Point), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// --- Función para dibujar los puntos acumulados ---
void drawPoints() {
    if (points.empty()) return;
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);
    
    glUseProgram(shaderProgram);
    glDrawArrays(GL_POINTS, 0, points.size());
    
    glBindVertexArray(0);
}

// =========================================================
// 1. ALGORITMO DEL PUNTO MEDIO PARA RECTAS
// =========================================================
void drawLine(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int d = 2 * dy - dx;
    int incE = 2 * dy;
    int incNE = 2 * (dy - dx);
    int x = x1, y = y1;

    points.push_back({x, y});
    while (x < x2) {
        if (d <= 0) {
            d += incE;
            x++;
        } else {
            d += incNE;
            x++;
            y++;
        }
        points.push_back({x, y});
    }
}

// =========================================================
// 2. ALGORITMO DEL PUNTO MEDIO PARA CIRCUNFERENCIAS
// =========================================================
void circlePoints(int xc, int yc, int x, int y) {
    points.push_back({xc + x, yc + y});
    points.push_back({xc - x, yc + y});
    points.push_back({xc + x, yc - y});
    points.push_back({xc - x, yc - y});
    points.push_back({xc + y, yc + x});
    points.push_back({xc - y, yc + x});
    points.push_back({xc + y, yc - x});
    points.push_back({xc - y, yc - x});
}

void drawCircle(int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 1 - r;
    
    circlePoints(xc, yc, x, y);
    while (y > x) {
        if (d < 0) {
            d += 2 * x + 3;
            x++;
        } else {
            d += 2 * (x - y) + 5;
            x++;
            y--;
        }
        circlePoints(xc, yc, x, y);
    }
}

// =========================================================
// 3. ALGORITMO DEL PUNTO MEDIO PARA PARÁBOLA (y^2 = 4ax)
// =========================================================
void parabolaPoints(int x, int y, int a) {
    // Simetría respecto al eje X
    points.push_back({x, y});
    points.push_back({x, -y});
}

void drawParabola(int a) {
    if (a <= 0) return;
    
    int x = 0;
    int y = 0;
    
    // Región 1: Cerca del vértice (pendiente > 1, avanzamos en Y)
    // Límite: y < 2a
    int d1 = 1 - 2 * a; // Valor inicial en (0,0)
    parabolaPoints(x, y, a);
    
    while (y < 2 * a) {
        if (d1 < 0) {
            // Escoger NE -> (x+1, y+1)
            d1 += 4 * y + 6 - 4 * a; // Aproximación incremental
            x++;
            y++;
        } else {
            // Escoger N -> (x, y+1)
            d1 += 4 * y + 4;
            y++;
        }
        parabolaPoints(x, y, a);
    }
    
    // Región 2: Pendiente <= 1 (avanzamos en X)
    // Recalculamos d para el punto de transición (a, 2a)
    int d2 = (2 * a + 1) * (2 * a + 1) - 4 * a * (a + 1); // Evaluamos F(M) en la transición
    
    // Seguimos avanzando en X
    while (x < 400) { // Límite de la pantalla
        if (d2 < 0) {
            // Escoger NE -> (x+1, y+1)
            d2 += 4 * y + 6 - 4 * a;
            x++;
            y++;
        } else {
            // Escoger E -> (x+1, y)
            d2 += -4 * a;
            x++;
        }
        parabolaPoints(x, y, a);
    }
}

// =========================================================
// PROGRAMA PRINCIPAL
// =========================================================
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Laboratorio de Trazado", NULL, NULL);
    if (window == NULL) {
        std::cout << "Fallo al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Fallo al inicializar GLAD" << std::endl;
        return -1;
    }

    initOpenGL();

    // --- Generar los puntos de las figuras ---
    // Limpiamos el vector antes de agregar nuevos puntos
    points.clear();
    
    // Dibujar Recta (Rojo)
    glUniform4f(glGetUniformLocation(shaderProgram, "ourColor"), 1.0f, 0.0f, 0.0f, 1.0f);
    drawLine(-300, -150, 250, 120);
    
    // 2. Circunferencia (Para cambiar el color, necesitaríamos otro shader o uniform)
    // Como el shader tiene color rojo fijo, todas las figuras serán rojas.
    // Para el lab, puedes cambiar el color en el fragment shader o usar uniforms.
    drawCircle(0, 0, 150);
    
    // 3. Parábolas con distintos valores de 'a'
    // Nota: Como todas se dibujan en rojo, se superpondrán. 
    // Para verlas separadas, puedes comentar las anteriores o cambiar el shader.
    drawParabola(20);  // Parábola más cerrada
    drawParabola(50);  // Parábola media
    drawParabola(100); // Parábola más abierta

    // --- Bucle de renderizado ---
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Fondo blanco
        glClear(GL_COLOR_BUFFER_BIT);

        drawPoints(); // Dibujamos todos los puntos acumulados

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}