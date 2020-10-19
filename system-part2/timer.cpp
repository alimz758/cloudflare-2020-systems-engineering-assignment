#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>
#include <ratio>


class Timer
{
    public:
        void start(){
            startTime = std::chrono::high_resolution_clock::now();
            running = true;
        }
        void stop(){
            stopTime = std::chrono::high_resolution_clock::now();
            running = false;
        }
        double elapsedMilliseconds(){
            std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
            if(running)
            {
                endTime = std::chrono::high_resolution_clock::now();
            }
            else{
                endTime = stopTime;
            } 
            return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        }
        double elapsed(){
            return elapsedMilliseconds() / 1000.0;
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> stopTime;
        bool                                               running = false;
};
