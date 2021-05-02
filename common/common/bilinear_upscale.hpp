#include <vector>

std::vector<std::uint8_t> BilinearUpscale(const std::vector<std::uint8_t>& image_data,
                                          std::uint32_t width, std::uint32_t height,
                                          std::uint8_t scale_factor);