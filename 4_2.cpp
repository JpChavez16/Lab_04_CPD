#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <ctime>

using namespace std;

long long int numberOfTosses;         
long long int numberInCircle = 0;      
int numThreads;                        
pthread_mutex_t mutex;                 


void* Darts(void* arg) 
{
    long long int localNumberInCircle = 0;
    long long int localTosses = numberOfTosses / numThreads;

    
    unsigned int seed = time(0) + pthread_self();  

    for (long long int toss = 0; toss < localTosses; ++toss) 
    {
        double x = (static_cast<double>(rand_r(&seed)) / RAND_MAX) * 2.0 - 1.0;
        double y = (static_cast<double>(rand_r(&seed)) / RAND_MAX) * 2.0 - 1.0;
        double distanceSquared = x * x + y * y;

        if (distanceSquared <= 1.0) 
        {
            localNumberInCircle++;
        }
    }

    pthread_mutex_lock(&mutex);
    numberInCircle += localNumberInCircle;
    pthread_mutex_unlock(&mutex);

    return nullptr;
}

int main() {
    cout << "Ingrese el número total de lanzamientos: ";
    cin >> numberOfTosses;
    cout << "Ingrese el número de threads a utilizar: ";
    cin >> numThreads;

    pthread_t threads[numThreads];
    pthread_mutex_init(&mutex, nullptr);

    
    for (int i = 0; i < numThreads; ++i) 
    {
        pthread_create(&threads[i], nullptr, Darts, nullptr);
    }

    
    for (int i = 0; i < numThreads; ++i) 
    {
        pthread_join(threads[i], nullptr);
    }

    
    double piEstimate = 4.0 * static_cast<double>(numberInCircle) / static_cast<double>(numberOfTosses);
    cout << "Valor estimado de pi: " << piEstimate << endl;

    
    pthread_mutex_destroy(&mutex);

    return 0;
}
