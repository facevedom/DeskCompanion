#define DEBUG // comment to disable debug printing
#ifdef DEBUG
  // macros for debug printing
  #define DEBUG_PRINT(...)    Serial.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  // macros now define a blank line, taken care of by compiler
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
#endif
