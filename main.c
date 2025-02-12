#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLint WIDTH = 800;
const GLint HEIGHT = 600;

GLuint VAO;
GLuint VBO;
GLuint shader;

static const char *vertexShader = "\
#version 330\n\
\n\
layout(location = 0) in vec3 pos;\n\
\n\
void main()\n\
{\n\
    gl_Position = vec4(0.5 * pos, 1.0f);\n\
}\n\
";

static const char *fragmentShader = "\
#version 330\n\
\n\
out vec4 color;\n\
\n\
void main()\n\
{\n\
    color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n\
}\n\
";

void AddShader(GLuint program, const char *shaderCode, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    const GLchar *code[1]; // = shaderCode;
    code[0] = shaderCode;

    GLint length[1];
    length[0] = strlen(shaderCode);

    glShaderSource(shader, 1, code, length);
    glCompileShader(shader);

    GLint result = 0;
    GLchar errorBuffer[1024] = {0};

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(shader, sizeof(errorBuffer), NULL, errorBuffer);
        printf("compiling shader %d failed with error: %s\n", shaderType,
               errorBuffer);
        return; // TODO, handle error
    }

    glAttachShader(program, shader);
}

void CompileShaders()
{
    shader = glCreateProgram();

    if (!shader)
    {
        printf("glCreateProgram failed\n");
        return; // TODO, handle error
    }

    AddShader(shader, vertexShader, GL_VERTEX_SHADER);
    AddShader(shader, fragmentShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar errorBuffer[1024] = {0};

    // create executables on the GPU
    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(errorBuffer), NULL, errorBuffer);
        printf("linking program failed with error: %s\n", errorBuffer);
        return; // TODO, handle error
    }

    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shader, sizeof(errorBuffer), NULL, errorBuffer);
        printf("validating program failed with error: %s\n", errorBuffer);
        return; // TODO, handle error
    }
}

void CreateTriangle()
{
    // triangle, 3 vertices
    GLfloat vertices[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f,
                          0.0f,  0.0f,  1.0f, 0.0f};

    // what data is
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // actual data
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3 floats for each vertex (x,y,z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // unbind stuff
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main()
{
    if (!glfwInit())
    {
        printf("glfwInit failed\n");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // no backwards compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *mainWindow =
        glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);

    if (!mainWindow)
    {
        printf("glfwCreateWindow failed\n");
        glfwTerminate();
        return 1;
    }

    // space inside the window
    int bufferWidth;
    int bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // set context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("glewInit failed\n");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // setup viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    CreateTriangle();
    CompileShaders();

    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // each pixel has more data than color (for example depth)
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}
