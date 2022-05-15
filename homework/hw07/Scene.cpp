//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const {
    return this->bvh->Intersect(ray);
}

/**
 * 采样一个光源的点，并给出 pdf
 */
void Scene::sampleLight(Intersection &pos, float &pdf) const {
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()) {
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()) {
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum) {
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

// Deprecated!!!
bool Scene::trace(
        const Ray &ray,
        const std::vector<Object *> &objects,
        float &tNear, uint32_t &index, Object **hitObject) {
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const {
    Intersection intersection = intersect(ray);
    if (intersection.happened) {
        if (intersection.m->hasEmission()) {
            return intersection.m->getEmission();
        }

        Vector3f lDir = 0, lIndir = 0;
        Intersection lightPoint;
        float pdfLight;
        sampleLight(lightPoint, pdfLight);

        auto material = intersection.m;
        auto &normal = intersection.normal;
        auto &hitPoint = intersection.coords;
        auto toLight = lightPoint.coords - hitPoint;
        auto toLightDirection = normalize(toLight);
        auto intersectionLight = intersect(Ray(lightPoint.coords, -toLightDirection));
        // No block in the middle
        if (intersection.obj == intersectionLight.obj) {
            lDir = lightPoint.emit * material->eval(toLightDirection, -ray.direction, normal) *
                   dotProduct(toLightDirection, normal) *
                   dotProduct(-toLightDirection, lightPoint.normal) /
                   dotProduct(toLight, toLight) / pdfLight;
        }

        if (get_random_float() < RussianRoulette) {
            auto indirection = material->sample(ray.direction, normal);
            Ray ray1(hitPoint, indirection);
            Intersection intersectionNext = intersect(ray1);
            float pdf = material->pdf(ray.direction, indirection, normal);
            if (intersectionNext.happened && !intersectionNext.m->hasEmission() && pdf > EPSILON) {
                lIndir = castRay(ray1, depth) * material->eval(indirection, -ray.direction, normal) *
                         dotProduct(indirection, normal) / pdf / RussianRoulette;
            }
        }

        return lDir + lIndir;
    }

    return 0;
}