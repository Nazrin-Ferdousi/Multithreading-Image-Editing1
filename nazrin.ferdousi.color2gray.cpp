/*
Author: Nazrin Ferdousi
Date: 05/20/2020
Description: This program uses multithreading to convert an image to gray scale.

Contributing authors: Saeed Salem - 04/13/2020, Joshua DeNio - 04/18/2020

To compile: g++ -O3 -w nazrin.ferdousi.color2gray.cpp -lpthread -o nazrin.ferdousi.color2gray
To run: ./nazrin.ferdousi.color2gray InputFile outFile thread_count
*/

#include "stb_image.c"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <pthread.h>

using namespace std;

const int MAX_THREADS = 1024;
int thread_count;

uint8* src;
uint8* dst;
int width=0;
int height=0;
int actual_comps=0;
int YR = 19595, YG = 38470, YB = 7471;

void convertPixelToGray(int y, int x){
    int index = (y*width+x)*actual_comps;
    int Red = src[index];
    int Green = src[index+1];
    int Blue = src[index+2];

    //Get a weighted average of the three colors as the gray value.
    int avg = (Red* YR + Green * YG + Blue * YB + 32768) / 65536;

    //Destination index
    dst[index] = avg;
    dst[index+1] = avg;
    dst[index+2] = avg;
}

//Function to convert pixels to gray
void* Thread_convertToGray(void* rank){

    //Each thread will do some of the rows (number of rows is height).

    // cast the rank to a long
    long my_rank = (long) rank;

    long long my_n = height/thread_count;

    // Start and end possitions for each thread
    int my_start = my_n * my_rank;
    int my_end = (my_start + my_n);

    // If the data can be evenly spread spread it or send the extra to the last thread
    if (my_rank == thread_count - 1)
    {
        my_end = (height % thread_count) + my_end ;
    }

    for (int y = my_start; y < my_end; y++)
    {
        for(int x = 0; x < width; x++)
        {
            convertPixelToGray(y, x);
        }
    }
}

int main(int argc, char** argv)
{
    if(argc<2){
      cout<<"To run: ./assign5 fileName outFileName"<<endl;
      cout<<"./assign5 stone.jpeg stoneOut.jpeg"<<endl;
      cout<<"For Multithreading::i \n To run: ./assign5 fileName outFileName thread_count"<<endl;
      cout<<"./assign5 stone.jpeg stoneOut.jpeg 3"<<endl;
      return 0;
    }

    // Declarations for threads.
    long thread;
    pthread_t* thread_handles;

   
     
  const char* pSrc_filename = argv[1];
  const char* pDst_filename = argv[2];
  thread_count = atoi(argv[3]);

  // Give memory to the thread handles
  thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));

  // Load the source image.
  const int req_comps = 3; // request RGB image
  src = stbi_load(pSrc_filename, &width, &height, &actual_comps, req_comps);
  if (!src)
  {
    cout<<"Failed loading file "<<endl;
    return 1;
  }

  cout<<"Source file: "<< pSrc_filename <<" "<< width <<" " <<height <<" "<< actual_comps<<endl;
  cout<<"RGB for pixel 0 (0,0) ::: "<<(int) src[0]<< " "<< (int) src[1]<< " "<< (int) src[2]<<endl;

  //Allocate memory for the output image. 
  dst = (uint8*) malloc(width* height*3);

    // Start threads

    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, Thread_convertToGray, (void*)thread);
    }

    // Join the threads
    for (thread = 0; thread < thread_count; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }

    // End of threads
  
  if (!stbi_write_tga(pDst_filename, width, height, 3, dst))
  {
    cout<<"Failed writing image to file "<< pDst_filename <<endl;
    free(src);
    free(dst);
    return EXIT_FAILURE;
  }

  free(src);
  free(dst);
  return 0;
}


