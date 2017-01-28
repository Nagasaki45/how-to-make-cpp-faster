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
	//Sorted by day
	std::vector<int> day     {0,0,0,1,2,2,5,5,5,5,6,7,7,9,9,9,10,10,11,12,14,14};
	std::vector<int> position{2,3,0,1,1,2,3,2,0,1,2,3,0,2,1,3,0, 2, 2, 1, 0, 1 };
	std::vector<int> amount  {1,3,8,3,4,3,1,2,2,5,7,2,5,9,1,4,7, 6, 10,2, 1, 8 };

	//Step 1: upload to device
	const int N = day.size();
	thrust::device_vector<int> d_day(N);
	thrust::device_vector<int> d_position(N);
	thrust::device_vector<int> d_amount(N);
	thrust::copy(day.begin(), day.end(), d_day.begin());
	thrust::copy(position.begin(), position.end(), d_position.begin());
	thrust::copy(amount.begin(), amount.end(), d_amount.begin());
	// Create global begin zip iterator to reorder everything together when needed
	auto global_begin = thrust::make_zip_iterator(thrust::make_tuple(d_day.begin(), d_position.begin(), d_amount.begin()));

	//Step 2: number of days with any measurements
	{
		// Data already sorted by day, ready to call unique
		thrust::device_vector<int> d_unique_day(N);  // Can't be more than N
		auto end = thrust::unique_copy(d_day.begin(), d_day.end(), d_unique_day.begin());
		int unique_days = std::distance(d_unique_day.begin(), end);
		std::cout << "Unique days: " << unique_days << '\n';
	}

	//Step 3: total amount at each site
	{
		thrust::sort_by_key(d_position.begin(), d_position.end(), global_begin);
		thrust::device_vector<int> d_unique_position(N);  // Can't be more than N
		thrust::device_vector<int> d_position_amount(N);
		auto pair = thrust::reduce_by_key(
			d_position.begin(), d_position.end(), d_amount.begin(),
			d_unique_position.begin(), d_position_amount.begin()
		);

		// Print
		std::transform(  // thrust::transform fails to compile
			d_unique_position.begin(), pair.first, d_position_amount.begin(),
			std::ostream_iterator<std::string>(std::cout, "\n"),
			[](int x, int y){ return "Position " + std::to_string(x) + " total: " + std::to_string(y); }
		);
	}

	//Step 4: number of days where total amount exceeded 6
	thrust::device_vector<int> d_unique_day(N);  // Will be reused later
	thrust::device_vector<int> d_day_amount(N);
	{
		thrust::sort_by_key(d_day.begin(), d_day.end(), global_begin);
		auto pair = thrust::reduce_by_key(
			d_day.begin(), d_day.end(), d_amount.begin(),
			d_unique_day.begin(), d_day_amount.begin()
		);
		d_unique_day.erase(pair.first, d_unique_day.end());
		d_day_amount.erase(pair.second, d_day_amount.end());

		int days_exceeded6 = thrust::count_if(
			d_day_amount.begin(), d_day_amount.end(),
			[](int x){ return x > 6; }
		);
		std::cout << "# of days where total amount exceeded 6: " << days_exceeded6 << '\n';
	}

	//Step 5: first day where total amount (so far) exceeded 30
	{
		thrust::device_vector<int> d_cumsum_amount(d_day_amount.size());
		thrust::inclusive_scan(d_day_amount.begin(), d_day_amount.end(), d_cumsum_amount.begin());
		auto exceeded30_iterator = thrust::find_if(
			d_cumsum_amount.begin(), d_cumsum_amount.end(),
			[](int x){ return x > 30; }
		);
		int exceeded30_index = std::distance(d_cumsum_amount.begin(), exceeded30_iterator);
		std::cout << "First day where cumsum exceeded 30: " << d_unique_day[exceeded30_index] << '\n';
	}

	//Step 6: total amount between days 4 and 10
	{
		// zip the two iterators to filter them together
		auto begin = thrust::make_zip_iterator(thrust::make_tuple(d_unique_day.begin(), d_day_amount.begin()));
		auto end = thrust::make_zip_iterator(thrust::make_tuple(d_unique_day.end(), d_day_amount.end()));
		auto last = thrust::remove_if(
			begin, end,
			[](thrust::tuple<int, int> t){ int day = thrust::get<0>(t); return day < 4 || day > 10; }
		);
		int total = thrust::reduce(
			d_day_amount.begin(), d_day_amount.begin() + std::distance(begin, last)
		);
		std::cout << "Total between days 4 and 10: " << total << '\n';
	}
}
