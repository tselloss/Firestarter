<h2>Introduction</h2>
<p>Firestarter is a C program that simulates a forest fire and calculates the average percentage of the forest that burns based on different fire spread probabilities.</p>

<h2>Command line arguments</h2>
<p>The program accepts three optional command line arguments:</p>

<h3>First Edition</h3>
<ul>
  <li>Number of Trials: Specifies the number of trials to run in each probability calculation. The default value is 1000.</li>
  <li>Number of Probabilities: Specifies the number of different fire spread probabilities to use in the simulation. The default value is 10.</li>
  <li>Size of the forest: Determines the size of the forest, i.e. the number of trees in the forest. The default value is 1000.</li>
</ul>

<p>The program creates 4 threads using pthreads and distributes the task of calculating the average burn rate between the threads. Each thread calculates the average percentage burned for a different probability range, and the final result is stored in the percent_burned table. Specifically in this application we have divided them into four pieces of twenty-five values/probabilities.</p>

<h3>Second Edition</h3>
<p>The program uses multiple threads to speed up the simulation. It uses an array called "percent_burned" to store the average percentage of trees burned for each probability, and each thread computes the average percentage for a set of probabilities. Threads access the "percent_burned" table in a thread-safe manner, using a mutex lock. The main thread waits for all worker threads to finish and then outputs the results to the console.</p>

<h2>Output</h2>
<p>The program outputs the average burn rate for each probability. The results are displayed as a table with the probabilities and the corresponding average burn rate.</p>

<h2>Conclusion</h2>
<p>Firestarter is a powerful tool for analyzing wildfire spread and understanding the effect of different probabilities on burn rate. By using multiple threads and dividing the workload across multiple threads, the program is able to efficiently and accurately calculate the average burn rate for a large number of trials and probabilities.</p>
