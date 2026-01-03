// 01-basic-window.c
// A simple example demonstrating a basic window with microui

#define MICROUI_IMPLEMENTATION
#include <microui.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static mu_Context ctx;
static char logbuf[64000];
static int logbuf_updated = 0;

const char *vertex_shader_src = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "out vec4 vertexColor;\n"
    "uniform mat4 projection;\n"
    "void main() {\n"
    "    gl_Position = projection * vec4(aPos, 0.0, 1.0);\n"
    "    vertexColor = aColor;\n"
    "}";

const char *fragment_shader_src = "#version 330 core\n"
    "in vec4 vertexColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vertexColor;\n"
    "}";

unsigned int shader_program;
unsigned int vao, vbo;

int text_width(mu_Font font, const char *text, int len) {
    // Replace with actual text width calculation logic
    return len * 8; // Assume each character is 8 pixels wide
}

int text_height(mu_Font font) {
    // Replace with actual text height calculation logic
    return 16; // Assume text height is 16 pixels
}

void setup_opengl() {
    // Compile shaders
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Set up vertex data and buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void render_rect(float x, float y, float w, float h, mu_Color color) {
    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;
    float a = color.a / 255.0f;

    float vertices[] = {
        x,     y,     r, g, b, a,
        x + w, y,     r, g, b, a,
        x + w, y + h, r, g, b, a,
        x,     y,     r, g, b, a,
        x + w, y + h, r, g, b, a,
        x,     y + h, r, g, b, a
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glUseProgram(shader_program);
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render(mu_Context *ctx) {
    glUseProgram(shader_program);

    // Set up orthographic projection
    float projection[16] = {
        2.0f / WINDOW_WIDTH, 0.0f, 0.0f, 0.0f,
        0.0f, -2.0f / WINDOW_HEIGHT, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f
    };
    int proj_loc = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection);

    mu_Command *cmd = NULL;
    while (mu_next_command(ctx, &cmd)) {
        switch (cmd->type) {
            case MU_COMMAND_RECT: {
                render_rect(cmd->rect.rect.x, cmd->rect.rect.y, cmd->rect.rect.w, cmd->rect.rect.h, cmd->rect.color);
                break;
            }
            default: break;
        }
    }
}

void setup_ui(mu_Context *ctx) {
    if (mu_begin_window(ctx, "Basic Window", mu_rect(50, 50, 300, 200))) {
        mu_layout_row(ctx, 2, (int[]) { 80, -1 }, 0);
        mu_label(ctx, "Label:");
        if (mu_button(ctx, "Click Me")) {
            printf("Button clicked!\n");
        }
        mu_end_window(ctx);
    }
}

// Added input handling for mouse and keyboard
void handle_mouse_input(GLFWwindow *window, mu_Context *ctx) {
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    mu_input_mousemove(ctx, (int)mouse_x, (int)mouse_y);

    int mouse_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    mu_input_mousedown(ctx, (int)mouse_x, (int)mouse_y, mouse_pressed);
    mu_input_mouseup(ctx, (int)mouse_x, (int)mouse_y, !mouse_pressed);
}

void handle_keyboard_input(GLFWwindow *window, mu_Context *ctx) {
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
        mu_input_keydown(ctx, MU_KEY_BACKSPACE);
    } else {
        mu_input_keyup(ctx, MU_KEY_BACKSPACE);
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) { // Corrected key
        mu_input_keydown(ctx, MU_KEY_RETURN);
    } else {
        mu_input_keyup(ctx, MU_KEY_RETURN);
    }

    // Add more keys as needed
}

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Set OpenGL version to 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "microui - Basic Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    mu_init(&ctx);

    // Assign text measurement functions
    ctx.text_width = text_width;
    ctx.text_height = text_height;

    setup_opengl();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Set background color
        glClear(GL_COLOR_BUFFER_BIT);

        handle_mouse_input(window, &ctx);
        handle_keyboard_input(window, &ctx);

        mu_begin(&ctx);
        setup_ui(&ctx);
        mu_end(&ctx);

        render(&ctx);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
