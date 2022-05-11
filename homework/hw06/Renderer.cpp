//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"


inline float deg2rad(const float &deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene &scene) {
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    // 距离屏幕为 1
    float scale = std::tan(deg2rad(scene.fov * 0.5f)) / (scene.height / 2);
    // float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(-1, 5, 10);
    int m = 0;
    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
            float x = (i + 0.5 - scene.width / 2) * scale;
            float y = (scene.height / 2 - (j + 0.5)) * scale;
            Vector3f dir = normalize(Vector3f(x, y, -1));
            framebuffer[m++] = scene.castRay(Ray(eye_pos, dir), 0);
        }
        UpdateProgress(j / (float) scene.height);
    }
    UpdateProgress(1.f);

    // save framebuffer to file
    FILE *fp = fopen("binary.ppm", "wb");
    (void) fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char) (255 * clamp(0, 1, framebuffer[i].x));
        color[1] = (unsigned char) (255 * clamp(0, 1, framebuffer[i].y));
        color[2] = (unsigned char) (255 * clamp(0, 1, framebuffer[i].z));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
}
