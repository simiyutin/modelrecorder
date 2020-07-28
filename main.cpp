// todo: rotate, navigate
// todo: capture screeenshot every n seconds
// todo: check valgrind

#include <iostream>
#include <unistd.h>

#include "renderers/PositioningRenderer.h"

static bool lbutton_down = false;
static bool lbutton_fresh = false;
static double prevX;
static double prevY;
static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    (void) mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if(GLFW_PRESS == action) {
            glfwGetCursorPos(window, &prevX, &prevY);
            lbutton_down = true;
            lbutton_fresh = true;
        }
        else if(GLFW_RELEASE == action) {
            lbutton_down = false;
            lbutton_fresh = false;
        }
    }
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
    glfwSetKeyCallback(window, key_callback);

    //enable depth test
    glEnable(GL_DEPTH_TEST);

    Model model = loadModelByFileName(modelPath, modelFile);
    Trackball trackball(0.005);
    Scene scene{{0, 0, 1}, model, trackball};
    std::shared_ptr<Scene> scenePtr = std::make_shared<Scene>(scene);

    //init view renderer
    float scale = 3;
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
            if (lbutton_fresh) {
                trackball.setPrev((float) x, (float) -y);
                lbutton_fresh = false;
            } else {
                trackball.update((float) x, (float) -y);
            }
        }

        if (keyPressed) {
            if (keyPressed == GLFW_KEY_E) {
                saveImage(modelPath + "/screen.png", pRenderer.getFrontBuffer().data(), VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 3);
            } else if (keyPressed == GLFW_KEY_UP) {
                scale *= 2;
            } else if (keyPressed == GLFW_KEY_DOWN) {
                scale /= 2;
            }
            keyPressed = 0;
        }

        //render
        glClear(GL_COLOR_BUFFER_BIT);
        glm::mat4 view_matrix = glm::mat4(1.0);
        glm::mat4 projection_matrix = glm::ortho(-1 / scale * aspect_ratio, 1 / scale * aspect_ratio, -1 / scale, 1 / scale, -1 / scale, 1 / scale);
        VPHandler pVPHandler(view_matrix, projection_matrix);
        pRenderer.render(pVPHandler);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //free resources
    glfwTerminate();

    return 0;
}
