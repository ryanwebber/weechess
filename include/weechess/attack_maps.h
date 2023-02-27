#pragma once

#include <weechess/bit_board.h>
#include <weechess/color_map.h>
#include <weechess/piece.h>

namespace weechess::attack_maps {

BitBoard generate_rook_attacks(Location location, BitBoard blockers);
BitBoard generate_bishop_attacks(Location location, BitBoard blockers);
BitBoard generate_queen_attacks(Location location, BitBoard blockers);

BitBoard generate_knight_attacks(Location location);
BitBoard generate_king_attacks(Location location);
BitBoard generate_pawn_attacks(Location location, Color color);

}
