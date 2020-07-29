// todo: capture screeenshot every n seconds

#include <iostream>
#include <unistd.h>

#include "renderers/PositioningRenderer.h"

static bool lbutton_down = false;
static bool rbutton_down = false;
static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    (void) mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if(GLFW_PRESS == action) {
            lbutton_down = true;
        }
        else if(GLFW_RELEASE == action) {
            lbutton_down = false;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if(GLFW_PRESS == action) {
            rbutton_down = true;
        }
        else if(GLFW_RELEASE == action) {
            rbutton_down = false;
        }
    }
}

static double scrollX = 0;
static double scrollY = 0;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void) window;
    scrollX = xoffset;
    scrollY = yoffset;
}

static int keyPressed = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void) window;
    (void) scancode;
    (void) mods;

    if (action == GLFW_PRESS) {
        keyPressed = key;
    }
    if (action == GLFW_RELEASE) {
        keyPressed = 0;
    }
}


int main(int argc, char** argv)
{
    {
        const char * cwd = getcwd(nullptr, 0);
        std::cout << cwd << std::endl;
        delete [] cwd;
    }

    if (!argv[1]) {
        throw std::runtime_error("No model path specified (call as: <executable> <path to model> <model file name, e.g. model.obj>)");
    }

    if (!argv[2]) {
        throw std::runtime_error("No model file specified (call as: <executable> <path to model> <model file name, e.g. model.obj>)");
    }

    std::string modelPath = argv[1];
    std::string modelFile = argv[2];

    //init GLFW
    glfwInit();

    //init window
    const int VIEWPORT_WIDTH  = 1280;
    const int VIEWPORT_HEIGHT = 720;
    const float aspect_ratio = (float) VIEWPORT_WIDTH / (float) VIEWPORT_HEIGHT;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(VIEWPORT_WIDTH, VIEWPORT_HEIGHT, "modelrecorder", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);

    //init GLAD to dynamically find openGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    //enable depth test
    glEnable(GL_DEPTH_TEST);

    Model model = loadModelByFileName(modelPath, modelFile);
    Camera camera(0.3);
    Trackball trackball(0.3);
    std::shared_ptr<Scene> scenePtr = std::make_shared<Scene>(glm::vec3(0, 0, 1), model);

    //init view renderer
    RenderOptions pRenderOptions = {
            {
                    0,
                    0,
                    VIEWPORT_WIDTH,
                    VIEWPORT_HEIGHT
            },

            GL_DEPTH_BUFFER_BIT
    };
    PositioningRenderer pRenderer(scenePtr, pRenderOptions);

    const glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.001f, 100.f);

    double prevTime = glfwGetTime();
    int framesRendered = 0;
    //render loop
    while(!glfwWindowShouldClose(window))
    {
        //measure perf
        double currTime = glfwGetTime();
        ++framesRendered;
        if (currTime - prevTime >= 1.0) {
            std::cout << "ms per frame: " << (currTime - prevTime) / framesRendered * 1000 << std::endl;
            prevTime = currTime;
            framesRendered = 0;
        }

        //handle user input
        if(lbutton_down) {
            double x;
            double y;
            glfwGetCursorPos(window, &x, &y);
            x -= (double) VIEWPORT_WIDTH / 2;
            y -= (double) VIEWPORT_HEIGHT / 2;
            camera.update((float) x, (float) y);
        } else {
            camera.stopMotion();
        }

        if(rbutton_down) {
            double x;
            double y;
            glfwGetCursorPos(window, &x, &y);
            x -= (double) VIEWPORT_WIDTH / 2;
            y -= (double) VIEWPORT_HEIGHT / 2;
            trackball.update((float) x, (float) y, camera.getTransformMatrix(), projection_matrix);
        } else {
            trackball.stopMotion();
        }

        if (keyPressed) {
            float tr = 0.00025f;
            if (keyPressed == GLFW_KEY_R) {
                std::cout << "saved " << modelPath + "/screen.png" << std::endl;d
                saveImage(modelPath + "/screen.png", pRenderer.getFrontBuffer().data(), VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 3);
                keyPressed = 0;
            }
            if (keyPressed == GLFW_KEY_W) {
                camera.translate(0, 0, -tr);
            }
            if (keyPressed == GLFW_KEY_S) {
                camera.translate(0, 0, tr);
            }
            if (keyPressed == GLFW_KEY_A) {
                camera.translate(-tr, 0, 0);
            }
            if (keyPressed == GLFW_KEY_D) {
                camera.translate(tr, 0, 0);
            }
            if (keyPressed == GLFW_KEY_E) {
                camera.translate(0, tr, 0);
            }
            if (keyPressed == GLFW_KEY_Q) {
                camera.translate(0, -tr, 0);
            }
            if (keyPressed == GLFW_KEY_0) {
                camera.reset();
                trackball.reset();
            }
        }

        if (scrollY != 0) {
            float t = (float) scrollY * 0.05f;
            camera.translate(0, 0, t);
            scrollY = 0;
        }

        //render
        glClear(GL_COLOR_BUFFER_BIT);
        scenePtr->model.matrix = trackball.getTransformMatrix();
        glm::mat4 view_matrix = camera.getTransformMatrix();
        VPHandler pVPHandler(view_matrix, projection_matrix);
        pRenderer.render(pVPHandler);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //free resources
    glfwTerminate();

    return 0;
}
