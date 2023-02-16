#pragma once

#include <weechess/piece.h>

namespace weechess {

template <typename T> class ColorMap {
public:
    ColorMap()
        : m_white_value()
        , m_black_value()
    {
    }

    ColorMap(T shared_value)
        : m_white_value(shared_value)
        , m_black_value(shared_value)
    {
    }

    ColorMap(T white_value, T black_value)
        : m_white_value(white_value)
        , m_black_value(black_value)
    {
    }

    ColorMap<T> flipped() const { return ColorMap<T>(m_black_value, m_white_value); }

    T& operator[](Color color)
    {
        if (color == Color::White) {
            return m_white_value;
        } else {
            return m_black_value;
        }
    }

    const T& operator[](Color color) const
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
