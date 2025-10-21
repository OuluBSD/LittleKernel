#include "Shell.h"

NAMESPACE_UPP

void EscPow(EscEscape& e)
{
	if (e[0].IsNumber() && e[1].IsNumber()) {
		double a = e[0].GetNumber();
		double b = e[1].GetNumber();
		e = pow(a, b);
	}
}

void EscSqrt(EscEscape& e)
{
	if (e[0].IsNumber()) {
		double a = e[0].GetNumber();
		e = sqrt(a);
	}
}

END_UPP_NAMESPACE
