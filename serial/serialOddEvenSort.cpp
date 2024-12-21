#include <iostream>
#include <time.h>
#include <algorithm>
#include <iomanip>

#define ARRAY_SIZE 100000000

using namespace std;

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// A function to sort the algorithm using Odd Even sort
void oddEvenSort(int arr[], int n)
{
    bool isSorted = false; // Initially array is unsorted
    while (!isSorted)
    {
        isSorted = true;
        // Perform Bubble sort on odd indexed element
        for (int i = 1; i <= n - 2; i = i + 2)
        {
            if (arr[i] > arr[i + 1])
            {
                swap(&arr[i], &arr[i + 1]);
                isSorted = false;
            }
        }

        // Perform Bubble sort on even indexed element
        for (int i = 0; i <= n - 2; i = i + 2)
        {
            if (arr[i] > arr[i + 1])
            {
                swap(&arr[i], &arr[i + 1]);
                isSorted = false;
            }
        }
    }
    return;
}

// A utility function ot print an array of size n
void printArray(int arr[], int n)
{
    for (int i = 0; i < n; i++)
        printf("%d ", i);
    printf("\n\n");
}

// Driver program to test above functions.
int main()
{
    int *arr = new int[ARRAY_SIZE];
	srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++)
         arr[i] = rand() % ARRAY_SIZE;

    clock_t start, end;
	start = clock();

    oddEvenSort(arr, ARRAY_SIZE);

    end = clock();
	double time_taken = double(end - start) / double(CLOCKS_PER_SEC);

	cout << "TIME: " << time_taken << setprecision(10) << " secondi" << endl;

    return (0);
}
