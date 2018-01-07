/**
 * @file simplePong.ino
 * @author Christopher Poul Rosendorf
 * @date 05/01/2018
 * @version 0.3
 * 
 * A pong game where the bot will play the right paddle, and the player controls the left paddle.
 * The ball has a random starting direction from the center of the field.
 * As the game progresses, the ball will move faster, to avoid infinite games.
 * After each goal, the score of the player and bot - respectively - will be displayed.
 * 
**/

#define PADDLE_WIDTH 3
#define PLAYER_COLOUR BLUE
#define BOT_COLOUR RED
#define MAX_SCORE 5
#define BOT_SPEED 550

#define MOVE_UP -1
#define MOVE_DOWN 1
#define DONT_MOVE 0
#define MOVE_LEFT -1
#define MOVE_RIGHT 1

#define MINIMUM_GAME_SPEED 50
int PONG_GAME_SPEED;

int ballX, ballY, xVelocity, yVelocity;

int playerPaddleX, playerPaddleY;
int botPaddleX, botPaddleY;

int playerScore, botScore;

boolean simplePongRunning;

int nextPlayerDirection, lastBotDecision;

void initSimplePong() {
    PONG_GAME_SPEED = 300;
    ballX = FIELD_WIDTH / 2;    //Start in the middle (X Coord) of the screen
    ballY = FIELD_HEIGHT / 2;   //Start in the middle (Y Coord) of the screen
    xVelocity = getRandomDirection(); //random(-1, 2);  //Random int between -1 and 1.     2 is exclusive
    yVelocity = 0; //random(-1, 2);
    playerPaddleX = 0;              //Start on the left side of the screen
    playerPaddleY = FIELD_HEIGHT / 2;   //Start in the middle of the left side of the screen
    botPaddleX = FIELD_WIDTH - 1;   //Start on the right side of the screen
    botPaddleY = FIELD_HEIGHT / 2;   //Start in the middle of the right side of the screen
    simplePongRunning = true;
    playerScore = 0;
    botScore = 0;
    lastBotDecision = millis();
}

void runSimplePong() {
    initSimplePong();

    while (simplePongRunning) {
        if (playerScore == MAX_SCORE || botScore == MAX_SCORE) {
            break;
        }
        drawGame();
        long startTime = millis();
        do {
            handleController();
        } while (millis() - startTime < PONG_GAME_SPEED);

        if (checkCollisions()) {
            PONG_GAME_SPEED -= (10 + playerScore * 5);  //Increasingly difficult, speed increases each goal player has scored
            if (PONG_GAME_SPEED <= MINIMUM_GAME_SPEED) {
                PONG_GAME_SPEED = MINIMUM_GAME_SPEED;
            }
        }

        if (checkGoal()) {
            resetAfterGoal();
            drawScore();
            continue;
        }
        moveBall();
        movePlayers();
    }
    if (playerScore == MAX_SCORE) { //Player won, congratulate
        char buf[12];
        int len = sprintf(buf, "Win %i:%i", playerScore, botScore);
        scrollTextBlocked(buf, len, GREEN);
    } else if (botScore == MAX_SCORE) {
        char buf[13];
        int len = sprintf(buf, "Lost %i:%i", playerScore, botScore);
        scrollTextBlocked(buf, len, RED);
    }
    customFadeout();
}

/** Game logic **/

void resetAfterGoal() {
    PONG_GAME_SPEED = 300;
    ballX = FIELD_WIDTH / 2;    //Start in the middle (X Coord) of the screen
    ballY = FIELD_HEIGHT / 2;   //Start in the middle (Y Coord) of the screen
    xVelocity = getRandomDirection();  //Random int between -1 and 1.     2 is exclusive
    yVelocity = 0;              //random(-1, 2);
    botPaddleY = FIELD_HEIGHT / 2;
    playerPaddleY = FIELD_HEIGHT / 2;
    lastBotDecision = millis();
}

int getRandomDirection() {
    int random1 = random(0, 2);     //Either 0, or 1            random(n1, n2); (n1 is inclusive, n2 is exclusive)
    switch (random1) {
        case 0:
            return MOVE_LEFT;
        case 1:
            return MOVE_RIGHT;
        default:
            return MOVE_LEFT;
    }
}

void moveBall() {
    ballX += xVelocity;
    if (ballY <= 0 && yVelocity == MOVE_UP) {
        yVelocity = MOVE_DOWN;
        int extraTile = random(0, 10);
        if (extraTile == 3) {
            ballY += yVelocity;     //To ensure no "stalemate" of game
        }
    } else if (ballY >= FIELD_HEIGHT - 1 && yVelocity == MOVE_DOWN) {
        yVelocity = MOVE_UP;
        int extraTile = random(0, 10);
        if (extraTile == 3) {
            ballY += yVelocity;     //To ensure no "stalemate" of game
        }
    }
    ballY += yVelocity;
}

boolean checkCollisions() {
    if (paddleCollision(playerPaddleX, playerPaddleY, 1)) {     //Ball collided with player
        if (ballY > playerPaddleY) {    //Ball is further down than middle of player paddle
            yVelocity = MOVE_DOWN;
        } else if (ballY < playerPaddleY) {  //Ball is further up than middle of player paddle
            yVelocity = MOVE_UP;
        }
        xVelocity = MOVE_RIGHT;
        return true;
    } else if (paddleCollision(botPaddleX, botPaddleY, -1)) {
        if (ballY > playerPaddleY) {    //Ball is further down than middle of player paddle
            yVelocity = MOVE_DOWN;
        } else if (ballY < playerPaddleY) {  //Ball is further up than middle of player paddle
            yVelocity = MOVE_UP;
        }
        xVelocity = MOVE_LEFT;
        return true;
    }  
    return false;
}

boolean paddleCollision(int x, int y, int xOffset) {
    for (int i = y - getOuterPaddlePieces(); i <= y + getOuterPaddlePieces(); i++) {    //Check from either side of the paddle
        if (collision(x + xOffset, ballX, i, ballY)) {
            return true;
        }
    }
    return false;
}

boolean collision(int x1, int x2, int y1, int y2) {
    return x1 == x2 && y1 == y2;
}

boolean checkGoal() {
    if (ballX == 0) {   //x == left side
        botScore++;
        return true;
    } else if (ballX == FIELD_WIDTH - 1) {
        playerScore++;
        return true;
    }
    return false;
}

void handleController() {
    readInput();
    switch (curControl) {
        case BTN_EXIT:
            simplePongRunning = false;
            return;
        case BTN_DOWN:
            if (playerPaddleY + getOuterPaddlePieces() < FIELD_HEIGHT - 1) {
                nextPlayerDirection = MOVE_DOWN;
            } else {
                nextPlayerDirection = DONT_MOVE;
            }
            break;
        case BTN_UP:
            //Upper paddle Y = playerPaddleY - getOuterPaddlePieces
            if (playerPaddleY - getOuterPaddlePieces() > 0) {
                nextPlayerDirection = MOVE_UP;
            } else {
                nextPlayerDirection = DONT_MOVE;
            }
            break;
    }
}

void movePlayers() {
    playerPaddleY += nextPlayerDirection;
    nextPlayerDirection = DONT_MOVE;
    //Bot logic
    if (millis() - lastBotDecision > BOT_SPEED) {   //If enough time has passed since last bot move, move bot paddle.
        if (ballY > botPaddleY) {      //If ball is further down than bot
            int shouldMove = random(0, 100);
            if (shouldMove > (40 - playerScore * 7) && botPaddleY < FIELD_HEIGHT - 2) { //Makes the AI less random for every goal the player scores
                botPaddleY += MOVE_DOWN;
            }
        } else if (ballY < botPaddleY) {      //If ball is further up than bot
            int shouldMove = random(0, 100);
            if (shouldMove > (40 - playerScore * 7) && botPaddleY > 1) {
                botPaddleY += MOVE_UP;
            }
        }
        lastBotDecision = millis();
    }
}

/** Drawing methods **/

void drawGame() {
    clearTablePixels();
    drawBall();
    drawPaddles();
    showPixels();
}

void drawPaddles() {
    for (int y = playerPaddleY - getOuterPaddlePieces(); y <= playerPaddleY + getOuterPaddlePieces(); y++) {
        setTablePixel(playerPaddleX, y, PLAYER_COLOUR);
    }
    for (int y = botPaddleY - getOuterPaddlePieces(); y <= botPaddleY + getOuterPaddlePieces(); y++) {
        setTablePixel(botPaddleX, y, BOT_COLOUR);
    }
}

void drawBall() {
    setTablePixel(ballX, ballY, WHITE);
}

void drawScore() {
    char buf[3];
    int len = sprintf(buf, "%i:%i", playerScore, botScore);
    scrollTextBlocked(buf, len, WHITE);
}

/** Utility methods **/

int getOuterPaddlePieces() {
    //return 1;
    return PADDLE_WIDTH / 2;
}