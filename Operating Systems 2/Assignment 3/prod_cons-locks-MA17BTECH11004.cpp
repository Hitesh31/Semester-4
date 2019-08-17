#include<iostream>
#include<stdlib.h>
#include <fstream>
#include <pthread.h>
#include <random>
#include <stdio.h>
#include <unistd.h>
#include <atomic>
#include<time.h>
using namespace std;
long int capacity,np,nc;
double t1,t2;
int cfreq,pfreq;
int in=0,out=0,*buffer;
pthread_mutex_t mut1=PTHREAD_MUTEX_INITIALIZER,mut2=PTHREAD_MUTEX_INITIALIZER,lock1=PTHREAD_MUTEX_INITIALIZER,lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full  = PTHREAD_COND_INITIALIZER; 
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; 
int count=0;
double pav=0,cav=0;
default_random_engine eng1,eng2;	
exponential_distribution <double> c;
exponential_distribution <double> p;
FILE* file = fopen("Buffer_mutex_log.txt","w");
string getTime(time_t input)
{
  struct tm * timeinfo;
  timeinfo = localtime (&input);
  static char output[10];
  sprintf(output,"%.2d:%.2d:%.2d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
  string tim(output);
  return tim;
}
void *producer(void *param)
{
	long int id=(long int)param;
	for(int i=0;i<pfreq;i++)
	{
		clock_t entry=clock();
		pthread_mutex_lock(&mut1);
		while (count==capacity)
   		 	pthread_cond_wait( &empty, &mut1);
		time_t prodTime = time(NULL);
		buffer[in]=rand()%100;
		fprintf(file, "%dth item : %d produced by thread %ld at %s into buffer location %d\n", i,buffer[in],id,getTime(prodTime).c_str(), in);
		in=(in + 1)%capacity;
		count++;
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mut1);
		usleep(p(eng1) * 1e6);
		pthread_mutex_lock(&lock1);
		clock_t exit=clock();
		pav=pav+(exit-entry);
		pthread_mutex_unlock(&lock1);
	}
	pthread_exit(0);
}
void *consumer(void *param)
{
	long int id=(long int)param;
	for(int i=0;i<cfreq;i++)
	{
		clock_t entry=clock();
		pthread_mutex_lock(&mut1);
		while(count==0)
			pthread_cond_wait( &full, &mut1);
		time_t consTime = time(NULL);
		fprintf(file, "%dth item : %d consumed by thread %ld at %s into buffer location %d\n", i,buffer[out],id, getTime(consTime).c_str(), out);
		out = (out + 1) % capacity;
		count--;                      /* decrease counter         */
		pthread_cond_signal(&empty);   /* signal consumers         */
    	pthread_mutex_unlock(&mut1); 
		usleep(c(eng2) *  1e6);
		pthread_mutex_lock(&lock2);
		clock_t exit=clock();
		cav=cav+(exit-entry);
		pthread_mutex_unlock(&lock2);
	}
	pthread_exit(0);
}
int main()
{
	ifstream fout;
	fout.open("input.txt"); 
	fout>>capacity>>np>>nc>>pfreq>>cfreq>>t1>>t2;
	//cout<<capacity<<" "<<np<<" "<<nc<<" "<<pfreq<<" "<<cfreq<<" "<<t1<<" "<<t2<<endl;
	exponential_distribution <double> p (1.0/t1);
	exponential_distribution <double> c (1.0/t2);
	pthread_mutex_init( &mut1, NULL);
	pthread_cond_init( &full, NULL);
	pthread_cond_init( &empty, NULL);
	pthread_mutex_init( &lock1, NULL);
	pthread_mutex_init( &lock2, NULL);
	buffer = new int[capacity];
	pthread_t prod[np],cons[nc];
	for(int i=0;i<np;i++)
		pthread_create(&prod[i], NULL, producer, (void*)i);
	for(int i=0;i<nc;i++)
		pthread_create(&cons[i], NULL, consumer , (void*)i);
	for(int i=0;i<np;i++)
		pthread_join(prod[i],NULL);
	for(int i=0;i<nc;i++)
		pthread_join(cons[i],NULL);
	// cout<<"Average waiting time for producer :"<<pav/(pfreq*np*CLOCKS_PER_SEC)<<endl;
	// cout<<"Average waiting time for consumer :"<<cav/(cfreq*nc*CLOCKS_PER_SEC)<<endl;
}