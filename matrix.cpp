// ALA' HADER
// SEWAR GARIBEH
// SHEFA'A AL- ZOU'BI
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <pthread.h>
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int *N, *NumOfThreads;
long numOfOdd, numOfEven, totalCells;
long long **Matrix1, **Matrix2, **mul;
void *Multiply(void *threadid)
{
    int start, end;
    int id = (int64_t)threadid;
    start = (id * (*N)) / (*NumOfThreads);     // Start of the thread loop.
    end = ((id + 1) * (*N)) / (*NumOfThreads); // End of the thread loop.
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < *N; j++)
        {
            for (int k = 0; k < *N; k++)
            {
                mul[i][j] += Matrix1[i][k] * Matrix2[k][j];
            }
            pthread_mutex_lock(&mutex);
            totalCells++;
            pthread_mutex_unlock(&mutex);

            if (mul[i][j] % 2 != 0)
            {
                pthread_mutex_lock(&mutex);
                numOfOdd++;
                pthread_mutex_unlock(&mutex);
            }

            else
            {
                pthread_mutex_lock(&mutex);
                numOfEven++;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    // Print the ThreadID, startLoop and endLoop.
    cout << "ThreadID=" << id << ", startLoop=" << start << ", endLoop=" << end << endl;
    pthread_exit(NULL);
}

int main(int arg, char *argv[])
{
    string TempNumThreads = argv[1];
    int NumOfThreadstemp = stoi(TempNumThreads);
    NumOfThreads = &NumOfThreadstemp;
    pthread_t *threads = new pthread_t[*NumOfThreads];
    // open the file.
    ifstream infile(argv[2]);
    string line;
    getline(infile, line);
    int Ntemp = stoi(line);
    N = &Ntemp;
    // Declare Matrix1,Matrix2, and the Result.
    Matrix1 = new long long *[*N];
    Matrix2 = new long long *[*N];
    mul = new long long *[*N];

    for (int i = 0; i < *N; i++)
    {
        Matrix1[i] = new long long[*N];
        Matrix2[i] = new long long[*N];
        mul[i] = new long long[*N];
    }
    // Read Matrix1 and Matrix2 from the file.
    if (infile.is_open())
    {
        int i = 0, j = 0, val = 0;
        // Read Matrix1 and store it.
        for (i = 0; i < *N; i++)
        {
            getline(infile, line);
            stringstream sep(line);
            for (j = 0; j < *N; j++)
            {
                sep >> val;
                Matrix1[i][j] = (int)val;
                mul[i][j] = 0;
            }
        }
        // Read Matrix2 and store it.
        for (i = 0; i < *N; i++)
        {
            getline(infile, line);
            stringstream sep(line);
            for (j = 0; j < *N; j++)
            {
                sep >> val;
                Matrix2[i][j] = (int)val;
            }
        }
        infile.close();

        long t;
        int rc;
        // For loop for create threading.
        for (t = 0; t < *NumOfThreads; t++)
        {
            rc = pthread_create(&threads[t], NULL, Multiply, (void *)t); // (void *) is meant to be a pointer-
            //-to data that you want to pass into that particular thread.
            if (rc)
            {
                cout << "ERROR; return code from pthread_create() is " << rc << "\n";
                exit(-1);
            }
        }
        // For loop for joining threads.
        for (int i = 0; i < *NumOfThreads; i++)
        {
            pthread_join(threads[i], NULL);
        }
        // Save the result matrix in out.txt
        ofstream store;
        store.open("out.txt");
        for (int i = 0; i < *N; i++)
        {
            for (int j = 0; j < *N; j++)
            {
                store << mul[i][j] << " ";
            }
            store << endl;
        }
    }
    else
        cout << "unable to open file \n";

    // Delete Dynamic Arrays:
    for (int i = 0; i < *N; i++) // To delete the inner arrays
        delete[] Matrix1[i];
    delete[] Matrix1;
    for (int i = 0; i < *N; i++) // To delete the inner arrays
        delete[] Matrix2[i];
    delete[] Matrix2;
    for (int i = 0; i < *N; i++) // To delete the inner arrays
        delete[] mul[i];
    delete[] mul;
    // Print number of odd,even, and total cells.
    cout << "numOfEven=" << numOfEven << ", numOfODD=" << numOfOdd << ", totalCells=" << totalCells << endl;
    pthread_exit(NULL);
}
