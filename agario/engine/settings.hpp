#ifndef AGARIO_SETTINGS_HPP
#define AGARIO_SETTINGS_HPP

#define SEED 42

#define PELLET_SIZE 1
#define PELLET_MASS 1

#define FOOD_SIZE 10
#define FOOD_MASS 10
#define FOOD_SPEED 10

#define VIRUS_SIZE 75
#define VIRUS_MASS 0

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


#endif //AGARIO_SETTINGS_HPP
