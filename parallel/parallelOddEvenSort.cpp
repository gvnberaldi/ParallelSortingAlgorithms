#include <mpi.h>
#include <time.h>
#include <algorithm>
#include <iomanip>
#include <iostream>

#define array_size 100000000

using namespace std;

void Merge_low(int local_A[], int temp_B[], int temp_C[], int local_n);
void Merge_high(int local_A[], int temp_B[], int temp_C[], int local_n);
int  compare(const void* a_p, const void* b_p);
void Sort(int* local_array, int elements, int rank, int p, MPI_Comm comm);
void Odd_even_iter(int* local_array, int temp_B[], int temp_C[], int local_n, int phase, int even_partner, int odd_partner, 
                   int my_rank, int p, MPI_Comm comm);
void Print_global_list(int local_A[], int local_n, int my_rank, int p, MPI_Comm comm);

void fill(int *a){
    for(int i = 0; i < array_size; i++)
        a[i] = rand()%array_size;
}

void print_local_list(int *a, int local_n){
    for(int i = 0; i < local_n ;i++)
        cout << a[i] << " ";
    cout<<endl;
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
         printf("%d ", i);
      printf("\n\n");
      free(A);
   } 
   else 
      MPI_Gather(local_A, local_n, MPI_INT, A, local_n, MPI_INT, 0, comm);
} 

int compare (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

void Sort(int* local_array, int elements, int rank, int p, MPI_Comm comm) {
   int phase;
   int *temp_B, *temp_C;
   int even_partner; 
   int odd_partner;  

   temp_B = (int*) malloc(elements*sizeof(int));
   temp_C = (int*) malloc(elements*sizeof(int));

   if (rank % 2 != 0) {   /* odd rank */
      even_partner = rank - 1;
      odd_partner = rank + 1;
      if (odd_partner == p) odd_partner = MPI_PROC_NULL;
   } else {                 
      even_partner = rank + 1;
      if (even_partner == p) even_partner = MPI_PROC_NULL;
      odd_partner = rank-1;
   }

   qsort(local_array, elements, sizeof(int), compare);

   for (phase = 0; phase < p; phase++)
      Odd_even_iter(local_array, temp_B, temp_C, elements, phase, even_partner, odd_partner, rank, p, comm);

   free(temp_B);
   free(temp_C);
}

void Odd_even_iter(int* local_array, int temp_B[], int temp_C[], int local_n, int phase, int even_partner, int odd_partner, 
                   int my_rank, int p, MPI_Comm comm) {
   
   MPI_Status status;

   if (phase % 2 == 0) { /* even phase */
      if (even_partner >= 0) { /* check for even partner */
         MPI_Sendrecv(local_array, local_n, MPI_INT, even_partner, 0,
            temp_B, local_n, MPI_INT, even_partner, 0, comm,
            &status);
         if (my_rank % 2 != 0) /* odd rank */
            // local_array have largest local_n ints from local_A and even_partner
            Merge_high(local_array, temp_B, temp_C, local_n);
         else /* even rank */
            // local_array have smallest local_n ints from local_A and even_partner
            Merge_low(local_array, temp_B, temp_C, local_n);
      }
   } else { /* odd phase */
      if (odd_partner >= 0) {  /* check for odd partner */
         MPI_Sendrecv(local_array, local_n, MPI_INT, odd_partner, 0,
            temp_B, local_n, MPI_INT, odd_partner, 0, comm,
            &status);
         if (my_rank % 2 != 0) /* odd rank */
            Merge_low(local_array, temp_B, temp_C, local_n);
         else /* even rank */
            Merge_high(local_array, temp_B, temp_C, local_n);
      }
   }
}

void Merge_low(int local_A[], int temp_B[], int temp_C[], int local_n) {
   int ai = 0, bi = 0, ci = 0;
   while (ci < local_n) {
      if (local_A[ai] <= temp_B[bi]) {
         temp_C[ci] = local_A[ai];
         ci++; ai++;
      } else {
         temp_C[ci] = temp_B[bi];
         ci++; bi++;
      }
   }

   memcpy(local_A, temp_C, local_n*sizeof(int));
}

void Merge_high(int local_A[], int temp_B[], int temp_C[], int local_n) {
   int ai, bi, ci;
   
   ai = local_n-1;
   bi = local_n-1;
   ci = local_n-1;
   while (ci >= 0) {
      if (local_A[ai] >= temp_B[bi]) {
         temp_C[ci] = local_A[ai];
         ci--; ai--;
      } else {
         temp_C[ci] = temp_B[bi];
         ci--; bi--;
      }
   }

   memcpy(local_A, temp_C, local_n*sizeof(int));
}

int main(int argc,char** argv){
    
    int rank, num_proc;
    int* array;
    double time_taken;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&num_proc);

    int elements = array_size / num_proc;
	 array = new int[elements];

    if (rank == 0){
		int* tmp = new int[elements];
		for (int i = 0; i < elements; i++)
			array[i] = rand() % array_size;

		for (int j = 1; j < num_proc; j++){
			for (int i = 0; i < elements; i++)
				tmp[i] = rand() % array_size;
			MPI_Send(tmp, elements, MPI_INT, j, 50, MPI_COMM_WORLD);
		}
		delete[] tmp;
	}
	else
		MPI_Recv(array, elements, MPI_INT, 0, 50, MPI_COMM_WORLD, &status);
    
    srand(time(NULL));
 
    time_taken -= MPI_Wtime();

    Sort(array, elements, rank, num_proc, MPI_COMM_WORLD);
    
    time_taken += MPI_Wtime();

    if (rank == 0) 
        cout << "TIME: " << time_taken << setprecision(10) << " secondi" << endl;

 
    MPI_Finalize();
    delete[] array;
    return 0;
}
