#pragma once

#include "vec3.h"

class Ray {
public:
  Ray() {}
  Ray(const point3 &_origin, const vec3 &_dir)
      : orgi(_origin), dir(_dir) {}
  ~Ray() {}

  point3 origin() const { return orgi; }
  vec3 direction() const { return dir; }

  point3 at(double t) const { return orgi + t * dir; }

private:
  point3 orgi;
  vec3 dir;
};
