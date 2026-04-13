#include "app.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    App app;
    init_app(&app);

    while (app.is_running) {
        handle_app_events(&app);
        update_app(&app);
        render_app(&app);
    }

    destroy_app(&app);

    return 0;
}
