#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

struct Pixel {
  Pixel() = default;
  Pixel(std::uint8_t _r, std::uint8_t _g, std::uint8_t _b)
      : r(_r), g(_g), b(_b) {}
  std::uint8_t r;
  std::uint8_t g;
  std::uint8_t b;
};

class Image {
public:
  Image(const std::string &outputName, const std::uint32_t &w,
        const std::uint32_t &h, const std::uint32_t &ch);
  ~Image();

  void setPixel(const std::uint32_t &x, const std::uint32_t &y,
                const Pixel &pixelColor);

  void savePng() noexcept;

  auto data() { return imageData; }
  void *imgData() { return (void *)imageData.data();
  }

  size_t size() const { return imageData.size(); }
  size_t dataSize() const { return imageData.size() * sizeof(Pixel); }

private:
  std::int32_t width;
  std::int32_t height;
  std::int32_t channels;
  std::vector<Pixel> imageData;
  std::string outputFileName;
  bool hasBeenSaved;
};
