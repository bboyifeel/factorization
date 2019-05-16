// Taken from:
// https://www.geeksforgeeks.org/sieve-eratosthenes-0n-time-complexity/
#include "sieve.h"
#include <iostream>


Sieve::Sieve(int _N)
{
	if (_N >= MAX_SIZE)
		_N = MAX_SIZE - 1;
	
	N = _N;
	manipulated(N);
}


Sieve& Sieve::operator=(Sieve&& rhs)
{
	N = rhs.N;
	prime.swap(rhs.prime);
}


void Sieve::manipulated(int _N)
{
	// isPrime[] : isPrime[i] is true if number is prime  
	// prime[] : stores all prime number less than N 
	// SPF[] that store smallest prime factor of number 
	// [for Exp : smallest prime factor of '8' and '16' 
	// is '2' so we put SPF[8] = 2 , SPF[16] = 2 ] ; 
	std::vector<short> 		isprime(MAX_SIZE , true); 
	std::vector<long long> 	SPF(MAX_SIZE);
	// 0 and 1 are not prime 
	isprime[0] = isprime[1] = false; 

	for (long long int i = 2; i <= _N ; i++) 
	{ 
		if (isprime[i]) 
		{ 
			prime.push_back(i); 
			SPF[i] = i; 
		} 

		// Remove all multiples of  i * prime[j] which are 
		// not prime by making isPrime[i * prime[j]] = false 
		// and put smallest prime factor of i * Prime[j] as prime[j] 
		// [ for exp :let  i = 5 , j = 0 , prime[j] = 2 [ i*prime[j] = 10 ] 
		// so smallest prime factor of '10' is '2' that is prime[j] ] 
		// this loop run only one time for number which are not prime 
		for (long long int j = 0; j < (int)prime.size() && i * prime[j] < _N && prime[j] <= SPF[i]; j++) 
		{ 
			isprime[i * prime[j]] = false; 
			SPF[i * prime[j]] = prime[j] ; 
		}
	}
}


void Sieve::print() 
{
	for(int i = 0; i < prime.size(); i++)
	{
		std::cout << prime[i] << std::endl;
	}
}