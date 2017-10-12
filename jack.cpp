#define _USE_MATH_DEFINES

#include "mbed.h"
#include "ADXL362.h"
#include "math.h"

/*  Set up USB  */
Serial pc(USBTX, USBRX);

/*  Configure LEDs */
DigitalOut led1(LED1);
DigitalOut led2(LED2);

/*  Configure accelerometer */
ADXL362 adxl(p11, p12, p13, p10);
    
/*  Function declarations   */
void capture(int, float);
void calcAngles(int, int, int, float*, float*);

/*  Define pi    */
const double pi = 3.14159265358979323846;

int main()
{   
    /*  Set up accelerometer    */
    adxl.init_spi();
    adxl.init_adxl362();
    wait(0.1);
    
    /*  Assign variables    */
    int N;
    float T;
    T = 0.1;
    N = 50;
    
    /*  User Menu   */
    bool exit = false;
    int input;
    
    while (exit == false) {
        
        pc.printf("\n-----------------------------------------------------------------------------");
        pc.printf("\n\t\t\t\tTilt Sensor\n\t\t\t\t-----------\n\n\nTime period, T = %f\nNumber of readings, N = %d", T, N);
        pc.printf("\n\nMenu:\n 1. Capture and display data\n 2. Change variable T\n 3. Change variable N\n 4. Exit\nEnter a number: ");
        pc.scanf("%d",&input);
        
        switch(input) {
            case 1:         /*  1. Capture and display results */
                capture(N, T);
                break;
            case 2 :        /*  2. Change T    */
                float newT;     
                pc.printf("Enter a new positive value for T: ");
                pc.scanf("%f", &newT);
                T = newT;
                break;
            case 3 :        /*  3. Change N    */
                int newN;
                pc.printf("Enter a new positive integer value for N: ");
                pc.scanf("%d", &newN);
                N = newN;
                break;
            case 4 :        /* 4. Exit  */
                exit = true;
        }
    }
    
    /* End of main  */
    pc.printf("Exiting...");
                
}
            
    
/*  Retrieves x, y, z values    */
/*  Performs calculations to find tilt angles   */
/*  Prints a table of information   */
void capture(int N, float T)
{
    
    /*  Temporary values    */ 
    int8_t xdata, ydata, zdata;
    float xAngle, yAngle;
    bool excesX, excesY;    /*Flag for excessive tilt*/
    
    /*  Data storage    */
    int data[N][3];
    float angles[N][2];
    bool exces[N][2];
    
    
    
    
    for (int i=0; i<N; i++) {      /*Take N readings*/
        
        /*  Take readings   */
        adxl.ACC_GetXYZ8(&xdata, &ydata, &zdata);
        
        /*  Compute Angles  */
        calcAngles(xdata, ydata, zdata, &xAngle, &yAngle);
        
        /*  Light LED1 if tilt on x-axis is more than 10 degrees */
        if (xAngle > 10 || xAngle < -10) {
            led1 = 1;
            excesX = true;
        } else {
            led1 = 0;
            excesX = false;
        }
        
        /*  Light LED2 if tilt on y-axis is more than 10 degrees */
        if (yAngle > 10 || yAngle < -10) {
            led2 = 1;
            excesY = true;
        } else {
            led2 = 0;
            excesY = false;
        }
            
        
        /*  Store readings in data array    */
        data[i][0] = xdata;
        data[i][1] = ydata;
        data[i][2] = zdata;
        
        /*  Store angles in separate array  */
        angles[i][0] = xAngle;
        angles[i][1] = yAngle;
        
        /*  Store excessive tilt flags in separate array    */
        exces[i][0] = excesX;
        exces[i][1] = excesY;

        /*  Wait the time period    */
        wait(T);
    }
    
    
    /*  Print table */
    pc.printf("\n-----------------------------------------------------------------------------\nX\tY\tZ\tX Angle (Deg)\tY Angle (Deg)\tExcessive Tilt Flag X:Y\n-----------------------------------------------------------------------------");
    for (int i=0; i<N; i++) {
        pc.printf("\n%d\t%d\t%d\t%f\t%f\t%d : %d", data[i][0], data[i][1], data[i][2], angles[i][0], angles[i][1], exces[i][0], exces[i][1]);
    }
    
}

/* Calculate angle of tilt to x- and y-axis */ 
void calcAngles(int x, int y, int z, float* xA, float* yA) {
    
    /* Calculate angles, convert to degrees and store using pointers */
    *xA = atan(x / sqrt( static_cast<float>(y*y + z*z) )) * 180 / pi;
    *yA = atan(y / sqrt( static_cast<float>(x*x + z*z) )) * 180 / pi;       
}
