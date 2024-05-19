/** =========================
 * ID: 3019420
 * REF: 20729960
 * NAME: ESMOND ADJEI
 * =========================
 */
#include <iostream>
#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ========== SHADER SOURCES ==========
//  Vertex shader source code
const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
)";

// Fragment shader source code
const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);
}
)";

// ========== VERTEX DATA ==========
float OBJECT_SIZE = 0.2f;

GLfloat vertices[] = {
    // positions                        // colors           // texture coords
    -OBJECT_SIZE, -OBJECT_SIZE, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
     OBJECT_SIZE, -OBJECT_SIZE, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
     OBJECT_SIZE, OBJECT_SIZE, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top-right
    -OBJECT_SIZE, OBJECT_SIZE, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top-left
};

GLuint indices[] = {
    0, 1, 2, // First triangle (bottom-right)
    0, 2, 3  // Second triangle (top-left)
};

void onQuit(GLFWwindow *window);
void processInput(GLFWwindow *window, glm::vec2 &velocity, bool &isPaused);

int main()
{
    // ========== GLFW INITIALIZATION ==========
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ========== WINDOW CREATION ==========
    GLFWwindow *window = glfwCreateWindow(600, 400, "bouncer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Set viewport size
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                   { glViewport(0, 0, width, height); });

    // ========== GLAD INITIALIZATION ==========
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ========== VAO, VBO, EBO SETUP ==========
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind buffers
    glBindVertexArray(VAO);
    // Copy vertex data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Copy index data to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // Position attribute

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // Color attribute

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2); // Texture coordinate attribute

    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // ========== SHADER COMPILATION ==========
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // ========== TEXTURE ==========
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    // !!! have to use abosulte path here, if relavite path doesn't work.
    unsigned char *data = stbi_load("./textures/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // ========== SHADER PROGRAM LINKING ==========
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // ========== TRANSFORMATION MATRIX INITIALIZATION ==========
    glm::mat4 transform = glm::mat4(1.0f);
    glm::vec2 velocity(0.00015f, 0.00010f);
    bool isPaused = false;

    // ========== RENDER LOOP ==========
    while (!glfwWindowShouldClose(window))
    {
        onQuit(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // process input should only be called when object is not at the edge
        if (transform[3][0] + OBJECT_SIZE < 1.0f && transform[3][0] - OBJECT_SIZE > -1.0f &&
            transform[3][1] + OBJECT_SIZE < 1.0f && transform[3][1] - OBJECT_SIZE > -1.0f)
        {
            processInput(window, velocity, isPaused);
        }

        // Update position only if not paused
        if (!isPaused)
        {
            transform = glm::translate(transform, glm::vec3(velocity, 0.0f));

            // Check boundaries and handle bouncing
            if (transform[3][0] + OBJECT_SIZE > 1.0f || transform[3][0] - OBJECT_SIZE < -1.0f)
            {
                velocity.x *= -1;
                std::cout << "Impact velocity at X-Axis: (" << velocity.x << ", " << velocity.y << ")" << std::endl;
            }
            if (transform[3][1] + OBJECT_SIZE > 1.0f || transform[3][1] - OBJECT_SIZE < -1.0f)
            {
                velocity.y *= -1;
                std::cout << "Impact velocity at Y-Axis: (" << velocity.x << ", " << velocity.y << ")" << std::endl;
            }
        }

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(transform));

        // Draw the square
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ========== CLEANUP ==========
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}

// ========== AUXILIARY FUNCTION ==========
// press escape to quit
void onQuit(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// control the square with arrow keys
void processInput(GLFWwindow *window, glm::vec2 &velocity, bool &isPaused)
{
    const float acceleration = 0.00005f;
    const float maxSpeed = 0.00015f;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        velocity.x -= acceleration;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        velocity.x += acceleration;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        velocity.y -= acceleration;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        velocity.y += acceleration; 
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        isPaused = !isPaused;
        while (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        { glfwPollEvents(); }
    }

    // Limit velocity to maximum speed
    velocity.x = glm::clamp(velocity.x, -maxSpeed, maxSpeed);
    velocity.y = glm::clamp(velocity.y, -maxSpeed, maxSpeed);
}
