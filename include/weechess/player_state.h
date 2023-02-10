#pragma once

#include <weechess/piece.h>

namespace weechess {

template <typename T> class PlayerState {
public:
    PlayerState(T shared_state)
        : m_white_state(shared_state)
        , m_black_state(shared_state)
    {
    }

    PlayerState(T white_state, T black_state)
        : m_white_state(white_state)
        , m_black_state(black_state)
    {
    }

    T& operator[](Color color)
    {
        if (color == Color::White) {
            return m_white_state;
        } else {
            return m_black_state;
        }
    }

    const T& operator[](Color color) const
    {
        if (color == Color::White) {
            return m_white_state;
        } else {
            return m_black_state;
        }
    }

private:
    T m_white_state;
    T m_black_state;
};

}
