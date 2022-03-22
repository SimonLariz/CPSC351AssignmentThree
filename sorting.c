/*********************************************************************************************
 *                        Sorting Program                                                     *
 *   Author(s):                                                                               *
 *       Joshua Land, Simon Lariz, Jesus Contreras, Quan Duong                                *
 *                                                                                            *
 *  Description:                                                                              *
 *                                                                                            *
 *  See the read me for further information.                                                  *
 *********************************************************************************************/
// Includes
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>

// Macro for array size
#define SIZE (sizeof(list) / sizeof(*list))
// Initialize starting and result arrays
int list[] = {7, 12, 19, 3, 18, 4, 2, -5, 6, 15, 8};
int result[SIZE] = {0};

// Structures for passing data to worker threads
typedef struct
{
    int *subArray;
    unsigned int size;
} SortingThreadParameters;

typedef struct
{
    SortingThreadParameters left;
    SortingThreadParameters right;
} MergingThreadParameters;

// Print Array Content
void printArray(int *array)
{
    printf("{");
    for (int i = 0; i < SIZE; i++)
    {
        printf("  %d", array[i]);
    }
    printf("  }\n");
}

// Sorting Function (insertion sort)
void *sortArray(void *SortingThreadParametersArg)
{
    // Copy passed function arguements
    SortingThreadParameters *myParameter = (SortingThreadParameters *)SortingThreadParametersArg;
    // Vaiables for insertion sort
    int i, j, temp;
    // Insertion sort
    for (i = 1; i < myParameter->size; i++)
    {
        temp = *(myParameter->subArray + i);
        j = i - 1;
        while (temp < *(myParameter->subArray + j) && j >= 0)
        {
            *(myParameter->subArray + (j + 1)) = *(myParameter->subArray + j);
            j--;
        }
        *(myParameter->subArray + (j + 1)) = temp;
    }
}

// Merging Function
void *mergeArray(void *MergingThreadParametersArg)
{
    // Copy passed function arguements
    MergingThreadParameters *myArgs = (MergingThreadParameters *)MergingThreadParametersArg;

    // Merge Function Variables
    int i = 0, j = 0, k = 0;

    // Traverse both array
    while ((i < myArgs->left.size) && (j < myArgs->right.size))
    {
        // Check if left element is less than right element
        if (*(myArgs->left.subArray + i) < *(myArgs->right.subArray + j))
        {
            // If so update result with left element
            *(result + k) = *(myArgs->left.subArray + i);
            // Update counter variables
            i++;
            k++;
        }
        else
        {
            // Else update result with right element
            *(result + k) = *(myArgs->right.subArray + j);
            // Update counter variables
            j++;
            k++;
        }
    }
    // Store remaining elements of left array
    while (i < myArgs->left.size)
    {
        *(result + k) = *(myArgs->left.subArray + i);
        // Update counter variables
        i++;
        k++;
    }

    // Store remaining elements of right array
    while (j < myArgs->right.size)
    {
        *(result + k) = *(myArgs->right.subArray + j);
        // Update counter variables
        j++;
        k++;
    }
}

int main(int argc, char **argv)
{
    // Thread setup
    pthread_t sortLeftThread, sortRightThread, MergeThread;

    // Print list
    printf("Array before sorting: \n");
    printArray(list);

    // First sorting thread
    SortingThreadParameters *paramsLeft = malloc(sizeof(SortingThreadParameters));
    paramsLeft->subArray = list;
    paramsLeft->size = SIZE / 2;
    // Create thread and call sortArray with left as arguement
    pthread_create(&sortLeftThread, NULL, &sortArray, paramsLeft);
    // Wait for thread termination
    pthread_join(sortLeftThread, NULL);

    // Second sorting thread;
    SortingThreadParameters *paramsRight = malloc(sizeof(SortingThreadParameters));
    paramsRight->subArray = list + paramsLeft->size;
    paramsRight->size = SIZE - paramsLeft->size;
    // Create thread and call sortArray with right as arguement
    pthread_create(&sortRightThread, NULL, &sortArray, paramsRight);
    // Wait for thread termination
    pthread_join(sortRightThread, NULL);

    // Merge thread
    MergingThreadParameters *paramsMerge = malloc(sizeof(MergingThreadParameters));
    paramsMerge->left = *paramsLeft;
    paramsMerge->right = *paramsRight;
    // Create thread and call merge with mergeParams as arguement
    pthread_create(&MergeThread, NULL, &mergeArray, paramsMerge);
    // Wait for thread termination
    pthread_join(MergeThread, NULL);

    // Print result
    printf("Array after sorting:\n");
    printArray(result);
    return 0;
}