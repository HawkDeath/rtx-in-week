#pragma once

#include "Hitable.h"

class Shape : public Hitable {
public:
  Shape() {}
  Shape(point3 cen, double r, std::shared_ptr<Material> mat)
      : center(cen), radius(r), material{mat} {}

  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const override;

private:
  point3 center;
  double radius;
  std::shared_ptr<Material> material;
};

