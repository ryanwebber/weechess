#include "fen.h"
#include "log.h"
#include <weechess/game_state.h>

namespace weechess {

bool CastleRights::has_rights() const { return can_castle_kingside || can_castle_queenside; }

GameState::GameState()
    : m_turn_to_move(Color::White)
    , m_castle_rights(CastleRights::all())
    , m_en_passant_target({})
    , m_analysis({})
    , m_piece_locations({})
{
}

GameState::GameState(
    Board board, Color turn_to_move, ColorMap<CastleRights> castle_rights, std::optional<Location> en_passant_target)
    : m_board(std::move(board))
    , m_turn_to_move(turn_to_move)
    , m_castle_rights(castle_rights)
    , m_en_passant_target(en_passant_target)
    , m_analysis({})
    , m_piece_locations({})
{
    for (auto i = 0; i < Board::cell_count; i++) {
        Location l(i);
        auto piece = m_board.piece_at(l);
        if (piece.exists()) {
            m_piece_locations.push_back(l);
        }
    }
}

const Color& GameState::turn_to_move() const { return m_turn_to_move; }

const ColorMap<CastleRights>& GameState::castle_rights() const { return m_castle_rights; }

const std::optional<Location>& GameState::en_passant_target() const { return m_en_passant_target; }

std::span<const Location> GameState::piece_locations() const { return m_piece_locations; }

std::string GameState::to_fen() const { return fen::to_fen(*this); }

std::optional<GameState> GameState::from_fen(std::string_view fen_sv) { return fen::from_fen(fen_sv); }

GameState GameState::new_game() { return GameState::from_fen(fen::initial_gamestate_fen).value(); }

const Board& GameState::board() const { return m_board; }

const GameState::Analysis& GameState::analysis() const
{
    assert(!m_analysis_disabled);

    if (!m_analysis) {
        auto mut_this = const_cast<GameState*>(this);

        // This isn't thread safe, but we use it for debugging
        mut_this->m_analysis_disabled = true;
        mut_this->m_analysis = Analysis::from(*this);
        mut_this->m_analysis_disabled = false;
    }

    return *m_analysis;
}

GameState GameState::as_monochromatic(Color perspective) const
{
    if (perspective == m_turn_to_move) {
        return *this;
    }

    auto castle_rights = m_castle_rights.flipped();

    auto en_passant_target = m_en_passant_target;
    if (en_passant_target) {
        en_passant_target = en_passant_target->chromatic_inverse();
    }

    Board board;
    for (const auto& l : m_piece_locations) {
        board.set_piece_at(l.chromatic_inverse(), m_board.piece_at(l).chromatic_inverse());
    }

    return GameState { board, perspective, castle_rights, en_passant_target };
}

std::optional<GameState> GameState::by_performing_move(const GameState& game_state, const Move& move, MoveDetail*)
{
    if (!game_state.board().piece_at(move.origin).is(game_state.turn_to_move())) {
        return {};
    }

    if (!game_state.analysis().is_legal_move(move)) {
        return {};
    }

    // TODO: This does not account for:
    //  * Castle rights
    //  * Castling
    //  * En passant
    //  * Promotion

    Board board(game_state.board());
    board.set_piece_at(move.destination, board.piece_at(move.origin));
    board.set_piece_at(move.origin, Piece::none());

    return GameState { std::move(board), invert_color(game_state.turn_to_move()), game_state.castle_rights(), {} };
}

GameState::Analysis::Analysis(bool is_check, std::vector<Move> legal_moves)
    : m_is_check(is_check)
    , m_legal_moves(std::move(legal_moves))
    , m_legal_moves_by_location({})
{
    auto legal_moves_begin = m_legal_moves.begin();
    while (legal_moves_begin != m_legal_moves.end()) {
        auto legal_moves_end = std::find_if(legal_moves_begin, m_legal_moves.end(), [&](const Move& move) {
            return move.origin != legal_moves_begin->origin;
        });

        auto legal_moves_span = std::span<const Move> { &*legal_moves_begin,
            static_cast<size_t>(std::distance(legal_moves_begin, legal_moves_end)) };
        m_legal_moves_by_location[legal_moves_begin->origin.offset] = legal_moves_span;
        legal_moves_begin = legal_moves_end;
    }
}

bool GameState::Analysis::is_check() const { return m_is_check; }
bool GameState::Analysis::is_checkmate() const { return legal_moves().empty() && is_check(); }
bool GameState::Analysis::is_stalemate() const { return legal_moves().empty() && !is_check(); }

std::span<const Move> GameState::Analysis::legal_moves() const { return m_legal_moves; }
std::span<const Move> GameState::Analysis::legal_moves_from(Location location) const
{
    return m_legal_moves_by_location[location.offset];
}

bool GameState::Analysis::is_legal_move(const Move move) const
{
    return std::find(m_legal_moves.begin(), m_legal_moves.end(), move) != m_legal_moves.end();
}

GameState::Analysis GameState::Analysis::from(const GameState& game_state)
{
    ;
    return GameState::Analysis(false, {});
}

// namespace {
//     using GeneratorFn
//         = std::function<void(std::vector<Move>&, Location, const GameState&, const GameState::ThreatMap&)>;
//     extern const std::array<GeneratorFn, 7> pseudo_legal_moves_for_piece;

//     using ThreatmappingFn = std::function<void(Location, const Board&, const std::function<void(Location)>&)>;
//     extern const std::array<ThreatmappingFn, 7> threat_map_for_piece;
// }

// GameState::Analysis analyze(const GameState& game_state)
// {
//     auto mono_game_state = game_state.as_monochromatic(Color::White);
//     mono_game_state.m_analysis_disabled = true;

//     bool is_check = false;
//     std::vector<Move> pseudo_legal_moves;

//     // Compute the threat map for all pieces
//     GameState::ThreatMap threat_map;
//     for (const auto& l : game_state.piece_locations()) {
//         Piece piece = mono_game_state.board().piece_at(l);
//         threat_map_for_piece[static_cast<uint8_t>(piece.type())](l,
//             mono_game_state.board(),
//             [&](Location threat_location) { threat_map.set_attacked_by(l, piece.color()); });
//     }

//     // Compute the pseudo-legal moves from the white pieces
//     for (const auto& l : game_state.piece_locations()) {
//         Piece piece = mono_game_state.board().piece_at(l);
//         if (piece.is(Color::White))
//             pseudo_legal_moves_for_piece[static_cast<uint8_t>(piece.type())](
//                 pseudo_legal_moves, l, mono_game_state, threat_map);
//     }

//     // Flip the board back to the original perspective
//     if (game_state.turn_to_move() != mono_game_state.turn_to_move()) {
//         for (auto& move : pseudo_legal_moves) {
//             move = move.chromatic_inverse();
//         }
//     }

//     return GameState::Analysis { is_check, std::move(pseudo_legal_moves), std::move(threat_map) };
// }

// namespace {

//     template <typename F>
//     void slide_from(Location location,
//         const Board& board,
//         const Color& color_to_move,
//         std::span<const std::tuple<Location::FileShift, Location::RankShift>> steps,
//         const F& f)
//     {
//         auto opponent_color = invert_color(color_to_move);
//         for (const auto& step : steps) {
//             auto file_shift = std::get<0>(step);
//             auto rank_shift = std::get<1>(step);
//             std::optional<Location> offset_location = location;
//             while ((offset_location = offset_location->offset_by(file_shift, rank_shift))) {
//                 if (board.piece_at(*offset_location).is(color_to_move))
//                     break;

//                 f(*offset_location);

//                 if (board.piece_at(*offset_location).is(opponent_color))
//                     break;
//             }
//         }
//     }

//     template <typename F>
//     void jump_from(Location location,
//         const Board& board,
//         std::span<const std::tuple<Location::FileShift, Location::RankShift>> offsets,
//         const F& f)
//     {
//         for (const auto& hop : offsets) {
//             std::optional hop_location = location.offset_by(std::get<0>(hop), std::get<1>(hop));
//             if (hop_location && !board.piece_at(*hop_location).is(Color::White)) {
//                 f(*hop_location);
//             }
//         }
//     }

//     std::array<std::tuple<Location::FileShift, Location::RankShift>, 2> pawn_jumps = {
//         std::make_tuple(Location::Left, Location::Down),
//         std::make_tuple(Location::Right, Location::Down),
//     };

//     std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> knight_jumps = {
//         std::make_tuple(Location::Left * 2, Location::Down),
//         std::make_tuple(Location::Left * 2, Location::Up),
//         std::make_tuple(Location::Right * 2, Location::Down),
//         std::make_tuple(Location::Right * 2, Location::Up),
//         std::make_tuple(Location::Left, Location::Down * 2),
//         std::make_tuple(Location::Left, Location::Up * 2),
//         std::make_tuple(Location::Right, Location::Down * 2),
//         std::make_tuple(Location::Right, Location::Up * 2),
//     };

//     std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> king_jumps = {
//         std::make_tuple(Location::Left, Location::Down),
//         std::make_tuple(Location::Left, Location::Up),
//         std::make_tuple(Location::Right, Location::Down),
//         std::make_tuple(Location::Right, Location::Up),
//         std::make_tuple(Location::Left, Location::RankShift {}),
//         std::make_tuple(Location::Right, Location::RankShift {}),
//         std::make_tuple(Location::FileShift {}, Location::Down),
//         std::make_tuple(Location::FileShift {}, Location::Up),
//     };

//     std::array<std::tuple<Location::FileShift, Location::RankShift>, 4> bishop_slides = {
//         std::make_tuple(Location::Left, Location::Up),
//         std::make_tuple(Location::Left, Location::Down),
//         std::make_tuple(Location::Right, Location::Up),
//         std::make_tuple(Location::Right, Location::Down),
//     };

//     std::array<std::tuple<Location::FileShift, Location::RankShift>, 4> rook_slides = {
//         std::make_tuple(Location::Left, Location::RankShift {}),
//         std::make_tuple(Location::Right, Location::RankShift {}),
//         std::make_tuple(Location::FileShift {}, Location::Up),
//         std::make_tuple(Location::FileShift {}, Location::Down),
//     };

//     std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> queen_slides = {
//         std::make_tuple(Location::Left, Location::Up),
//         std::make_tuple(Location::Left, Location::Down),
//         std::make_tuple(Location::Right, Location::Up),
//         std::make_tuple(Location::Right, Location::Down),
//         std::make_tuple(Location::Left, Location::RankShift {}),
//         std::make_tuple(Location::Right, Location::RankShift {}),
//         std::make_tuple(Location::FileShift {}, Location::Up),
//         std::make_tuple(Location::FileShift {}, Location::Down),
//     };

//     const std::array<GeneratorFn, 7> pseudo_legal_moves_for_piece = {
//         [](std::vector<Move>& moves, Location location, const GameState&, const auto& threat_map) {
//             // Piece::Type::None
//         },
//         [](std::vector<Move>& moves, Location location, const GameState& game_state, const auto& threat_map) {
//             // Piece::Type::Pawn

//             auto one_hop = location.offset_by(Location::Down);
//             auto two_hop = location.offset_by(Location::Down * 2);

//             if (one_hop && game_state.board().piece_at(*one_hop).is_none()) {
//                 moves.emplace_back(location, *one_hop);

//                 if (two_hop && game_state.board().piece_at(*two_hop).is_none() && location.rank() == 1) {
//                     moves.emplace_back(location, *two_hop);
//                 }
//             }

//             jump_from(location, game_state.board(), pawn_jumps, [&](Location hop_location) {
//                 if (game_state.board().piece_at(hop_location).is(Color::Black)) {
//                     moves.emplace_back(location, hop_location);
//                 }
//             });
//         },
//         [](std::vector<Move>& moves, Location location, const GameState& game_state, const auto& threat_map) {
//             // Piece::Type::Knight
//             jump_from(location, game_state.board(), knight_jumps, [&](Location hop_location) {
//                 moves.emplace_back(location, hop_location);
//             });
//         },
//         [](std::vector<Move>& moves, Location location, const GameState& game_state, const auto& threat_map) {
//             // Piece::Type::Bishop
//             slide_from(location, game_state.board(), Color::White, bishop_slides, [&](Location slide_location) {
//                 moves.emplace_back(location, slide_location);
//             });
//         },
//         [](std::vector<Move>& moves, Location location, const GameState& game_state, const auto& threat_map) {
//             // Piece::Type::Rook
//             slide_from(location, game_state.board(), Color::White, rook_slides, [&](Location slide_location) {
//                 moves.emplace_back(location, slide_location);
//             });
//         },
//         [](std::vector<Move>& moves, Location location, const GameState& game_state, const auto& threat_map) {
//             // Piece::Type::Queen
//             slide_from(location, game_state.board(), Color::White, queen_slides, [&](Location slide_location) {
//                 moves.emplace_back(location, slide_location);
//             });
//         },
//         [](std::vector<Move>& moves, Location location, const GameState& game_state, const auto& threat_map) {
//             // Piece::Type::King
//             jump_from(location, game_state.board(), king_jumps, [&](Location hop_location) {
//                 moves.emplace_back(location, hop_location);
//             });

//             // Castling

//             auto is_any_occupied = [&](std::span<const Location> locations) {
//                 for (const auto& location : locations) {
//                     if (game_state.board().piece_at(location).exists()) {
//                         return true;
//                     }
//                 }

//                 return false;
//             };

//             auto is_any_attacked = [&](std::span<const Location> locations) {
//                 for (const auto& location : locations) {
//                     if (threat_map.is_attacked_by(location, Color::Black)) {
//                         return true;
//                     }
//                 }

//                 return false;
//             };

//             // Fast way we can look if white is in check
//             if (!threat_map.is_attacked_by(location, Color::Black)) {
//                 if (game_state.castle_rights()[Color::White].can_castle_kingside) {
//                     std::array<Location, 2> castle_through_squares = {
//                         Location::F1,
//                         Location::G1,
//                     };

//                     if (!is_any_attacked(castle_through_squares) && !is_any_occupied(castle_through_squares)) {
//                         moves.emplace_back(location, Location::G1);
//                     }
//                 }

//                 if (game_state.castle_rights()[Color::White].can_castle_queenside) {
//                     std::array<Location, 2> castle_through_squares = {
//                         Location::D1,
//                         Location::C1,
//                     };

//                     std::array<Location, 3> castle_between_squares = {
//                         Location::D1,
//                         Location::C1,
//                         Location::B1,
//                     };

//                     if (!is_any_attacked(castle_through_squares) && !is_any_occupied(castle_between_squares)) {
//                         moves.emplace_back(location, Location::C1);
//                     }
//                 }
//             }
//         },
//     };

//     const std::array<ThreatmappingFn, 7> threat_map_for_piece = {
//         [](Location, const Board&, const auto& f) {
//             // Piece::Type::None
//         },
//         [](Location location, const Board& board, const auto& f) {
//             // Piece::Type::Pawn
//             if (board.piece_at(location).is(Color::White)) {
//                 jump_from(location, board, pawn_jumps, f);
//             } else {
//                 std::array<std::tuple<Location::FileShift, Location::RankShift>, 8> black_pawn_jumps = {
//                     std::make_tuple(Location::Left, Location::Up),
//                     std::make_tuple(Location::Right, Location::Up),
//                 };

//                 jump_from(location, board, black_pawn_jumps, f);
//             }
//         },
//         [](Location location, const Board& board, const auto& f) {
//             // Piece::Type::Knight
//             jump_from(location, board, knight_jumps, f);
//         },
//         [](Location location, const Board& board, const auto& f) {
//             // Piece::Type::Bishop
//             slide_from(location, board, board.piece_at(location).color(), bishop_slides, f);
//         },
//         [](Location location, const Board& board, const auto& f) {
//             // Piece::Type::Rook
//             slide_from(location, board, board.piece_at(location).color(), rook_slides, f);
//         },
//         [](Location location, const Board& board, const auto& f) {
//             // Piece::Type::Queen
//             slide_from(location, board, board.piece_at(location).color(), queen_slides, f);
//         },
//         [](Location location, const Board& board, const auto& f) {
//             // Piece::Type::King
//             jump_from(location, board, king_jumps, f);
//         },
//     };
// }

} // namespace weechess
