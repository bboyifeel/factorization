#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <gmpxx.h>
#include "sieve.h"


const long long MIN_B = 2;
const long long MAX_B = 100000000;

Sieve s;

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


long long logWBase(long long x, long long base)
{
	double	result = log(x) / log(base);
	auto 	str = std::to_string(result);
	return 	std::stoi(str);
}


long long CalculateOptimalB(mpz_class _n)
{
	int 		logB 	= ceil(mpz_sizeinbase(_n.get_mpz_t(), 2) / 6.0);
	long long 	B 		= pow(2, logB);

	if (B > MAX_B) 		{ B = MAX_B; }
	else if (B < MIN_B)	{ B = MIN_B; }
	return B;
}


mpz_class PollardPMinusOne(mpz_class _n)
{
	mpz_class result(1);
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

	long long B = CalculateOptimalB(_n);
	mpz_class a(2);
	int numberOfIteration = 0;
	
	while(true)
	{
		if (numberOfIteration >= 6)
		{
			// std::cout << "Changing a" << std::endl;
			numberOfIteration = 0;
			a = generateGMPRandomNumber(_n);
			
			mpz_class gcd(0);
			mpz_gcd(gcd.get_mpz_t(), a.get_mpz_t(), _n.get_mpz_t());
			if(gcd != 1)
				return gcd;
		}

		numberOfIteration++;
		unsigned long long power;
		mpz_class result;

		for(int i = 0; i < s.size() && s[i] <= B; i++)
		{
			power = pow(s[i], logWBase(B, s[i]));
			mpz_powm_ui(result.get_mpz_t(), a.get_mpz_t(), power, _n.get_mpz_t());
			a = result;	
		}

		a--;

		mpz_gcd(result.get_mpz_t(), a.get_mpz_t(), _n.get_mpz_t());
		
		// {
		// 	std::cout << "N = " << _n << std::endl;
		// 	std::cout << "B = " << B << std::endl;
		// 	std::cout << "a^m - 1 = " << a << std::endl;
		// 	std::cout << result << std::endl;
		// } // print log

		if (result == _n)
		{
			if (B == MIN_B)
			{
				numberOfIteration = 6;
				B = CalculateOptimalB(_n);
				continue;
			}
			B /= 10;
			if (B < MIN_B)
				B = MIN_B;
		}
		else if (result == 1)
		{
			if (B == MAX_B)
			{
				result = -1;
				return result;
			}
			B *= 10;
			if (B > MAX_B)
				B = MAX_B;
		}
		else
		{
			return result;
		}
	}

	return result;
}


std::map<mpz_class,int> runPollardPMinusOne(mpz_class _n)
{
	mpz_class				result(1);
	std::map<mpz_class,int>	factor;
	std::vector<mpz_class>	toSkip;
	const int 				c_reps = 25;
	
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

		result = PollardPMinusOne(_n);
		if(result == -1)
		{
			if(factor.find(_n) != factor.end())
				factor[_n]++;
			else
				factor[_n] = 1;

			toSkip.push_back(_n);
			break;
		}

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
			if (std::find(toSkip.begin(), toSkip.end(), item.first) != toSkip.end())
				continue;

			if(!mpz_probab_prime_p(item.first.get_mpz_t(), c_reps))
			{
				arePrime		= false;
				result 			= PollardPMinusOne(item.first);
				
				if(result == -1)
				{
					toSkip.push_back(item.first);
					continue;
				}

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


void testPollardPMinusOne()
{
	std::string number;
	std::cout << "Enter valid number " << std::endl;
	getline(std::cin, number);
	mpz_class n(number);

	auto start = std::chrono::steady_clock::now();
	std::map<mpz_class,int> arr = runPollardPMinusOne(n);
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
// they don't time sieve generation
void testPollardPMinusOneFullTiming(mpz_class n)
{
	auto start = std::chrono::steady_clock::now();
	std::map<mpz_class,int> arr = runPollardPMinusOne(n);
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


void testPollardPMinusOneFirtsTiming(mpz_class n)
{
	auto start = std::chrono::steady_clock::now();
	mpz_class firstFactor = PollardPMinusOne(n);
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
	mpz_class n("44343535354351600000003434353");
	testPollardPMinusOneFirtsTiming(n);
	testPollardPMinusOneFullTiming(n);
	return 0;
}