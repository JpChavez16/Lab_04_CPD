#include <iostream>
#include <pthread.h>
#include <chrono>  
#include <vector> 
using namespace std;
using namespace std::chrono;

void* threadFunction(void* arg) 
{
    return nullptr;
}

int main() {
    int numThreads;
    cout << "Ingrese el número de threads a crear: ";
    cin >> numThreads;

    pthread_t threads[numThreads];
    vector<double> creationTimes(numThreads);

    
    double totalElapsedTime = 0;

    
    for (int i = 0; i < numThreads; ++i) {
        
        auto start = high_resolution_clock::now();

        
        pthread_create(&threads[i], nullptr, threadFunction, nullptr);

        
        pthread_join(threads[i], nullptr);

       
        auto end = high_resolution_clock::now();
        double elapsedTime = duration<double, milli>(end - start).count();

        
        creationTimes[i] = elapsedTime;
        totalElapsedTime += elapsedTime;
    }

    double averageTime = totalElapsedTime / numThreads;

    cout << "Tiempo promedio para crear y terminar un thread: " << averageTime << " ms" << endl;

    return 0;
}
