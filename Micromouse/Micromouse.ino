/** Resources
  * https://docs.arduino.cc/learn/programming/bit-math
  * https://marsuniversity.github.io/ece387/FloodFill.pdf
  * http://craga89.github.io/Micromouse/
  * https://www.geeksforgeeks.org/set-clear-and-toggle-a-given-bit-of-a-number-in-c/
*/

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "globals.h"
#include "menu.h"
#include "floodfill.h"
#include "save_maze.h"

// Encoder Setup
Encoder myEnc1(2, 8);
Encoder myEnc2(3, 12);

const uint8_t rows = 16, cols = 16;

struct cell floodArray[rows * cols];  // This array stores the flood value and neighbour data for all the cells

uint8_t targetCells[4];  // This array stores the target cells

uint8_t startCell, currentCell, targetCell;                                           // startCell is the starting position
uint8_t startDir, leftDir, currentDir, rightDir, nextLeftDir, nextDir, nextRightDir;  // startDir is the initial orientation of the bot

short cellDirectionAddition[4] = { -rows, 1, rows, -1 };  // The location of a neighbouring cell can be obtained using the values in this dictionary
uint8_t targetScoreFromDirection[4] = { 0, 1, 2, 1 };

uint8_t readingCellLoc, readingCellDistance, readingCellScore, minNeighbourDistance, targetRelativeDirection, targetScore;
uint8_t distanceFromTarget = 1;

int sensorValue[7];

uint8_t resetMazeEEPROM = 0;  // This determines whether or not the maze data stored in the EEPROM should be reset
uint8_t menu = 0;             // This determines which value the encoder updates
short change;
uint8_t* values[7] = { &startCell, &(targetCells[0]), &(targetCells[1]), &(targetCells[2]), &(targetCells[3]), &startDir, &resetMazeEEPROM };

long newPosition1, newPosition2, oldPosition1, oldPosition2;

void setup() {
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
  pinMode(11, INPUT_PULLUP);
  pinMode(sensor_On_Pin, OUTPUT);
  oledSetup();
  updateMazeValuesFromEEPROM();
  oled.clear();
  oled.println("Configure");
  while (digitalRead(11)){}
  delay(500);
  newPosition1 = myEnc1.read(), newPosition2 = myEnc2.read(), oldPosition1 = myEnc1.read(), oldPosition2 = myEnc2.read();
  displayMenu();
  while (digitalRead(11)) updateEncoder();
  oled.clear();
  oled.println("Saving");
  if (resetMazeEEPROM) resetMazeValuesInEEPROM();
  else updateMazeValuesInEEPROM();
  resetEnc();
  oled.clear();
  oled.println("CALLIBRATE");
  while (digitalRead(11)) {};
  delay(1000);
  calibrate();
}

void loop() {
  currentCell = startCell;
  initialiseDirections();
  oled.clear();
  oled.println("START");
  while (digitalRead(11)) {}
  oled.clear();
  oled.println("Running");
  delay(1000);
  while (currentCell != targetCells[0] && currentCell != targetCells[1] && currentCell != targetCells[2] && currentCell != targetCells[3]) {
    updateWalls();
    flood();
    updateTargetCell();
    goToTargetCell();
    currentCell = targetCell;
    floodArray[currentCell].visited = 1;
  }
  updateMazeValuesInEEPROM();
}