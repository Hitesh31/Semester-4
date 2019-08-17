#include <iostream>
#include <fstream>
#include <pthread.h>
#include <random>
#include <stdio.h>
#include <unistd.h>
#include <atomic>
using namespace std;
FILE* f=fopen("CAS-Bounded-Log.txt","w");

long long int n,k;
double t1,t2;
std::atomic <int> lock(0);

/*Checks whether all are threads are fullfilled*/
bool flag = true;
double waiting=0,worst,diff;
//get formatted time
string getTime(time_t input)
{
  struct tm * timeinfo;
  timeinfo = localtime (&input);
  static char output[10];
  sprintf(output,"%.2d:%.2d:%.2d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
  string tim(output);
  return tim;
}
void* testCS(void *ptr)
{
    long long int id = (long long int)ptr;
    /*Calculating current system time */
    //timeval tv;
    default_random_engine eng;
    exponential_distribution <double> cs(1.0/t1);
    exponential_distribution <double> rem(1.0/t2);
    bool waitingThread[n];
    int j;
    for(long long int i=1;i<=k;i++)
     {
        waitingThread[id] = true;
        bool key = false;
        time_t now1=time(NULL);
        fprintf(f,"%lldst CS request by Thread %lld at %s\n",i,id,(getTime(now1).c_str()));
        while(waitingThread[id] && !key)
       {
            int x = 0, y = 1;
            key = lock.compare_exchange_strong(x ,y);
        }
        waitingThread[id] = false;
        //Critical Section
        time_t now2=time(NULL);
        waiting+=now2-now1;
        diff=now2-now1;
        if(diff > worst)
        {
            worst=diff;
        }
        fprintf(f,"%lldst CS entry by Thread %lld at %s\n",i,id,(getTime(now2).c_str()));
        usleep(cs(eng)*1e6);
         j = (id) % n;
        while((j != id) && !waitingThread[j])
            j = (j) % n;
        time_t now3 = time(NULL);
        fprintf(f,"%lldst CS exit by Thread %lld at %s\n",i,id,getTime(now3).c_str());
         if(j == id)
            lock = 0;
        else
            waitingThread[j] = false;
        usleep(rem(eng)*1e6);
    }
    pthread_exit(0);
}
int main()
{
    ifstream in;
    in.open("inp.txt");
    in>>n>>k;
    in>>t1>>t2;
    pthread_t workers[n]; 
    /*Thread id's*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    /* get the default attributes */
 
    for(int i=0;i<n;i++)
    {
        /*Create n workers*/
        pthread_create(workers+i,&attr,testCS,(void*)i);
    }
    /*Join the threads*/
    for (int i = 0; i < n; i++) 
        pthread_join(workers[i], NULL);
    fclose(f);
    f = fopen("Average_times_CAS_bounded.txt","w");
    fprintf(f,"Average wait time of CAS Bounded :%lf\n",(double)waiting/(n*k));
    fprintf(f,"Worst wait time of CAS Bounded:%lf\n",(double)worst);
}
