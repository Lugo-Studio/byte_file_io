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
    [[nodiscard]] inline auto read_bytes(const std::filesystem::path& in_path) -> std::optional<std::vector<uint8_t>>
    {
      if (std::ifstream in{ in_path, std::ios::binary }; in.is_open()) {
        const std::streamsize size{ static_cast<std::streamsize>(file_size(in_path)) };
        std::vector<uint8_t> in_byte_buffer(size);
        in.read(reinterpret_cast<char*>(in_byte_buffer.data()), size);
        
        return in_byte_buffer;
      }
      
      return std::nullopt;
    }

    inline auto write_bytes(const std::filesystem::path& out_path, const std::vector<uint8_t>& bytes) -> bool
    {
      if (std::ofstream out{ out_path, std::ios::binary }; out.is_open()) {
        out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        return true;
      }
      
      return false;
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

      io::write_bytes(out_path, v);
    }
    
    template<class P>
    concept Path = requires(P& p) {
      std::filesystem::path();
    }; // Path concept
    
    struct ToFileFn {
      template<std::ranges::range Rng, Path P>
      requires std::constructible_from<std::decay_t<P>, P>
      constexpr auto operator()(Rng&& r, P out_path)
      const noexcept(std::is_nothrow_constructible_v<std::decay_t<P>, P>) -> bool
      {
        auto r_common = r | std::views::common;
        std::vector<uint8_t> v;
        if constexpr (requires { std::ranges::size(r); }) {
          v.reserve(std::ranges::size(r));
        }
        v.insert(v.begin(), r_common.begin(), r_common.end());
        
        return io::write_bytes(out_path, v);
      }
      
      template<Path P>
      requires std::constructible_from<std::decay_t<P>, P>
      [[nodiscard]] constexpr auto operator()(P out_path)
      const noexcept(std::is_nothrow_constructible_v<std::decay_t<P>, P>)
      {
        return std::ranges::_Range_closure<ToFileFn, decltype(out_path)>{ out_path };
      }
    }; // to_file_fn
    
    inline constexpr ToFileFn write_bytes;
  } // ranges
} // fx