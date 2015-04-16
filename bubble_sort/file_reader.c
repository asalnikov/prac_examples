#include <stdio.h>
#include <stdlib.h>


#define ALLONGATION_SIZE 1024

int read_array(int **array,int* array_length, const char *file_name)
{
	FILE *f;
	int *new_arr=NULL;
	int i;
	int items_read=1;
	size_t current_array_size=0;
	int num_elements=0;

	f=fopen(file_name,"r");
	if(f==NULL)
	{
		fprintf(stderr,"Can't open file '%s'\n",file_name);
		return -1;
	}

	while(items_read==1)
	{	
		current_array_size+=ALLONGATION_SIZE*sizeof(int);
		new_arr=(int *)realloc(new_arr,current_array_size);
		
		if(new_arr==NULL)
		{
			fprintf(stderr,"Can't allocate memory for array\n");
			return -2;
		}
	
		for(i=num_elements;i<num_elements+ALLONGATION_SIZE;i++)
		{
			items_read=fscanf(f,"%d",&new_arr[i]);
			if(items_read!=1)
			{
				break;
			}
		}
		num_elements+=ALLONGATION_SIZE;
		
	}
	
	*array=new_arr;
	*array_length=i;

	return 0;
}


