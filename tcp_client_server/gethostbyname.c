#include <netdb.h>
#include <stdio.h>

int main(int argc,char **argv)
{
	struct hostent *h=NULL;

	h=gethostbyname(argv[1]);

	printf("%d\n",h->h_addr_list[0]);

}
