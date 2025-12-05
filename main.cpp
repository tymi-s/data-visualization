#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
using namespace std;
using namespace glm;
using namespace sf;

void shaderCompilationStatus(GLuint shader, const char* shaderName) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        cout << "\n BŁĄD: Niepoprawna kompilacja shadera o nazwie: " << shaderName;
    }
    else {
        cout << "\n SUKCES: Kompilacja shadera: " << shaderName << " przebiegla poprawnie!\n";
    }
}

// Definiowanie shaderów ( programy GPU ). Vertex Shader ( przetwarza każdy wierzchołek ):
const GLchar* vertexSource = R"glsl(
#version 150 core
in vec3 position; // Pozycja wierzchołka
in vec3 color; // Kolor wierzchołka
in vec2 aTexCoord;  // obsługa shaderów

out vec3 Color; // Kolor przekazywany do fragment-shadera
out vec2 TexCoord;  // obsługa shaderów

uniform mat4 model; // Macierz modelu (pozycja/rotacja obiektu)
uniform mat4 view; // Macierz widoku (pozycja kamery )
uniform mat4 proj; // Macierz projekcji (perspektywa)



/*
Jak działa zielony main poniżej:
Bierze pozycję wierzchołka (x, y, z)
Mnoży przez 3 macierze: proj * view * model
Przekształca punkt 3D na pozycję ekranową
*/

void main(){
    Color = color;
TexCoord = aTexCoord; // obsługa tekstur
    gl_Position = proj * view * model * vec4(position, 1.0);
}
)glsl";



const GLchar* fragmentSource = R"glsl(
#version 150 core
in vec3 Color; // Kolor z vertex shadera
in vec2 TexCoord;  // użycie tekstury

out vec4 outColor; // Finalny kolor piksela

uniform sampler2D texture1; // użycie tekstury
void main()
{
    outColor = texture(texture1,TexCoord); // tekstura
}
)glsl";

int main()
{

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    zmienne programu

    double yaw = -90.0; // Obrót w lewo/prawo
    double pitch = 0.0; // Obrót góra/dół
    double lastX = 400.0; // Ostatnia pozycja myszy X
    double lastY = 300.0; // Ostatnia pozycja myszy Y
    double sensitivity = 0.1; // Czułość myszy



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  TWORZENIE OKNA SFML
    sf::ContextSettings settings;
    settings.depthBits = 24; // Włączenie testu głębi.  - Dalsze obiekty z bliższymi
    settings.stencilBits = 8;

    Window window(sf::VideoMode(800, 600), "OpenGL 3D Camera", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setMouseCursorGrabbed(true); // Schwytaj kursor - czyli ruch myszy zmienia ustawienie kamery
    window.setMouseCursorVisible(false); // Ukryj kursor
    window.setFramerateLimit(60); // Max 20 FPS



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  Inicjalizacja GLEW  i włączanie testu głębi
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   Tworzenie VAO i VBO
    /// VAO  - służy do przechowywania wierzchołków
    /// VBO  - służy do przechowywania buforów
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   Definicja wierzchołków sześcianu z kolorami:
    /// Każde 6 liczb = 1 wierzchołek (3 pozycja + 3 kolor)
    /// 36 wierzchołków tworzą 6 ścian sześcianu
    /// Każda ściana ma inny kolor (czerwony, zielony, niebieski, żółty, magenta, cyan)
    // Każde 8 liczb = 1 wierzchołek (3 pozycja + 3 kolor + 2 tekstura)
    float vertices[] = {
        // Pozycja          // Kolor           // TexCoord
        // Przednia ściana (czerwona)
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        // Tylna ściana (zielona)
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        // Lewa ściana (niebieska)
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        // Prawa ściana (żółta)
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        // Dolna ściana (magenta)
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,

        // Górna ściana (cyan)
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// przesyłanie danych do GPU

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// wczytywanie tektury

        unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("metal.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        cout << "Tekstura załadowana pomyślnie!" << endl;
    }
    else {
        cout << "Failed to load texture" << endl;
    }
    stbi_image_free(data);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Kompilacja i linkowanie shaderów

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    shaderCompilationStatus(vertexShader, "vertex shader");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    shaderCompilationStatus(fragmentShader, "fragment shader");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    GLint textureLoc = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(textureLoc, 0); // Ustaw sampler na texture unit 0


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Konfiguracja atrybutów wierzchołków

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  Konfiguracja współrzędnych tekstury
    GLint texCoordAttrib = glGetAttribLocation(shaderProgram, "aTexCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Inicjalizacja kamery i macierzy
    vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
    vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
    float obrot = 0.0f;


    mat4 model = mat4(1.0f);
    mat4 proj = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Pobieranie lokalizacji uniform
    /// - uniform to zmienna globalna w shaderze, jednakowa dla wszystkich wierzchołków.

    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniView = glGetUniformLocation(shaderProgram, "view");
    GLint uniProj = glGetUniformLocation(shaderProgram, "proj");


    glUniformMatrix4fv(uniProj, 1, GL_FALSE, value_ptr(proj));


    Clock clock;
    bool running = true;


    int frameCount = 0;
    float fpsTimer = 0.0f;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// główna pętla gry

    while (running) {
        Event windowEvent;

        float deltaTime = clock.restart().asSeconds();
        float cameraSpeed = 2.5f * deltaTime;

        while (window.pollEvent(windowEvent)) {
            if (windowEvent.type == Event::Closed) {
                running = false;
            }
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Obsługa ruchu myszy
            if (windowEvent.type == Event::MouseMoved) {
                sf::Vector2i localPosition = sf::Mouse::getPosition(window);

                double xoffset = localPosition.x - lastX;
                double yoffset = localPosition.y - lastY;
                lastX = localPosition.x;
                lastY = localPosition.y;

                xoffset *= sensitivity;
                yoffset *= sensitivity;

                yaw += xoffset;
                pitch -= yoffset;

                if (pitch > 89.0f) pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;

                vec3 front;
                front.x = cos(radians(yaw)) * cos(radians(pitch));
                front.y = sin(radians(pitch));
                front.z = sin(radians(yaw)) * cos(radians(pitch));
                cameraFront = normalize(front);
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// obsługa klawiatury
        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) {
            cameraPos += cameraSpeed * cameraFront;
        }
        if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) {
            cameraPos -= cameraSpeed * cameraFront;
        }

        if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) {
            cameraPos -= normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) {
            cameraPos += normalize(cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// akltualizacja macierzy widoku
        mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(uniView, 1, GL_FALSE, value_ptr(view));

        glUniformMatrix4fv(uniModel, 1, GL_FALSE, value_ptr(model));

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// renderowanie

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Ustawienie tekstury jako bieżącej przed rysowaniem

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        window.display();

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Licznik FPS
        frameCount++;
        fpsTimer += deltaTime;

        if (fpsTimer >= 1.0f) {
            string title = "OpenGL 3D Camera - FPS: " + to_string(frameCount);
            window.setTitle(title);
            frameCount = 0;
            fpsTimer = 0.0f;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Czyszczenie zasobów
    glDeleteTextures(1, &texture1);
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    window.close();

    return 0;
}