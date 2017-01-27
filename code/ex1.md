# Parallel computing module (ECS748P), Queen Mary University of London, Lab 1 exercise 2

We are going to use the Monte Carlo method for estimating the value of Pi ( = 3.1415...). The idea can be summarised as follows: imagine we have a darts board with 2 unit long sides, and a centre at (0, 0). If we were to start throwing darts at this board (with a uniform distribution), the ratio of the number of darts falling within the unit circle to the number of darts thrown would help us estimate Pi, because area circle = r^2 * pi, and area square = (2 \* r)^2, where r = 1 now.

Ultimately we are estimating the integral of f(x, y) = 1 over a unit circle, and comparing it to the integral of the same function over the encompassing square. The Monte Carlo method can be used to estimate high-dimensional integrals that cannot be computed analytically, by evaluating the functions at random points and counting the points that fall below or above the surface defined by the function.
