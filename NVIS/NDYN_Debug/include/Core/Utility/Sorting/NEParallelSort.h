#pragma once


namespace nesort
{

	template<typename Iterator, class StrictWeakOrdering>
	Iterator _pivot_range(Iterator first, Iterator last,
        const typename std::iterator_traits<Iterator>::value_type pivot,
		StrictWeakOrdering comp)
	{
		while (first < last)
		{
			if (comp(*first, pivot))
				++first;
			else
			{
				while ( (first < --last) && !comp(*last, pivot) )
					/* nop */;
				iter_swap(first, last);
			}
		}

		return last;
	}

	template<typename Iterator, class StrictWeakOrdering>
	void _partition_range_by_pivots(Iterator first, Iterator last,
		const std::vector<typename
        std::iterator_traits<Iterator>::value_type> &pivots,
        std::vector< std::pair<Iterator, Iterator> > &partitions,
		StrictWeakOrdering comp, const unsigned P)
	{
		assert(partitions.size() == P);

//		typedef pair<Iterator, Iterator> Partition;

		std::vector<Iterator> ptable(P);
        std::vector<typename std::iterator_traits<Iterator>::value_type>
			pvts(pivots.size());

		std::vector<Iterator> borders;

		std::vector<bool> used(pivots.size());
		std::fill(used.begin(), used.end(), false);

		// These end-points are certainly borders. Sorting will be done later.
		borders.push_back(first);
		borders.push_back(last);


		partitions[0].first  = first;
		partitions[0].second = last;
		for (unsigned p = 1; (1 << p) <= (int)P; ++p)
		{
			const int PROC = (1 << p);
			const int PIVOTS = (1 << (p-1)); // ??
			assert(PIVOTS <= (int)pivots.size());

			//#pragma omp parallel for // probably unsafe due to std::vector<bool>
			for (int t = 0; t < PIVOTS; ++t)
			{
				const int index = int(P / PROC) +
					2 * t * int(P / PROC) - 1;
				assert(index < (int)pivots.size());
				assert(!used[index]);
				used[index] = true;
				pvts[t] = pivots[index];
				/*std::cout << "pvts T: " << t << " --> " << index <<
				" " << pvts[t] << std::endl;*/
			}

#pragma omp parallel for
			for (int t = 0; t < PIVOTS; ++t)
				ptable[t] = _pivot_range(partitions[t].first,
				partitions[t].second,
				pvts[t], comp);

			for (int i = 0; i < PIVOTS; ++i)
			{
				// std::cout << "ADD: " << std::distance(first, ptable[t]) << std::endl;
				borders.push_back(ptable[i]);
			}

			std::sort(borders.begin(), borders.end());

			for (unsigned i = 0; i < borders.size() - 1; ++i)
			{
				partitions[i].first	= borders[i];
				partitions[i].second	= borders[i + 1];
			}

			/*std::cout << "PASS: " << p << std::endl;
			for (t = 0; t < (1 << p); ++t)
			std::cout << t << ": " << std::distance(first, partitions[t].first)
			<< " - " << std::distance(first, partitions[t].second)
			<< std::endl;*/
		}

		for (unsigned i = 0; i < pivots.size(); ++i)
			if(!used[i])
				pvts[i] = pivots[i];

#pragma omp parallel for
		for (int t = 0; t < int(pivots.size()); ++t)
			if (!used[t])
				ptable[t] = _pivot_range(partitions[t].first,
				partitions[t].second,
				pvts[t], comp);


		for (unsigned i = 0; i < pivots.size(); ++i)
		{
			if (!used[i])
			{
				// std::cout << "LAST ADD: " << std::distance(first, ptable[i])
				// 	<< std::endl;
				borders.push_back(ptable[i]);
			}
		}
		assert(borders.size()-1 == P);

		std::sort(borders.begin(), borders.end());
		for (unsigned i = 0; i < P; ++i)
		{
			partitions[i].first	= borders[i];
			partitions[i].second	= borders[i + 1];
		}
	}

/*
 * The sample ratio is used to sample more data. This way, the pivots can be
 * chosen more wisely, which is our only guarantee we can generate partitions
 * of equal size.
 */
	template<typename RandomAccessIterator, class StrictWeakOrdering>
	void _find_pivots(RandomAccessIterator first, RandomAccessIterator last,
		std::vector<typename
        std::iterator_traits<RandomAccessIterator>::value_type> &pivots,
		StrictWeakOrdering comp, const unsigned P,
		unsigned SAMPLE_RATIO = 8)
	{
		assert(SAMPLE_RATIO > 0);
		const std::size_t N = std::distance(first, last);
		assert(N >= 2u*P);

		// Adjust the constant. Erm.
		while (SAMPLE_RATIO * P > N)
			SAMPLE_RATIO /= 2;
		assert(SAMPLE_RATIO > 0);

		pivots.clear();
		pivots.reserve(P - 1);

		typedef typename
            std::iterator_traits<RandomAccessIterator>::value_type value_type;

		std::vector<value_type> samples;
		const std::size_t NSAMPLES = SAMPLE_RATIO * P;
		assert(NSAMPLES <= N);
		samples.reserve(NSAMPLES);

		for (std::size_t i = 0; i < NSAMPLES; ++i)
		{
			const std::size_t index = i * (N-1) / (NSAMPLES - 1);
			assert(index < N);
			samples.push_back(*(first + index));
			// std::cout << "index: " << index << " sample: " << samples[i] << std::endl;
		}
		assert(samples.size() == NSAMPLES);

		// Sort samples to create relative ordering in data
		std::sort(samples.begin(), samples.end(), comp );

		// Take pivots from sampled data
		for (std::size_t i = 1; i < P; ++i)
		{
			pivots.push_back(samples[i * samples.size() / P]);
			/*std::cout << "pivot: " << i << " idx: " << (i * samples.size() / P)
			<< " " << pivots[i-1] << std::endl;*/
		}
		assert(pivots.size() == P - 1);
	}


	template<typename RandomAccessIterator, class StrictWeakOrdering>
    void parallelSort(std::vector< std::pair<RandomAccessIterator, RandomAccessIterator>>& partitions,
		StrictWeakOrdering comp, const unsigned P)
	{
			#pragma omp parallel for
			for(int t = 0; t < int(P); ++t)
			{
				std::sort(partitions[t].first, partitions[t].second, comp);
			}
	}

	template<typename RandomAccessIterator, class StrictWeakOrdering>
	void sort(RandomAccessIterator first, RandomAccessIterator last,
		StrictWeakOrdering comp, const unsigned P)
	{
		if ( nesort::_nlogn_serial_is_faster(first, last, P) )
		{
			std::sort(first, last, comp);
			return;
		}

		// Generate pivots
		typedef typename
            std::iterator_traits<RandomAccessIterator>::value_type value_type;

		std::vector<value_type> pivots;
		_find_pivots(first, last, pivots, comp, P);

		// Sort sufficiently to respect pivot order
        std::vector< std::pair<RandomAccessIterator, RandomAccessIterator> >
			partitions(P);
		nesort::_partition_range_by_pivots(first, last, pivots,
			partitions, comp, P);

		// Sort
#ifdef NE_BUILD_64
		parallelSort(partitions,comp,P);
#else
        #pragma(omp parallel for)
			for(int t = 0; t < int(P); ++t)
			{
				std::sort(partitions[t].first, partitions[t].second, comp);
			}
#endif

		//}
	}
	

	template<typename RandomAccessIterator>
	void sort(RandomAccessIterator first, RandomAccessIterator last,
		const unsigned P)
	{
		typedef typename
            std::iterator_traits<RandomAccessIterator>::value_type VT;
		nesort::sort(first, last, std::less<VT>(), P);
	}


};

//extend this class whenever there are ideas for other efficient parallel sorting algorithms
//http://www.coryhardman.com/
