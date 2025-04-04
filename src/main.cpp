#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "mesh.hpp"
#include "bvh.hpp"
#include "shader.hpp"

unsigned int WINDOW_WIDTH = 1280;
unsigned int WINDOW_HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;
bool rmbPressed = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* window;
static Camera camera{};

void glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
}

void GLAPIENTRY openglDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::cerr << "OpenGL Debug Message (" << id << "): " << message << std::endl;
    std::cerr << "Source: " << source << ", Type: " << type << ", Severity: " << severity << std::endl;
}

void ProcessCameraMovement(const float deltatime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Move(FORWARD, deltatime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Move(BACKWARD, deltatime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Move(LEFT, deltatime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Move(RIGHT, deltatime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.Move(UP, deltatime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.Move(DOWN, deltatime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;

	camera.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	camera.UpdateProjectionMatrix();

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.MouseCallback(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double cursor_x, cursor_y;
	glfwGetCursorPos(window, &cursor_x, &cursor_y);
	camera.MouseButtonCallback(cursor_x, cursor_y, button, action, mods);

}

int main() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BVH Visualizer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglDebugCallback, nullptr);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
        throw std::runtime_error("ImGui_ImplGlfw_InitForOpenGL() failed.");

    if (!ImGui_ImplOpenGL3_Init())
        throw std::runtime_error("ImGui_ImplOpenGL3_Init() failed.");

    camera.SetOrigin({ 0.0f, 0.0f, 10.0f });
	camera.SetVerticalFOV(45.0f);
	camera.SetAspectRatio(static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT));
	camera.SetNearPlane(0.1f);
	camera.SetFarPlane(1000.0f);
	camera.UpdateProjectionMatrix();

    int depth = 4;

    Mesh mesh("assets/dragon.obj");
    BVH bvh(mesh, depth);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessCameraMovement(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::mat4 viewProj = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        mesh.draw(viewProj, camera.GetOrigin());
        bvh.draw(viewProj);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Text("# triangles = %zu", bvh.triangles.size());
        ImGui::Text("Max depth %d", bvh.maxDepth);
        ImGui::Text("# of nodes %zu", bvh.nodes.size());
        ImGui::Text("# of leaves = %d", bvh.numInstances);
        if (ImGui::SliderInt("Depth", &depth, 1, 16)) {
            bvh.rebuild(depth);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}