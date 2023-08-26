#ifndef floodfill_h
#define floodfill_h

#define absolute(number) (((number) > 0)? (number) : (-(number)))
#define minimum(num1, num2) (((num1) < (num2))? (num1) : (num2))

void turn(int angle, int speed);
void alignFront();
void moveForward(long blocks, int speed);
void readWall();

// Matrix macros
#define linearise(row, col) (row* cols + col)
#define delineariseRow(location) (location / cols)
#define delineariseCol(location) (location % cols)

// Wall macros
#define distance(loc1, loc2) (absolute(delineariseRow(loc1) - delineariseRow(loc2)) + absolute(delineariseCol(loc1) - delineariseCol(loc2)))
#define markWall(location, direction) (floodArray[location].neighbours |= 1 << direction)
#define wallExists(location, direction) (floodArray[location].neighbours & ~(1 << direction))

// Neighbour macros
#define getNeighbourLocation(location, direction) ((uint8_t)((short)location + cellDirectionAddition[direction]))  // Calculates the location of neighbour
#define getNeighbourDistanceIfAccessible(location, direction) (floodArray[getNeighbourLocation(location, direction)].flood)
#define getNeighbourDistance(location, direction) (wallExists(location, direction) ? 255 : getNeighbourDistanceIfAccessible(location, direction))

// Direction macros
#define updateDirection(currentDirection, turn) *currentDirection = (*currentDirection + turn) % 4  // Updates the passed direction

void flood(void);
void updateTargetCell(void);
void goToTargetCell(void);
void updateWalls(void);
bool isNeighbourValid(uint8_t location, uint8_t direction);
uint8_t getTargetAbsoluteDirection(uint8_t target);
uint8_t getTargetRelativeDirection(uint8_t target);
bool isDestination(uint8_t location);
bool isEnclosed(uint8_t location);
bool isTunnel(uint8_t location);
void initialiseDirections(void);

#endif