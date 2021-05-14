#pragma once

#ifdef _OPENMP
#ifndef _OMPTL_DEBUG_NO_OMP
  #include <omp.h>
#else
  #define omp_get_max_threads() (2)
#endif
#endif

namespace nesort
{

	struct _NrProcs
	{
		static unsigned NrProcs()
		{
#ifdef _OPENMP
			assert(omp_get_max_threads() > 0);
			int nrThreads = omp_get_max_threads();
			//check for power of two
			if( (nrThreads & (nrThreads - 1)) == 0 )				
				return nrThreads;
			else
			{	//nr procs must be power of two otherwise it crashes
				omp_set_num_threads(4);
				return 4;
			}
			//return omp_get_max_threads();
#else
			return 0;
#endif
		}
	};
	// Log of the number of operations that is expected to run faster in a single
	// thread.
	const unsigned C = 12;

	template <typename T>
	T log2N_(T n)
	{
		assert(n > 0);
		const std::size_t N = CHAR_BIT*sizeof(T);

		T result = 0;
		for (std::size_t i = 1; i < N; ++i)
		{
			const std::size_t M = N-i;
			if ( n >= (std::size_t(1) << M) )
			{
				n >>= M;
				result |= M;
			}
		}

		return result;
	}

	template<typename Iterator>
	bool _nlogn_serial_is_faster(Iterator first, Iterator last,
		const unsigned P)
	{
		assert(P > 0u);
		assert(::std::distance(first, last) >= 0);
		const std::size_t N = ::std::distance(first, last);

		return (N < 2u*P) || (N*log2N_(N) < (1 << C));
	}

	template<typename RandomAccessIterator, class StrictWeakOrdering>
	void sort(RandomAccessIterator first, RandomAccessIterator last,
		StrictWeakOrdering comp, const unsigned P =_NrProcs::NrProcs());

	template<typename RandomAccessIterator>
	void sort(RandomAccessIterator first, RandomAccessIterator last,
		const unsigned P = _NrProcs::NrProcs());


	inline uint secondPower(uint P);
};

inline uint nesort::secondPower(uint P)
{

	int highestBit=0;
	for(int i = 31; i>=0; --i)
	{
		if((P>>i) & 1)
		{
			highestBit = i;
			break;
		}
	}	
	return 1 << highestBit;
}

#ifdef _OPENMP
  #include "Utility/Sorting/NEParallelSort.h"
#else
  #include "Utility/Sorting/NESerialSort.h"
#endif
