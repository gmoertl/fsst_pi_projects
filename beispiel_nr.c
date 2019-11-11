/* Demonstration of pthreads */
/* Build with: gcc thread-example.c -o thread-example -lpthread -Wall */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

// print a character i times with random delays in between
void *show_char(void *c)
{
  for (int i=0; i<20; i++) {
    printf ("%c", *(char *)c);
    fflush(stdout); // prints each char instantaneously
    int w = rand() % 100; // loop with random delay
    for(long j=0; j<1000000*w; j++)
        ;
  }
  return NULL;
}

int main ()
{
  srand(time(NULL)); // seed for true randomness
  char x = 'V', y = '_';

  printf("Without threads:\n");
  show_char(&x); 
  show_char(&y);
  printf ("\n\n");
  
  printf("With threads:\n");
  pthread_t p1, p2;

  pthread_create (&p1, NULL, show_char, &x);
  pthread_create (&p2, NULL, show_char, &y);

  pthread_join (p1, NULL); // wait until each thread is finished
  pthread_join (p2, NULL);
  
  printf ("\n");
  return 0;
}