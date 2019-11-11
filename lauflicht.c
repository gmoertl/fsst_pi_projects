/* Moving light pattern controlled by 3 buttons (forward, backward, stop)
   Edgar Neukirchner 2019
*/
   
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

#define LED1 18    
#define LED2 23
#define LED3 24     
#define LED4 25     

#define BUTTON_FW 22
#define BUTTON_BW 27
#define BUTTON_STOP 17

#define TBOUNCE 200

void writeDisplay(int val);
void clearDisplay(void);
void button_pressed(void); 
void handler(int sig);
void *blink(void *x);

// globale Variablen
int leds[] = {LED1, LED2, LED3, LED4};
enum bstates {BW = -1, STOP, FW};
unsigned long last_ir = 0;
int button_state = FW;
pthread_t pt;

void handler(int sig) 
{
        pthread_cancel(pt); // Blinken beenden
        clearDisplay(); // Alle LEDs aus
        // delay(1000);  // zum Testen, ob Thread wirklich beendet wird
        printf("Program terminated!\n");
        exit(0);
     
}

void button_pressed(void) 
{
        unsigned long ir_time = millis();
        if (ir_time - last_ir > TBOUNCE) {
            if (digitalRead(BUTTON_FW) == LOW) 
                    button_state = FW;
            else if (digitalRead(BUTTON_BW) == LOW) 
                    button_state = BW;
            else
                    button_state = STOP;
        }
        last_ir = ir_time;
}

void writeDisplay(int val)
{
    for (int i = 0; i < 4; i++) 
        if (i == val)
            digitalWrite(leds[i], HIGH);
        else
            digitalWrite(leds[i], LOW);
}
    
void clearDisplay(void)
{
    for (int i = 0; i < 4; i++) {
        pinMode (leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
    }
}

void *blink(void *x)
{
    int i = 0;
    for (;;) {
        if (button_state == STOP)
            clearDisplay();
        else if (button_state == FW) {
            i++;
            if (i == 4)
                i = 0;
            writeDisplay(i);
       } 
       else {
            i--;
            if (i < 0)
                i = 3;
            writeDisplay(i);
       }

       delay(100);
    }
    return NULL;
}

int main(void)
{
    wiringPiSetupGpio() ;
    wiringPiISR (BUTTON_FW, INT_EDGE_FALLING, &button_pressed);
    wiringPiISR (BUTTON_BW, INT_EDGE_FALLING, &button_pressed);
    wiringPiISR (BUTTON_STOP, INT_EDGE_FALLING, &button_pressed);

    signal(SIGINT, handler);
    signal(SIGTERM, handler);

    clearDisplay();
    if (pthread_create(&pt, NULL, &blink, NULL) != 0) {
        printf("Kann thread nicht erzeugen!\n");
        return EXIT_FAILURE;
    }
    
    int richtung;
    for(;;) {
        printf("1 | -1 | 0 [%d] > ", button_state);
        scanf("%d", &richtung);
        
        if((richtung == 0) || (richtung == 1) || (richtung == -1))
            button_state = richtung;
    }
}

