/** Resources
  * https://docs.arduino.cc/learn/programming/bit-math
  * https://marsuniversity.github.io/ece387/FloodFill.pdf
  * http://craga89.github.io/Micromouse/
  * https://www.geeksforgeeks.org/set-clear-and-toggle-a-given-bit-of-a-number-in-c/
*/

#include "src/CircularBufferQueue/CircularBufferQueue.h"

#define rows 16
#define cols 16

// Matrix macros
#define linearise(row, col) row* cols + col
#define delineariseRow(location) location / cols
#define delineariseCol(location) location % cols

// Wall macros
#define distance(loc1, loc2) abs(delineariseRow(loc1) - delineariseRow(loc2)) + abs(delineariseCol(loc1) - delineariseCol(loc2))
#define markWall(location, direction) floodArray[location].neighbours |= 1 << direction
#define wallExists(location, direction) bitRead(floodArray[location].neighbours, direction)

// Neighbour macros
#define getNeighbourLocation(location, direction) (byte)((short)location + cellDirectionAddition[direction])  // Calculates the location of neighbour
#define getNeighbourDistanceIfAccessible(location, direction) floodArray[getNeighbourLocation(location, direction)].flood
#define getNeighbourDistance(location, direction) wallExists(location, direction) ? 255 : getNeighbourDistanceIfAccessible(location, direction)

// Direction macros
#define updateDirection(currentDirection, turn) *currentDirection = (*currentDirection + turn) % 4  // Updates the passed direction

#define north 0
#define east 1
#define south 2
#define west 3

#define rightTurn 1
#define uTurn 2
#define leftTurn 3

#define leftSensor 0
#define diagonalLeftSensor 1
#define centreSensor 2
#define diagonalRightSenson 3
#define rightSensor 6

#define threshold 200

struct cell {
  byte flood;
  byte neighbours;
};

cell floodArray[rows * cols];  // This array stores the flood value and neighbour data for all the cells

byte targetCells[] = { linearise(7, 7), linearise(7, 8), linearise(8, 7), linearise(8, 8) };  // This array stores the target cells

CircularBufferQueue floodQueue(256);  // This queue stores the cells that need to be flooded

byte currentCell = linearise(0, 0), targetCell;
byte leftDir = north, currentDir = east, rightDir = south;
short cellDirectionAddition[4] = { -rows, 1, rows, -1 };  // The location of a neighbouring cell can be obtained using the values in this dictionary
byte updateDirectionTurnAmount[4] = { 0, rightTurn, uTurn, leftTurn };
byte targetScoreFromDirection[4] = { 0, 1, 2, 1 };

byte readingCellLoc, readingCellDistance, readingCellScore, minNeighbourDistance, targetRelativeDirection, targetScore;

void setup() {
  for (byte i = 0; i < (rows * cols); i++) {
    floodArray[i].flood = 255;
    for (byte j = 0; j < 4; j++) floodArray[i].flood = min(floodArray[i].flood, distance(i, targetCells[j]));
    floodArray[i].neighbours = 0;
    if (delineariseRow(i) == 0) markWall(i, north);
    if (delineariseCol(i) == 0) markWall(i, west);
    if (delineariseRow(i) == (rows - 1)) markWall(i, south);
    if (delineariseCol(i) == (cols - 1)) markWall(i, east);
    if (i == 255) break;
  }
}

void loop() {
  while (currentCell != targetCells[0] && currentCell != targetCells[1] && currentCell != targetCells[2] && currentCell != targetCells[3]) {
    updateWalls();
    flood();
    updateTargetCell();
    goToTargetCell();
    currentCell = targetCell;
  }
}

void flood() {
  floodQueue.enqueue(currentCell);
  while (!floodQueue.isEmpty()) {
    readingCellLoc = *floodQueue.dequeue();
    if (isEnclosed(readingCellLoc)) continue;
    readingCellDistance = floodArray[readingCellLoc].flood;
    minNeighbourDistance = 255;
    for (byte i = 0; i < 4; i++) {
      minNeighbourDistance = min(minNeighbourDistance, getNeighbourDistance(readingCellLoc, i));
    }
    if (minNeighbourDistance != readingCellDistance - 1) {
      floodArray[readingCellLoc].flood = minNeighbourDistance + 1;
      for (byte i = 0; i < 4; i++) {
        if (isNeighbourValid(readingCellLoc, i)) {
          if (!isDestination(getNeighbourLocation(readingCellLoc, i))) {
            floodQueue.enqueue(getNeighbourLocation(readingCellLoc, i));
          }
        }
      }
    }
  }
}

void updateTargetCell() {
  minNeighbourDistance = getNeighbourDistance(currentCell, 0);
  targetScore = 3;
  for (byte i = 0; i < 4; i++) {
    if (!wallExists(currentCell, i)) {
      readingCellLoc = getNeighbourLocation(currentCell, i);
      readingCellDistance = getNeighbourDistance(currentCell, i);
      readingCellScore = targetScoreFromDirection[getTargetRelativeDirection(readingCellLoc)];
      if ((readingCellDistance < minNeighbourDistance) || ((readingCellDistance == minNeighbourDistance) && (readingCellScore < targetScore))) {
        minNeighbourDistance = readingCellDistance;
        targetScore = readingCellScore;
        targetCell = readingCellLoc;
      }
    }
  }
}

void goToTargetCell() {
  targetRelativeDirection = getTargetRelativeDirection(targetCell);
  if (targetRelativeDirection == north) {
    // motor function to go straight
  } else if (targetRelativeDirection == east) {
    // motor function to take a left-turn
  } else if (targetRelativeDirection == south) {
    // motor function to take a u-turn
  } else if (targetRelativeDirection == west) {
    // motor function to take a left-turn
  }
  updateDirection(&leftDir, updateDirectionTurnAmount[targetRelativeDirection]);
  updateDirection(&currentDir, updateDirectionTurnAmount[targetRelativeDirection]);
  updateDirection(&rightDir, updateDirectionTurnAmount[targetRelativeDirection]);
}

void updateWalls() {
  if (analogRead(leftSensor) > threshold) {
    markWall(currentCell, leftDir);
    if (isNeighbourValid(currentCell, leftDir)) {
      markWall(getNeighbourLocation(currentCell, leftDir), (leftDir + 2) % 4);
    }
  }
  if (analogRead(centreSensor) > threshold) {
    markWall(currentCell, currentDir);
    if (isNeighbourValid(currentCell, currentDir)) {
      markWall(getNeighbourLocation(currentCell, currentDir), (currentDir + 2) % 4);
    }
  }
  if (analogRead(rightSensor) > threshold) {
    markWall(currentCell, rightDir);
    if (isNeighbourValid(currentCell, rightDir)) {
      markWall(getNeighbourLocation(currentCell, rightDir), (rightDir + 2) % 4);
    }
  }
}

bool isNeighbourValid(byte location, byte direction) {
  if (direction == north) return delineariseRow(location) > 0;
  else if (direction == east) return delineariseCol(location) < (cols - 1);
  else if (direction == south) return delineariseRow(location) < (rows - 1);
  else if (direction == west) return delineariseCol(location) > 0;
}

byte getTargetAbsoluteDirection(byte target) {
  short diff = (short)target - (short)currentCell;
  if (diff == -rows) return north;
  if (diff == 1) return east;
  if (diff == rows) return south;
  if (diff == -1) return west;
}

byte getTargetRelativeDirection(byte target) {
  return (getTargetAbsoluteDirection(target) + 4 - currentDir) % 4;
}

bool isDestination(byte location) {
  return floodArray[location].flood == 0;
}

bool isEnclosed(byte location) {
  // 15 is 00001111 in binary, which means that there are walls in 4 all 4 directions of the cell
  return floodArray[location].neighbours == 15;
}