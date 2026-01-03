// 03-popup-dialog.c
// Demonstrates a popup dialog with microui

#include <stdio.h>
#include <GLFW/glfw3.h>
#include <microui.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static mu_Context ctx;
static int show_popup = 0;

void render(mu_Context *ctx) {
    const mu_Command *cmd = NULL;
    while ((cmd = mu_next_command(ctx, cmd))) {
        switch (cmd->type) {
            case MU_COMMAND_TEXT: {
                printf("Text: %s\n", cmd->text.str);
                break;
            }
            case MU_COMMAND_RECT: {
                printf("Rect: x=%d, y=%d, w=%d, h=%d\n", cmd->rect.x, cmd->rect.y, cmd->rect.w, cmd->rect.h);
                break;
            }
            default: break;
        }
    }
}

void setup_ui(mu_Context *ctx) {
    if (mu_begin_window(ctx, "Popup Dialog", mu_rect(50, 50, 400, 300))) {
        mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
        mu_label(ctx, "Click the button to open a popup:");

        if (mu_button(ctx, "Open Popup")) {
            show_popup = 1;
        }

        if (show_popup) {
            mu_open_popup(ctx, "My Popup");
        }

        if (mu_begin_popup(ctx, "My Popup")) {
            mu_label(ctx, "This is a popup dialog!");
            if (mu_button(ctx, "Close")) {
                show_popup = 0;
            }
            mu_end_popup(ctx);
        }

        mu_end_window(ctx);
    }
}

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "microui - Popup Dialog", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    mu_init(&ctx);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

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
