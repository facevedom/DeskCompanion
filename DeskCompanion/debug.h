#define DEBUG // Comment to disable debug printing
#ifdef DEBUG
  // Macros for debug printing
  #define DEBUG_PRINT(...)    Serial.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  // Macros now define a blank line
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
#endif