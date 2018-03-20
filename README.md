# Objective

- Two dimensional heat equation is an example of a stencil computation. 
 It computes iteratively the following equation for a stencil of size N.
For 2D heat equation, the problem is defined on a discrete 2D space of size n × n; let’s call it H.
The kth iteration of the heat equation is defined by Hk is defined by 

H(k)[i][j] = 1(H(k−1)[i − 1][j − 1] + H(k−1)[i − 1][j] + H(k−1)[i − 1][j + 1] +H(k−1)[i][j − 1] + H(k−1)[i][j] + H(k−1)[i][j + 1]
+H(k−1)[i + 1][j − 1] + H(k−1)[i + 1][j] + H(k−1)[i + 1][j + 1]) / 9

(Take the elements out of the array as H(k−1)[i][j]). Initialize H in some fashion (random works)

The implementation needs to keep H(k) and H(k−1) in memory.

- Question: Implement a distributed memory version of the 2D heat equation problem. 
Partition the data the way you like better.

- Question: Perform a strong scaling experiment to compute H20 from 1 core to 32 cores. 
(Feel free to restrict number of cores to particular numbers that matches your implementation.) 
Pick n such that H is about 1GB large, 10GB large, and 50GB large.

- Question: Perform a weak scaling experiment to compute H20 from 1 core to 32 cores.
(Feel free to restrict number of cores to particular numbers that matches your implementation.) 
Pick n such that on one core H is about 500MB large, 1GB large, and 2GB large.

## Implementation
- The distributed 2D heat equation has been implemnted using MPI.
- The implementation increases the computation & communication overlap by using asynchonous send & receive.
- Strong scaling and weak scaling experiments have been done & the corresponding speedup for the same has been plotted.
