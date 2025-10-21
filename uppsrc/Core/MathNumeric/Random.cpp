#include "MathNumeric.h"


NAMESPACE_UPP

Vector<int> RandomNumberGenerator::Permutation(int num) {
	Vector<int> v;
	v.SetCount(num);
	for(int& i : v)
		i = Random(num);
	return v;
}

END_UPP_NAMESPACE
