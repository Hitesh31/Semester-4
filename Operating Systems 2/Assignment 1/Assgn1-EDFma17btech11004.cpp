#include <iostream>
#include <fstream>
#include<cmath>
#include <vector>
#include <algorithm>
using namespace std;
ofstream outf;
ifstream in;

class Process
{
    
    public:
        void define(int pid,int t,int p,int k,int i)
        {
            p_id = pid; 
            this->deadline = p;
            this->time_left = t;
            this->index = i;
            this->queue_status = false;
            this->t=t;
            this->period = p;
            this->k = k;
        }
        int k; //times process repeats
        bool queue_status; //status in the queue
        int deadline; //deadline
        int index; 
        int time_left;
        bool on_cpu; //boolean for it is executing on CPU or not
        int p_id;       //process id
        int t; //processing time
        int period; //deadline/period
};
struct period_sort //Sorting function comparator
{
    inline bool operator() (Process* struct1, Process* struct2)
    {
        return (struct1->deadline < struct2->deadline);
    }
};
int main()
{
    int n;
    in.open("input.txt"); //Reads from input.txt file
    in>>n;
    vector<Process> task(n);
    for(int i=0;i<n;i++)
    {
        int pid,t,p,k;
        in>>pid>>t>>p>>k;
        task[i].define(pid,t,p,k,i);
    }
    in.close();
    outf.open("EDF-Log.txt");
    vector<Process*> ready_queue;
    Process current;
    current.on_cpu = false;
    int t=0;
    int fail=0;
    int total=0;
    double average_wait =0;
    double td_time=0;           //turnaround time
    for(int i=0;i<n;i++) 
        total+=task[i].k;
    for(int i=0;i<n;i++)
    {
        outf<<"Process P"<<task[i].p_id<<": processing time="<<task[i].t<<"; deadline:"
        <<task[i].deadline<<"; period:"<<task[i].period<<" joined the system at time 0\n";
    }
    while(1)
    {
        bool flag=true;
        for(int f=0;f<n;f++)
        for(int i=0;i<n;i++)
        {
            if(task[i].k>0) flag=false;
        }
        if(flag==true) break;
                //Flags the current process as false
        if(current.on_cpu ==true && t%current.period==0) 
        {
            current.on_cpu = false;
            if(task[current.index].k>0)
            {
                int x = current.index;
                task[x].queue_status = true;
                ready_queue.push_back(&task[x]);
            }
        }
        //Adds the processes whose period is here and removes the processes whose deadline is expired
        for(int i=0;i<n;i++)
        {
            if(t%task[i].period==0 && task[i].k>0)
            {
                if(task[i].queue_status==false)
                {
                    task[i].queue_status=true;                //Process at its period is pushed in the queue

                    ready_queue.push_back(&task[i]);
                }
                else
                {
                    //Deadline expired 
                    outf<<"Deadline miss by Process"<<task[i].p_id<<" at time "<<t<<"\n";
                    td_time+=task[i].period;                   
                    average_wait+= t -  task[i].deadline+task[i].period - (task[i].t-task[i].time_left)+1;
                    fail++;
                    task[i].time_left = task[i].t;
                    task[i].deadline+=task[i].period;
                    task[i].k--;
                }
            }
        }
        sort(ready_queue.begin(),ready_queue.end(),period_sort()); //sorts based on deadlline
        
        if(current.on_cpu==false)
        {
            if(!ready_queue.empty())
            {
                //is no process executing , we select minimum period process from ready queue
                current = *ready_queue[0];
                if(task[current.index].time_left == current.t)
                {
                    outf<<"Process P"<<current.p_id<<" starts execution at time "<<t<<".\n";
                }
                else
                {
                    outf<<"Process P"<<current.p_id<<" resumes execution at time "<<t<<".\n";
                }
                ready_queue.erase(ready_queue.begin());
                task[current.index].queue_status =false;
                current.on_cpu = true;
            }
            else
            {
                //if ready queue is empty we run the loop till CPU is not idle or another processs causes period
                bool flag=true;
                while(flag==true)
                {
                    t++;
                    int i=0;
                    while(i<n)
                    {
                        if(t%task[i].period==0 && task[i].k>0) 
                            {flag=false; break;}
                        i++;
                    }
                }
                outf<<"CPU is idle till "<<t<<".\n";
                continue;
            }
        }

        
        if(!ready_queue.empty() && ready_queue[0]->deadline < task[current.index].deadline) 
        { //preemption
            Process r = *ready_queue[0];
            // if the top of queue has period less than current process , it is preempted and running process is pushed to  queue
            task[r.index].queue_status = false;
            ready_queue.erase(ready_queue.begin());
            ready_queue.push_back(&task[current.index]);
            outf<<"Process P"<<current.p_id<<" is preempted by Process P"<<r.p_id
            <<" at time "<<t<<". Remaining processing time:"<<task[current.index].time_left<<"\n";
            outf<<"Process P"<<r.p_id<<" starts execution at time "<<t<<"\n";
            task[current.index].queue_status =true;
            current = r;
            current.on_cpu=true;
            task[current.index].time_left--; //reduce current current time
            t++;
            continue;
        }
        task[current.index].time_left--; //reduce current current time
        if(task[current.index].time_left<=0) 
        { // removes the processes which ended
                outf<<"Process P"<<current.p_id<<" finishes execution at time "<<t<<".\n";
                td_time+=t-task[current.index].deadline+task[current.index].period;
                average_wait+= t -  task[current.index].deadline+task[current.index].period - task[current.index].t+1;
                task[current.index].time_left = current.t;           
                task[current.index].k--;
                task[current.index].queue_status=false;
                current.on_cpu = false;
                task[current.index].deadline += task[current.index].period;
        }
        t++;
    }
    outf.close();
    outf.open("EDF-Stats.txt");
    average_wait = average_wait/total;
    td_time = td_time/total;
    ofstream av;
    av.open("average.txt",fstream::app | fstream::out | fstream::in);
    av<<"EDF : Average wait time : "<<average_wait<<"; Average turnaround time : "<<td_time<<"\n";
    outf<<"No. of Processes entering the system : "<<total<<"\n";
    outf<<"No. of Processes successfully completed : "<<total-fail<<"\n";
    outf<<"No. of Processes which missed their deadline : "<<fail<<"\n"; 
    return 0;
}