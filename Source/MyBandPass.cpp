#include "MyBandPass.h"

void MyBandPass::makeMyBandPass(const double inverseSampleRate, const double centerFreq,
	const double Q) noexcept
{

	coefficients = IIRCoefficients::makeBandPass(1.0 / inverseSampleRate, centerFreq, Q);

	//normalized to a0
	setCoefficients(coefficients);
}

void MyBandPass::copyCoefficientsFrom(const MyBandPass& other) noexcept
{
	setCoefficients(other.coefficients);
	active = other.active;
}
