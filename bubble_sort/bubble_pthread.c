#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "file_reader.h"


int *array=NULL;
int array_length=0;

pthread_mutex_t* mutex_border=NULL;

pthread_mutex_t global_sum_mutex;
pthread_cond_t global_sum_cond;

int *thread_ids=NULL;
int num_threads=0;
int thread_counter=0;

int num_swaps=-1;

int flag=1;


void *handler (void *arg)
{
	int left_border;
	int right_border;
	int local_num_swaps=0;

	int i,val;

	int thread_id=*(int *)arg;
	
	printf("thread %d is alive\n",thread_id);
		
	left_border=array_length/num_threads*thread_id;
	if(thread_id==num_threads-1)
	{
		right_border=array_length;
	}
	else
	{
		right_border=array_length/num_threads*(thread_id+1);
	}


	while(flag)
	{
		
		if(left_border)
		{
			pthread_mutex_lock(&mutex_border[thread_id-1]);
				if(array[left_border-1]>array[left_border])
				{
					val=array[left_border];
					array[left_border]=array[left_border-1];
					array[left_border-1]=val;
					local_num_swaps=1;
				}
		
				if(array[left_border]>array[left_border+1])
				{
					val=array[left_border];
					array[left_border]=array[left_border+1];
					array[left_border+1]=val;
					local_num_swaps++;	
				}				
			pthread_mutex_unlock(&mutex_border[thread_id-1]);
		
		}
		else /* thread 0 */
		{
			if(array[0]>array[1])
			{
				val=array[0];
                        	array[0]=array[1];
                        	array[1]=val;
                        	local_num_swaps=1;
			}
		}

		for(i=left_border+2;i<right_border-1;i++)
		{
			if(array[i-1]>array[i])
			{
				val=array[i];
				array[i]=array[i-1];
				array[i-1]=val;
				local_num_swaps++; 
			}
		}

		if(right_border==array_length)
		{
			if(array[array_length-2]>array[array_length-1])
			{
				val=array[array_length-2];
				array[array_length-2]=array[array_length-1];
				array[array_length-1]=val;
				local_num_swaps++;
			}
		}
		else /* Internal thread */
		{
			pthread_mutex_lock(&mutex_border[thread_id]);
				if(array[right_border-2]>array[right_border-1])
				{
					val=array[right_border-2];
					array[right_border-2]=array[right_border-1];
					array[right_border-1]=val;
					local_num_swaps++;
				}
				
				if(array[right_border-1]>array[right_border])
				{
					val=array[right_border-1];
					array[right_border-1]=array[right_border];
					array[right_border]=val;
					local_num_swaps++;
				}

			pthread_mutex_unlock(&mutex_border[thread_id]);

		}
		
		pthread_mutex_lock(&global_sum_mutex);
			num_swaps+=local_num_swaps;
			printf("thread %d num swaps %d\n",thread_id,local_num_swaps);
			thread_counter++;
			local_num_swaps=0;
			if(thread_counter==num_threads)
			{ 
				if(num_swaps==0) flag=0;
				printf("thread %d global num swaps %d\n",thread_id,num_swaps);
				num_swaps=0;
				thread_counter=0;
				pthread_cond_broadcast(&global_sum_cond);
			}
			else
				pthread_cond_wait(&global_sum_cond,&global_sum_mutex);
			
		pthread_mutex_unlock(&global_sum_mutex);
		
	}
		

  return NULL;
}

int main(int argc,char **argv)
{
	int i;
	
	pthread_t* threads=NULL;
	
	if(argc<3)
	{
		printf("Arguments are: sorter num_threads file\n");
		return 1;
	}	
	
	num_threads=atoi(argv[1]);
	
	threads=(pthread_t *)malloc(num_threads*sizeof(pthread_t));
	thread_ids=(int *)malloc(num_threads*sizeof(int));
	mutex_border=(pthread_mutex_t *)malloc((num_threads-1)*sizeof(pthread_mutex_t));
	
	if(read_array(&array,&array_length,argv[2]))
	{
		fprintf(stderr,"Can't read array from file '%s'\n",argv[2]);
		return -1;
	}
	
	for(i=0;i<num_threads;i++)
	{
		thread_ids[i]=i;
	}

	for(i=0;i<num_threads-1;i++)
	{
		pthread_mutex_init(&mutex_border[i],NULL);
	}
	pthread_mutex_init(&global_sum_mutex,NULL);
	pthread_cond_init(&global_sum_cond,NULL);
	
	for(i=0;i<num_threads;i++)
	{
		if(pthread_create(&threads[i],NULL,handler,&thread_ids[i]))
		{
			perror("Can't create thread\n");
			return -1;
		}
	}
	/*
	while(!flag)
	{
		pthread_mutex_lock(&global_sum_mutex);
			
			if(num_swaps==0)
			{
				flag=1;
			}
			thread_counter=0;
			num_swaps=0;
			pthread_cond_wait(&global_sum_cond,&global_sum_mutex);	
			
	      pthread_mutex_unlock(&global_sum_mutex);

	}
	*/

	for(i=0;i<num_threads;i++)
	{
		pthread_join(threads[i],NULL);
	}

	free(threads);
	free(thread_ids);
	
	for(i=0;i<num_threads-1;i++)
	{
		pthread_mutex_destroy(&mutex_border[i]);
	}
	pthread_mutex_destroy(&global_sum_mutex);
	pthread_cond_destroy(&global_sum_cond);
	
	
	for(i=0;i<array_length;i++)
	{
		printf("array[%d]=%d\n",i,array[i]);
	}
	
	free(array);	

	return 0;
}

