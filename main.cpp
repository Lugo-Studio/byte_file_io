#include <filesystem>
#include <random>
#include <optional>
#include <ranges>

#include "iobytes.hpp"
#include "if_optional.hpp"

int main()
{
  bool encrypted{ true };
  std::filesystem::path in_path{ "out.txt" };
  std::filesystem::path out_path{ "out2.txt" };
  fx::if_optional(fx::io::read_bytes(in_path), [&](auto& in_bytes) {
    std::default_random_engine eng{ 69 };
    std::uniform_int_distribution<int32_t> dist{ 0, 127 };
  
    in_bytes | std::views::transform([&](uint8_t byte) {
                 uint8_t rand{ static_cast<uint8_t>(dist(eng)) };
                 return (byte + ((encrypted) ? 128 - rand : rand)) % 128;
               })
             | fx::ranges::write_bytes(out_path);
  });
}
