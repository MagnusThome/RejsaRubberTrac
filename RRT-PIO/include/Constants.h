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

// Autozoom fail reasons
#define AUTOZOOM_SUCCESSFUL                 0 // Autozoom detected valid tire edges
#define TIRE_TOO_THIN                       1 // Detected tire does not have the minimum width (AUTOZOOM_MINIMUM_TIRE_WIDTH)
#define SLOPE_DELTA_TEMP_TOO_SMALL_INNER    2 // The minimum threshold for detecting the inner tire edge was not reached (i.e. temp delta not "sharp" enough)
#define SLOPE_DELTA_TEMP_TOO_SMALL_OUTER    3 // The minimum threshold for detecting the outer tire edge was not reached (i.e. temp delta not "sharp" enough)
#define AMBIENT_DELTA_TOO_SMALL_INNER       4 // Tire is not significantly hotter than inner ambient
#define AMBIENT_DELTA_TOO_SMALL_OUTER       5 // Tire is not significantly hotter than outer ambient
#define EDGE_NOT_IN_FOV                    23 // Inner or outer edge of tire out of camera view

#endif