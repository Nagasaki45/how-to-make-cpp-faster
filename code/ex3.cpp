#include <thrust/for_each.h>
#include <thrust/transform_reduce.h>
#include <thrust/extrema.h>
#include <thrust/inner_product.h>
#include <thrust/sort.h>
#include <thrust/unique.h>
#include <thrust/sequence.h>
#include <thrust/device_vector.h>
#include <thrust/count.h>
#include <thrust/iterator/zip_iterator.h>
#include <thrust/iterator/constant_iterator.h>

#include <iostream>
#include <algorithm>
#include <random>

int main() {
	std::random_device rd;
	std::mt19937 gen(rd());
  std::normal_distribution<> d(500,200);
	const int N = 1<<20;
	std::vector<int> inputdata(N);
	std::generate(inputdata.begin(), inputdata.end(),[&d,&gen](){return std::max(0,(int)std::round(d(gen)));});
	std::vector<int> inputkeys(N);
	std::generate(inputkeys.begin(), inputkeys.end(),[&d,&gen](){return gen()%20;});

	/*You are given a vector of scores between 0 and 1000 (inputdata)
	  Copy those values into a thrust::device_vector and compute
	  the average of scores (~500), and their standard deviation (~200).
	  Make sure that you return and calculate with double values
	  instead of ints, otherwise you will get integer overflow problems.
	  Hint: for st. dev. use thrust::transform_reduce
	  where the operators take integers but return doubles
	  Calculate the number of scores above 800, 500, 200.
	  Find the minimum an maximum element. Try using thrust::minmax_element!
	  Hint: use "auto" for the return type
	  Check if there is an element with value 1234
	*/
	{
		thrust::device_vector<int> d_inputdata(N);
		thrust::copy(inputdata.begin(), inputdata.end(), d_inputdata.begin());
		double sum = thrust::reduce(d_inputdata.begin(), d_inputdata.end());
		double average = sum / N;
		double distancesFromAverage = thrust::transform_reduce(
			d_inputdata.begin(),
			d_inputdata.end(),
			[average](int datum){ return std::pow(datum - average, 2); },
			0.0,
			std::plus<double>()
		);
		double variance = distancesFromAverage / N;
		auto it1234 = thrust::find(d_inputdata.begin(), d_inputdata.end(), 1234);
		bool contains1234 = it1234 != d_inputdata.end();
		std::cout << "Average:       " << average << "\n";
		std::cout << "Std:           " << std::sqrt(variance) << "\n";
		std::cout << "Contains 1234: " << contains1234 << '\n';
	}

	/* You are given a vector of scores between 0 and 1000 (inputdata)
	 * and each score belongs to one of 20 groups (intputkeys).
	 * Copy both vectors into thrust::device_vectors
	 * Calculate the sum of scores of each group.
	 * Hint: use reduce_by_key, which requires sorted keys.
	 * Next you will calculate the number of scores in each group
	 * by using reduce_by_key again, but instead of reducing
	 * scores, you reduce only values "1"
	 * Hint: use a thrust::constant_iterator for that
	 * Finally, compute the average scores of each group
	 * Hint use thrust::transform with two inputs and one output
	 */
	{
		thrust::device_vector<int> d_inputkeys(N);
		thrust::device_vector<int> d_inputdata(N);
		thrust::device_vector<int> d_keys(20);
		thrust::device_vector<int> d_keysCount(20);
		thrust::device_vector<long int> d_keysSumOfScores(20);
		thrust::copy(inputkeys.begin(), inputkeys.end(), d_inputkeys.begin());
		thrust::copy(inputdata.begin(), inputdata.end(), d_inputdata.begin());
		thrust::sort_by_key(d_inputkeys.begin(), d_inputkeys.end(), d_inputdata.begin());
		thrust::reduce_by_key(d_inputkeys.begin(), d_inputkeys.end(), d_inputdata.begin(),
													d_keys.begin(), d_keysSumOfScores.begin());
		thrust::reduce_by_key(d_inputkeys.begin(), d_inputkeys.end(), thrust::constant_iterator<int>(1),
													d_keys.begin(), d_keysCount.begin());
		for (int i = 0; i < 20; i++)
		{
			std::cout << "key " << d_keys[i] << ", sum of scores " << d_keysSumOfScores[i] << ", # of scores " << d_keysCount[i] << '\n';
		}
	}

	/* Copy both arrays to the device again, and create a separate
	 * array with an index for each score (0->N-1).
	 * Sort the scores in descending order, along with the group
	 * and index values
	 * Hint: use make_zip_iterator to zip groups and indices
	 * From the best 20 scores, select the ones that are in different
	 * groups. Hint: use unique, with a zip iterator and with all
	 * 3 arrays in a single tuple. Keep in mind, that unique expects
	 * a sorted input (by group in this case).
	 * Print the indices, groups and scores of these
	 */
	{
		thrust::device_vector<int> d_inputkeys(N);
		thrust::device_vector<int> d_inputdata(N);
		thrust::device_vector<int> d_index(N);
		thrust::copy(inputkeys.begin(), inputkeys.end(), d_inputkeys.begin());
		thrust::copy(inputdata.begin(), inputdata.end(), d_inputdata.begin());
		thrust::copy_n(thrust::counting_iterator<int>(0), N, d_index.begin());
		auto begin = thrust::make_zip_iterator(thrust::make_tuple(d_inputkeys.begin(), d_inputdata.begin(), d_index.begin()));
		auto end = thrust::make_zip_iterator(thrust::make_tuple(d_inputkeys.end(), d_inputdata.end(), d_index.end()));
		typedef thrust::tuple<int, int, int> triplet;
		// Sort by descending data order
		thrust::sort(begin, end, [](triplet t1, triplet t2){ return thrust::get<1>(t1) > thrust::get<1>(t2); });
		// Sort the first 20 results by key
		thrust::sort_by_key(d_inputkeys.begin(), d_inputkeys.begin() + 20, begin);
		// Drop duplicate keys
		auto range = thrust::unique_by_key(d_inputkeys.begin(), d_inputkeys.begin() + 20, begin);
		// Print it
		int toKeep = std::distance(begin, range.second);
		for (int i = 0; i < toKeep; i++)
		{
			std::cout << "key " << d_inputkeys[i] << ", index " << d_index[i] << ", score " << d_inputdata[i] << "\n";
		}
	}
}
