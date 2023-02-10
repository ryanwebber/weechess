#pragma once

#include <optional>
#include <span>
#include <vector>

#include <weechess/location.h>

class BoardPrinter {
public:
    static constexpr size_t row_char_count = 17;
    static constexpr size_t col_char_count = 33;

    template <typename T> using Buffer = std::array<T, row_char_count * col_char_count>;

    enum class Border { Integrated, Detached };

    class Cell {
    public:
        static constexpr std::array<int, 15> offsets = { -35, -34, -33, -32, -31, -2, -1, 0, 1, 2, 31, 32, 33, 34, 35 };

        void paint_border(Border);
        void paint_symbol(char16_t);

        size_t offset() const;

        char16_t operator*() const;

    private:
        Cell(BoardPrinter&, size_t offset);

        size_t m_offset;
        BoardPrinter& m_printer;

        friend class BoardPrinter;
    };

    BoardPrinter();

    std::u16string_view str() const;
    std::u16string_view row_str(size_t row) const;

    constexpr size_t rows() const { return row_char_count; }
    constexpr size_t cols() const { return col_char_count; }

    Cell cell_at(size_t row, size_t col);
    Cell operator[](weechess::Location);

private:
    std::u16string m_data;

    friend class Cell;
};
