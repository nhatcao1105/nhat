///
/// \file 				main.c
/// \author 			Geoffrey Hunter (www.mbedded.ninja) <gbmhunter@gmail.com>
/// \edited             n/a
/// \created			2017-04-15
/// \last-modified		2018-03-19
/// \brief 				Contains the main entry point to the state machine example.
/// \details
///		See README.md in root dir for more info.

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "StateMachine.h"
#include <time.h>
#include <pthread.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond1 =PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 =PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3  =PTHREAD_COND_INITIALIZER;
int input;


void nhap()
{
    printf("nhap: ");
    scanf("%d",&input);
}


stateMachine_t stateMachine;
    

void* StateMachine(void* State)
{                       //stateMachine_t stateMachine;        
    StateMachine_Init(&stateMachine);   
    printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState));                  
    while(1)
        {
                   
            pthread_cond_wait(&cond,&mutex);
            
            
            printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState)); 

        }
}

void* EvenStateMachine(void* Even)
{
        while(1)
        {
            nhap();
          
            switch(input)
            {    
            case 1:        
                    printf("Even -> Button pushed.\r\n");
                    StateMachine_RunIteration(&stateMachine, EV_BUTTON_PUSHED);
                                 
                    pthread_cond_broadcast(&cond); 
            break;
            case 2:         
                    printf("Even -> Scan QR code.\r\n");
                    StateMachine_RunIteration(&stateMachine, EV_SCAN_QR);
                    
                    pthread_cond_broadcast(&cond);
                                   
            break;
            case 3:
                    
                    printf("Even -> check wifi\n");
                    StateMachine_RunIteration(&stateMachine, EV_WIFI_SUCCESSFUL);

                    pthread_cond_broadcast(&cond);
            break;
            case 4:
                    printf("Even -> Call API\n");
                    StateMachine_RunIteration(&stateMachine, EV_CALL_API);
                    StateMachine_RunIteration(&stateMachine, EV_NONE);

                    pthread_cond_broadcast(&cond);
            break;
            case 5:

                    printf("Even -> disconnect\n");
                    StateMachine_RunIteration(&stateMachine, EV_WIFI_FAIL);
                    StateMachine_RunIteration(&stateMachine, EV_NONE);

                    pthread_cond_broadcast(&cond);
            break;
            case 6:
//
                    printf("Even -> Button pushed again.\r\n");
                    StateMachine_RunIteration(&stateMachine, EV_BUTTON_PUSHED);
                    StateMachine_RunIteration(&stateMachine, EV_NONE);

                    pthread_cond_broadcast(&cond);
            break;

            }  
        }       
}


void thoigian(int count )
{
    unsigned int x_minutes=0;
    unsigned int x_seconds=0;
    unsigned int x_milliseconds=0;
    unsigned int totaltime=0,time_left=0;

    clock_t x_startTime,x_countTime;
    x_startTime=clock();

    
    time_left=count-1;
    while (time_left>0) 
    {           

        x_countTime=clock(); 
        x_milliseconds=x_countTime-x_startTime;
        x_seconds=(x_milliseconds/(CLOCKS_PER_SEC))-(x_minutes*60);
        x_minutes=(x_milliseconds/(CLOCKS_PER_SEC))/60;
        time_left=count--;  
        //printf( "%d s\n",time_left);
        sleep(1);            
    }                        
} 


void* timer(void* hihi)
{
    while(1)
    {             
        sleep(0.5);
                    
        if( stateMachine.currState==ST_CAMERA_STARTING)
        {   
            thoigian(3);
            printf("Even -> Time out QR code\r\n");
            StateMachine_RunIteration(&stateMachine,EV_SCAN_QR_FAIL);
            StateMachine_RunIteration(&stateMachine, EV_NONE);
            pthread_cond_broadcast(&cond);
        }
        if(stateMachine.currState==ST_CAMERA_REGISTERING)
        {
            thoigian(5);
            printf("Even -> Time out\n");   
            StateMachine_RunIteration(&stateMachine, EV_TIME_OUT) ;
            StateMachine_RunIteration(&stateMachine, EV_NONE);
            pthread_cond_broadcast(&cond);

        }
        if(stateMachine.currState==ST_CAMERA_CONNECTING)
        {
            thoigian(5);
            printf("Even -> Time out\n");   
            StateMachine_RunIteration(&stateMachine, EV_TIME_OUT_CONNECTING) ;
            StateMachine_RunIteration(&stateMachine, EV_NONE);
            pthread_cond_broadcast(&cond);
        }                   
    }                
}
int main()
{

    pthread_t thread_id1,thread_id2,thread_id3;
    // main
    pthread_create(&thread_id1,NULL,&StateMachine,NULL);
    // test
    pthread_create(&thread_id2,NULL,&EvenStateMachine,NULL);
    //timer
    pthread_create(&thread_id3,NULL,&timer,NULL);
    pthread_join(thread_id1,NULL);
    pthread_join(thread_id2,NULL);
    pthread_join(thread_id3,NULL);
    return 0;
}