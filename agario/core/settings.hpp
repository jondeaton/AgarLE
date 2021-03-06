#pragma once

#define SEED 42

#define CELL_MIN_SIZE 10
#define CELL_MAX_SPEED 200
#define CELL_SPLIT_MINIMUM 35
#define SPLIT_DECELERATION 100

#define FOOD_SPEED 100
#define FOOD_DECEL 80

#define RECOMBINE_TIMER_SEC 30

// must be `CELL_EAT_MARGIN` times larger
// than another cell in order to eat it
#define CELL_EAT_MARGIN 1.1

#define MASS_AREA_RADIO 1.0

// the (approximate) amount by which a cell is reduced in size
// when it collides with a virus
#define CELL_POP_REDUCTION 2
#define CELL_POP_SIZE  25

#define DEFAULT_ARENA_WIDTH 500
#define DEFAULT_ARENA_HEIGHT 500

#define DEFAULT_NUM_PELLETS 1024
#define DEFAULT_NUM_VIRUSES 25
#define PLAYER_CELL_LIMIT 25