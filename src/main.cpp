#include <OpenGLPrj.hpp>

#include <GLFW/glfw3.h>

#include <Camera.hpp>
#include <Shader.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <cmath>


#define M_PI 3.1415926535897932384626433832795
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


const std::string program_name = ("Rick's Icy-Hot Adventure");

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// Keyboard Input 
void processInput(GLFWwindow* window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int loadTexture(char const*);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
static Camera camera(glm::vec3(0.0f, 0.0f, 15.0f));
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;

static bool firstMouse = true;

// timing
static float deltaTime = 0.0f;
static float lastFrame = 0.0f;

// world speed
// Recommended multiplyer settings
// 2.0f for 60Hz screen
// 1.0f for 144-165Hz screen 
// 0.5f for 240Hz screen
static float multiplyer = 1.0f;

// lighting
static glm::vec3 lightPos( 0.0f, 15.0f, 15.0f);

//mouse
static bool canLookAround = false;

//Block status
static char currentState = 'L';
static char lastState = 'L';

//Player
static float playerScale = 0.75f;
static glm::vec3 move = glm::vec3(-0.125f, -0.125f, 0.0f);    // Displacement in relation to starting position
static bool isGrounded = true;
static float xMovement = 0.0f, yMovement = 0.0f;
bool collision = false;



//Player Movement
static float velocity = 0.0625f * multiplyer;
static float xStride = 1.0f * multiplyer;
static float yJump = 1.75f * multiplyer;
static float gravity = 0.0625f * multiplyer;




int main() {

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  // glfw window creation
  // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
        program_name.c_str(), nullptr, nullptr);

    glfwSetKeyCallback(window, keyCallback);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    std::string shader_location("../res/shaders/");
    std::string texture_location("../res/textures/");

    std::string material_shader("material");
    std::string lamp_shader("lamp");

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader(
        shader_location + material_shader + std::string(".vert"),
        shader_location + material_shader + std::string(".frag"));
    Shader lampShader(shader_location + lamp_shader + std::string(".vert"),
        shader_location + lamp_shader + std::string(".frag"));

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // 
    //Cube Vertices
    float vertices[] = {
        // positions          // normals           // texture coords

        //REAR
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,
        0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,
        0.5f,  0.5f,  -0.5f, 0.0f, 0.0f,  -1.0f, 1.0f,  1.0f,
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

        //FRONT
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f,  1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        //LEFT
        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f, 0.0f,  1.0f,
        -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

        //RIGHT
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        //BOTTOM
        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f, 
        0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f, 
        0.5f,  -0.5f, 0.5f,  0.0f, -1.0f, 0.0f,  1.0f,  0.0f,  
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f, 1.0f,  0.0f, 
        -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,
        
        //TOP
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f,  0.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f

         };

    //Obstacle positions
    // 0 	  -> Player
    // 1-70   -> Stone
    // 71-73  -> Finish
    // 74-91  -> Lava
    // 92-109 -> Ice
    // 110-111 -> Background
    glm::vec3 cubePositions[] = {

        // Player
        glm::vec3(-10.0f, -5.0f, 0.0f),

        //Bottom                         //Top
        glm::vec3(-11.0f, -6.0f, 0.0f),  glm::vec3(-11.0f,  6.0f, 0.0f),
        glm::vec3(-10.0f, -6.0f, 0.0f),  glm::vec3(-10.0f,  6.0f, 0.0f),
        glm::vec3( -9.0f, -6.0f, 0.0f),  glm::vec3( -9.0f,  6.0f, 0.0f),
        glm::vec3( -8.0f, -6.0f, 0.0f),  glm::vec3( -8.0f,  6.0f, 0.0f),
        glm::vec3( -7.0f, -6.0f, 0.0f),  glm::vec3( -7.0f,  6.0f, 0.0f),
        glm::vec3( -6.0f, -6.0f, 0.0f),  glm::vec3( -6.0f,  6.0f, 0.0f),
        glm::vec3( -5.0f, -6.0f, 0.0f),  glm::vec3( -5.0f,  6.0f, 0.0f),
        glm::vec3( -4.0f, -6.0f, 0.0f),  glm::vec3( -4.0f,  6.0f, 0.0f),
        glm::vec3( -3.0f, -6.0f, 0.0f),  glm::vec3( -3.0f,  6.0f, 0.0f),
        glm::vec3( -2.0f, -6.0f, 0.0f),  glm::vec3( -2.0f,  6.0f, 0.0f),
        glm::vec3( -1.0f, -6.0f, 0.0f),  glm::vec3( -1.0f,  6.0f, 0.0f),
        glm::vec3(  0.0f, -6.0f, 0.0f),  glm::vec3(  0.0f,  6.0f, 0.0f),
        glm::vec3(  1.0f, -6.0f, 0.0f),  glm::vec3(  1.0f,  6.0f, 0.0f),
        glm::vec3(  2.0f, -6.0f, 0.0f),  glm::vec3(  2.0f,  6.0f, 0.0f),
        glm::vec3(  3.0f, -6.0f, 0.0f),  glm::vec3(  3.0f,  6.0f, 0.0f),
        glm::vec3(  4.0f, -6.0f, 0.0f),  glm::vec3(  4.0f,  6.0f, 0.0f),
        glm::vec3(  5.0f, -6.0f, 0.0f),  glm::vec3(  5.0f,  6.0f, 0.0f),
        glm::vec3(  6.0f, -6.0f, 0.0f),  glm::vec3(  6.0f,  6.0f, 0.0f),
        glm::vec3(  7.0f, -6.0f, 0.0f),  glm::vec3(  7.0f,  6.0f, 0.0f),
        glm::vec3(  8.0f, -6.0f, 0.0f),  glm::vec3(  8.0f,  6.0f, 0.0f),
        glm::vec3(  8.0f, -6.0f, 0.0f),  glm::vec3(  8.0f,  6.0f, 0.0f),
        glm::vec3(  9.0f, -6.0f, 0.0f),  glm::vec3(  9.0f,  6.0f, 0.0f),
        glm::vec3( 10.0f, -6.0f, 0.0f),  glm::vec3( 10.0f,  6.0f, 0.0f),
        glm::vec3( 11.0f, -6.0f, 0.0f),  glm::vec3( 11.0f,  6.0f, 0.0f),

        //Left                           //Right
        glm::vec3(-11.0f, -5.0f, 0.0f),  glm::vec3(11.0f, -5.0f, 0.0f),
        glm::vec3(-11.0f, -4.0f, 0.0f),  glm::vec3(11.0f, -4.0f, 0.0f),
        glm::vec3(-11.0f, -3.0f, 0.0f),  glm::vec3(11.0f, -3.0f, 0.0f),
        glm::vec3(-11.0f, -2.0f, 0.0f),  glm::vec3(11.0f, -2.0f, 0.0f),
        glm::vec3(-11.0f, -1.0f, 0.0f),  glm::vec3(11.0f, -1.0f, 0.0f),
        glm::vec3(-11.0f,  0.0f, 0.0f),  glm::vec3(11.0f,  0.0f, 0.0f),
        glm::vec3(-11.0f,  1.0f, 0.0f),  glm::vec3(11.0f,  1.0f, 0.0f),
        glm::vec3(-11.0f,  2.0f, 0.0f),  glm::vec3(11.0f,  2.0f, 0.0f),
        glm::vec3(-11.0f,  3.0f, 0.0f),  glm::vec3(11.0f,  3.0f, 0.0f),
        glm::vec3(-11.0f,  4.0f, 0.0f),  glm::vec3(11.0f,  4.0f, 0.0f),
        glm::vec3(-11.0f,  5.0f, 0.0f),  glm::vec3(11.0f,  5.0f, 0.0f),

        //Finish
        glm::vec3(  8.0f,  4.0f, 0.0f),
        glm::vec3(  9.0f,  4.0f, 0.0f),
        glm::vec3( 10.0f,  4.0f, 0.0f),


        //Lava
        glm::vec3( -8.0f,  2.0f, 0.0f),  
        glm::vec3( -7.0f,  3.0f, 0.0f),  
        glm::vec3( -6.0f,  3.0f, 0.0f),  
        glm::vec3( -5.0f,  3.0f, 0.0f),  
        glm::vec3( -7.0f,  0.0f, 0.0f),  
        glm::vec3( -6.0f, -1.0f, 0.0f),  
        glm::vec3( -5.0f, -1.0f, 0.0f),  
        glm::vec3(  0.0f,  2.0f, 0.0f),  
        glm::vec3(  1.0f,  2.0f, 0.0f),  
        glm::vec3(  2.0f,  2.0f, 0.0f),  
        glm::vec3(  1.0f, -1.0f, 0.0f),  
        glm::vec3(  2.0f, -1.0f, 0.0f),  
        glm::vec3(  4.0f,  2.0f, 0.0f),  
        glm::vec3(  7.0f,  3.0f, 0.0f),  
        glm::vec3(  7.0f,  0.0f, 0.0f),  
        glm::vec3(  8.0f,  0.0f, 0.0f),  
        glm::vec3(  9.0f, -2.0f, 0.0f),  
        glm::vec3(  7.0f, -4.0f, 0.0f),

        //Ice
        glm::vec3(-10.0f,  1.0f, 0.0f),
        glm::vec3(-9.0f,  0.0f, 0.0f),
        glm::vec3( -4.0f,  2.0f, 0.0f),
        glm::vec3( -3.0f,  1.0f, 0.0f),
        glm::vec3( -2.0f,  1.0f, 0.0f),
        glm::vec3( -1.0f,  1.0f, 0.0f),
        glm::vec3(  5.0f,  2.0f, 0.0f),
        glm::vec3(  6.0f,  2.0f, 0.0f),
        glm::vec3(  7.0f,  2.0f, 0.0f),
        glm::vec3( -4.0f, -2.0f, 0.0f),
        glm::vec3(  0.0f, -2.0f, 0.0f),
        glm::vec3( -3.0f, -3.0f, 0.0f),
        glm::vec3( -2.0f, -3.0f, 0.0f),
        glm::vec3( -1.0f, -3.0f, 0.0f),
        glm::vec3(  4.0f, -1.0f, 0.0f),
        glm::vec3( 10.0f, -1.0f, 0.0f),
        glm::vec3(  8.0f, -3.0f, 0.0f),
        glm::vec3(  6.0f, -5.0f, 0.0f),

        //Background
        glm::vec3(-10.0f,  0.0f, -22.0f),
        glm::vec3( 10.0f,  0.0f, -22.0f),

      
    };

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the
    // same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the
    // updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load textures (we now use a utility function to keep the code more
    // organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap =
        loadTexture((texture_location + "rock.png").c_str());

    unsigned int diffuseMapF =
        loadTexture((texture_location + "chess4.png").c_str());

    unsigned int diffuseMap1 =
        loadTexture((texture_location + "lava.png").c_str());

    unsigned int diffuseMap2 =
        loadTexture((texture_location + "ice.png").c_str());

    unsigned int rick1 =
        loadTexture((texture_location + "rick1.png").c_str());

    unsigned int rick2 =
        loadTexture((texture_location + "rick2.png").c_str());

    unsigned int rick3 =
        loadTexture((texture_location + "rick3.png").c_str());

    unsigned int rick4 =
        loadTexture((texture_location + "rick4.png").c_str());

    unsigned int bg1 =
        loadTexture((texture_location + "cave_bg.png").c_str());
    unsigned int bg2 =
        loadTexture((texture_location + "cave_bg2.png").c_str());

    unsigned int specularMap = loadTexture((texture_location + "molten_glass.jpg").c_str());

    unsigned int emmisionMap = loadTexture((texture_location + "black.png").c_str());

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);
    lightingShader.setInt("material.emission", 2);

    // render loop
    // -----------

    static int count = 0;

    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // light properties
        lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("light.diffuse",  0.75f,0.75f,0.75f);
        lightingShader.setVec3("light.specular", 0.5f, 0.5f, 0.5f);

        // material properties
        //lightingShader.setVec3("material.specular", 0.75f, 0.75f, 0.75f);
        lightingShader.setFloat("material.shininess", 64.0f);

        // view/projection transformations
        glm::mat4 projection =
            glm::perspective(glm::radians(camera.Zoom),
                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);

        lightingShader.setFloat("time", currentFrame);

        
        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emmisionMap);

        // render the frame of cubes 
        glBindVertexArray(cubeVAO);

        if (currentState == 'L')
            lightingShader.setVec3("light.diffuse", (sin(currentFrame * 2.5f) + 2) / 4, (sin(currentFrame * 2.5f) + 2) / 4, (sin(currentFrame * 2.5f) + 2) / 4);
        if (currentState == 'I')
            lightingShader.setVec3("light.diffuse", 0.75f,0.75f,0.75f);

        for (unsigned int i = 0; i < 112; i++) {
            // calculate the model matrix for each object and pass it to shader before drawing

            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            model = glm::scale(model, glm::vec3(1.0f));
            float scale = 1.0f;
            glActiveTexture(GL_TEXTURE0);


            // Player object i = 0
            if (i == 0) {
                //Texture
                if (count >= 0 && count < 20)
                    glBindTexture(GL_TEXTURE_2D, rick1);
                else if (count >= 20 && count < 40)
                    glBindTexture(GL_TEXTURE_2D, rick2);
                else if (count >= 40 && count < 60)
                    glBindTexture(GL_TEXTURE_2D, rick3);
                else if (count >= 60 && count < 80)
                    glBindTexture(GL_TEXTURE_2D, rick4);
                count++;
                if (count == 80)
                    count = 0;


                //Movement
                glm::vec3 tempMove = move;
                // x-axis
                glm::vec3 newMove = move + glm::vec3(xMovement * velocity, 0.0f, 0.0f);
                if (newMove.x <= -0.125f)
                    newMove.x = -0.125f;
                if (newMove.x >= 20.125f)
                    newMove.x = 20.125f;
                tempMove = newMove;

                // y-axis
                newMove = tempMove + glm::vec3(0.0f, yMovement * velocity, 0.0f);
                if (newMove.y <= -0.125f) {
                    newMove.y = -0.125f;
                    yMovement = 0.0f;
                    isGrounded = true;
                }
                if (newMove.y >= 10.125f)
                    newMove.y = 10.125f;
                tempMove = newMove;
                


                //Center of Player
                float xCenterPlayer = model[3][0] + tempMove.x;
                float yCenterPlayer = model[3][1] + tempMove.y;
                //Bounding box of Player
                float xMinPlayer = xCenterPlayer - playerScale / 2;
                float xMaxPlayer = xCenterPlayer + playerScale / 2;
                float yMinPlayer = yCenterPlayer - playerScale / 2;
                float yMaxPlayer = yCenterPlayer + playerScale / 2;

                float xCenterObstacle;
                float yCenterObstacle;
                float xMinObstacle;
                float xMaxObstacle;
                float yMinObstacle;
                float yMaxObstacle;

                int collisionBlockIndex = 0;

                // Collision Detection
                for (int i = 1; i < 110; i++)
                {
                    if (currentState == 'L' && i > 91)
                        continue;
                    if (currentState == 'I' && i > 73 && i < 92)
                        continue;

                    xCenterObstacle = cubePositions[i][0];
                    yCenterObstacle = cubePositions[i][1];
                    //Bounding box of Player
                    xMinObstacle = xCenterObstacle - 1.0f / 2;
                    xMaxObstacle = xCenterObstacle + 1.0f / 2;
                    yMinObstacle = yCenterObstacle - 1.0f / 2;
                    yMaxObstacle = yCenterObstacle + 1.0f / 2;

                    //AABB Collision Detection

                    if (xMinPlayer < xMaxObstacle) {
                        if (xMaxPlayer > xMinObstacle) {
                            if (yMinPlayer < yMaxObstacle) {
                                if (yMaxPlayer > yMinObstacle) {
                                    collisionBlockIndex = i;
                                    collision = true;
                                    break;
                                }
                                else
                                    collision = false;
                            }
                            else
                                collision = false;
                        }
                        else
                            collision = false;
                    }
                    else
                        collision = false;
                }

                float xOffset = 0.0f, yOffset = 0.0f;
                int xCollisionType = 0;
                int yCollisionType = 0;

                if (collision) {
                    // X axis collisions
                    // 
                    // x left collision
                    if (xMinPlayer < xMinObstacle && xMaxPlayer > xMinObstacle) {
                        xOffset = 1.75f / 2 - (xCenterObstacle - xCenterPlayer);
                        xOffset *= -1;
                        //std::cout << "Left " << xOffset << std::endl;
                        xCollisionType = 1;
                    }
                    // x right collision
                    else if (xMinPlayer < xMaxObstacle && xMaxPlayer > xMaxObstacle) {
                        xOffset = 1.75f / 2 - (xCenterPlayer - xCenterObstacle);
                        //std::cout << "Right" << xOffset << std::endl;
                        xCollisionType = 3;
                    }
                    //x middle collision1
                    else if (xMinPlayer >= xMinObstacle && xMaxPlayer <= xMaxObstacle) {
                        float distance = xCenterObstacle - xCenterPlayer;
                        xOffset = 1.75f / 2 - (xCenterObstacle - xCenterPlayer);
                        if (distance < 0)
                            xOffset *= -1;
                        //std::cout << "x Middle" << xOffset << std::endl;
                        xCollisionType = 2;
                    }
                    // pls no.
                    else
                        std::cout << "MovementErrorXaxis" << xOffset << std::endl;

                    //Y axis collisions 
                    // y top collision
                    if (yMinPlayer < yMinObstacle && yMaxPlayer > yMinObstacle) {
                        yOffset = 1.75f / 2 - (yCenterObstacle - yCenterPlayer);
                        yOffset *= -1;
                        //std::cout << "Top" << yOffset << std::endl;
                        yCollisionType = 1;
                    }
                    // y bottom collision - stand on platform
                    else if (yMinPlayer < yMaxObstacle && yMaxPlayer > yMaxObstacle) {
                        yOffset = 1.75f / 2 - (yCenterPlayer - yCenterObstacle);
                        //std::cout << "Bottom" << yOffset << std::endl;
                        yCollisionType = 3;
                    }
                    //y middle collision
                    else if (yMinPlayer >= yMinObstacle && yMaxPlayer <= yMaxObstacle) {
                        float distance = yCenterObstacle - yCenterPlayer;
                        yOffset = 1.75f / 2 - (yCenterObstacle - yCenterPlayer);
                        if (distance > 0)
                            yOffset *= -1;
                        //std::cout << "y Middle" << yOffset << std::endl;
                        yCollisionType = 2;
                    }
                    // pls no.
                    else
                        std::cout << "MovementErrorYaxis" << xOffset << std::endl;
                }
               
                //Displace by closest offset
                if (abs(xOffset) < abs(yOffset)) {
                    tempMove += glm::vec3(xOffset, 0.0f, 0.0f);
                    yCollisionType = 0;
                }
                else {
                    if (yOffset > 0) {
                        isGrounded = true;
                    }
                    if (tempMove.y < 0) {
                        tempMove += glm::vec3(xOffset, 0.0f, 0.0f);
                        yCollisionType = 0;
                    }
                    else {
                        tempMove += glm::vec3(0.0f, yOffset, 0.0f);
                        xCollisionType = 0;
                    }
                }



                std::cout << "Ground: " << isGrounded << "\t Collision: " << collision << "\t yMovement: " << yMovement << std::endl;

                if (tempMove.y <= -0.125f && !isGrounded) {
                    tempMove.y = -0.125f;
                    isGrounded = true;
                }

               

                move = tempMove;
                
                if (isGrounded && move.y == 0) {
                    yMovement = 0.0f;
                    std::cout << "y0 : Ground" << std::endl;
                }
                else if (collision && yCollisionType == 1) {
                    yMovement = 0.0f;
                    std::cout << "y0 : Head" << std::endl;
                }
                else if (collision && yCollisionType == 3 && yMovement < 0) {
                    yMovement = 0.0f;
                    std::cout << "y0 : Standing" << std::endl;
                }

                else if (collision && xCollisionType == 2) {
                    yMovement = 0.0f;
                    std::cout << "y0 : by X" << std::endl;
                }
                

                model = glm::translate(model, move);

                model = glm::scale(model, glm::vec3(playerScale));

                xMovement = 0.0f;
                yMovement -= gravity;
                collision = false;
            }

            if (i > 109)
                model = glm::scale(model, glm::vec3(20.0f));

            lightingShader.setMat4("model", model);

            //Select texutures
            if ( i<71 && i != 0) 
                glBindTexture(GL_TEXTURE_2D, diffuseMap); // Cobblestone
            else if ( i < 74 && i != 0)
                glBindTexture(GL_TEXTURE_2D, diffuseMapF); // Finish
            else if (currentState == 'L' && i != 0 && i < 109)
                glBindTexture(GL_TEXTURE_2D, diffuseMap1); // Lava
            else if (currentState == 'I' && i != 0 && i < 109)
                glBindTexture(GL_TEXTURE_2D, diffuseMap2); // Ice
            else if (i == 110)
                glBindTexture(GL_TEXTURE_2D, bg2); // Background
            else if (i == 111)
                glBindTexture(GL_TEXTURE_2D, bg1); 


            //Draw blocks
            if (i == 0)
                glDrawArrays(GL_TRIANGLES, 0, 36); // Draw Player
            if (i < 74 && i != 0)
                glDrawArrays(GL_TRIANGLES, 0, 36); // Draw frame and finish blocks
            if (currentState == 'L' && i < 74 + 18 && i != 0)
                glDrawArrays(GL_TRIANGLES, 0, 36); // Draw Lava Blocks
            if (currentState == 'I' && i >= 74 + 18 && i != 0)
                glDrawArrays(GL_TRIANGLES, 0, 36); // Draw Ice Blocks
            if (i > 109)    //Draw Background
                glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // draw the lamp object
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        auto rot_center = glm::mat4(1.0f);
        rot_center = glm::translate(rot_center, glm::vec3(1, 1, 1));

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(1.0f)); // a smaller cube
        lampShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

//Custom input processing
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_W) {
        switch (action) {
        case GLFW_PRESS:
            if (isGrounded || collision) {
                yMovement = 1.75f;
                std::cout << "JUMP!" << std::endl;
                isGrounded = false;
            }
            break;
        default:
            break;
        }
    }

    if (key == GLFW_KEY_R) {
        switch (action) {
        case GLFW_PRESS:
            move = glm::vec3(-0.125f, -0.125f, 0.0f);
            isGrounded = true;
            break;
        default:
            break;
        }
    }

    if (key == GLFW_KEY_SPACE) {
        switch (action) {
        case GLFW_PRESS:
            if (currentState == 'L')
                currentState = 'I';
            else
                currentState = 'L';
            break;
        default:
            break;
        }
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        xMovement = -xStride;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        xMovement = xStride;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (canLookAround) {
            glfwSetCursorPosCallback(window, nullptr);
        }
        else {
            glfwSetCursorPosCallback(window, mouse_callback);
        }
        canLookAround = !canLookAround;
    }
}



// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposd, double yposd) {
    float xpos = static_cast<float>(xposd);
    float ypos = static_cast<float>(yposd);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset =
        lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {

        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
            GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {

        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }



    return textureID;
}


