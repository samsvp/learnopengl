#include <iostream>
#include <cmath>

#include "../include/glad/glad.h" 
#include "../include/shader.hpp"
#include "../include/compute_shader.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


struct randomf
{
    std::random_device dev;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

    randomf() : rng(dev()), dist(0.0, 1.0) { }
    float gen() { return dist(rng); }
} randf;


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    // glEnable(GL_MULTISAMPLE);  

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); // enabled by default on some drivers, but not all so always enable to make sure

    // build and compile our shader zprogram
    // ------------------------------------
    ComputeShader particleComputeShader("shaders/particle.comp");
    Shader particleShader("shaders/particle.vert", "shaders/particle.frag");

    particleShader.use();
    glm::mat4 model = glm::mat4(1.0f);
    particleShader.set_mat4("model", model);

    int numberOfParticles = 2048;
    float particles[numberOfParticles* 2];
    glPointSize(4.0f);

    for(int i = 0; i < numberOfParticles; i++) {
        particles[i * 2] = (randf.gen() - 0.5f) * 1.0f;
        particles[i * 2 + 1] = -1.0f;
    }
    unsigned int PARTICLE_VAO, PARTICLE_VBO;
    glGenVertexArrays(1, &PARTICLE_VAO);
    glGenBuffers(1, &PARTICLE_VBO);

    glBindVertexArray(PARTICLE_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, PARTICLE_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particles), particles, GL_DYNAMIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, PARTICLE_VBO);

    particleComputeShader.use();
    for (size_t i = 0; i < numberOfParticles; i++)
    {
        particleComputeShader.set_vec2("initialPos["+ std::to_string(i) + "]", 
            particles[2 * i], particles[2 * i + 1]);
    }

    // timing 
    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f; // time of last frame
    int fCounter = 0;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // timing
        // Set frame time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if(fCounter > 500) {
                std::cout << "FPS: " << 1 / deltaTime << std::endl;
                fCounter = 0;
        } else {
            fCounter++;
        }	
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // activate shader
        particleComputeShader.use();
        particleComputeShader.set_float("t", 0.005 * glm::sin(0.005f * currentFrame));
        glBindVertexArray(PARTICLE_VAO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, PARTICLE_VBO);
        glDispatchCompute(numberOfParticles / 1024, 1, 1);

        glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        particleShader.use();
        particleShader.set_vec4("u_color", 0.0f, 0.5f, 1.0f, 1.0f);
        glDrawArrays(GL_POINTS, 0, numberOfParticles);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &PARTICLE_VAO);
    glDeleteBuffers(1, &PARTICLE_VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}