#include<iostream>
#include<stdlib.h>
#include <fstream>
#include <pthread.h>
#include <random>
#include <stdio.h>
#include <unistd.h>
#include <atomic>
#include<time.h>
#include<semaphore.h>
using namespace std;
long int capacity,np,nc;
double t1,t2;
int cfreq,pfreq;
int in=0,out=0,*buffer;
sem_t full,empty,mutex;
default_random_engine eng1,eng2;
exponential_distribution <double> c;
exponential_distribution <double> p;
double pav=0,cav=0;
FILE* file = fopen("Buffer_Semaphore_log.txt","w");
sem_t lock1,lock2;
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
		sem_wait(&empty);
		sem_wait(&mutex);
		time_t prodTime = time(NULL);
		buffer[in]=rand()%100;
		fprintf(file, "%dth item : %d produced by thread %ld at %s into buffer location %d\n", i,buffer[in],id,getTime(prodTime).c_str(), in);
		in=(in + 1)%capacity;
		sem_post(&mutex);
		sem_post(&full);
		usleep(p(eng1) * 1e6);
		sem_wait(&lock1);
		clock_t exit=clock();
		pav=pav+(exit - entry);
		sem_post(&lock1);
	}
	pthread_exit(0);
}
void *consumer(void *param)
{
	long int id=(long int)param;
	for(int i=0;i<cfreq;i++)
	{
		clock_t entry=clock();
		sem_wait(&full);
		sem_wait(&mutex);
		time_t consTime = time(NULL);
		fprintf(file, "%dth item : %d consumed by thread %ld at %s into buffer location %d\n", i,buffer[out],id, getTime(consTime).c_str(), out);
		out = (out + 1) % capacity;
		sem_post(&mutex);
		sem_post(&empty);
		usleep(c(eng2) *  1e6);
		sem_wait(&lock2);
		clock_t exit=clock();
		cav=cav+(exit - entry);
		sem_post(&lock2);
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
	sem_init(&full,0,0);
	sem_init(&empty,0,capacity);
	sem_init(&mutex,0,1);
	sem_init(&lock1,0,1);
	sem_init(&lock2,0,1);
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
	return 0;
}