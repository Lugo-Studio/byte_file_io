//
// Created by galex on 10/6/2022.
//

#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <ranges>
#include <type_traits>

namespace fx {
  namespace io {
    auto read_bytes(const std::filesystem::path& in_path) -> std::optional<std::vector<uint8_t>>
    {
      if (std::ifstream in{ in_path, std::ios::binary }; in.is_open()) {
        const std::streamsize size{ static_cast<std::streamsize>(file_size(in_path)) };
        std::vector<uint8_t> in_byte_buffer(size);
        in.read(reinterpret_cast<char*>(in_byte_buffer.data()), size);
        
        return in_byte_buffer;
      }
      
      return std::nullopt;
    }
    
    auto write_bytes(const std::filesystem::path& out_path, const std::vector<uint8_t>& bytes)
    {
      if (std::ofstream out{ out_path, std::ios::binary }; out.is_open()) {
        out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
      }
    }
  } // io
  
  namespace ranges {
    template<std::ranges::range R>
    auto write_to_file(R&& r, const std::filesystem::path& out_path)
    -> std::enable_if_t<std::is_same_v<std::ranges::range_value_t<R>, uint8_t>>
    {
      auto r_common = r | std::views::common;
      std::vector<uint8_t> v;
    
      // if we can get a size, reserve that much
      if constexpr (requires { std::ranges::size(r); }) {
        v.reserve(std::ranges::size(r));
      }
    
      v.insert(v.begin(), r_common.begin(), r_common.end());
  
      fx::io::write_bytes(out_path, v);
    }
  
    template<class Path>
    concept path = requires(Path& p) {
      std::filesystem::path();
    }; // Path concept
  
    struct to_file_fn {
      template<std::ranges::range Rng, path Path>
      requires std::constructible_from<std::decay_t<Path>, Path>
      constexpr void operator()(Rng&& r, Path out_path) const
      noexcept(std::is_nothrow_constructible_v<std::decay_t<Path>, Path>)
      {
        auto r_common = r | std::views::common;
        std::vector<uint8_t> v;
        if constexpr (requires { std::ranges::size(r); }) {
          v.reserve(std::ranges::size(r));
        }
        v.insert(v.begin(), r_common.begin(), r_common.end());
      
        fx::io::write_bytes(out_path, v);
      }
    
      template<path Path>
      requires std::constructible_from<std::decay_t<Path>, Path>
      [[nodiscard]] constexpr auto operator()(Path out_path) const
      noexcept(std::is_nothrow_constructible_v<std::decay_t<Path>, Path>)
      {
        return std::ranges::_Range_closure<to_file_fn, decltype(out_path)>{ out_path };
      }
    }; // to_file_fn
    
    inline constexpr to_file_fn write_bytes;
  } // ranges
} // fx