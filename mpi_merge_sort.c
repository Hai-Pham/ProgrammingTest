		/* C program for merge sort */
		#include <stdlib.h>
		#include <stdio.h>
		#include <time.h>
		#include <mpi.h>

	
		//SAMPLE CODE - no touch - BEGIN
		/* Function to merge the two haves arr[l..m] and arr[m+1..r] of num arr[] */
		void merge(int arr[], int l, int m, int r)
		{
				int i, j, k;
				int n1 = m - l + 1;
				int n2 = r - m;


				/* create temp nums */
				int L[n1], R[n2];


				/* Copy data to temp nums L[] and R[] */
				for(i = 0; i < n1; i++)
						L[i] = arr[l + i];
				for(j = 0; j < n2; j++)
						R[j] = arr[m + 1+ j];


				/* Merge the temp nums back into arr[l..r]*/
				i = 0;
				j = 0;
				k = l;
				while (i < n1 && j < n2)
				{
						if (L[i] <= R[j])
						{
								arr[k] = L[i];
								i++;
						}
						else
						{
								arr[k] = R[j];
								j++;
						}
						k++;
				}

				/* Copy the remaining elements of L[], if there are any */
				while (i < n1)
				{
						arr[k] = L[i];
						i++;
						k++;
				}


				/* Copy the remaining elements of R[], if there are any */
				while (j < n2)
				{
						arr[k] = R[j];
						j++;
						k++;
				}
		} /* END MERGE METHOD */


		/* l is for left index and r is right index of the sub-num
		of arr to be sorted */
		void mergeSort(int arr[], int l, int r)
		{
				if (l < r)
				{
						int m = l+(r-l)/2; //Same as (l+r)/2, but avoids overflow for large l and h
						mergeSort(arr, l, m);
						mergeSort(arr, m+1, r);
						merge(arr, l, m, r);
				}
		}


		/* Function to print an num */
		void printarray(int * n, int size)
		{
				int i;
				for (i=0; i < size; i++)
				printf("%d ", n[i]);
				printf("\n");
		}
		//SAMPLE CODE - no touch - END

		void log_to_file(char * filename, int array_size, int num_proc, double elapse)
		{
			FILE *logfile; 
			double temp;
			int i; 
			
			logfile = fopen(filename,"a");
			fprintf(logfile,"\nThe total time taken for array size of %d with %d processors is: %f seconds\n", 
											array_size, num_proc, elapse);
			fclose(logfile);
		}
		 
		
		
		int main (int argc, char * argv[])
		{

				int * num;
				time_t t;
				int count; 
				
				//receive the array size from input as the first paramerter
				if (sscanf (argv[1], "%i", &count)!=1) 
				{
					printf ("Error of input. Terminating now ...");
					return -1;
				}			

				//allocate memory for num[] for each processor 
				srand((unsigned)time(&t));
				num = (int*)malloc(sizeof(int)*count)+1;	
				
				int comm_sz; //number of processors
				int my_rank; // rank number for a process
				int s; //scatter size for each  
				
				double start_time, stop_time; 
		
				//MPI Initialization
				MPI_Init(NULL, NULL);
				MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
				MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);		
				
				MPI_Status status; 
				
		
				//******************* SCATTER PHASE  *********************//
				s = count / comm_sz; //size of array each processor will receive
		
				if (my_rank%comm_sz == 0) //master will send the num to every process
				{					
					int i; 
					//randomly initialization the num, init only once and send to others
					for (i=0; i<count; i++) {
						num[i] = rand();
						//num[i] = i*(-10);
					}		
					
					start_time = clock (); //start clock count on only master process 
					
					//Scatter each s-size array to slave processes
					for (i=1; i< comm_sz; i++)
					{
						MPI_Send(num+i*s, s, MPI_INT, i, 0, MPI_COMM_WORLD);
						//printf(" Scatter phase SEND: process %d of %d sending %d to process %d\n", my_rank%comm_sz, comm_sz, num[i*s], i); //Checkpoint
					}
					mergeSort(num, my_rank*s, my_rank*s + s-1); // 0 -> s-1
				} 
				else 
				{
					MPI_Recv(num+my_rank*s, s, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
					mergeSort(num, my_rank*s, my_rank*s + s-1); // r=1: s -> 2s-1; r=2: 2s -> 3s -1		
				}
				MPI_Barrier(MPI_COMM_WORLD); 
				//******************* SCATTER PHASE END *********************//
				
				
			
				//******************* MERGE PHASE  *********************//
				
				//printf("MERGE PHASE---I am rank %d \n", my_rank); //Checkpoint
				//Example of 16 numbers, 4 processors, s = 4
				//Step 1: 1 -> 0; 3 -> 2; ....; 15 -> 14 
					//Send: 1, 3, ..., 15	: rank % 2 = 1
					//Recv: 0, 2, ..., 14
				//Step 2: 2 -> 0; 6 -> 4, 10 -> 8; 14 -> 12 
					//Send: 2, 6, 10, 14	: rank % 4 = 2 
					//Recv: 0, 4, 8, 12 
				//Step 4: 4 -> 0; 12 -> 8
					//Send: 4, 12			: rank % 8 = 4
					//Recv: 0, 8
				//Step 8: 8 -> 0
					//Send: 8				: rank % 16 = 8 
					//Recv: 0 
				//Number of steps = 4 = comm_sz/2
		
				int step = 1; 
				while (step <= (comm_sz / 2))	
				{
						//printf("Step %d \n", step);  //Checkpoint
						if (my_rank % (2*step) == step)
						{
							MPI_Send(num+my_rank*s, s*step, MPI_INT, my_rank-step, 0, MPI_COMM_WORLD);
								/*printf(" MERGE PHASE SEND: Step %d: process %d of %d sending %d to process %d\n", step, my_rank, comm_sz, num[my_rank*s], my_rank-step); //Checkpoint */
						} else if (my_rank % (2*step)  == 0)
						{
							MPI_Recv(num+my_rank*s + s*step, s*step, MPI_INT, my_rank+step, 0, MPI_COMM_WORLD, &status);
								//printf("-------MERGE PHASE RECEIVE: Step %d Process %d received number %d from process %d\n", step, my_rank, num[my_rank*s + s*step], my_rank+step); //checkpoint
								//printf("process %d involved in step %d \n", my_rank, step);  // checkpoint
							merge(num,my_rank*s, my_rank*s + s*step-1, my_rank*s + 2*s*step -1);
								//printf("MERGE PHASE my first current element of rank %d after step %d is now %d \n", my_rank, step, num[my_rank*s]); //checkpoint
						}			
					step *=2; 
				}
				//******************* MERGE PHASE END *********************/	
				

				//CHECKPOINT OUT_SCREEN PRINT and OUTPUT
				
				if (my_rank == 0)
				{
					stop_time = clock ();
					double time = (stop_time-start_time)/CLOCKS_PER_SEC; //elapse time
					
					//log to file 
					log_to_file("mpi_merge_sort.log", count, comm_sz, time); 
					
					//Last CHECKPOINT
					//printf("After sorting of rank %d\n", my_rank);
					//printarray(num, count);
					//printf("\n"); 
					printf("\nThe total time taken for array size of %d with %d processors is: %f seconds\n", count, comm_sz, time); 
				}
				
				//End MPI
				MPI_Finalize();
				return 0;
		}


