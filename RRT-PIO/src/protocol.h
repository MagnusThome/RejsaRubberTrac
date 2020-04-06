#define PROTOCOL 0x02

typedef struct {
  uint8_t  protocol;         // version of protocol used
  uint8_t  unused;
  int16_t  distance;         // millimeters
  int16_t  temps[8];         // all even numbered temp spots (degrees Celsius x 10)
} one_t;


typedef struct {
  uint8_t  protocol;         // version of protocol used
  uint8_t  charge;           // percent: 0-100
  uint16_t voltage;          // millivolts (normally circa 3500-4200)
  int16_t  temps[8];         // all uneven numbered temp spots (degrees Celsius x 10)
} two_t;


typedef struct {
  uint8_t  protocol;         // version of protocol used
  uint8_t  unused;
  int16_t distance;          // millimeters
  int16_t  temps[8];         // all 16 temp spots averaged together in pairs of two and two into 8 temp values (degrees Celsius x 10)
} thr_t;
