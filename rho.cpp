#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <unistd.h>
#include <gmpxx.h>


// from 2 to n-1 
mpz_class generateGMPRandomNumber(mpz_class _n)
{
	mpz_class result(1);
	unsigned engineSeed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine 			engine{engineSeed};
	std::uniform_int_distribution<int> 	distribution;

	int 				seed;
	gmp_randstate_t 	r_state;
	
	while(result < 2)
	{
		seed = distribution(engine);
		gmp_randinit_mt(r_state);
		gmp_randseed_ui(r_state, seed);
		mpz_urandomm(result.get_mpz_t(), r_state, _n.get_mpz_t());
	}

	return result;
}


mpz_class f(mpz_class _x, mpz_class _n)
{
	mpz_class result = (_x * _x + 1) % _n;
	return result;
}


// returns 1 if unable to factorize --> the number is prime
mpz_class PollardRho(mpz_class _n)
{
	mpz_class result;

	// Trivial cases test
	{
		if(_n == 1)
		{
			result = _n;
			return result;
		}

		if(! _n % 2)
		{
			result = 2;
			return result;
		}
	}

	mpz_class x = generateGMPRandomNumber(_n);
	mpz_class y = f(x,_n);
	mpz_class distance;

	do
	{
		x = f(x, _n);
		y = f(f(y, _n), _n);

		distance = abs(x - y);
		mpz_gcd(result.get_mpz_t(), distance.get_mpz_t(), _n.get_mpz_t());
		
		if (result > 1 && result != _n)
		{
			return result;
		}

	} while(x != y);

	return mpz_class(1);
}


std::map<mpz_class,int> runPollardRho(mpz_class _n)
{
	std::map<mpz_class,int>	factor;
	mpz_class				result(1);

	const int 	c_reps = 25;
	
	while(_n != 1)
	{
		if (mpz_probab_prime_p(_n.get_mpz_t(), c_reps))
		{
			if(factor.find(_n) != factor.end())
				factor[_n]++;
			else
				factor[_n] = 1;

			break;
		}

		result = PollardRho(_n);
		_n = _n / result;
		if(factor.find(result) != factor.end())
			factor[result]++;
		else
			factor[result] = 1;
	}


	bool arePrime = true;
	do
	{
		arePrime = true;
		std::vector<mpz_class>	toDelete;
		mpz_class secondResult(1);
		
		for(auto item: factor)
		{
			if(!mpz_probab_prime_p(item.first.get_mpz_t(), c_reps))
			{
				arePrime		= false;
				result 			= PollardRho(item.first);
				secondResult	= item.first / result;
				
				if(factor.find(result) != factor.end())
					factor[result] += item.second;
				else
					factor[result] = item.second;

				if(factor.find(secondResult) != factor.end())
					factor[secondResult] += item.second;
				else
					factor[secondResult] = item.second;

				toDelete.push_back(item.first);
			}
		}
		for (auto item: toDelete)
		{
			factor.erase(item);
		}
	} while(!arePrime);


	return factor;
}


void testPollardRho()
{
	/*
		Test number:
		210
		271
		297
		10967535067
		44343535354351600000003434353
		44343535354351600000003434353876698756435645
	*/
	
	std::cout << "Please enter a valid number: ";
 	std::string number;
  	getline(std::cin, number);
  	mpz_class n(number);
	
	auto start = std::chrono::steady_clock::now();
	std::map<mpz_class,int> arr = runPollardRho(n);
	auto end = std::chrono::steady_clock::now();
	std::cout 	<< "Elapsed time "
				<< std::chrono::duration_cast< std::chrono::milliseconds>(end - start).count()
				<< " milliseconds"
				<< std::endl;

	bool firstEl = true;
	for(auto item: arr)
	{
		if(firstEl)		firstEl = false;
		else			std::cout << " x ";

		std::cout << item.first;
		if (item.second > 1)
			std::cout << "^" << item.second << "";
	}
}


// function for testing (milisecs)
void testPollardRhoFullTiming(mpz_class n)
{
	auto start = std::chrono::steady_clock::now();
	std::map<mpz_class,int> arr = runPollardRho(n);
	auto end = std::chrono::steady_clock::now();
	std::cout 	<< "Elapsed time "
				<< std::chrono::duration_cast< std::chrono::milliseconds>(end - start).count()
				<< " milliseconds"
				<< std::endl;

	// log infor, you can delete if you want
	bool firstEl = true;
	for(auto item: arr)
	{
		if(firstEl)		firstEl = false;
		else			std::cout << " x ";

		std::cout << item.first;
		if (item.second > 1)
			std::cout << "^" << item.second << "";
	}
}


void testPollardRhoFirstTiming(mpz_class n)
{
	auto start = std::chrono::steady_clock::now();
	mpz_class firstFactor = PollardRho(n);
	auto end = std::chrono::steady_clock::now();
	std::cout 	<< "Elapsed time "
				<< std::chrono::duration_cast< std::chrono::milliseconds>(end - start).count()
				<< " milliseconds"
				<< std::endl;
	std::cout << firstFactor << std::endl;
}



int main()
{
	// Examples
	mpz_class n("44343535354351600000003434353876698756435645");
	testPollardRhoFirstTiming(n);
	testPollardRhoFullTiming(n);
	return 0;
}