#include "app.h"
#include "scripting.h"
#include "sound.h"
#include "model.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    App app;

    init_app(&app);
    init_model_cache();
    init_sound();
    init_scripting(&(app.camera), &(app.scene));

    while (app.is_running) {
        handle_app_events(&app);
        update_app(&app);
        render_app(&app);
    }

    close_scripting();
    close_sound();
    free_model_cache();
    destroy_app(&app);

    return 0;
}
