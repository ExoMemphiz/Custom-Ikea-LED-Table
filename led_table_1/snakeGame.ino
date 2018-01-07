#define APPLE_COLOUR GREEN

int SNAKE_GAME_SPEED;  //Lower = Faster

const int DECREASE_AMOUNT = (SNAKE_GAME_SPEED - 50) / (NUM_LEDS - 3); //How much the game should speed up every time an apple is eaten.

int curLength;      //Curren length of snake
int snakeX[127];    //Array containing all snake segments,
int snakeY[127];    //max snake length is array length
int dir;            //Current Direction of snake
int prevDir;        //Saving previous direction to hinder self-collision when making a new direction switch
int score;          //Selfexplanatory

boolean snakeGameOver;

int appleX = 0;//Apple x position
int appleY = 0;//Apple y position


void snakeInit() {

  //Snake start position and direction & initialise variables
  curLength = 3;
  snakeX[0] = FIELD_WIDTH / 2;
  snakeX[1] = (FIELD_WIDTH / 2) - 1;
  snakeX[2] = (FIELD_WIDTH / 2) - 2;
  snakeY[0] = FIELD_HEIGHT / 2; //Sets starting position to middle of the grid
  snakeY[1] = FIELD_HEIGHT / 2; 
  snakeY[2] = FIELD_HEIGHT / 2;

  SNAKE_GAME_SPEED = 250;

  dir = DIR_RIGHT;
  prevDir = DIR_RIGHT;
  score = 0;
  //Generate random apple position
  createValidApple();
  
  snakeGameOver = false;
}

void runSnake(){
  snakeInit();
  unsigned long prevUpdateTime = 0;
  boolean snakeRunning = true;
  while (snakeRunning) {    
  
  //Check self-collision with snake
    int i = curLength - 1;
    while (curLength > 3 && i > 0) {  //i > 0 because head tile is at index [0]
      if (collide(snakeX[0], snakeX[i], snakeY[0], snakeY[i])) {
        Serial.println("Selfcollision");
        snakeGameOver = true;
      }
      i--;
    }
    
    if (snakeGameOver) {
      snakeRunning = false;
      break;
    }

    //Check collision of snake head with apple
    if (collide(snakeX[0], appleX, snakeY[0], appleY)) {
      //Increase score and snake length;
      score++;
      curLength++;
      //Add snake segment with temporary position of new segments
      snakeX[curLength - 1] = 255;    //curLength - 1 is the newest snake tail part. Temporarily store it outside of game until position shift
      snakeY[curLength - 1] = 255;    //curLength - 1 is the newest snake tail part. Temporarily store it outside of game until position shift
      
      SNAKE_GAME_SPEED -= DECREASE_AMOUNT;

      //Generate a valid placement for the apple coordinates
      createValidApple();
    }
    
    //Shift snake parts (except head, handled below) position by one.
    //Starts from the tail, moves towards head
    for (int i = curLength - 1; i >= 1; i--) {
      snakeX[i] = snakeX[i - 1];
      snakeY[i] = snakeY[i - 1];
    }
   
    //Determine new position of head of snake
    if (dir == DIR_RIGHT) {
      snakeX[0]++;
      prevDir = DIR_RIGHT;
    } else if (dir == DIR_LEFT) {
      snakeX[0]--;
      prevDir = DIR_LEFT;
    } else if (dir == DIR_UP) {
      snakeY[0]--;
      prevDir = DIR_UP;
    } else {    //DOWN
      snakeY[0]++;
      prevDir = DIR_DOWN;
    }
    
    //Check if outside plappleYing field
    if (snakeX[0] < 0) {
      snakeX[0] = FIELD_WIDTH -1;
    } else if (snakeX[0] >= FIELD_WIDTH) {
      snakeX[0] = 0;
    } else if (snakeY[0] < 0) {
      snakeY[0] = FIELD_HEIGHT -1;
    } else if (snakeY[0] >= FIELD_HEIGHT) {
      snakeY[0] = 0;
    }

    clearTablePixels();
   
    //Draw apple
    setTablePixel(appleX, appleY, GREEN);

    //Draw snake
    for (int i=0; i<curLength; i++) {
      setTablePixel(snakeX[i], snakeY[i], HOTPINK);
    }
    
    showPixels();

    //Check buttons and set snake movement direction while we are waiting to draw the next move
    unsigned long curTime;
    boolean dirChanged = false;
    do {
      readInput();  //Reads value for the variable "curControl"
      if (curControl == BTN_EXIT) {
        snakeRunning = false;
        break;
      }
      if (curControl != BTN_NONE && !dirChanged) { //Can only change (valid) direction once per loop
        dirChanged = setDirection();
      }
      curTime = millis();
    } while ((curTime - prevUpdateTime) < SNAKE_GAME_SPEED); //Wait for next tick
    prevUpdateTime = curTime;
  }
  char buf[4];
  int len = sprintf(buf, "%i", score);
  scrollTextBlocked(buf, len, WHITE);
  customFadeout();
}

/* Check if apple placement is not within the snake */
boolean isValidPlacement(int x, int y) {
  for (int i = 0; i < curLength; i++) {
    if (collide(snakeX[i], x, snakeY[i], y)) {  //If the apple is placed inside the snake
      return false;
    }
  }
  return true;
}

/* Sets a new valid X and Y position for the apple */
void createValidApple() {
  while (!isValidPlacement(appleX, appleY)) {
    appleX = random(FIELD_WIDTH-1);
    appleY = random(FIELD_HEIGHT-1);
  }
}

/* Set direction from current button state */
/* Ignores inputs if they kill the snake */
boolean setDirection() {
  switch(curControl) {
    case BTN_LEFT:
      if (prevDir != DIR_RIGHT) {
        dir = DIR_LEFT;
        return true;
      }
      break;
    case BTN_RIGHT:
      if (prevDir != DIR_LEFT) {
        dir = DIR_RIGHT;
        return true;
      }
      break;
    case BTN_DOWN:
      if (prevDir != DIR_UP) {
        dir = DIR_DOWN;
        return true;
      }
      break;
    case BTN_UP:
      if (prevDir != DIR_DOWN) {
        dir = DIR_UP;
        return true;
      }
      break;
    case BTN_START:
      break;
  }
  return false;
}

/* Collision detection function */
boolean collide(int x1, int x2, int y1, int y2) {
  return (x1 == x2 && y1 == y2);
}
