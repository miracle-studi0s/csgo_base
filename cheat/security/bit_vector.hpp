#pragma once

template<typename type_t = std::uint32_t>
// DON'T USE IT
// u should use bitsXX instead
class c_bit_vector {
protected:
    type_t m_value = 0u;
    using this_type = c_bit_vector<type_t>;
public:
    __forceinline constexpr c_bit_vector() {
        static_assert(std::is_same<type_t, uint8_t>() || std::is_same<type_t, uint16_t>()
            || std::is_same<type_t, uint32_t>() || std::is_same<type_t, uint64_t>(),
            "Don't use raw c_bit_vector class! Available usage: bits8, bits16, bits32, bits64");

        m_value = 0u;
    }

    __forceinline constexpr c_bit_vector(type_t value) {
        static_assert(std::is_same<type_t, uint8_t>() || std::is_same<type_t, uint16_t>()
            || std::is_same<type_t, uint32_t>() || std::is_same<type_t, uint64_t>(),
            "Don't use raw c_bit_vector class! Available usage: bits8, bits16, bits32, bits64");

        m_value = value;
    }

    class iterator {
    private:
        ptrdiff_t m_idx = 0;
        this_type* m_parent = nullptr;
    public:
        __forceinline constexpr iterator(this_type* parent, ptrdiff_t position) : m_parent(parent), m_idx(position) { };

        __forceinline constexpr void operator=(ptrdiff_t new_flag) {
            m_parent->set(m_idx, new_flag);
        }
        __forceinline constexpr operator bool() const {
            return this->get();
        }
        __forceinline constexpr bool get() const {
            return m_parent->at(m_idx);
        }
    };

    __forceinline constexpr type_t& get() {
        return m_value;
    }

    __forceinline constexpr type_t get() const {
        return m_value;
    }

    __forceinline constexpr this_type& set(ptrdiff_t idx, bool value) {
        if (value)
            m_value |= (type_t)1u << (type_t)idx;
        else
            m_value &= ~((type_t)1u << (type_t)idx);
        return *this;
    }

    __forceinline constexpr this_type& inverse(ptrdiff_t idx) {
        static_assert(idx < sizeof(type_t) * 8u, "c_bit_vector subscript out of range");
        m_value ^= 1 << idx;
        return *this;
    }

    template <typename _type_t>
    __forceinline constexpr this_type& _xor(const c_bit_vector<_type_t>& vec) {
        return _xor(vec.get());
    }

    __forceinline constexpr this_type& _xor(type_t n) {
        for (ptrdiff_t i = 0u; i < sizeof(type_t) * 8u; ++i) {
            if ((m_value & ((type_t)1u << (type_t)i)) == (n & ((type_t)1u << (type_t)i)))
                this->set(i, false);
            else
                this->set(i, true);
        }
        return *this;
    }

    __forceinline constexpr this_type& shift_right(ptrdiff_t n) {
        if (!n)
            return *this;
        do {
            this_type old(*this);
            ptrdiff_t len = sizeof(type_t) * 8u;
            for (ptrdiff_t i = len - 1; i > 0; --i) {
                if (i == len - 1)
                    this->set(i, false);
                this->set(i - 1, old.at(i));
            }
        } while (--n > 0);

        return *this;
    }

    __forceinline constexpr this_type& shift_left(ptrdiff_t n) {
        if (!n)
            return *this;
        do {
            this_type old(*this);
            for (ptrdiff_t i = 0; i < sizeof(type_t) * 8u - 1; ++i) {
                if (i == 0)
                    this->set(i, false);
                this->set(i + 1, old.at(i));
            }
        } while (--n > 0);

        return *this;
    }

    __forceinline constexpr std::string bin() const {
        std::string ret;
        for (ptrdiff_t i = sizeof(type_t) * 8u - 1; i >= 0; --i) {
            ret += this->at(i) ? '1' : '0';
            if (i % 8 == 0 && i > 0)
                ret += ' ';
        }
        return ret;
    }

    __forceinline constexpr operator type_t() const {
        return this->get();
    }

    __forceinline constexpr operator type_t& () {
        return this->get();
    }

    __forceinline constexpr bool at(ptrdiff_t i) const {
        return m_value & ((type_t)1u << (type_t)i);
    }

    __forceinline constexpr iterator operator[](ptrdiff_t i) {
        return iterator(this, i);
    }

    __forceinline constexpr bool operator[](ptrdiff_t i) const {
        return this->at(i);
    }
};

using bits8_t = c_bit_vector<std::uint8_t>;
using bits16_t = c_bit_vector<std::uint16_t>;
using bits32_t = c_bit_vector<std::uint32_t>;
using bits64_t = c_bit_vector<std::uint64_t>;