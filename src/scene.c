#include "scene.h"
#include "model.h"

#include <GL/gl.h>

#include <stdio.h>

void init_scene(Scene* scene) {
    scene->material.ambient.red   = 0.5;
    scene->material.ambient.green = 0.5;
    scene->material.ambient.blue  = 0.5;

    scene->material.diffuse.red   = 0.8;
    scene->material.diffuse.green = 0.75;
    scene->material.diffuse.blue  = 0.65;

    scene->material.specular.red   = 0.3;
    scene->material.specular.green = 0.3;
    scene->material.specular.blue  = 0.3;

    scene->material.shininess = 50.0;
    scene->entity_count = 0;
}

void create_entity(Scene* scene, const char* modelname, float x, float y, float z, float rx, float ry, float rz) {
    if (scene->entity_count >= MAX_ENTITIES) {
        printf("Error: Max entities reached\n");
        return;
    }
    
    Model* model = get_model(modelname);
    if (model != NULL) {
        Entity* e = &(scene->entities[scene->entity_count]);
        e->model = model;
        e->x = x; e->y = y; e->z = z;
        e->rx = rx; e->ry = ry; e->rz = rz;
        scene->entity_count++;
    }
}

void render_scene(const Scene* scene) {
    GLfloat global_ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

    GLfloat sun_direction[] = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat sun_diffuse[] = {0.8f, 0.8f, 0.4f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);

    draw_floor();

    glMaterialfv(GL_FRONT, GL_AMBIENT, (float*)&(scene->material.ambient));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, (float*)&(scene->material.diffuse));
    glMaterialfv(GL_FRONT, GL_SPECULAR, (float*)&(scene->material.specular));
    glMaterialf(GL_FRONT, GL_SHININESS, scene->material.shininess);

    for (int i = 0; i < scene->entity_count; i++) {
        glPushMatrix();
        glTranslatef(scene->entities[i].x, scene->entities[i].y, scene->entities[i].z);
        glRotatef(scene->entities[i].rx, 1.0f, 0.0f, 0.0f);
        glRotatef(scene->entities[i].ry, 0.0f, 1.0f, 0.0f);
        glRotatef(scene->entities[i].rz, 0.0f, 0.0f, 1.0f);
        
        draw_model(scene->entities[i].model);
        glPopMatrix();
    }
}

void update_scene(Scene* scene) {
    (void)scene;
    // TODO
}

void draw_floor() {
    Model* cube = get_model("assets/models/cube.obj");
    
    if (cube != NULL) {
        GLfloat floor_ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat floor_diffuse[]  = { 0.4f, 0.4f, 0.4f, 1.0f };
        GLfloat floor_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat floor_shininess  = 0.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, floor_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, floor_shininess);

        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -0.01f);
        glScalef(20.0f, 20.0f, 0.01f);
        draw_model(cube);
        glPopMatrix();
    }
}