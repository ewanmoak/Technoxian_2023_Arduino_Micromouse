#ifndef globals_h
#define globals_h

#include <stdint.h>
#include <Encoder.h>

#define north 0
#define east 1
#define south 2
#define west 3

#define leftSensor 0
#define diagonalLeftSensor 1
#define centreSensor 2
#define diagonalRightSenson 3
#define rightSensor 4

#define wallThreshold 120

#define sensor_On_Pin 17

struct cell {
  uint8_t flood;
  uint8_t neighbours;
  uint8_t visited;
};

extern struct cell floodArray[];

extern const uint8_t rows, cols;

extern uint8_t startCell, currentCell, targetCell;
extern uint8_t startDir, leftDir, currentDir, rightDir, nextLeftDir, nextDir, nextRightDir;
extern uint8_t targetCells[];

extern short cellDirectionAddition[];
extern uint8_t targetScoreFromDirection[];

extern uint8_t readingCellLoc, readingCellDistance, readingCellScore, minNeighbourDistance, targetRelativeDirection, targetScore;
extern uint8_t distanceFromTarget;

extern uint8_t resetMazeEEPROM;

extern uint8_t *values[];

extern int sensorValue[];

extern long newPosition1, newPosition2, oldPosition1, oldPosition2;
extern uint8_t menu;
extern short change;

extern Encoder myEnc1;
extern Encoder myEnc2;

#endif