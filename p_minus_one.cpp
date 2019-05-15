#include <iostream>
#include <map>
#include <string>
#include <random>
#include <chrono>
#include <gmpxx.h>
#include "sieve.h"

const long long DEFAULT_B = 100000000;

long long logWBase(long long x, long long base)
{
	double	result = log(x) / log(base);
	auto 	str = std::to_string(result);
	return 	std::stoi(str);
}


mpz_class pMinusOne(mpz_class n)
{
	Sieve s;
	std::cout << "Sieve has finished " << std::endl;

	long long B = DEFAULT_B;
	mpz_class g(1);
	
	do
	{	 
		mpz_class a(2); // $todo: randomly pick a coprime to n; #unnecessarily
		unsigned long long power;
		mpz_class result;

		for(int i = 0; i < s.size() && s[i] <= B; i++)
		{
			power = pow(s[i], logWBase(B, s[i]));
			mpz_powm_ui(result.get_mpz_t(), a.get_mpz_t(), power, n.get_mpz_t());
			a = result;	
		}

		a--;

		mpz_gcd(g.get_mpz_t(), a.get_mpz_t(), n.get_mpz_t());
		
		{
			std::cout << "B " << B << std::endl;
			std::cout << "a^m - 1 = " << a << std::endl;
			std::cout << g << std::endl;
		} // print log

		B /= 10; // $todo: #ask what step should be?
	} while(g == n);

	return g;
}


void pMinusOneTest()
{
	std::string number;
	std::cout << "Enter valid number " << std::endl;
	getline(std::cin, number);
	mpz_class n(number);

	std::cout << pMinusOne(n) << std::endl;
}


int main()
{
	pMinusOneTest();
	return 0;
}