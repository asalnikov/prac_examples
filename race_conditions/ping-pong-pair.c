/*
 * Task:
 * There are two processes gaming into the ping-pong.
 * First process 'father' creates by fork() second process 'son'.
 *
 * Then father begin the game. It put ball (integer variable) with value zero into the
 * pipe. Next, the  son get it from the pipe, incriment the ball value and  put it back
 * into the pipe.
 *
 * Next, the father get ball  from the pipe  ....
 *
 * Game finishes when one of processes achive limit specified in arguments 
 * of program.
 *
 * It is allowed to use only one pipe and signals. Other IPC primitives are
 * disallowed.
 *
 * But really we see race-condition here.
 *
 * Reasoning for race is a signal catch by handler before actual
 * remote_pid is assigned.
 * 
 */
#define _POSIX_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define UNKNOWN -1
#define SON      0
#define FATHER   1

volatile int ball = 0;
volatile pid_t target_pid;
volatile int stop;
volatile int who_am_is=UNKNOWN;

int fd[2];


void myhnd_work(int s)
{
    int check;
    int cur;
    

    check = read(fd[0], &cur, sizeof(int));
    if (check == 0)
    {
        printf("\nCan not read from %s\n",
                (who_am_is == FATHER) ? "son" : "father" );
        kill(target_pid, SIGUSR2);
        exit(0);
    }
 
    ball = cur;
    ++ball;
   
    printf("\nball = %d, in %s\n", ball,
                (who_am_is == FATHER) ? "son" : "father" );


    if (ball == stop)
    {
        if (who_am_is == SON)
        {
            printf("\nEnd of son on %d\n", ball);
            printf("\nSon`s PID = %d\n", getpid());
            printf("\nFather`s PID = %d\n", getppid());
        } 
        else /* father */
        {
            printf("\nEnd of father on %d\n", ball);
            printf("\nFather`s PID = %d\n", getpid());
            printf("\nSon`s PID = %d\n", target_pid);
        }
       
        close(fd[0]);
        close(fd[1]);
        
        kill(target_pid, SIGUSR2);

        exit(0);
    }

    cur=ball;

    check = write(fd[1], &cur, sizeof(int));
    if (check != sizeof(int)) 
    {
        printf("\nSometimes we have problems\n");
        exit(0);
    }

    signal(SIGUSR1, myhnd_work);
    kill(target_pid, SIGUSR1);
}


void myhnd_finish(int s)
{
    close(fd[0]);
    close(fd[1]);
    if(who_am_is == FATHER)
    {
        wait(NULL);
    }

    printf("\nGame is over\n");
    exit(0);
}


int main(int argc, char **argv)
{
    pid_t pid;
    pid_t father_pid;

    if (argc < 2)
    {
        printf("\nNot enough arguments\n");
        return 1;    
    }
    
    stop = atoi(argv[1]);
    pipe(fd);

    signal(SIGUSR1, myhnd_work);    
    signal(SIGUSR2, myhnd_finish);
        
    father_pid=getpid();

    pid = fork();

    
    if (pid == -1)
    {
        printf("\nFork has been failed\n");
        perror("Troubles with fork:");
        return 2;
    }

    if (pid==0)
    {
        target_pid = father_pid;
        who_am_is=SON;
         
        while (1)
        {
            pause();
        }
    }

    else /* father */
    { 
       int check;
       int tmp;

       target_pid = pid;
       who_am_is=FATHER;

       if (ball == stop)
       {
           kill(target_pid, SIGUSR2);
           printf("\nBall in father = 0\n");
           printf("\nGame is over\n");
           exit(0);
       }
       
       tmp=ball;
       check = write(fd[1], &tmp, sizeof(int));
       if (!check)
       {
           printf("\nSometimes we have mistakes\n");
           kill(target_pid, SIGUSR2);

           exit(0);
       }

       printf("\nBall in father = %d\n", ball);
       
       kill(target_pid, SIGUSR1);
      
       while (1)
       {
           pause();
       }
    }
    
    /*
     * Very serious error if we are here!!!
     */
    return 100;
}

