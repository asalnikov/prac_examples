/*
 * This code author is Alexey Salnikov. (2012)
 *
 * This is an example of running thomething in background mode.
 *
 * For mode details you should read the book:
 * Michael Kerrisk The Linux Programming Interface: A Linux and Unix 
 * System Programming Handbook pp. 699-732
 * ISBN-10: 1-59327-220-0
 * ISBN-13: 978-1-59327-220-3 
 * 
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

/**
 * This is a handler for catching All!!!
 */ 
void handler(int sig)
{
	fprintf(stderr,"Catch signal %d\n",sig);
	signal(sig,handler); 
}

int main()
{
	int c;
	pid_t ses_id;
	pid_t pid;

    pid=fork();

    /*
     * Son
     */
	if(pid==0)
	{
		/*
         * Uncomment next code if you want to see
         * messages on recieving SIGTTIN when 
         * process tries to catch stdin in background
         */  
        signal(SIGTTIN,handler);
		pid=getpid();
		ses_id=getpgid(pid);
		/*
		*/
        /*
         * Ну в общем мне надоело писать по английски,
         * буду писать по русски.
         *
         * До следующих строчек в коде созданый сыновий процесс и
         * родительский процесс находятся в одной группе процессов,
         * что означает, что терминал принадлежит им обоим.
         * Чтобы сына "отодрать" от терминала меняем ему группу 
         * процессов на новую, собственно, на такую же как какой
         * у него сыновий pid. В результате он не будет получать 
         * всякую лабуду с клавиатуры в виде ctrl+c и
         * ctrl+z.
         */

        /*
         * Распечатка старой группы
         */
		printf("son: pid=%d, session_id=%d\n",pid,ses_id);
		setpgid(pid,pid);
		ses_id=getpgid(pid);

        /*
         * Собственно здесь мы уже фоновый
         * процесс. 
         */

        /*
         * Распечатка новой группы
         */
		printf("son: pid=%d, session_id=%d\n",pid,ses_id);

        /*
         * Это чтобы сигнал словить
         */
        /*
		c=getchar();

        printf("Son: read symbol from stdin with code %d drowed as '%c'\n",c,c);
        */

        /*
         * Запускаем wc  (подсчёт числа символов и слов в фоне).
         * Отец потом должен его вернуть обратно на передний план.
         */
        printf("Son: running wc, please type anything and then ctrl+d\n");
		execlp("wc","wc",NULL);
		printf("Sorry can't exec 'cat'\n");

		return -1;
	}
	
    /*
     * Father 
     */
    
    /*
     * А вот отец по идее должен смочь читать из терминала.
     */
	c=getchar();
	printf("Father: c value is '%c'\n",(char)c);
	/* close(0); */

    /*
     * Устанавливаем для сына ту же группу, что и у отца,
     * и выставляем для него сигнал, чтобы он мог продолжить
     * работу.
     *
     * По идее он теперь может читать с терминала и по 
     * предлагаемому коду дочитает от туда символ по getchar.
     */
	if(tcsetpgrp(0 /* stdin */,pid))
    {
        perror("Father main: ");
    }
	kill(pid,SIGCONT); 
    
    /*
     * Собственно ждём сына.
     */
	wait(&c);
    
    /*
     * Казалось бы всё нужно вернуть обратно здесь,
     * но похоже со смертью сына всё возвращается
     * обратно само.
     */    
    #if 0
    tcsetpgrp(0 /* stdin */,getpid());
    #endif

	return 0;
}
 
