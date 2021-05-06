#include "Image.h"
#include "../stb_image_write.h"
#include <iostream>

Image::Image(const std::string &outputName, const std::uint32_t &w,
             const std::uint32_t &h, const std::uint32_t &ch)
    : outputFileName(outputName), width(w), height(h), channels(ch),
      hasBeenSaved(false) {
  auto bufferSize = w * h;
  imageData.resize(bufferSize, Pixel(0, 0, 0));
  std::cout << imageData.size() * sizeof(Pixel) << " size\n";
}

Image::~Image() {
  if (!hasBeenSaved)
    savePng();

  imageData.clear();
}

void Image::setPixel(const std::uint32_t &x, const std::uint32_t &y,
                     const Pixel &pixelColor) {
  auto idxPixel = (width * x) + y;
  const auto max = width * height;
  imageData[idxPixel] = pixelColor;
}

void Image::savePng() noexcept {
  outputFileName += ".png";
  stbi_flip_vertically_on_write(true);
  stbi_write_png(outputFileName.data(), width, height, channels,
                 imageData.data(), width * channels);
  hasBeenSaved = true;
}
