#ifndef Constants_h
#define Constants_h


// FIS identifiers
#define FIS_DUMMY    0
#define FIS_MLX90640 1
#define FIS_MLX90621 2

// Distance sensor identifiers
#define DIST_NONE    0
#define DIST_VL53L0X 1

// Boards
#define BOARD_NRF52_FEATHER  1
#define BOARD_ESP32_FEATHER  2
#define BOARD_ESP32_LOLIND32 3

// Column aggregation algo identifiers
#define COLUMN_AGGREGATE_MAX 1
#define COLUMN_AGGREGATE_AVG 2
#define COLUMN_AGGREGATE_AVG_MINUS_OUTLIERS 3

#endif