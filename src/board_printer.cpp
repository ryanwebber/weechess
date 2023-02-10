#include "board_printer.h"

constexpr std::u16string_view board_template = u"╭───┬───┬───┬───┬───┬───┬───┬───╮"
                                               u"│   │   │   │   │   │   │   │   │"
                                               u"├───┼───┼───┼───┼───┼───┼───┼───┤"
                                               u"│   │   │   │   │   │   │   │   │"
                                               u"├───┼───┼───┼───┼───┼───┼───┼───┤"
                                               u"│   │   │   │   │   │   │   │   │"
                                               u"├───┼───┼───┼───┼───┼───┼───┼───┤"
                                               u"│   │   │   │   │   │   │   │   │"
                                               u"├───┼───┼───┼───┼───┼───┼───┼───┤"
                                               u"│   │   │   │   │   │   │   │   │"
                                               u"├───┼───┼───┼───┼───┼───┼───┼───┤"
                                               u"│   │   │   │   │   │   │   │   │"
                                               u"├───┼───┼───┼───┼───┼───┼───┼───┤"
                                               u"│   │   │   │   │   │   │   │   │"
                                               u"├───┼───┼───┼───┼───┼───┼───┼───┤"
                                               u"│   │   │   │   │   │   │   │   │"
                                               u"╰───┴───┴───┴───┴───┴───┴───┴───╯";

constexpr std::u16string_view cell_template = u"╭───╮"
                                              u"│   │"
                                              u"╰───╯";

BoardPrinter::BoardPrinter()
    : m_data(board_template)
{
}

std::u16string_view BoardPrinter::str() const { return m_data; }

std::u16string_view BoardPrinter::row_str(size_t row) const
{
    return str().substr(row * col_char_count, col_char_count);
}

BoardPrinter::Cell BoardPrinter::operator[](weechess::Location location)
{
    auto row = 1 + location.rank() * 2;
    auto col = 2 + location.file() * 4;
    return this->cell_at(row, col);
}

BoardPrinter::Cell BoardPrinter::cell_at(size_t row, size_t col) { return Cell(*this, row * col_char_count + col); }

BoardPrinter::Cell::Cell(BoardPrinter& printer, size_t offset)
    : m_printer(printer)
    , m_offset(offset)
{
}

void BoardPrinter::Cell::paint_border(Border border)
{
    switch (border) {
    case Border::Integrated: {
        for (auto offset : BoardPrinter::Cell::offsets)
            if (offset != 0)
                m_printer.m_data[m_offset + offset] = board_template[m_offset + offset];
        break;
    }
    case Border::Detached: {
        for (auto i = 0; i < BoardPrinter::Cell::offsets.size(); i++)
            if (BoardPrinter::Cell::offsets[i] != 0)
                m_printer.m_data[m_offset + BoardPrinter::Cell::offsets[i]] = cell_template[i];
        break;
    }
    }
}

void BoardPrinter::Cell::paint_symbol(char16_t symbol) { m_printer.m_data[m_offset] = symbol; }

char16_t BoardPrinter::Cell::operator*() const { return m_printer.m_data[m_offset]; }

size_t BoardPrinter::Cell::offset() const { return m_offset; }
