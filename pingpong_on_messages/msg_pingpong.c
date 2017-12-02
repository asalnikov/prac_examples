#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#define MTYPEBALL_1 11
#define MTYPEBALL_2 12
#define MYSTOP_1 1
#define MYSTOP_2 2
#define WORK 1
#define STOP 0

typedef struct 
{
    long mtype;
    int data;
    
} Message;

int main(int argc,char *argv[])
{
    key_t key;
    Message my_msg,my_msg1;
    int msgid;
    int ball,stop;
    stop = atoi(argv[2]);
    key = ftok("/bin/ls",'L');
    if(atoi(argv[1])==1)
    {
        msgid = msgget(key,0664|IPC_CREAT|IPC_EXCL);
        if(msgid == -1) 
        {
            perror("msgget "); 
            return 2;
        }
        ball = 0;
        while(ball < stop)
        {
            ball++;
            my_msg1.data = WORK;
            my_msg1.mtype = MYSTOP_1;
            my_msg.data = ball;
            my_msg.mtype = MTYPEBALL_1;
            msgsnd(msgid,&my_msg1,sizeof(Message)-sizeof(long),0);
            msgsnd(msgid,&my_msg,sizeof(Message)-sizeof(long),0);
            msgrcv(msgid,&my_msg1,sizeof(Message)-sizeof(long),MYSTOP_2,0);
            if(my_msg1.mtype == STOP)
            {
                msgctl(msgid,IPC_RMID,NULL);
                printf(" ADIOS %ld\n",ball);
                return 0;
            }
            msgrcv(msgid,&my_msg,sizeof(Message) - sizeof(long),MTYPEBALL_2,0);
            ball = my_msg.data;
            printf("ball  %ld \n",ball); 
            
        }
        my_msg1.data = STOP;
        my_msg1.mtype = MYSTOP_1;
        msgsnd(msgid,&my_msg1,sizeof(Message)-sizeof(long),0);
        msgrcv(msgid,&my_msg1,sizeof(Message)-sizeof(long),MYSTOP_2,0);
        msgctl(msgid,IPC_RMID,NULL);
        printf(" ADIOS %ld\n",ball);
        return 0;

    }
     if(atoi(argv[1])==2)
    {
        msgid = msgget(key,0);
        if(msgid == -1) 
        {
            perror("msgget "); 
            return 2;
        }
        
        while(ball < stop)
        {
            msgrcv(msgid,&my_msg1,sizeof(Message)-sizeof(long),MYSTOP_1,0);
            if(my_msg1.mtype == STOP)
            {
                my_msg1.data = STOP;
                my_msg1.mtype = MYSTOP_2;
                msgsnd(msgid,&my_msg1,sizeof(Message)-sizeof(long),0);
                printf(" ADIOS %ld\n",ball);
                return 0;
            }
            msgrcv(msgid,&my_msg,sizeof(Message) - sizeof(long),MTYPEBALL_1,0);
            ball = my_msg.data;
            printf("ball  %ld \n",ball); 
         
            ball++;
            my_msg1.data = WORK;
            my_msg1.mtype = MYSTOP_2;
            my_msg.data = ball;
            my_msg.mtype = MTYPEBALL_2;
            msgsnd(msgid,&my_msg1,sizeof(Message)-sizeof(long),0);
            msgsnd(msgid,&my_msg,sizeof(Message)-sizeof(long),0);
           
        }
        my_msg1.data = STOP;
        my_msg1.mtype = MYSTOP_2;
        msgsnd(msgid,&my_msg1,sizeof(Message)-sizeof(long),0);
        printf(" ADIOS %ld\n",ball);
        return 0;

    }
    
    return 1;
}
