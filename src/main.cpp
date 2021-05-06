#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <SDL.h>
#undef main
#include "Camera.h"
#include "HittableList.h"
#include "Image.h"
#include "Material.h"
#include "Shape.h"
#include "vec3.h"

std::promise<void> endSignal;
std::promise<void> drawSignal;
std::future<void> futureObj = endSignal.get_future();
std::future<void> drawFutureObj = drawSignal.get_future();
std::mutex mutex;

const auto aspect_ratio = 16.0 / 9.0;
const auto width = 800;
const auto height = std::uint32_t(width / aspect_ratio);
const int sampler_per_pixel = 100;
const int max_depth = 50;
static bool lastUpdate = false;
// Camera setup
point3 lookfrom(13, 2, 3);
point3 lookat(0, 0, 0);
vec3 vup(0, 1, 0);
auto dist_to_focus = 10.0;
auto aperture = 0.1;
Camera camera = {lookfrom,     lookat,   vup,          20.0,
                 aspect_ratio, aperture, dist_to_focus};

std::unique_ptr<Image> testImage =
    std::make_unique<Image>("testImg", width, height, 3);
Pixel getPixelColor(const color inPixelColor, int samples_per_pixel) {
  auto r = inPixelColor.x();
  auto g = inPixelColor.y();
  auto b = inPixelColor.z();

  // gamma = 2.0f
  auto scale = 1.0 / samples_per_pixel;
  r = std::sqrt(r * scale);
  g = std::sqrt(g * scale);
  b = std::sqrt(b * scale);

  return {static_cast<std::uint8_t>(256 * clamp(r, 0.0, 0.999)),
          static_cast<std::uint8_t>(256 * clamp(g, 0.0, 0.999)),
          static_cast<std::uint8_t>(256 * clamp(b, 0.0, 0.999))};
}

color ray_color(const Ray &r, const Hitable &world, int depth) { // scene render
  HitRecord rec;

  if (depth <= 0)
    return color(0, 0, 0);

  if (world.hit(r, 0.001, inifinity, rec)) {
    Ray scattered;
    color attenuation;
    if (rec.matPtr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * ray_color(scattered, world, depth - 1);
    }
    return color(0, 0, 0);
  }
  vec3 unit_direction = unit_vector(r.direction());
  auto t = 0.5 * (unit_direction.y() + 1.0f);
  return (1.0f - t) * color(1.0f, 1.0f, 1.0f) + t * color(0.5f, 0.7f, 1.0f);
}

HittableList random_scene() {
  HittableList world;
  auto ground_material = std::make_shared<Lambertian>(color(0.5, 0.5, 0.5));
  world.add(
      std::make_shared<Shape>(point3(0, -1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) {
      auto choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        std::shared_ptr<Material> sphere_material;
        if (choose_mat < 0.8) {
          auto albedo = color::random() * color::random();
          sphere_material = std::make_shared<Lambertian>(albedo);
          world.add(std::make_shared<Shape>(center, 0.2, sphere_material));

        } else if (choose_mat < 0.95) {
          auto albedo = color::random(0.5, 1.0);
          auto fuzz = random_double(0.0, 0.5);
          sphere_material = std::make_shared<Metal>(albedo, fuzz);
          world.add(std::make_shared<Shape>(center, 0.2, sphere_material));

        } else {
          sphere_material = std::make_shared<Dielectric>(1.5);
          world.add(std::make_shared<Shape>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = std::make_shared<Dielectric>(1.5);
  world.add(std::make_shared<Shape>(point3(0, 1, 0), 1.0, material1));

  auto material2 = std::make_shared<Lambertian>(color(0.4, 0.2, 0.1));
  world.add(std::make_shared<Shape>(point3(-4, 1, 0), 1.0, material2));

  auto material3 = std::make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
  world.add(std::make_shared<Shape>(point3(4, 1, 0), 1.0, material3));

  return world;
}

int main() {

  std::cout << "img size: " << width << "x" << height << "\n";

  // Scene


  std::thread printThred(
      [](std::future<void> futureObj) -> void {
        while (futureObj.wait_for(std::chrono::milliseconds(1)) ==
               std::future_status::timeout) {
          std::lock_guard<std::mutex> l{mutex};
          std::cout << ".";
          std::this_thread::sleep_for(std::chrono::seconds(1));
        }
      },
      std::move(futureObj));
  std::cout << "Start render";
  printThred.detach();

  std::thread renderThread([]() -> void {
    auto startTime = std::chrono::high_resolution_clock::now();
    auto world = random_scene();

    for (int j = height - 1; j >= 0; --j) {
      for (int i = 0; i < width; ++i) {
        color pixel_color = {0.0, 0.0, 0.0};

#pragma omp parallel for
        for (int sample = 0; sample < sampler_per_pixel; ++sample) {

          auto u = (i + random_double()) / (width - 1);
          auto v = (j + random_double()) / (height - 1);
          Ray r = camera.getRay(u, v);
          pixel_color += ray_color(r, world, max_depth);
        }
        Pixel pixel = getPixelColor(pixel_color, sampler_per_pixel);
        testImage->setPixel(j, i, pixel);
      }
    }
    lastUpdate = true;
    drawSignal.set_value();
    endSignal.set_value();
    std::cout << "done\n";
    std::chrono::duration<double> endTime =
        std::chrono::high_resolution_clock::now() - startTime;

    auto hours = std::chrono::duration_cast<std::chrono::hours>(endTime);
    auto minutes =
        std::chrono::duration_cast<std::chrono::minutes>(endTime - hours);
    auto seconds =
        std::chrono::duration_cast<std::chrono::seconds>(endTime - minutes);

    std::cout << "Render time: " << hours.count() << " hours, "
              << minutes.count() << " min, " << seconds.count() << " sec\n";
  });

  // SDL stuff

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *screen =
      SDL_CreateWindow("rtx_in_weekend", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, width, height, 0);

  SDL_Renderer *sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

  SDL_Texture *texture =
      SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STATIC, width, height);

  bool closeWindow = false;
  renderThread.detach();
  SDL_Event sdlEvents;
  while (!closeWindow) {
    SDL_PollEvent(&sdlEvents);
    switch (sdlEvents.type) {
    case SDL_QUIT:
      closeWindow = true;
      break;
    }
    if (drawFutureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout) {
      if (SDL_UpdateTexture(texture, NULL, testImage->imgData(),
                            width * sizeof(Pixel)) < 0) {
        std::cout << SDL_GetError() << "\n";
      }
    }

    if (lastUpdate) {
      if (SDL_UpdateTexture(texture, NULL, testImage->imgData(),
                            width * sizeof(Pixel)) < 0) {
        std::cout << SDL_GetError() << "\n";
      }

      lastUpdate = false;
    }

    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopyEx(sdlRenderer, texture, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
    SDL_RenderPresent(sdlRenderer);
  }


  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(sdlRenderer);
  SDL_DestroyWindow(screen);
  SDL_Quit();
  return 0;
}