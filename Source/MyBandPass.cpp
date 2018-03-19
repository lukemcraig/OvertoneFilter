#include "MyBandPass.h"

void MyBandPass::makeMyBandPass(const double inverseSampleRate, const double centerFreq,
	const double Q, const double gain) noexcept
{
	/*auto n = 1.0 / std::tan(MathConstants<double>::pi * centerFreq * inverseSampleRate);
	auto nSquared = n * n;
	auto c1 = 1.0 / (1.0 + 1.0 / Q * n + nSquared);

	coefficients = IIRCoefficients(c1 * n / Q,
		0.0,
		-c1 * n / Q,
		1.0,
		c1 * 2.0 * (1.0 - nSquared),
		c1 * (1.0 - 1.0 / Q * n + nSquared));*/
	//coefficients = IIRCoefficients::makeBandPass(1.0 / inverseSampleRate, centerFreq, Q);
	coefficients = IIRCoefficients::makeLowPass(1.0 / inverseSampleRate, centerFreq, Q);
	setCoefficients(coefficients);
}

void MyBandPass::copyCoefficientsFrom(const MyBandPass& other) noexcept
{
	setCoefficients(other.coefficients);
	active = other.active;
}
