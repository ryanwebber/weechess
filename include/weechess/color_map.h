#pragma once

#include <weechess/piece.h>

namespace weechess {

template <typename T> class ColorMap {
public:
    constexpr ColorMap()
        : m_white_value()
        , m_black_value()
    {
    }

    constexpr ColorMap(T shared_value)
        : m_white_value(shared_value)
        , m_black_value(shared_value)
    {
    }

    constexpr ColorMap(T white_value, T black_value)
        : m_white_value(white_value)
        , m_black_value(black_value)
    {
    }

    constexpr ColorMap<T> flipped() const { return ColorMap<T>(m_black_value, m_white_value); }

    constexpr T& operator[](Color color)
    {
        if (color == Color::White) {
            return m_white_value;
        } else {
            return m_black_value;
        }
    }

    constexpr const T& operator[](Color color) const
    {
        if (color == Color::White) {
            return m_white_value;
        } else {
            return m_black_value;
        }
    }

private:
    T m_white_value;
    T m_black_value;
};

}
