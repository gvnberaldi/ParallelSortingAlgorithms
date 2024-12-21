#include <iostream>
#include <time.h>
#include <iomanip>
using namespace std;

#define ARRAY_SIZE 10000000

void swap(int* a, int* b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

int partition (int arr[], int low, int high){
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

/* Function to print an array */
void printArray(int arr[], int size){
	for (int i = 0; i < size; i++)
		cout << arr[i] << " ";
	cout << endl;
}

// Driver Code
int main()
{
	int *arr = new int[ARRAY_SIZE];
	srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++)
         arr[i] = rand() % ARRAY_SIZE;

	clock_t start, end;
	start = clock();

	quickSort(arr, 0, ARRAY_SIZE - 1);

	end = clock();
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

	cout << "TIME: " << time_taken << setprecision(10) << " secondi" << endl;
    
	return 0;
}
