#pragma once

#define SEED 42

#define CELL_MIN_SIZE 10
#define CELL_MAX_SPEED 10

// must be `CELL_EAT_MARGIN` times larger
// than another cell in order to eat it
#define CELL_EAT_MARGIN 1.1

#define MASS_AREA_RADIO 1.0

// the (approximate) amount by which a cell is reduced in size
// when it collides with a virus
#define CELL_POP_REDUCTION 2
#define CELL_POP_SIZE  25
#define CELL_POP_SPEED 25


#define DEFAULT_ARENA_WIDTH 1000
#define DEFAULT_ARENA_HEIGHT 1000