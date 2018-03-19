#include "MyBandPass.h"

void MyBandPass::makeMyBandPass(const double inverseSampleRate, const double centerFreq,
	const double Q) noexcept
{
	// http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
	// BPF: H(s) = s / (s^2 + s/Q + 1)  (constant skirt gain, peak gain = Q)
	double w0 = MathConstants<double>::twoPi * centerFreq * inverseSampleRate;
	double alpha = std::sin(w0) / (2.0 * Q);

	// b (numerator) are the feedfoward coeffecients
	// a (denominator) are the feedbackward coeffecients
	double b0 = alpha;
	double b1 = 0.0;
	double b2 = -alpha;
	double a0 = 1.0 + alpha;
	double a1 = -2.0 * std::cos(w0);
	double a2 = 1.0 - alpha;

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
