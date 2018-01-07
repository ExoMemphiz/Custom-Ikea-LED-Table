/**
 * @file customFadeout.ino
 * @author Christopher Poul Rosendorf
 * @date 05/01/2018
 * @version 0.1
 * 
 * An inward spiraling fadeout transition.
 * 
**/

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

int direction, x, y;

void customFadeoutInit() {
    direction = WEST;
    x = FIELD_WIDTH - 1;
    y = 0;
}

void customFadeout() {
    customFadeoutInit();
    //blink();
    setAllLEDs(WHITE);
    inwardSpiral();
}

void blink() {
    setAllLEDs(BLACK);
    delay(200);
    setAllLEDs(WHITE);
    delay(200);
    setAllLEDs(BLACK);
    delay(200);
    setAllLEDs(WHITE);
    delay(200);
}

void setAllLEDs(int colour) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
        for (int y = 0; y < FIELD_HEIGHT; y++) {
            setTablePixel(x, y, colour);
        }
    }
    showPixels();
}

void inwardSpiral() {
    int count = FIELD_HEIGHT * FIELD_WIDTH;
    for (int i = 0; i < count - 1; i++) {
        setTablePixel(x, y, BLACK);
        showPixels();
        delay(16);
        updateCoordinates(x, y);
    }

}

void updateCoordinates(int &x, int &y) {
    switch (direction) {
        case NORTH: {
            int n = getTablePixel(x, y - 1);
            if (y <= 0 || n == BLACK) {
                direction = WEST;
                updateCoordinates(x, y);
                return;
            } else {
                y--;
            }
            break;
        }
            
        case EAST: {
            int e = getTablePixel(x + 1, y);
            if (x >= FIELD_WIDTH - 1 || e == BLACK) {
                direction = NORTH;
                updateCoordinates(x, y);
                return;
            } else {
                x++;
            }
            break;
        }
            
        case SOUTH: {
            int s = getTablePixel(x, y + 1);
            if (y >= FIELD_HEIGHT - 1 || s == BLACK) {
                direction = EAST;
                updateCoordinates(x, y);
                return;
            } else {
                y++;
            }
            break;
        }
            
        case WEST: {
            int w = getTablePixel(x - 1, y);
            if (x <= 0 || w == BLACK) {
                direction = SOUTH;
                updateCoordinates(x, y);
                return;
            } else {
                x--;
            }
            break;
        }
    }
}

int getTablePixel(int x, int y) {
    #ifdef ORIENTATION_HORIZONTAL
        return getPixel(y%2 ? y*FIELD_WIDTH + x : y*FIELD_WIDTH + ((FIELD_HEIGHT-1)-x));
    #else
        return getPixel(x%2 ? x*FIELD_WIDTH + ((FIELD_HEIGHT-1)-y) : x*FIELD_WIDTH + y);
    #endif
}