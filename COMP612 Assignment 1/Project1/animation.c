/******************************************************************************
 *
 * Animation v1.0 (23/02/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene.
 *
 ******************************************************************************/

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

// Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_EXIT				27  // Escape key.
#define KEY_TOGGLE_SNOW			115 // s key.
#define KEY_TOGGLE_DIAGNOSTICS	100 // d key.
#define KEY_CYCLE_OUTFIT		111	// o key.
#define KEY_CYCLE_TIME			116 // t key.

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char** argv);
void init(void);
void think(void);
void drawCircle(float x, float y, float radius, float color[]);
void drawOutfit(int outfit);
void drawSemiCircle(float x, float y, float radius, float color[], int tilt);
void drawFace(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

#define MAX_PARTICLES 1000

typedef struct {
	float x;
	float y;
} Position2d;

typedef struct {
	Position2d position; // x, y coordiantes of particle
	float size; // GL point size
	float dy; // velocity
	int active; // is active/inactive
} Particle_t;

Particle_t particleSystem[MAX_PARTICLES];

float vertices[10][2];
int particleCount;

bool snow;
bool diagnostics;
int outfit;
int dayTime;
float skyColorTop[4];
float skyColorBottom[4];
float skyColorTopDay[4];
float skyColorBottomDay[4];
float skyColorTopNight[4];
float skyColorBottomNight[4];

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char** argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(900, 900);
	glutCreateWindow("Animation");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE

		Separate reusable pieces of drawing code into functions, which you can add
		to the "Animation-Specific Functions" section below.

		Remember to add prototypes for any new functions to the "Animation-Specific
		Function Prototypes" section near the top of this template.
	*/
	glClear(GL_COLOR_BUFFER_BIT);


	// Background
	// Day
	glEnable(GL_BLEND);

	glBegin(GL_QUADS);

	glColor4f(skyColorTop[0], skyColorTop[1], skyColorTop[2], skyColorTop[3]);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glColor4f(skyColorBottom[0], skyColorBottom[1], skyColorBottom[2], skyColorBottom[3]);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);

	glEnd();

	glDisable(GL_BLEND);

	// Sky effects
	if (!dayTime)
	{
		
	}
	else
	{
		glPointSize(2); 
		glBegin(GL_POINTS);

		glColor4f(1.0f, 1.0f, 0.0f, skyColorBottom[3]);

		glVertex2f(-0.45, 0.45);
		glVertex2f(-0.25, 0.75);
		glVertex2f(0.45, 0.38);
		glVertex2f(0.15, 0.49);
		glVertex2f(0.81, 0.29);
		glVertex2f(-0.35, 0.35);

		glEnd();
	}
	
	// Terrain
	float totalX = 0;

	while (totalX < 2.0f) {
		for (int i = 1; i < 11; i++)
		{
			if (i == 0) {
				glBegin(GL_POLYGON);

				glColor3f(0.35f, 0.35f, 0.35f);
				glVertex2f(-1.0f, -1.0f);
				glVertex2f(-1.0f + vertices[i][0], -1.0f);
				glColor3f(0.98f, 0.98f, 0.98f);
				glVertex2f(-1.0f + vertices[i][0], vertices[i][1]);
				glVertex2f(-1.0f, vertices[i - 1][1]);

				glEnd();

				totalX += vertices[i][0];
			}

			else {
				glBegin(GL_POLYGON);

				glColor3f(0.35f, 0.35f, 0.35f);
				glVertex2f(-1.0f + totalX, -1.0f);
				glVertex2f(-1.0f + vertices[i][0] + totalX, -1.0f);
				glColor3f(0.98f, 0.98f, 0.98f);
				glVertex2f(-1.0f + vertices[i][0] + totalX, vertices[i][1]);
				glVertex2f(-1.0f + totalX, vertices[i - 1][1]);

				glEnd();

				totalX += vertices[i][0];
			}
		}
	}

	// Snowman
	// Bottom circle
	float body[4] = { 0.95f, 0.93f, 0.93f, 1.0f };
	drawCircle(0.3f, -0.5f, 0.15f, body);
	// Face
	drawFace();

	// Outfit
	drawOutfit(outfit);


	// Snow
	glEnable(GL_BLEND);

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		glColor4f(0.753f, 0.753f, 0.753f, particleSystem[i].dy - 0.2f);
		glPointSize(particleSystem[i].size);

		glBegin(GL_POINTS);
		glVertex2f(particleSystem[i].position.x, particleSystem[i].position.y);

		glEnd();
	}

	glDisable(GL_BLEND);

	// Diagnostics

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glColor3f(1.0f, 0.0f, 0.0f);

	if (diagnostics) {
		// Diagnostics toggle
		glRasterPos2f(-0.95f, 0.95f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Press [d] to toggle diagnostics off");
		
		// Quit
		glRasterPos2f(-0.95f, 0.90f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Press [ESC] to quit");

		// Snow toggle
		glRasterPos2f(-0.95f, 0.85f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Press [o] to cycle through outfits");

		// Snow toggle
		glRasterPos2f(-0.95f, 0.8f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Press [s] to toggle snow");

		// Time cycle
		glRasterPos2f(-0.95f, 0.75f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Press [t] to cycle between day/night");

		// Active number of particles on screen
		glRasterPos2f(-0.95f, 0.70);
		char particleCountDisplay[40];
		sprintf_s(particleCountDisplay, 40, "Number of particles on screen: %d", particleCount);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, particleCountDisplay);
	}
	else {
		// Diagnostics off text
		glRasterPos2f(-0.95f, 0.95f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_12, "Press [d] to toggle diagnostics on");
	}


	glutSwapBuffers();
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
			TEMPLATE: Add any new character key controls here.

			Rather than using literals (e.g. "d" for diagnostics), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
		*/
	case KEY_EXIT:
		exit(0);
		break;

	case KEY_TOGGLE_SNOW:
		snow = !snow;
		break;

	case KEY_TOGGLE_DIAGNOSTICS:
		diagnostics = !diagnostics;
		break;

	case KEY_CYCLE_OUTFIT:
		if (outfit < 4)
			outfit++;
		else
			outfit = 0;
		break;
	case KEY_CYCLE_TIME:
		dayTime = 1 - dayTime;
		break;
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

void drawCircle(float x, float y, float radius, float color[])
{
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(color[0], color[1], color[2], color[3]);


	for (int i = 0; i <= 360; i++)
	{
		float angle = (float)(i * 3.1415 / 180.0);
		float localX = cos(angle) * radius;
		float localY = sin(angle) * radius;
		glVertex2f(localX + x, localY + y);
	}

	glEnd();
}

void drawSemiCircle(float x, float y, float radius, float color[], int tilt)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(color[0], color[1], color[2], color[3]);
	glVertex2f(x, y);

	for (int i = 0 + tilt; i <= 180 + tilt; i++)
	{
		float angle = (float)(i * 3.1415 / 180.0);
		float localX = cos(angle) * radius;
		float localY = sin(angle) * radius;
		glVertex2f(localX + x, localY + y);
	}

	glEnd();
}

void drawFace()
{
	// Colours
	float body[4] = { 0.95f, 0.93f, 0.93f, 1.0f };
	float eyes[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float nose[4] = { 1.0f, 0.647f, 0.0f, 1.0f };

	// Top circle
	drawCircle(0.3f, -0.35f, 0.1f, body);
	// Left eye
	drawCircle(0.275f, -0.30f, 0.009f, eyes);
	// Right eye
	drawCircle(0.325f, -0.30f, 0.009f, eyes);
	// Nose
	glColor4f(1.0f, 0.647f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(0.3f, -0.335f);
	glVertex2f(0.4f, -0.325f);
	glVertex2f(0.3f, -0.315f);
	glEnd();
}

void drawOutfit(int outfit)
{
	if (outfit == 0)
		;
	// Fancy outfit
	else if (outfit == 1)
	{
		// Top Hat Brim
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_POLYGON);

		glVertex2f(0.2f, -0.27f);
		glVertex2f(0.4f, -0.27f);
		glVertex2f(0.4f, -0.245f);
		glVertex2f(0.2f, -0.245f);

		glEnd();

		// Top Hat Crown
		glBegin(GL_POLYGON);

		glVertex2f(0.23f, -0.245f);
		glVertex2f(0.37f, -0.245f);
		glVertex2f(0.37f, -0.15f);
		glVertex2f(0.23f, -0.15f);

		glEnd();

		// Top Hat Stripe
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_POLYGON);

		glVertex2f(0.23f, -0.245f);
		glVertex2f(0.37f, -0.245f);
		glVertex2f(0.37f, -0.22f);
		glVertex2f(0.23f, -0.22f);

		glEnd();

		// Top Tie Triangle
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_TRIANGLES);

		glVertex2f(0.3f, -0.45f);
		glVertex2f(0.32f, -0.4f);
		glVertex2f(0.28f, -0.4f);

		glEnd();

		// Bottom Triangle Strip/Flag of Tie
		glBegin(GL_TRIANGLE_STRIP);

		glVertex2f(0.3f, -0.44f);
		glVertex2f(0.28f, -0.55f);
		glVertex2f(0.32f, -0.55f);
		glVertex2f(0.3f, -0.57f);

		glEnd();

		// Left Shoe Foot
		glBegin(GL_POLYGON);

		glVertex2f(0.2f, -0.66f);
		glVertex2f(0.275f, -0.66f);
		glVertex2f(0.275f, -0.635f);
		glVertex2f(0.2f, -0.635f);

		glEnd();

		// Left Shoe Toes
		glBegin(GL_TRIANGLES);

		glVertex2f(0.15f, -0.66f);
		glVertex2f(0.2f, -0.66f);
		glVertex2f(0.2f, -0.635f);

		glEnd();

		// Right Shoe Foot
		glBegin(GL_POLYGON);

		glVertex2f(0.325f, -0.66f);
		glVertex2f(0.4f, -0.66f);
		glVertex2f(0.4f, -0.635f);
		glVertex2f(0.325f, -0.635f);

		glEnd();

		// Right Shoe Toes
		glBegin(GL_TRIANGLES);

		glVertex2f(0.4f, -0.66f);
		glVertex2f(0.45f, -0.66f);
		glVertex2f(0.4f, -0.635f);

		glEnd();
	}
	// Warm outfit
	else if (outfit == 2)
	{
		float beanie[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		float earmuff[4] = { 0.97f, 0.78f, 0.86f, 1.0f };
		drawSemiCircle(0.21f, -0.335f, 0.03f, earmuff, 90, 5);
		drawSemiCircle(0.39f, -0.335f, 0.03f, earmuff, 270, 5);
		drawFace();
		drawSemiCircle(0.3f, -0.275f, 0.065f, beanie, 0, 0);

		// Scarf main
		glColor4f(0.75f, 0.0f, 0.0f, 1.0f);

		glBegin(GL_POLYGON);

		glVertex2f(0.2f, -0.4);
		glVertex2f(0.4f, -0.4);
		glVertex2f(0.39f, -0.375);
		glVertex2f(0.2f, -0.375f);

		glEnd();

		// Scarf tail
		glBegin(GL_POLYGON);

		glVertex2f(0.23f, -0.49);
		glVertex2f(0.25f, -0.4);
		glVertex2f(0.23f, -0.375);
		glVertex2f(0.21f, -0.48f);

		glEnd();
	}
	// Christmas Tree Outfit
	else if (outfit == 3)
	{
		glColor4f(0.0f, 0.65f, 0.0f, 1.0f);

		// First Triangle
		glBegin(GL_TRIANGLES);

		glVertex2f(0.175f, -0.22f);
		glVertex2f(0.425f, -0.22f);
		glVertex2f(0.3f, 0.0f);

		glEnd();

		// Second Triangle
		glBegin(GL_TRIANGLES);

		glVertex2f(0.05f, -0.45f);
		glVertex2f(0.55f, -0.45f);
		glVertex2f(0.3f, -0.18f);

		glEnd();

		// Third Triangle
		glBegin(GL_TRIANGLES);

		glVertex2f(0.0f, -0.6f);
		glVertex2f(0.6f, -0.6f);
		glVertex2f(0.3f, -0.3f);

		glEnd();

		float goldBauble[4] = { 1.0f, 0.843f, 0.0f, 1.0f };
		float redBauble[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		drawCircle(0.3f, -0.05f, 0.02f, goldBauble);
		drawCircle(0.18f, -0.22f, 0.01f, redBauble);
		drawCircle(0.42f, -0.22f, 0.01f, redBauble);
		drawCircle(0.055f, -0.45f, 0.01f, redBauble);
		drawCircle(0.545f, -0.45f, 0.01f, redBauble);
		drawCircle(0.005f, -0.6f, 0.01f, redBauble);
		drawCircle(0.595f, -0.6f, 0.01f, redBauble);

		drawFace();
	}
	else if (outfit == 4)
	{
		float ecms[4] = { 1.0f, 0.5547f, 0.0f, 1.0f };
		glColor4f(1.0f, 0.6547f, 0.0f, 1.0f);

		// Hood
		drawCircle(0.3f, -0.35f, 0.13f, ecms);

		// Hoodie Bodie
		glBegin(GL_TRIANGLES);

		glVertex2f(0.0f, -0.6f);
		glVertex2f(0.6f, -0.6f);
		glVertex2f(0.3f, -0.25f);


		glEnd();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// AUT
		glColor3f(1.0f, 1.0f, 1.0f);
		glRasterPos2f(0.2f, -0.5f);
		glutBitmapString(GLUT_BITMAP_HELVETICA_18, "AUT");

		drawFace();
	}
}


/*
	Initialise OpenGL and set up our scene before we begin the render loop.
*/
void init(void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	srand(time(NULL));
	snow = true;
	diagnostics = true;
	outfit = 0;
	dayTime = 0;

	skyColorTopDay[0] = 0.878f;
	skyColorTopDay[1] = 0.947f;
	skyColorTopDay[2] = 0.802f;
	skyColorTopDay[3] = 1.0f;

	skyColorBottomDay[0] = 0.678f;
	skyColorBottomDay[1] = 0.847f;
	skyColorBottomDay[2] = 0.902f;
	skyColorBottomDay[3] = 1.0f;

	skyColorTopNight[0] = 0.0f;
	skyColorTopNight[1] = 0.0f;
	skyColorTopNight[2] = 0.345f;
	skyColorTopNight[3] = 1.0f;

	skyColorBottomNight[0] = 0.0f;
	skyColorBottomNight[1] = 0.0f;
	skyColorBottomNight[2] = 0.555f;
	skyColorBottomNight[3] = 0.1f;

	skyColorTop[0] = skyColorTopDay[0];
	skyColorTop[1] = skyColorTopDay[1];
	skyColorTop[2] = skyColorTopDay[2];
	skyColorTop[3] = skyColorTopDay[3];

	skyColorBottom[0] = skyColorBottomDay[0];
	skyColorBottom[1] = skyColorBottomDay[1];
	skyColorBottom[2] = skyColorBottomDay[2];
	skyColorBottom[3] = skyColorBottomDay[3];

	// init the snow
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particleSystem[i].position.x = ((float)rand() / RAND_MAX * 3.5f) - 0.5f;

		particleSystem[i].position.y = ((float)rand() / RAND_MAX) * 2 + 2.0f;

		particleSystem[i].dy = (((float)rand() / RAND_MAX) + 0.2);
		particleSystem[i].size = ((float)rand() / RAND_MAX) * 9.0f + 1.0f;
		particleSystem[i].active = 1;
		particleCount++;
	}

	// init the landscape
	for (int i = 0; i < 10; i++)
	{
		vertices[i][0] = (float)rand() / RAND_MAX * 0.3f + 0.2f;
		vertices[i][1] = (float)rand() / RAND_MAX * 0.4f - 0.3f;
	}
}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE

		In this function, we update all the variables that control the animated
		parts of our simulated world. For example: if you have a moving box, this is
		where you update its coordinates to make it move. If you have something that
		spins around, here's where you update its angle.

		NOTHING CAN BE DRAWN IN HERE: you can only update the variables that control
		how everything will be drawn later in display().

		How much do we move or rotate things? Because we use a fixed frame rate, we
		assume there's always FRAME_TIME milliseconds between drawing each frame. So,
		every time think() is called, we need to work out how far things should have
		moved, rotated, or otherwise changed in that period of time.

		Movement example:
		* Let's assume a distance of 1.0 GL units is 1 metre.
		* Let's assume we want something to move 2 metres per second on the x axis
		* Each frame, we'd need to update its position like this:
			x += 2 * (FRAME_TIME / 1000.0f)
		* Note that we have to convert FRAME_TIME to seconds. We can skip this by
		  using a constant defined earlier in this template:
			x += 2 * FRAME_TIME_SEC;

		Rotation example:
		* Let's assume we want something to do one complete 360-degree rotation every
		  second (i.e. 60 Revolutions Per Minute, or RPM).
		* Each frame, we'd need to update our object's angle like this (we'll use the
		  FRAME_TIME_SEC constant as per the example above):
			a += 360 * FRAME_TIME_SEC;

		This works for any type of "per second" change: just multiply the amount you'd
		want to move in a full second by FRAME_TIME_SEC, and add or subtract that
		from whatever variable you're updating.

		You can use this same approach to animate other things like color, opacity,
		brightness of lights, etc.
	*/

	// Update background if time has been changed

	if (dayTime && skyColorTop[0] > skyColorTopNight[0])
	{
		skyColorTop[0] -= (skyColorTopDay[0] - skyColorTopNight[0]) / 100;
		skyColorTop[1] -= (skyColorTopDay[1] - skyColorTopNight[1]) / 100;
		skyColorTop[2] -= (skyColorTopDay[2] - skyColorTopNight[2]) / 100;

		skyColorBottom[0] -= (skyColorBottomDay[0] - skyColorBottomNight[0]) / 100;
		skyColorBottom[1] -= (skyColorBottomDay[1] - skyColorBottomNight[1]) / 100;
		skyColorBottom[2] -= (skyColorBottomDay[2] - skyColorBottomNight[2]) / 100;
		skyColorBottom[3] -= (skyColorBottomDay[3] - skyColorBottomNight[3]) / 100;
	}
	else if (!dayTime && skyColorTop[0] < skyColorTopDay[0])
	{
		skyColorTop[0] += (skyColorTopDay[0] - skyColorTopNight[0]) / 100;
		skyColorTop[1] += (skyColorTopDay[1] - skyColorTopNight[1]) / 100;
		skyColorTop[2] += (skyColorTopDay[2] - skyColorTopNight[2]) / 100;

		skyColorBottom[0] += (skyColorBottomDay[0] - skyColorBottomNight[0]) / 100;
		skyColorBottom[1] += (skyColorBottomDay[1] - skyColorBottomNight[1]) / 100;
		skyColorBottom[2] += (skyColorBottomDay[2] - skyColorBottomNight[2]) / 100;
		skyColorBottom[3] += (skyColorBottomDay[3] - skyColorBottomNight[3]) / 100;
	}
		

	// Update snow
	int activeCount = 1000;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{

		// Deactivate particles
		if (!snow && particleSystem[i].position.y < -1.0
			|| !snow && particleSystem[i].position.y > 1.0
			|| !snow && particleSystem[i].position.x < -1.0
			|| !snow && particleSystem[i].position.x > 1.0)
		{
			particleSystem[i].active = 0;
			particleSystem[i].dy = 0;
			particleSystem[i].position.x = ((float)rand() / RAND_MAX * 3.5f) - 0.5f;
			particleSystem[i].position.y = ((float)rand() / RAND_MAX) * 2 + 2.0f;
		}

		// Recycle particles to the top of the screen
		if (particleSystem[i].position.y <= -1.0 && particleSystem[i].active == 1)
		{

			particleSystem[i].position.x = ((float)rand() / RAND_MAX * 3.5f) - 0.5f;
			particleSystem[i].position.y = ((float)rand() / RAND_MAX) * 2 + 2.0f;
			particleSystem[i].dy = (((float)rand() / RAND_MAX) + 0.2);
		}

		// Reactivate particles
		if (snow && particleSystem[i].dy == 0)
		{
			particleSystem[i].dy = (((float)rand() / RAND_MAX) + 0.2);
			particleSystem[i].active = 1;
		}

		// Update particle position
		particleSystem[i].position.x -= 0.35f * FRAME_TIME_SEC;
		particleSystem[i].position.y -= particleSystem[i].dy * FRAME_TIME_SEC;

		// Count number of active particles
		if (particleSystem[i].position.y > -1.0
			&& particleSystem[i].position.y < 1.0
			&& particleSystem[i].position.x > -1.0
			&& particleSystem[i].position.x < 1.0)
			activeCount++;
		else
			activeCount--;
	}
	particleCount = activeCount;
}

/******************************************************************************/