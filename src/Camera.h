#pragma once

#include "Ray.h"
#include "rtweekend.h"

class Camera {
public:
  Camera(point3 lookfrom, point3 lookat, point3 vup, double vfov,
         double aspect_ratio, double aperture, double focus_dis) {
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2.0);

    auto viewport_height = 2.0f * h;
    auto viewport_width = aspect_ratio * viewport_height;

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    origin = lookfrom;
    horizontal = viewport_width * u * focus_dis;
    vertical = viewport_height * v * focus_dis;
    lower_left_corner = origin - horizontal / 2 - vertical / 2 - w * focus_dis;
    lens_radius = aperture / 2;
  }

  Ray getRay(double s, double t) {
    vec3 rd = lens_radius * random_in_unit_disk();
    vec3 offset = u * rd.x() + v * rd.y();
    return {origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset};
  }

private:
  point3 origin;
  point3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  double lens_radius;
};
