# HPQC
Enter HPQC then week_1 then run ./bin/Clock_write for task 2 run Clock_Read

Ran python code and c code for "Clock Write" code ran for a range of 
----8,000---
Time taken to write: 0.00686 seconds
real    0m0.038s
user    0m0.024s
sys     0m0.008s
80,000
Time taken to write: 0.04013 seconds

real    0m0.073s
user    0m0.043s
sys     0m0.011s
800,000
Time taken to write: 0.34926 seconds

real    0m0.383s
user    0m0.231s
sys     0m0.020s
8,000,000
Time taken to write: 3.16045 seconds

real    0m3.195s
user    0m2.186s
sys     0m0.101s

Ran Code for C

8,000
Time taken to write: 0.000690 seconds

real    0m0.008s
user    0m0.000s
sys     0m0.004s

80,000
Time taken to write: 0.000669 seconds

real    0m0.007s
user    0m0.000s
sys     0m0.004s
800,000
Time taken to write: 0.000777 seconds

real    0m0.009s
user    0m0.000s
sys     0m0.005s
8,000,000
Time taken to write: 0.000768 seconds

real    0m0.009s
user    0m0.004s
sys     0m0.000s

Ran code for reading with Python
8,000
Time taken to read: 0.000926 seconds

real    0m0.033s
user    0m0.022s
sys     0m0.008s

80,000
Time taken to read: 0.000659 seconds

real    0m0.035s
user    0m0.022s
sys     0m0.009s

800,000
Time taken to read: 0.001697 seconds

real    0m0.032s
user    0m0.021s
sys     0m0.009s
8,000,000
Time taken to read: 0.002408 seconds

real    0m0.035s
user    0m0.026s
sys     0m0.004s

Ran code for reading with C
8,000
Time taken to read: 0.000310 seconds

real    0m0.006s
user    0m0.000s
sys     0m0.004s
80,000
Time taken to read: 0.000385 seconds

real    0m0.007s
user    0m0.000s
sys     0m0.004s

800,000
Time taken to read: 0.000380 seconds

real    0m0.006s
user    0m0.004s
sys     0m0.000s
8,000,000
Time taken to write: 0.000680 seconds

real    0m0.007s
user    0m0.000s
sys     0m0.004s

C showed very small variance in write time for increasing number of variables, whearas python showed linear increase directly preportional to variable number increase.
C showed Small increase in read time with a noteable increase between 800,000 variables and 8,000,000 variable. Python read time increased with increasing variables.
For all operations system time always increased at a slower rate than user and real time.
