#ifndef CRYPTO_PK_SIEVE
#define CRYPTO_PK_SIEVE

#include <vector>

class Sieve
{
public:
	Sieve(int N = 100000000);
	Sieve& 	operator=(Sieve&&);
	int 	operator[] (int i) 	{ return prime[i];	 	}
	int		size()				{ return prime.size(); 	}
	int 	UpTo()				{ return N;				}
	void	print();
private:
	void 	manipulated(int _N);
	int 	N = 0;
	const long long MAX_SIZE = 100000001; 
	std::vector<long long> prime;
};

#endif // CRYPTO_PK_SIEVE