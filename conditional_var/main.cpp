#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>
#include <deque>
#include  <condition_variable>

// Develop/feature_1 change  
// Test 
//
using namespace std;

unsigned int x = 0; // added Develop/feature_1 change 
unsigned int y = 0; // added main.cpp  int y , next change
unsigned int z = 0; // added main.cpp  int z 
unsigned int dev = 0; // added main.cpp  int dev 
unsigned int dev1 = 10; // from feature 5 
std::deque<int> q; // shared resource
std::mutex mu;     // protect shared resource
std::condition_variable cond; //je synchronizacny objekt suvisi s mutexom , umoznuje poslat notifikaciu druhemu vlaknu ktory caka na notifikaciu unique_lock<mutex>

void function_1(){ // PRODUCER of data
    int count = 10;
    while(count > 0){
        unique_lock<mutex> locker(mu); // lock resource
        //...do something
        //q.push_front(count);
         cout << "t1 write a value: " << count << endl;
        // unlock and then perform the broadcast signal operation..
        locker.unlock();
        cond.notify_one(); //notify one waiting thread, if there is one.(this will wake up thread)
        this_thread::sleep_for(chrono::seconds(1)); // let the thread to take a nap(sleep for short time) zdriemnut si
        count--;
    }
}

void function_2(){  // CONSUMER of data
    int data = 0;
    //while(data != 1){ // busy waiting - is unefficent => cakame vcikle kedy sa fronta uvolni
        unique_lock<mutex> locker(mu); // Wait until t1 produces sends data ..unique_lock - programer can handle lock/unlock of resource
        //if(!q.empty()) { // dalsi param Predicate urci kedy skutocne mozeme citat z fronty..ak queue je empto  sleep again
        cond.wait(locker, [](){return !q.empty();}); // cakame pokial t1 nezapise do fronty // spurious wake (not real ..false waike up)
            data = q.back();
            q.pop_back();
            locker.unlock();
            cout << "t2 got a value from t1: " << data << endl;
        //} else {
        //    locker.unlock(); // if queue is not empty go to next loop
             //this_thread::sleep_for(chrono::seconds(1)); // zredukuje mnozstvo sluciek ?? ako vieme ze treba vycitat z fronty
        //}
    //}
}

int main()
{
    cout << "Hello conditional variable!" << endl;
    thread t1(function_1);
    thread t2(function_2);

    // main thred is waiting for completition of t1, t2.
    t1.join();
    t2.join();

    return 0;
}
