Steps to Run the program.
1)
	Make a file "input.txt". 
	Provide data in the file named inp.txt with the Format Below :

    n(number of processes)
    then n lines follow where ith line is :
    pid[i] t[i] p[i] k[i]
    
    Example:
    2
    1 15 40 2
    2 25 70 2
2)
    a) Run RMS program using the following command:
    g++ Assign1-RMSma17btech11004.cpp -o rms

    b) Run EDF program using the following command:
    g++ Assign1-EDFma17btech11004.cpp -o edf

3) 
    a) Run the RMS file by executing :
    ./rms

    b) Run the EDF file by executing :
    ./edf

4) Check RMS-Log.txt file for logs of RMS while check EDF-Log.txt file for logs of EDF.

5) Average and waiting time is present in "average.txt".

6) Deadline miss stats are present in RMS-Stats.txt and EDF-Stats.txt for rms and edf respectively.