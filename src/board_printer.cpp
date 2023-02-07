#include "board_printer.h"

#include <iostream>

constexpr size_t row_char_count = 17;
constexpr size_t col_char_count = 33;

struct Vec2Int {
    int x;
    int y;
};

static const char16_t board_template[] = 
    u"╭───┬───┬───┬───┬───┬───┬───┬───╮"
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
    u"╰───┴───┴───┴───┴───┴───┴───┴───╯"
    ;

static const char16_t cell_template[] =
    u"╭───╮"
    u"│   │"
    u"╰───╯"
    ;

static const std::vector<Vec2Int> offsets = {
    Vec2Int {  0, -1 },
    Vec2Int {  1, -1 },
    Vec2Int {  2, -1 },
    Vec2Int {  2,  0 },
    Vec2Int {  2,  1 },
    Vec2Int {  1,  1 },
    Vec2Int {  0,  1 },
    Vec2Int { -1,  1 },
    Vec2Int { -2,  1 },
    Vec2Int { -2,  0 },
    Vec2Int { -2, -1 },
    Vec2Int { -1, -1 },
};

void draw_box(
    std::vector<std::vector<BoardRender::Cell>> &cells,
    Vec2Int offset,
    weechess::Piece piece,
    BoardRender::Decoration decoration)
{
    auto symbol = !piece.exists() && decoration != BoardRender::Decoration::None ? u'·' : piece.to_symbol();

    // Middle cell
    cells[offset.y][offset.x] = BoardRender::Cell {
        symbol,
        decoration
    };

    for (const auto &l : offsets) {
        auto row = offset.y + l.y;
        auto col = offset.x + l.x;

        if (row < 0 || row >= row_char_count || col < 0 || col >= col_char_count) {
            continue;
        }

        auto symbol = decoration == BoardRender::Decoration::Selected ?
            cell_template[(l.y + 1) * 5 + (2 + l.x)] :
            board_template[row * col_char_count + col];

        cells[row][col] = BoardRender::Cell { symbol, decoration };
    }
}

BoardRender BoardPrinter::print(const weechess::Board& board, std::optional<weechess::Location> selected_location) const {
    using Cell = BoardRender::Cell;
    using Decoration = BoardRender::Decoration;

    std::vector<std::vector<Cell>> cells;
    
    auto rank_to_row = [](uint8_t rank) {
        return 1 + rank * 2;
    };

    auto file_to_col = [](uint8_t file) {
        return 2 + file * 4;
    };

    // Fill up with spaces
    for (auto r = 0; r < row_char_count; r++) {
        cells.push_back(std::vector<Cell>());
        for (auto c = 0; c < col_char_count; c++) {
            cells[r].push_back(Cell { ' ', Decoration::None });
        }
    }

    // Draw each square individually, because it's more simple
    for (auto i = 0; i < weechess::Board::cell_count; i++) {
        weechess::Location location(i);
        auto row = rank_to_row(location.rank());
        auto col = file_to_col(location.file());
        draw_box(cells, { col, row }, board.piece_at(location), Decoration::None);
    }

    if (selected_location.has_value()) {
        auto row = rank_to_row(selected_location->rank());
        auto col = file_to_col(selected_location->file());
        draw_box(cells, { col, row }, board.piece_at(*selected_location), Decoration::Selected);
    }

    return BoardRender { cells, row_char_count, col_char_count };
}
