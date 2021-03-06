#ifndef WEBSOCKET_SERVER_PACKET_VIEW_HH
#define WEBSOCKET_SERVER_PACKET_VIEW_HH

#include <fmt/format.h>

#include <cstdint>
#include <ostream>
#include <string>
#include <type_traits>

namespace amadeus {
/// \brief A read-only (immutable), non-owning view, copyable of a contiguous
/// amount of data consisting of a pointer and size.
/// \remarks Make sure an object of this class does NOT exceed the lifetime of
/// the buffer that is being pointed to!
/* Example:
 *
 * // can also be std::array<std::uint8_t, 8> const buffer{...};
 * std::vector<std::uint8_t> const buffer {0x01, 0x00, 0x00, 0x00,
 *                                         0x02, 0x00, 0x00, 0x00};
 *
 *  struct Data {
 *      int x;
 *      int y;
 *  };
 *
 *  // A read-only view of a buffer.
 *  // packet_view<Data> const r {buffer};
 *  // or
 *  // packet_view<Data> const r {buffer.data(), buffer.size()};
 *
 *  // Access the underlying data.
 *  auto const& x = r->x;
 *  auto const& y = r->y;
 *
 */
template <typename PacketType, typename Type = std::uint8_t>
class packet_view final
{
    using value_type = Type;
    using value_size_type = std::size_t;

    using iterator = value_type*;
    using const_iterator = value_type const*;

  public:
    constexpr packet_view(void const* _data, value_size_type _size) noexcept
        : data_(static_cast<value_type const*>(_data))
        , size_(_size)
    {
        static_assert(std::is_class_v<PacketType>,
                      "Packet needs to be a struct or class.");
        static_assert(std::is_trivially_copyable_v<PacketType>,
                      "Packet needs to be trivially copyable.");
    }

    constexpr explicit packet_view(void const* _data) noexcept
        : data_(static_cast<value_type const*>(_data))
        , size_(sizeof(PacketType))
    {
        static_assert(std::is_class_v<PacketType>,
                      "Packet needs to be a struct.");
        static_assert(std::is_trivially_copyable_v<PacketType>,
                      "Packet needs to be trivially copyable.");
    }

    [[nodiscard]] constexpr iterator begin() noexcept
    {
        return iterator(data_);
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return const_iterator(data_);
    }

    [[nodiscard]] constexpr iterator end() noexcept
    {
        return iterator(data_ + size_);
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return const_iterator(data_ + size_);
    }

    [[nodiscard]] constexpr PacketType const& operator*() const noexcept
    {
        return *reinterpret_cast<PacketType const>(data_);
    }

    [[nodiscard]] constexpr PacketType const* operator->() const noexcept
    {
        return reinterpret_cast<PacketType const*>(data_);
    }

    [[nodiscard]] constexpr value_type const* data() const noexcept
    {
        return data_;
    }

    [[nodiscard]] constexpr value_size_type size() const noexcept
    {
        return size_;
    }

  private:
    value_type const* data_{};
    value_size_type size_{};
};
} // namespace amadeus

template <typename PacketType, typename Type>
struct fmt::formatter<amadeus::packet_view<PacketType, Type>>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(amadeus::packet_view<PacketType, Type> view, FormatContext& ctx)
    {
        return fmt::format_to(ctx.out(), "[{0:#04x}, {1}] --> {{{2:#04x}}}",
                              *view.data(), view.size(), fmt::join(view, ", "));
    }
};
#endif // !WEBSOCKET_SERVER_PACKET_VIEW_HH
