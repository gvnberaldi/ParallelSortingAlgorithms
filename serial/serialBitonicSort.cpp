#include <iostream>
#include <time.h>
#include <iomanip>

#define ARRAY_SIZE 100000000

using namespace std;

void swap(int* a, int* b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

void compAndSwap(int a[], int i, int j, int dir){
	if (dir==(a[i]>a[j]))
		swap(&a[i], &a[j]);
}

void bitonicMerge(int a[], int low, int cnt, int dir){
	if (cnt>1)
	{
		int k = cnt/2;
		for (int i=low; i<low+k; i++)
			compAndSwap(a, i, i+k, dir);
		bitonicMerge(a, low, k, dir);
		bitonicMerge(a, low+k, k, dir);
	}
}

void bitonicSort(int a[], int low, int cnt, int dir){
	if (cnt>1)
	{
		int k = cnt/2;

		bitonicSort(a, low, k, 1);
		bitonicSort(a, low+k, k, 0);
        
		bitonicMerge(a,low, cnt, dir);
	}
}

/* Function to print an array */
void printArray(int arr[], int size){
	for (int i = 0; i < size; i++)
		cout << arr[i] << " ";
	cout << endl;
}

int compare (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

// Driver code
int main(){
	int *arr = new int[ARRAY_SIZE];
	srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++)
         arr[i] = rand() % ARRAY_SIZE;

	int up = 1; // means sort in ascending order

    clock_t start, end;
	start = clock();

	//bitonicSort(arr, 0, ARRAY_SIZE, up);
	qsort(arr, ARRAY_SIZE, sizeof(int), compare);

	end = clock();
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

	cout << "TIME: " << time_taken << setprecision(10) << " secondi" << endl;
    
	return 0;
}
