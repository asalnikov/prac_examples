#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "file_reader.h"


int *array=NULL;
int array_length=0;
int num_swaps=-1;



int main(int argc,char **argv)
{
	int i;
	

	if(argc<2)
	{
		printf("Arguments are: sorter file_name\n");
		return 1;
	}	
	
	

	if(read_array(&array,&array_length,argv[1]))
	{
		fprintf(stderr,"Can't read array from file '%s'\n",argv[1]);
		return -1;
	}

	while(num_swaps)
	{
		num_swaps=0;
		for(i=1;i<array_length;i++)
		{
			if(array[i]<array[i-1])
			{
				int val;
				val=array[i];
				array[i]=array[i-1];
				array[i-1]=val;
				num_swaps++;
			}	
		}
		printf("num_swaps %d\n",num_swaps);		
	}	
	
	for(i=0;i<array_length;i++)
	{
		printf("array[%d]=%d\n",i,array[i]);
	}
	
	free(array);	

	return 0;
}

