#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>
#include <iomanip>
#include <iostream>

using namespace std;

#define arrayElements 100000000

int compare(const int *p, const int *q);
void mergeLow(int elements, int array[], int temp[]);
void mergeHigh(int elements, int array[], int temp[]);
int log_2(int x);
void Print_global_list(int local_A[], int local_n, int my_rank, int p, MPI_Comm comm);

int main(int argc, char *argv[]){

	int i, j, k;
	int elements;
	int nproc, rank;
	int *array, *tmp;
	int mask, mask2, partner, dim;
	double time_taken;

	MPI_Request request, request_array;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	elements = arrayElements / nproc;
	array = new int[elements];

	if (rank == 0){
		tmp = new int[elements];
		srand((double) time(NULL));
		for (i = 0; i < elements; i++)
			array[i] = rand() % arrayElements;

		for (j = 1; j < nproc; j++){
			for (i = 0; i < elements; i++)
				tmp[i] = rand() % arrayElements;
			MPI_Send(tmp, elements, MPI_INT, j, 50, MPI_COMM_WORLD);
		}
		delete[] tmp;
	}
	else
		MPI_Recv(array, elements, MPI_INT, 0, 50, MPI_COMM_WORLD, &status);

    time_taken -= MPI_Wtime();
	
	qsort(array, elements, sizeof(int), (int (*)(const void *, const void *))(compare));

    int* partnerArray = new int[elements];

	for (i = 2, mask = 2; i <= nproc; i *= 2, mask = mask << 1){
		dim = log_2(i);
		mask2 = 1 << (dim - 1);

		if ((rank & mask) == 0){
			for (j = 0; j < dim; j++){
				partner = rank ^ mask2;
				if (rank < partner){
					MPI_Sendrecv(array, elements, MPI_INT, partner, 100,
								 partnerArray, elements, MPI_INT, partner, 100, MPI_COMM_WORLD, &status); 
					mergeLow(elements, array, partnerArray); 
				}
				else{
					MPI_Sendrecv(array, elements, MPI_INT, partner, 100,
								 partnerArray, elements, MPI_INT, partner, 100, MPI_COMM_WORLD, &status);
					mergeHigh(elements, array, partnerArray);
				}
				mask2 = mask2 >> 1;
			}
		}
		else{
			for (j = 0; j < dim; j++){
				partner = rank ^ mask2;
				if (rank > partner){
					MPI_Sendrecv(array, elements, MPI_INT, partner, 100,
								 partnerArray, elements, MPI_INT, partner, 100, MPI_COMM_WORLD, &status);
					mergeLow(elements, array, partnerArray);
				}
				else{
					MPI_Sendrecv(array, elements, MPI_INT, partner, 100,
								 partnerArray, elements, MPI_INT, partner, 100, MPI_COMM_WORLD, &status);
					mergeHigh(elements, array, partnerArray);
				}
				mask2 = mask2 >> 1;
			}
		}
	}
	
    time_taken += MPI_Wtime();

	if(rank == 0)
        cout << "TIME: " << time_taken << setprecision(10) << " secondi" << endl;


	MPI_Finalize();

	return 0;
}

int compare(const int *p, const int *q){
	if (*p < *q)
		return -1;
	else if (*p == *q)
		return 0;
	else
		return 1;
}

int log_2(int x){
	int i = 0;
	while (x > 1){
		x = x / 2;
		i++;
	}
	return i;
}

void mergeLow(int elements, int* array, int* partenerArray){
	int i, j = 0, k = 0;

    int *result = new int[elements];

	for (i = 0; i < elements; i++)
		if (array[j] <= partenerArray[k]){
			result[i] = array[j];
			j++;
		}
		else{
			result[i] = partenerArray[k];
			k++;
		}

	for (i = 0; i < elements; i++)
		array[i] = result[i];

    delete[] result;
}

void mergeHigh(int elements, int* array, int* partenerArray){
	int i, j = 0, k = 0;

    int *result = new int[elements];

	for (i = 0; i < elements; i++)
		if (array[j] >= partenerArray[k]){
			result[i] = array[j];
			j++;
		}
		else{
			result[i] = partenerArray[k];
			k++;
		}

	for (i = 0; i < elements; i++)
		array[i] = result[i];

    delete[] result;
}


void Print_global_list(int local_A[], int local_n, int my_rank, int p, MPI_Comm comm) {
   int* A = NULL;
   int i, n;

   if (my_rank == 0) {
      n = p*local_n;
      A = (int*) malloc(n*sizeof(int));
      MPI_Gather(local_A, local_n, MPI_INT, A, local_n, MPI_INT, 0, comm);
      printf("Global list:\n");
      for (i = 0; i < n; i++)
         printf("%d ", A[i]);
      printf("\n\n");
      free(A);
   } 
   else 
      MPI_Gather(local_A, local_n, MPI_INT, A, local_n, MPI_INT, 0, comm);
}
