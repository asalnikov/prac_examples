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
   semop(semid, &unlock_server, 1);
   exit(0);
}

int main(int argc, char **argv)
{
    int num_philosophers;
    int my_number;
    int i;

    
    struct sembuf oper;
    oper.sem_flg=0;


    if(argc<3)
    {
        fprintf(stderr,"We need philosopher number in first program parameter\n"
                       "and total number of philosophers in second.\n"
                       "First philosopher number is 1\n");
        return 1;
    }

    my_number=atoi(argv[1]);
    if(my_number < 1)
    {
        fprintf(stderr,"First philosopher number is 1\n");
        return 1;
    }

    num_philosophers=atoi(argv[2]);
    if(num_philosophers<2)
    {
        fprintf(stderr,"Number of philosophers must be >=2\n");
        return 1;
    }


    key_t key=("/etc/passwd",'f');

    semid=semget(key,num_philosophers+SEM_NUM_OFFSET, 0);
    if(semid == -1)
    {
        perror("semget");
        return 1;
    }

    srand(getpid());

    int left_fork_sem  = (my_number - 1) + SEM_NUM_OFFSET;
    int right_fork_sem = ((my_number == num_philosophers)? 0 : my_number) + SEM_NUM_OFFSET;

    /*
     * Ждем пока сервер подготовит семафоры 
     * для палочек.
     */
    semop(semid, &lock_client, 1);

    struct sigaction sigact;
    sigact.sa_handler = handler;
    sigact.sa_flags   = SA_RESTART;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGINT, &sigact, NULL);


    while(1)
    {
        printf("Think\n");
        sleep(((double)rand()/RAND_MAX)*4.0);

        printf("Hungry\n");
        
        /*
         * Нам необходимо, чтобы в один момент 
         * времени палочки пытался брать только один философ.
         *
         * Это нужно, чтобы палочки всегда брались именно парой,
         * чтобы не получилось так, что один взял левую, а дугой правую первого 
         * и оба ждут, чтобы взять свою вторую палочку.
         */
        if(semop(semid, &lock_want_to_eat, 1))
        {
            return 0;
        } 
       
        printf("Take left %d\n",left_fork_sem);
        oper.sem_num=left_fork_sem;
        oper.sem_op=-1; /* блокируем левую палочку */
        if(semop(semid, &oper, 1) == -1)
        {
            return 0;
        }

        printf("Take right %d\n",right_fork_sem);

        oper.sem_num=right_fork_sem;
        oper.sem_op=-1; /* блокируем правую палочку */
        if(semop(semid, &oper, 1) == -1)
        {
            return 0;
        }

        /*
         * Позволяем другим процессам брать палочки.
         */
        if(semop(semid, &unlock_want_to_eat, 1))
        {
            return 0;
        } 

        printf("Eat\n");
        sleep(((double)rand()/RAND_MAX)*4.0);

        printf("Put left %d\n", left_fork_sem);
        oper.sem_num=left_fork_sem;
        oper.sem_op=1; /* кладем левую палочку */
        if(semop(semid, &oper, 1) == -1)
        {
            return 0;
        }

        printf("Put right %d\n",right_fork_sem);

        oper.sem_num=right_fork_sem;
        oper.sem_op=1; /* кладем правую палочку */
        if(semop(semid, &oper, 1) == -1)
        {
            return 0;
        }
    }

    return 100;
}

