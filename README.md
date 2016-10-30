# multitreat
A simple variable treatment library inspired by https://github.com/WinVector/vtreat

See also the paper "A Preprocessing Scheme for High Cardinality Categorical Attributes in Classification and Prediction Problems", Micci-Barreca, Daniele.

I created this so that I didn't have to redo this very basic kind of data science work for each project.

I initially wrote this for Spark but it was pretty easy so I added C++ and C# and will probably add more languages in the future.

The calculations for mean and variance in the C++ and C# version are done using an online update to avoid out-of-memory problems:
https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm

I did not implement precalculation using residuals of the value against the mean, but probably will at some point.

Feel free to just copy the raw source files if you want.  The build scripts are merely for convenience and I picked the Apache License so you could easily add and modify directly in your projects.
