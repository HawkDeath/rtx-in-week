#pragma once

#include "Hitable.h"

#include <memory>
#include <vector>

class HittableList : public Hitable {
public:
  HittableList() {}
  HittableList(std::shared_ptr<Hitable> object) { add(object); }

  void clear() { objects.clear(); }
  void add(std::shared_ptr<Hitable> object) { objects.push_back(object); }

  virtual bool hit(const Ray &r, double t_min, double t_max,
                   HitRecord &rec) const override;

private:
  std::vector<std::shared_ptr<Hitable>> objects;
};
