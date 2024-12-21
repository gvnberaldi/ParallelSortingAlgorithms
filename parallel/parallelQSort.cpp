#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <iostream>

#define ARRAY_SIZE 10000000

using namespace std;

void swap(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high){
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
        if (arr[j] < pivot){
            i++;
            swap(&arr[i], &arr[j]);
        }

    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int arr[], int low, int high){
    if (low < high){
        int pi = partition(arr, low, high);

        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int *merge(int *arr1, int n1, int *arr2, int n2){
    int *result = new int[n1 + n2];
    int i = 0, j = 0, k;

    for (k = 0; k < n1 + n2; k++){
        if (i >= n1){
            result[k] = arr2[j];
            j++;
        }
        else if (j >= n2){
            result[k] = arr1[i];
            i++;
        }  
        else if (arr1[i] < arr2[j]){
            result[k] = arr1[i];
            i++;
        }
        else {
            result[k] = arr2[j];
            j++;
        }
    }
    return result;
}

int main(int argc, char *argv[])
{
    int number_of_elements = ARRAY_SIZE;
    int *data = NULL;
    int chunk_size, own_chunk_size;
    int *chunk;
    double time_taken;
    MPI_Status status;

    int number_of_process, rank_of_process;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);

    if (rank_of_process == 0){
        chunk_size = (number_of_elements % number_of_process == 0) ? (number_of_elements / number_of_process)
                                                                   : (number_of_elements / (number_of_process - 1));
        data = new int[number_of_process * chunk_size];

        srand(time(NULL));
        for (int i = 0; i < number_of_elements; i++)
            data[i] = rand() % number_of_elements;

        for (int i = number_of_elements; i < number_of_process * chunk_size; i++)
            data[i] = 0;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    time_taken -= MPI_Wtime();

    MPI_Bcast(&chunk_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    chunk = new int[chunk_size];

    MPI_Scatter(data, chunk_size, MPI_INT, chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    delete[] data;

    own_chunk_size = (number_of_elements >= chunk_size * (rank_of_process + 1)) ? chunk_size
                                                                                : (number_of_elements - chunk_size * rank_of_process);

    quickSort(chunk, 0, own_chunk_size);

    int step = 1;
    while (step < number_of_process){
        if (rank_of_process % (2 * step) == 0){
            if (rank_of_process + step < number_of_process){
                int parter_chunk_size;
                int* parter_chunk;
                MPI_Recv(&parter_chunk_size, 1, MPI_INT, rank_of_process + step, 0, MPI_COMM_WORLD, &status);
                parter_chunk = new int[parter_chunk_size];
                MPI_Recv(parter_chunk, parter_chunk_size, MPI_INT, rank_of_process + step, 0, MPI_COMM_WORLD, &status);
                chunk = merge(chunk, own_chunk_size, parter_chunk, parter_chunk_size);
                own_chunk_size += parter_chunk_size;
                delete[] parter_chunk;
            }
        }
        else{
            MPI_Send(&own_chunk_size, 1, MPI_INT, rank_of_process - step, 0, MPI_COMM_WORLD);
            MPI_Send(chunk, own_chunk_size, MPI_INT, rank_of_process - step, 0, MPI_COMM_WORLD);
            break;
        }
        step = step * 2;
    }

    time_taken += MPI_Wtime();

    if(rank_of_process == 0)
        cout << "TIME: " << time_taken << setprecision(10) << " secondi" << endl;

    MPI_Finalize();
    return 0;
}
