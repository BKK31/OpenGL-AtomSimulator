#include <GL/glut.h> // OpenGL Utility Toolkit for graphics rendering
#include <math.h>    // Standard C library for mathematical functions
#include <stdio.h>   // Standard C library for input/output operations
#include <string.h>  // Standard C library for string handling
#include <stdbool.h> // Standard C library for boolean types

// Define the window width and height
int width = 800;
int height = 600;

// Define the radius of the nucleus, electron, and base orbit
float nucleusRadius = 0.2f;
float electronRadius = 0.07f;
float baseOrbitRadius = 1.0f;

// Define the maximum number of electrons in each shell
int maxElectronsInShells[7] = {2, 8, 18, 32, 32, 18, 8};

// Store the number of electrons for the current element
int numElectrons = 1;

// Define the name of the element
char elementName[20] = "Hydrogen";

// Global variable for error messages
char errorMessage[50] = "";

// Define the names of the elements
const char *elementNames[118] = {
    "Hydrogen", "Helium", "Lithium", "Beryllium", "Boron", "Carbon", "Nitrogen", "Oxygen",
    "Fluorine", "Neon", "Sodium", "Magnesium", "Aluminum", "Silicon", "Phosphorus", "Sulfur",
    "Chlorine", "Argon", "Potassium", "Calcium", "Scandium", "Titanium", "Vanadium", "Chromium",
    "Manganese", "Iron", "Cobalt", "Nickel", "Copper", "Zinc", "Gallium", "Germanium",
    "Arsenic", "Selenium", "Bromine", "Krypton", "Rubidium", "Strontium", "Yttrium", "Zirconium",
    "Niobium", "Molybdenum", "Technetium", "Ruthenium", "Rhodium", "Palladium", "Silver", "Cadmium",
    "Indium", "Tin", "Antimony", "Tellurium", "Iodine", "Xenon", "Cesium", "Barium",
    "Lanthanum", "Cerium", "Praseodymium", "Neodymium", "Promethium", "Samarium", "Europium", "Gadolinium",
    "Terbium", "Dysprosium", "Holmium", "Erbium", "Thulium", "Ytterbium", "Lutetium", "Hafnium",
    "Tantalum", "Tungsten", "Rhenium", "Osmium", "Iridium", "Platinum", "Gold", "Mercury",
    "Thallium", "Lead", "Bismuth", "Polonium", "Astatine", "Radon", "Francium", "Radium",
    "Actinium", "Thorium", "Protactinium", "Uranium", "Neptunium", "Plutonium", "Americium", "Curium",
    "Berkelium", "Californium", "Einsteinium", "Fermium", "Mendelevium", "Nobelium", "Lawrencium",
    "Rutherfordium", "Dubnium", "Seaborgium", "Bohrium", "Hassium", "Meitnerium", "Darmstadtium",
    "Roentgenium", "Copernicium", "Nihonium", "Flerovium", "Moscovium", "Livermorium", "Tennessine", "Oganesson"};

// Track the current input number and input state
int currentInputNumber = 0;
bool isInputtingNumber = false;
bool inputInProgress = false;

// Track the last input time and the input delay
unsigned long lastInputTime = 0;
unsigned long lastDigitTime = 0;
const unsigned long inputDelay = 1500;

// Track the state of the electrons
bool electronsMoving = false;

// Initialize the OpenGL environment
void init()
{
    // Set the clear color of the screen to white
    glClearColor(1.0, 1.0, 1.0, 1.0);

    // Enable depth testing to ensure correct drawing order
    glEnable(GL_DEPTH_TEST);

    // Enable color material to allow changing color of objects
    glEnable(GL_COLOR_MATERIAL);

    // Enable lighting to add realistic shading to objects
    glEnable(GL_LIGHTING);

    // Enable light 0 to add lighting to the scene
    glEnable(GL_LIGHT0);

    // Set the color material of front and back faces to be affected by color changes
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

/**
 * Draws a sphere with the specified radius and color.
 *
 * @param radius The radius of the sphere.
 * @param r The red component of the color.
 * @param g The green component of the color.
 * @param b The blue component of the color.
 */
void drawSphere(float radius, float r, float g, float b)
{
    // Set the color of the sphere.
    glColor3f(r, g, b);

    // Draw the sphere with 50 segments for latitude and 50 segments for longitude.
    glutSolidSphere(radius, 50, 50);
}

/**
 * Draws an orbit with the specified radius.
 *
 * @param radius The radius of the orbit.
 */
void drawOrbit(float radius)
{
    // Set the color of the orbit to gray.
    glColor3f(0.5f, 0.5f, 0.5f);

    // Begin drawing the orbit as a line loop.
    glBegin(GL_LINE_LOOP);

    // Iterate 100 times to draw 100 points around the orbit.
    for (int i = 0; i < 100; ++i)
    {
        // Calculate the angle for the current point.
        float angle = 2 * M_PI * i / 100;

        // Calculate the x and y coordinates of the point on the orbit using the radius and angle.
        float x = radius * cos(angle);
        float y = radius * sin(angle);

        // Add the vertex to the line loop.
        glVertex2f(x, y);
    }

    // End the line loop.
    glEnd();
}

/**
 * Draws text at the specified position on the screen using the Helvetica
 * 18 point bitmap font.
 *
 * @param text The text to draw.
 * @param x The x-coordinate of the position where the text should be drawn.
 * @param y The y-coordinate of the position where the text should be drawn.
 */
void drawText(const char *text, float x, float y)
{
    // Set the color of the text to white.
    glColor3f(0.0, 0.0, 0.0);

    // Set the position of the raster where the text should be drawn.
    glRasterPos2f(x, y);

    // Iterate over each character in the text string.
    while (*text)
    {
        // Draw the current character using the Helvetica 18 point bitmap font.
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
    }
}

/**
 * Display function is responsible for drawing the elements and orbit on the
 * OpenGL window.
 */
void display()
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); // Reset the current matrix

    glOrtho(-2.0, 2.0, -2.0, 2.0, -1.0, 1.0); // Set the projection matrix

    glPushMatrix();                           // Push the current matrix onto the stack
    drawText(elementName, -0.3f, 0.3f);       // Draw the element name
    drawSphere(nucleusRadius, 1.0, 0.0, 0.0); // Draw the nucleus
    glPopMatrix();                            // Pop the matrix from the stack

    // Calculate the current time in seconds
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float currentOrbitRadius = baseOrbitRadius;

    int remainingElectrons = numElectrons;
    // Iterate over the shells from 0 to 6
    for (int shell = 0; shell < 7 && remainingElectrons > 0; shell++)
    {
        // Calculate the number of electrons in the current shell
        int electronsInThisShell = (remainingElectrons > maxElectronsInShells[shell]) ? maxElectronsInShells[shell] : remainingElectrons;

        drawOrbit(currentOrbitRadius); // Draw the orbit

        // Iterate over the electrons in the current shell
        for (int i = 0; i < electronsInThisShell; i++)
        {
            float angle;
            // Calculate the angle of the electron based on the time and the number of electrons
            if (electronsMoving)
            {
                angle = time * 0.9 + (2 * M_PI / electronsInThisShell) * i;
            }
            else
            {
                angle = (2 * M_PI / electronsInThisShell) * i;
            }
            float x = currentOrbitRadius * cos(angle); // Calculate the x coordinate of the electron
            float y = currentOrbitRadius * sin(angle); // Calculate the y coordinate of the electron

            glPushMatrix();                            // Push the current matrix onto the stack
            glTranslatef(x, y, 0.0f);                  // Translate to the electron's position
            drawSphere(electronRadius, 1.0, 1.0, 0.0); // Draw the electron
            glPopMatrix();                             // Pop the matrix from the stack
        }

        currentOrbitRadius += 0.4f;                 // Increase the radius of the orbit
        remainingElectrons -= electronsInThisShell; // Decrease the number of remaining electrons
    }

    // Draw error message if there is one
    if (strlen(errorMessage) > 0)
    {
        drawText(errorMessage, -0.5f, -0.5f); // Adjusted position for better visibility
    }

    glutSwapBuffers(); // Swap the front and back buffers
}

/**
 * This function is called by GLUT whenever the window is resized.
 * It sets the viewport to the new window size and sets the projection matrix
 * to an orthographic projection. This means that the scene is displayed
 * in a traditional 2D manner, with the y-axis pointing upwards.
 *
 * @param w The new width of the window.
 * @param h The new height of the window.
 */
void reshape(int w, int h)
{
    // Set the viewport to the new window size
    glViewport(0, 0, w, h);

    // Set the projection matrix to an orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -1.0, 1.0);

    // Set the modelview matrix back to the default state
    glMatrixMode(GL_MODELVIEW);
}

/**
 * This function is called by GLUT when there is nothing else to do.
 * It calls glutPostRedisplay() to request that the display function
 * be called again. This is necessary for animations and other continuous
 * updates.
 */
void idle()
{
    // Request that the display function be called again
    glutPostRedisplay();
}

/**
 * This function is called by GLUT whenever a keyboard key is pressed.
 * It handles input of the atomic number by checking if the key pressed
 * is a digit (0-9). If a digit is pressed, it checks if enough time
 * has passed since the last input. If enough time has passed, it checks
 * if a number is currently being inputted. If a number is being inputted,
 * it appends the new digit to the current input number. If not enough
 * time has passed, it sets the current input number to the new digit.
 * It also sets a flag indicating that a number is being inputted.
 * After handling the input, it updates the element name and atomic number
 * based on the current input number. It also limits the input number to
 * a maximum value of 118 and prints the element name and atomic number.
 *
 * @param key The ASCII code of the key pressed.
 * @param x The x-coordinate of the mouse cursor.
 * @param y The y-coordinate of the mouse cursor.
 */
void keyboard(unsigned char key, int x, int y)
{
    if (key >= '0' && key <= '9')
    {
        unsigned long currentTime = glutGet(GLUT_ELAPSED_TIME);

        if (currentTime - lastDigitTime < inputDelay && isInputtingNumber)
        {
            currentInputNumber = currentInputNumber * 10 + (key - '0');
        }
        else
        {
            currentInputNumber = (key - '0');
            isInputtingNumber = true;
        }

        // Update the last input time
        lastDigitTime = currentTime;

        if (currentInputNumber > 118)
        {
            currentInputNumber = 118;
        }

        // Update the number of electrons and the element name based on the current input number
        numElectrons = currentInputNumber;
        strncpy(elementName, elementNames[numElectrons - 1], sizeof(elementName) - 1);
        elementName[sizeof(elementName) - 1] = '\0';

        // Print the element name and atomic number
        printf("Element: %s, Atomic Number: %d\n", elementName, numElectrons);

        // Update the last input time
        lastInputTime = currentTime;
    }
}

/**
 * Function to handle the sub-menu options.
 *
 * @param option The option selected from the sub-menu.
 */
void subMenu(int option)
{
    // If the user selects option 1, stop the electrons from moving.
    if (option == 1)
    {
        electronsMoving = false;
    }
    // If the user selects option 2, start the electrons moving.
    if (option == 2)
    {
        electronsMoving = true;
    }
    // Redisplay the OpenGL window to reflect the changes.
    glutPostRedisplay();
}

/**
 * Function to handle the main menu options.
 *
 * @param option The option selected from the main menu.
 */
void mainMenu(int option)
{
    // Switch statement to handle different menu options
    // Option 1 does nothing, so we add a semicolon to not execute any code for this option.
    if (option == 1)
        ;

    // Option 2 exits the program.
    // The exit() function is used to terminate the program with a status code of 0.
    if (option == 2)
    {
        // Exit the program.
        exit(0);
    }
    // Redisplay the OpenGL window to reflect the changes.
    glutPostRedisplay();
}

/**
 * The main function of the program.
 *
 * @param argc The number of command line arguments.
 * @param argv An array of strings containing the command line arguments.
 * @return The exit status of the program.
 */
int main(int argc, char **argv)
{

    int SubMenu; // Variable to store the ID of the sub-menu.

    // Initialize GLUT and set the display mode.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Set the size of the window.
    glutInitWindowSize(width, height);

    // Create a window and specify its title.
    glutCreateWindow("2D Atom Simulator");

    // Initialize the OpenGL environment.
    init();

    // Set the display function to be called whenever a new frame is to be drawn.
    glutDisplayFunc(display);

    // Create a sub-menu and add its options.
    SubMenu = glutCreateMenu(subMenu);
    glutAddMenuEntry("Fixed", 1);
    glutAddMenuEntry("Moving", 2);

    // Create the main menu and add its options.
    glutCreateMenu(mainMenu);
    glutAddSubMenu("Movement", SubMenu);
    glutAddMenuEntry("Exit", 2);

    // Attach the main menu to the right mouse button.
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // Set the function to be called whenever the window is resized.
    glutReshapeFunc(reshape);

    // Set the function to be called whenever a keyboard button is pressed.
    glutKeyboardFunc(keyboard);

    // Set the function to be called whenever there is no other event to process.
    glutIdleFunc(idle);

    // Start the GLUT event processing loop.
    glutMainLoop();

    // Return 0 to indicate successful execution.
    return 0;
}
