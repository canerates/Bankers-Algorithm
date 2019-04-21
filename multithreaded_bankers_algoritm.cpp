#include <iostream>
#include <iomanip>
#include <cstring>
#include <time.h>

#include <thread>
#include <mutex>

using namespace std;

/* these may be any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES] = {10,10,10};

/* the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

mutex myMutex;
thread t[NUMBER_OF_CUSTOMERS];

/* Prototypes */
void threadFunc(int customerNumber, int sleepTime);
int request_resources(int customer_num, int request[]);
int release_request(int customer_num, int release[]);
bool isSafe(int customer_num, int request[]);
template<typename T> void print_row(T t);
void generate_data();
void print_all_data();

int main()
{
    //srand (time(NULL));
    generate_data();
    print_all_data();

    int sleepTime[NUMBER_OF_CUSTOMERS];

    for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
    {
        sleepTime[i] = rand() % 10 + 1;
    }

    for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
    {
        t[i] = thread(threadFunc, i, sleepTime[i] );
    }
    for(int i= 0; i<NUMBER_OF_CUSTOMERS; i++)
    {
        t[i].join();
    }

    return 0;
}

void threadFunc(int customerNumber, int sleepTime)
{
    //srand (time(NULL));
    int counter =0;
    while(counter<10)
    {
        this_thread::sleep_for(chrono::milliseconds(sleepTime*200));

        myMutex.lock();
        int request[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

        for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
        {
            for(int j=0; j<NUMBER_OF_RESOURCES; j++)
            {
                request[i][j] = rand() % 4;
            }
        }
        cout<<"Customer #"<<customerNumber + 1<<" requested Resources: ";
        print_row(request[customerNumber]);
        int success = request_resources(customerNumber, request[customerNumber]);
        if(success == 0) //succesfull
        {
            cout<<"Customer #"<<customerNumber + 1<<" allocated Resources: ";
            print_row(request[customerNumber]);
            print_all_data();
        }
        else cout<<"Allocation is unsuccesfull due to availability.\n\n\n";
        myMutex.unlock();

        this_thread::sleep_for(chrono::milliseconds(sleepTime*1000));

        myMutex.lock();
        if(success == 0)
        {
            release_request(customerNumber,request[customerNumber]);
            cout<<"Customer #"<<customerNumber + 1<<" released Resources: ";
            print_row(request[customerNumber]);
            print_all_data();
        }
        myMutex.unlock();
        counter++;
    }
}

int request_resources(int customer_num, int request[])
{
    if(isSafe(customer_num,request))
    {
        for(int i=0; i<NUMBER_OF_RESOURCES; i++)
        {
            available[i] = available[i] - request[i];
            allocation[customer_num][i] = allocation[customer_num][i] + request[i];
            need[customer_num][i] = need[customer_num][i] - request[i];
        }
        return 0; //succesful
    }
    else
        return -1; //unsuccesful
}

int release_request(int customer_num, int release[])
{
    for(int i=0; i<NUMBER_OF_RESOURCES;i++)
    {
        available[i] = available[i] + release[i];
        need[customer_num][i] = need[customer_num][i] + release[i];
        allocation[customer_num][i] = allocation[customer_num][i] - release[i];
    }
    return 0; //succesful
}

bool isSafe(int customer_num, int request[])
{
    bool yes = true;
    for(int i=0; i<NUMBER_OF_RESOURCES; i++)
    {
        if(request[i] > need[customer_num][i])
        {
            yes = false;
            break;
        }
        if(yes && request[i] > available[i])
        {
            yes = false;
            break;
        }
    }
    return yes;
}

template<typename T> void print_row(T t)
{
    for (int i =0; i < NUMBER_OF_RESOURCES; i++ )
    {
        cout <<" "<< left << setw(3) << setfill(' ') << t[i];
    }
    cout<<"\n";
}

void generate_data()
{
    for (int j = 0; j<NUMBER_OF_RESOURCES; j++)
    {
        //available[j];
        for (int i =0; i < NUMBER_OF_CUSTOMERS; i++ )
        {
            maximum[i][j] = rand() % 5 + 5;
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

void print_all_data()
{
    cout<<" #MAXIMUM#\t #ALLOCATION#\t #AVAILABLE#\t #NEED#\n";
    for (int i = 0; i<NUMBER_OF_CUSTOMERS; i++)
    {
        for (int j =0; j < NUMBER_OF_RESOURCES; j++ )
        {
            cout<< " " << left << setw(3) << setfill(' ') << maximum[i][j];
        }
        cout<<"\t";
        for (int j =0; j < NUMBER_OF_RESOURCES; j++ )
        {
            cout<< " " << left << setw(3) << setfill(' ') << allocation[i][j];
        }
        cout<<"\t";
        if(i==0)
        {
            for (int j =0; j < NUMBER_OF_RESOURCES; j++ )
            {
                cout<< " " << left << setw(3) << setfill(' ') << available[j];
            }
            cout<<"\t";
        }
        else cout<<"\t\t";

        for (int j =0; j < NUMBER_OF_RESOURCES; j++ )
        {
            cout<< " " << left << setw(3) << setfill(' ') << need[i][j];
        }

        cout<<endl;
    }
    cout<<endl;
}
