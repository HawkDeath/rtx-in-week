#pragma once

#include "Ray.h"
#include <memory>

class Material;

struct HitRecord {
  point3 p;
  vec3 normal;
  std::shared_ptr<Material> matPtr;
  double t;
  bool front_face;

  inline void set_face_normal(const Ray& r, const vec3& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class Hitable {
public:
  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const = 0;
};
