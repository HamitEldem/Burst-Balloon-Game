#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 600
#define TIMER_PERIOD   20     // Period for the timer
#define TIMER_KNIFE  200
#define PI  3.14159265358

// Global constant variables for this program
#define RADIUS 10
#define HG      4.9f          // Half Gravity

bool spacebar = false;


int  winWidth, winHeight;     // Current Window width and height
int r = 180, g = 172, b = 50; // Initial color

// BALLOON
double xB[5], yB[5]; // array of balloon coordinates
int radiusB = 22; // BALLOON's radius
int curBal = 0;
int hitCount = 0; //hitcount for scoreboard
int boardX = 400;
int boardY = 250;

float Px = -300, Py = 0;      // Coordinates of the platform
float Cx = -200, Cy = Py + 75;; // x and y coordinate of the knife
float c, h, k;                // Parameters of the equation
float Time = 30; //playTime for game
float boatHitX, boatHitSpd, ballHitX;
int hitcount = 0;
float carX = WINDOW_WIDTH / 2 - 20; //car is just for fun
float carY = -WINDOW_HEIGHT / 2 + 30;
bool startFlag = true;
bool flagForF1 = true;

int   Vx = 90;                // Horizontal firing speed
int   steps;

bool  animation = false;      // Flag to show if the knife is fired/active
bool  gameover = true;       // Flag for game status

//chnage the team of the thrower (galatasaray:1, trabzonspor:2, fenerbahce:3, beşiktaş:4
int throwerTeam = 1;

// To draw a filled circle, centered at (x,y) with radius r
void circle(int x, int y, int r) {
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++) {
        angle = 2 * PI*i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void print(int x, int y, const char* string, void* font) {
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, string[i]);
}

// To display text with variables
void vprint(int x, int y, void* font, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, str[i]);
}



void drawGradient(int x1, int y1, int w, int h, float r, float g, float b) {
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(x1, y1);
    glVertex2f(x1 + w, y1);
    glColor3f(r + 0.4, g + 0.4, b + 0.4);
    glVertex2f(x1 + w, y1 - h);
    glVertex2f(x1, y1 - h);
    glEnd();

    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x1 + w, y1);
    glVertex2f(x1 + w, y1 - h);
    glVertex2f(x1, y1 - h);
    glEnd();
}

void drawCloud(int x, int y) {
    glColor3f(1, 1, 1); // White
    circle(0 + x, 0 + y, 30);
    circle(-25 + x, y, 20);
    circle(25 + x, -2 + y, 20);
    circle(21 + x, -19 + y, 10);
}


void initBalloonPos() {
    for (int i = 0; i < 5; i++) {
        xB[i] = 100 * i;
        yB[i] = -100 * i;
    }
}



void drawBackground() {
    drawGradient(-300, 300, 1000, 600, 0.3, 0.2, 1); // Sky
    drawGradient(-300, -200, 1000, 100, 0.4, 0.4, 0.4);  // road


    // Title
    glColor3f(1, 1, 1);
    print(-100, 275, "Balloon Game // Hamit Efe Eldem", GLUT_BITMAP_TIMES_ROMAN_24);

    // Bottom info panel
    glColor3f(0, 0, 0);
    glRectf(-400, -280, 600, -300);

    // Info message
    glColor3f(1, 1, 0); // Yellow
    print(-280, -295, "SpaceBar: Fire/Restart     Up/Down: Platform Height     Left/Right: Increase/Decrease Speed", GLUT_BITMAP_8_BY_13);

    // Draw 5 clouds
    drawCloud(-160, 180);
    drawCloud(-10, 100);
    drawCloud(90, 200);
    drawCloud(+340, 180);
    drawCloud(+190, 100);

    



}

void drawPlatform(int y) {
    drawGradient(-300 + 50, y, 100, y + 200, 1, 0.5, 0.0);
}

// draw the weapon (knife)
void drawknife() {
    glColor3f(0.8, 0.5, 0.7);
    glRectf(Cx + 5, Cy, Cx + 30, Cy + 5);


    glRectf(Cx + 30, Cy + 5, Cx + 60, Cy - 7);

    glBegin(GL_TRIANGLES);
    glVertex2f(Cx + 60, Cy + 5);
    glVertex2f(Cx + 60, Cy - 7);
    glVertex2f(Cx + 70, Cy - 1);
    glEnd();



}


//draw the taxi in the bottom of the window
void drawCar() {

    glColor3f(.9, 0.8, 0.);
    glRectf(carX, carY, carX - 100, carY + 30);
    glRectf(carX - 15, carY, carX - 85, carY + 40);

    glBegin(GL_TRIANGLES);
    glVertex2f(carX - 100, carY + 30);
    glVertex2f(carX - 70, carY + 30);
    glVertex2f(carX - 85, carY + 40);

    glBegin(GL_TRIANGLES);
    glVertex2f(carX, carY + 30);
    glVertex2f(carX - 30, carY + 30);
    glVertex2f(carX - 15, carY + 40);
    glEnd();

    glColor3f(0.3, 0, 0.);
    circle(carX - 75, carY, 8);
    circle(carX - 25, carY, 8);


    glRectf(carX - 60, carY + 15, carX - 80, carY + 30);
    glRectf(carX - 40, carY + 15, carX - 20, carY + 30);



}

void drawBalloon(int xibal, int yibal) {
    // Part 1 : String/rope
    glLineWidth(3);
    glColor3f(0, 0, 0.72);
    glBegin(GL_LINES);
    glVertex2f(xibal, yibal - radiusB);
    glVertex2f(xibal, yibal - radiusB - radiusB * 0.8);
    glEnd();
    glLineWidth(1);

    // Part 2 : Body
    glColor3ub(r, g, b);
    circle(xibal, yibal, radiusB);

    glColor3ub(r, g, b);
    // Part 3 : Tip of the body
    glBegin(GL_TRIANGLES);
    glVertex2f(xibal, yibal - (radiusB * 0.66));
    glVertex2f(xibal - (radiusB / 5), yibal - (radiusB * 1.2));
    glVertex2f(xibal + (radiusB / 5), yibal - (radiusB * 1.2));
    glEnd();

    // Part 4 : Light Reflection
    glColor3f(0.9, 0.9, 0.9);
    circle(xibal + (radiusB / 3), yibal + (radiusB / 3), (radiusB / 3));
}

void constructEquation() {

    h = Cx;
    k = Cy;
}

//if knife hits the target, it will be deleted and hitcount will be incremented
void calchit() {

    for (int i = 0; i < 5; i++) {
        if ((xB[i] + 25 >= Cx && xB[i] - 25 <= Cx) && (yB[i] + 40 >= Cy && yB[i] - 30 <= Cy)) {
            hitcount++;
            xB[i] = rand() % 400 + 50*i;
            yB[i] =-1*  rand() % 350 + 30*i;
            drawBalloon(xB[i], yB[i]);
        }
    }


}

//information screen on the left side on thw window consisting of remaining time, score, and speed of the knife
void informationScreen() {
    glColor3f(0.9, 0.9, 0.9);
    circle(-400, 200, 70);



    glColor3f(0.9, 0.9, 0.9);
    glRectf(-480, -50, -320, 50);

    glColor3f(0.9, 0.9, 0.9);
    glRectf(-490, -200, -310, -150);



    glColor3f(0.0, 0.0, 0.0);

    vprint(-250, -80, GLUT_BITMAP_8_BY_13, "Press 1: GS");
    vprint(-250, -100, GLUT_BITMAP_8_BY_13, "Press 2: BJK");
    vprint(-250, -120, GLUT_BITMAP_8_BY_13, "Press 3: FB");
    vprint(-250, -140, GLUT_BITMAP_8_BY_13, "Press 4: TS");

    glColor3f(0.8, 0.2, 0.0);
    if (!gameover) {
        vprint(-470, 0, GLUT_BITMAP_9_BY_15, "Time: %.2f sec", Time);
        vprint(-475, -175, GLUT_BITMAP_9_BY_15, "Total Points :  %d", hitcount);

        if (Time >= 0) {
            glColor3f(0.8, 0.2, 0.0);
            vprint(-453, 200, GLUT_BITMAP_9_BY_15, "Speed=%d m/s", Vx);

            if(flagForF1 == true)
                Time -= 0.02;
        }
        else {
            gameover = true;
            glColor3f(1, 1., 1.);
            glRectf(-600, -300, -400, 300);



        }
    }


}


// Initial position of the knife and reset it every time its out of the screen
void resetknife() {
    animation = false;
    Cx = -200;
    Cy = Py + 75;


}

//draw the player
void drawThrower() {

    glColor3f(1, 0.9, 0.7);
    circle(-200, Py + 100 + 20, radiusB + 5);
    glColor3f(0, 0, 0);

    if (flagForF1 == true) {
        print(-220, Py + 95 + 20, "*  *", GLUT_BITMAP_TIMES_ROMAN_24);
        print(-220, Py + 85 + 20, "\\___/", GLUT_BITMAP_8_BY_13);
        glColor3f(1, 0.9, 0.7);
        glRectf(-210, Py + 78 + 20, -190, Py + 65 + 20);
    }
    else {
        print(-220, Py + 95 + 20, "*  *", GLUT_BITMAP_TIMES_ROMAN_24);
        print(-200, Py + 85 + 20, "<3", GLUT_BITMAP_8_BY_13);
        glColor3f(1, 0.9, 0.7);
        glRectf(-210, Py + 98, -190, Py +85);
    }

    switch (throwerTeam) {
    case 1:
        glColor3f(1, 0.09, 0.15);
        glRectf(-225, Py + 65 + 23, -175, Py + 15 + 20);
        glColor3f(1, 0.84, 0.0);
        glRectf(-225, Py + 65 + 23, -200, Py + 15 + 20);
        break;
    case 2:
        glColor3f(0.0, 0.00, 0);
        glRectf(-225, Py + 65 + 23, -175, Py + 15 + 20);
        glColor3f(1, 1, 1.0);
        glRectf(-225, Py + 65 + 23, -200, Py + 15 + 20);
        break;
    case 3:
        glColor3f(0.1, 0.0, 0.9);
        glRectf(-225, Py + 65 + 23, -175, Py + 15 + 20);
        glColor3f(1, 0.84, 0.0);
        glRectf(-225, Py + 65 + 23, -200, Py + 15 + 20);
        break;
    case 4:
        glColor3f(0.8, 0.09, 0.1);
        glRectf(-225, Py + 65 + 23, -175, Py + 15 + 20);
        glColor3f(0.1, 0.0, 0.9);
        glRectf(-225, Py + 65 + 23, -200, Py + 15 + 20);
        break;

    }

    

    glColor3f(1, 0.9, 0.7);
    glRectf(-220, Py + 65 + 20, -230, Py + 30);

    glColor3f(0, 0.0, 0.0);
    glRectf(-218, Py + 15 + 20, -208, Py);
    glRectf(-193, Py + 15 + 20, -183, Py);

    //animation for players right hand (throwing hand)
    if (animation) {
        glColor3f(1, 0.9, 0.7);
        glRectf(-180, Py + 65 + 20, -170, Py + 30);
    }




}



// To display onto window using OpenGL commands
void display() {
    glClearColor(0.8, 0.8, 0.8, 0.8);
    glClear(GL_COLOR_BUFFER_BIT);


    drawBackground();
    drawPlatform(Py);
    drawCar();
    drawThrower();
    //drawExtraPoints();
    drawknife();

    //displaying throwers right hand
    if (!animation) {
        glColor3f(1, 0.9, 0.7);
        glRectf(-180, Py + 65 + 20, -130, Py + 75);
    }


    //if game is not over dislpay baloons and car
    if (!gameover && Time > 0) {
        
            srand(time(NULL));

            for (int i = 0; i < 5; i++) {
                
                drawBalloon(xB[i], yB[i]);
                
        
            }


            if (flagForF1 == true) {
                if (carX == -winWidth / 2) {
                    carX = WINDOW_WIDTH / 2;
                }
                carX -= 1;
            }

            

        
    }

    //calculate hitcount to display in the information screeen
    calchit();

    //information screen background
    glColor3f(0, 0., 0.);
    glRectf(-500, -300, -300, 300);

    //display information screen
    informationScreen();

    glColor3f(1, 0, 0);


    //if game is over display screen messages for a new game
    if (gameover) {
        glColor3f(1, 1., 1.);
        glRectf(-500, -300, -300, 300);

        glColor3f(0.8, 0.2, 0.0);
        print(-450, 0, "Press F1", GLUT_BITMAP_TIMES_ROMAN_24);
        print(-480, -50, "for a new game", GLUT_BITMAP_TIMES_ROMAN_24);

        hitcount = 0;


    }

    glutSwapBuffers();
}

// Key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
void onKeyDown(unsigned char key, int x, int y) {
    // Exit when ESC is pressed.
    if (key == 27)
        exit(0);

    if (key == '1') {
        throwerTeam = 1;
    }
    if (key == '2') {
        throwerTeam = 2;
    }
    if (key == '3') {
        throwerTeam = 3;
    }
    if (key == '4') {
        throwerTeam = 4;
    }


    // If the key is space character.
    if (key == ' ') {
        animation = true;

    }

    glColor3f(1, 0.9, 0.7);
    glRectf(-180, Py +85, -130, Py + 75);

    // To refresh the window it calls display() function
    glutPostRedisplay();
}

// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
void onSpecialKeyDown(int key, int x, int y) {

    switch (key) {
    case GLUT_KEY_RIGHT:
        Vx += 5;
        if (Vx > 120)
            Vx = 120;
        break;
    case GLUT_KEY_LEFT:
        Vx -= 5;
        if (Vx < 15)
            Vx = 15;
        break;
    case GLUT_KEY_UP:
        Py += 5;
        if (Py > 200)
            Py = 200;
        if (!animation) {
            Cy = Py + RADIUS;
        }
        break;
    case GLUT_KEY_DOWN:
        Py -= 5;
        if (Py < -50)
            Py = -50;
        if (!animation) {
            Cy = Py + RADIUS;
        }
        break;
    case GLUT_KEY_F1:
        flagForF1 = !flagForF1;
        
        // Restart the game if the game is already over
        if (gameover) {
            gameover = false;
            flagForF1 = !flagForF1;
            boardY -= 50;
            resetknife();
            Time = 30;
        }

        // Construct the new equation, if the vertex is modified
        // due to key presses on arrow keys
        constructEquation();
    }

    // To refresh the window it calls display() function
    glutPostRedisplay();
}

/*******************************************************
   This function is called when the window size changes.
   w : is the new width of the window in pixels
   h : is the new height of the window in pixels
*******************************************************/
void onResize(int w, int h) {
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // Refresh window
}

void onTimer(int v) {
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);

    if (!gameover) {
        
        if (Cx > 500) {
            resetknife();
        }

        srand(time(NULL));

        for (int i = 0; i < 5; i++) {
            if (yB[i] >= 300) {
                yB[i] = -1 * (rand() % 250 + 60 * i);
                xB[i] = rand() % 180 + i * 80;
            }
        

            if (flagForF1 == true) {
                yB[i] += 1.7 + i / 3;
            }
        }

        if (animation) {

            // New coordinates of the knife is
            Cx += Vx * TIMER_PERIOD / 200.0;


            if (Cx == 500) {
                resetknife();

            }
        }



    }
    // To refresh the window it calls display() function
    glutPostRedisplay(); //display()
}

void init() {
    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // In the beginning, find the parameters of the quadratic equation
    constructEquation();
}

int main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(500, 200);
    glutCreateWindow("Hamit Efe Eldem 164 HW2 Burst Baloons Game");

    //initialize balloons coordinate for the first display
    //after first display of ballons their new coordinates will be calculated randomly with rand function
    initBalloonPos();

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);


    // Keyboard Events
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    // Timer Event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);



    init();


    glutMainLoop();
    return 0;


}

