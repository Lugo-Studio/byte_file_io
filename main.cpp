#include <filesystem>
#include <random>
#include <optional>
#include <ranges>
#include <cstdlib>
#include <iostream>

#include "iobytes.hpp"
#include "if_optional.hpp"

int main(const int argc, char** argv)
{
  if (argc != 5) {
    std::cerr << "Usage: [encrypt|decrypt] <input_file> <output_file> <encryption_seed_int32>\n";
    return EXIT_FAILURE;
  }
  
  std::filesystem::path in_path{ argv[2] };
  std::filesystem::path out_path{ argv[3] };
  
  fx::if_optional(fx::io::read_bytes(in_path), [&](auto& in_bytes) {
    std::default_random_engine eng;
    eng.seed(std::strtol(argv[4], nullptr, 10));
    std::uniform_int_distribution<int32_t> dist{ 0, 127 };
  
    in_bytes | std::views::transform([&](uint8_t byte) {
                 uint8_t rand{ static_cast<uint8_t>(dist(eng)) };
                 return (byte + ((std::strcmp(argv[1], "encrypt")) ? rand : 128 - rand)) % 128;
               })
             | fx::ranges::write_bytes(out_path);
  });
}
