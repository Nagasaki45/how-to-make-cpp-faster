#include <iostream>
#include <algorithm>  // max
#include <math.h>  // exp
#include <cmath>  // sqrt
#include <random>

double ITERATIONS = 1e8;


double price(long int iterations, double S, double K, double r,
             double v, double T, bool is_call)
{
  // Create a "truely" random seed generator
  std::random_device rd;

  // Create a psuedo random generator from a true seed and a normal distribution
  std::minstd_rand generator(rd());
  std::normal_distribution<double> distribution(0.0, 1.0);

  double accumulator = 0;
  #pragma omp parallel for reduction(+:accumulator)
  for (long int i = 0; i < iterations; i++)
  {
    double x = distribution(generator);
    double datum = S * std::exp((r - 0.5 * v * v) * T + v * std::sqrt(T) * x);
    if (is_call)
    {
      accumulator += std::max(datum - K, 0.0);
    }
    else
    {
      accumulator += std::max(K - datum, 0.0);
    }
  }
  return std::exp(-r * T) * accumulator / iterations;
}


int main()
{
  // First we create the parameter list
  double S = 100.0;  // Option price
  double K = 100.0;  // Strike price
  double r = 0.05;   // Risk-free rate (5%)
  double v = 0.2;    // Volatility of the underlying (20%)
  double T = 1.0;    // One year until expiry

  // Then we calculate the call/put values
  double call = price(ITERATIONS, S, K, r, v, T, true);
  double put = price(ITERATIONS, S, K, r, v, T, false);

  // Finally we output the parameters and prices
  std::cout << "Underlying:      " << S << "\n";
  std::cout << "Strike:          " << K << "\n";
  std::cout << "Risk-Free Rate:  " << r << "\n";
  std::cout << "Volatility:      " << v << "\n";
  std::cout << "Maturity:        " << T << "\n";
  std::cout << "Call Price:      " << call << "\n";
  std::cout << "Put Price:       " << put << "\n";
}
