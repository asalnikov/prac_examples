#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>


#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>

#define SEM_NUM_OFFSET 3

#define CLIENTS_SEM     0
#define SERVER_SEM      1
#define WANT_TO_EAT_SEM 2

struct sembuf unlock_client = {CLIENTS_SEM, 1,0};
struct sembuf lock_client   = {CLIENTS_SEM,-1,0};
struct sembuf lock_server   = {SERVER_SEM,-1,0};
struct sembuf unlock_server = {SERVER_SEM, 1,0};
struct sembuf lock_want_to_eat   = {WANT_TO_EAT_SEM,-1,0};                                          
struct sembuf unlock_want_to_eat = {WANT_TO_EAT_SEM, 1,0};

int semid = -1;

void handler(int sig)
{                                
                   
   semctl(semid,0,IPC_RMID);
   write(1,"Dinner finished by server\n",26);
   exit(0);
}


int main(int argc, char **argv)
{
    int num_philosophers;
    int i;

    
    struct sembuf oper;
    oper.sem_flg=0;

    if(argc<2)
    {
        fprintf(stderr,"We need number of philosopers in first program parameter\n");
        return 1;
    }

    num_philosophers=atoi(argv[1]);
    if(num_philosophers<2)
    {
        fprintf(stderr,"Number of philosophers must be >=2\n");
        return 1;
    }


    key_t key=("/etc/passwd",'f');

    semid=semget(key, num_philosophers+SEM_NUM_OFFSET, IPC_CREAT|IPC_EXCL|0600);
    if(semid == -1) 
    { 
        perror("semget");
        return 1;
    }

    struct sigaction sigact;
    sigact.sa_handler = handler;
    sigact.sa_flags   = SA_RESTART;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGINT, &sigact, NULL);

    for(i=0; i<num_philosophers; i++)
    {
        oper.sem_num=i+SEM_NUM_OFFSET;
        oper.sem_op=1; /* Разблокируем палочки */
        semop(semid, &oper, 1);
    }

    /*
     * Позволяем есть, разблкируем семафор
     * ожидающих палочек.
     */
    semop(semid, &unlock_want_to_eat, 1);

    /*
     * Разрешаем философам начать обедать.
     */
    oper.sem_num=CLIENTS_SEM;
    oper.sem_op=num_philosophers;
    semop(semid, &oper, 1);

    printf("Dinner begin\n");

    /*
     * Блокируем сервер на время обеда философов.
     */
    semop(semid, &lock_server, 1);

    printf("Dinner finished by philosopher\n");

    semctl(semid,0,IPC_RMID);
    return 0;
}

