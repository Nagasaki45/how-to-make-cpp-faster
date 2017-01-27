# How to make C++ faster than python

*I have no idea...*

You heard that right. This repo propuses two two challanges:

1. Try to write C++ algorithms that are faster than python.
1. Try to write these algorithms in a way that the speed ratio between the C++ and the python implementations (the speedup) will be greater than the lines of code (LOC) ratio between them.

## "What???"

Don't underestimate python, and don't think that implementation algorithms in python means writing a lot of slow `for` loops.
The scientific ecosystem around python is huge and relatively easy to use.
Optimizing python code by vecotizing it with [numpy](http://www.numpy.org/) or [pandas](http://pandas.pydata.org/), or JIT compiling it with [numba](http://numba.pydata.org/) will drastically improve performance, and these are only few of the available options.

Some execution time results

                        time
                        mean
    exercise extention           
    ex1      .cpp       17.929961
             .py         3.093354
    ex2      .cpp       45.230240
             .py        49.245274
    ex3      .cpp       27.230966
             .py         1.183992


## "But this is not python"

Is it is! That's true that your code will start to call lower level routines in lower level and faster languages. But as a user, who cares?

## Some context

I'm taking a "Parallel Computing with C++" module this term.
Just for the exercise I tried to code some of the examples in python, only to find out that with minor and simple optimizations, and by choosing the right tools for the job, I'm getting much better performance than with C++.
But I want to improve.
I want to be able to write C++ code that will beat python in performace.
This is the main porpuse of this repo.
Which me luck :-)
