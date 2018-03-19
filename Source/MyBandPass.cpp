#include "MyBandPass.h"

void MyBandPass::makeMyBandPass(const double inverseSampleRate, const double centerFreq,
	const double Q) noexcept
{
	double normalizedFrequency = MathConstants<double>::pi * centerFreq * inverseSampleRate;
	double n = 1.0 / std::tan(normalizedFrequency);
	double nSquared = n * n;
	double c1 = 1.0 / (1.0 + (n / Q) + nSquared);

	// b (numerator) are the feedfoward coeffecients
	// a (denominator) are the feedbackward coeffecients
	//normalized to a0
	double b0 = c1 * n / Q;
	double b1 = 0.0;
	double b2 = -b0;
	double a0 = 1.0;
	double a1 = c1 * 2.0 * (1.0 - nSquared);
	double a2 = c1 * (1.0 - (n / Q) + nSquared);
	coefficients = IIRCoefficients(
		b0,
		b1,
		b2,
		a0,
		a1,
		a2);
	//coefficients = IIRCoefficients::makeBandPass(1.0 / inverseSampleRate, centerFreq, Q);	
	setCoefficients(coefficients);
}

void MyBandPass::copyCoefficientsFrom(const MyBandPass& other) noexcept
{
	setCoefficients(other.coefficients);
	active = other.active;
}
