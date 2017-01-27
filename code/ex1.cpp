#include <iostream>
#include <random>
#include <cmath>

double ITERATIONS = 1e8;

int main()
{
  // Create a random generator and distribution from a "true" random seed
  std::random_device rd;
  std::minstd_rand generator(rd());
  std::uniform_real_distribution<float> uniform_dist(-1.0, 1.0);

  // Simulate the darts game and count hits inside the unit circle
  long int inside = 0;
  #pragma omp parallel for reduction(+:inside)
  for (long int i = 0; i < (int) ITERATIONS; i++)
  {
    auto x = uniform_dist(generator);
    auto y = uniform_dist(generator);
    if (pow(x, 2) + pow(y, 2) < 1)
    {
      inside += 1;
    }
  }

  double pi = inside * 4.0 / ITERATIONS;

  std::cout << "pi estimation after " << ITERATIONS << " iterations: " << pi << '\n';
}
