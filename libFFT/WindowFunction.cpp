#include "WindowFunction.h"

#include "../libMath/MathConst.h"

#include <stdexcept>

#define IzeroEPSILON 1E-21               /* Max error acceptable in Izero */

static double Izero(double x)
{
	double sum, u, halfx, temp;
	int n;

	sum = u = n = 1;
	halfx = x / 2.0;
	do {
		temp = halfx / (double)n;
		n += 1;
		temp *= temp;
		u *= temp;
		sum += u;
	} while (u >= IzeroEPSILON * sum);
	return(sum);
}

double midn(int ntaps)
{
	return ntaps / 2.0;
}

double midm1(int ntaps)
{
	return (ntaps - 1.0) / 2.0;
}

double midp1(int ntaps)
{
	return (ntaps + 1.0) / 2.0;
}

double freq(int ntaps)
{
	return 2.0 * pi / ntaps;
}

double rate(int ntaps)
{
	return 1.0 / (ntaps >> 1);
}

double
WindowFunction::max_attenuation(win_type type, double beta)
{
	switch (type) {
	case(WIN_HAMMING):         return 53; break;
	case(WIN_HANN):            return 44; break;
	case(WIN_BLACKMAN):        return 74; break;
	case(WIN_RECTANGULAR):     return 21; break;
	case(WIN_KAISER):          return (beta / 0.1102 + 8.7); break;
	case(WIN_BLACKMAN_HARRIS): return 92; break;
	case(WIN_BARTLETT):        return 27; break;
	case(WIN_FLATTOP):         return 93; break;
	default:
		throw std::out_of_range("windowFunction::max_attenuation: unknown windowFunction type provided.");
	}
}

std::vector<float>
WindowFunction::coswindow(int ntaps, float c0, float c1, float c2)
{
	std::vector<float> taps(ntaps);
	float M = static_cast<float>(ntaps - 1);

	for (int n = 0; n < ntaps; n++)
		taps[n] = c0 - c1 * cosf((2.0f * pi * n) / M) + c2 * cosf((4.0f * pi * n) / M);
	return taps;
}

std::vector<float>
WindowFunction::coswindow(int ntaps, float c0, float c1, float c2, float c3)
{
	std::vector<float> taps(ntaps);
	float M = static_cast<float>(ntaps - 1);

	for (int n = 0; n < ntaps; n++)
		taps[n] = c0 - c1 * cosf((2.0f * pi * n) / M) + c2 * cosf((4.0f * pi * n) / M) \
		- c3 * cosf((6.0f * pi * n) / M);
	return taps;
}

std::vector<float>
WindowFunction::coswindow(int ntaps, float c0, float c1, float c2, float c3, float c4)
{
	std::vector<float> taps(ntaps);
	float M = static_cast<float>(ntaps - 1);

	for (int n = 0; n < ntaps; n++)
		taps[n] = c0 - c1 * cosf((2.0f * pi * n) / M) + c2 * cosf((4.0f * pi * n) / M) \
		- c3 * cosf((6.0f * pi * n) / M) + c4 * cosf((8.0f * pi * n) / M);
	return taps;
}

std::vector<float>
WindowFunction::rectangular(int ntaps)
{
	std::vector<float> taps(ntaps);
	for (int n = 0; n < ntaps; n++)
		taps[n] = 1;
	return taps;
}

std::vector<float>
WindowFunction::hamming(int ntaps)
{
	std::vector<float> taps(ntaps);
	float M = static_cast<float>(ntaps - 1);

	for (int n = 0; n < ntaps; n++)
		taps[n] = 0.54 - 0.46 * cos((2 * pi * n) / M);
	return taps;
}

std::vector<float>
WindowFunction::hann(int ntaps)
{
	std::vector<float> taps(ntaps);
	float M = static_cast<float>(ntaps - 1);

	for (int n = 0; n < ntaps; n++)
		taps[n] = 0.5 - 0.5 * cos((2 * pi * n) / M);
	return taps;
}

std::vector<float>
WindowFunction::hanning(int ntaps)
{
	return hann(ntaps);
}

std::vector<float>
WindowFunction::blackman(int ntaps)
{
	return coswindow(ntaps, 0.42, 0.5, 0.08);
}

std::vector<float>
WindowFunction::blackman2(int ntaps)
{
	return coswindow(ntaps, 0.34401, 0.49755, 0.15844);
}

std::vector<float>
WindowFunction::blackman3(int ntaps)
{
	return coswindow(ntaps, 0.21747, 0.45325, 0.28256, 0.04672);
}

std::vector<float>
WindowFunction::blackman4(int ntaps)
{
	return coswindow(ntaps, 0.084037, 0.29145, 0.375696, 0.20762, 0.041194);
}

std::vector<float>
WindowFunction::blackman_harris(int ntaps, int atten)
{
	switch (atten) {
	case(61): return coswindow(ntaps, 0.42323, 0.49755, 0.07922);
	case(67): return coswindow(ntaps, 0.44959, 0.49364, 0.05677);
	case(74): return coswindow(ntaps, 0.40271, 0.49703, 0.09392, 0.00183);
	case(92): return coswindow(ntaps, 0.35875, 0.48829, 0.14128, 0.01168);
	default:
		throw std::out_of_range("windowFunction::blackman_harris: unknown attenuation value (must be 61, 67, 74, or 92)");
	}
}

std::vector<float>
WindowFunction::blackmanharris(int ntaps, int atten)
{
	return blackman_harris(ntaps, atten);
}

std::vector<float>
WindowFunction::nuttal(int ntaps)
{
	return nuttall(ntaps);
}

std::vector<float>
WindowFunction::nuttall(int ntaps)
{
	return coswindow(ntaps, 0.3635819, 0.4891775, 0.1365995, 0.0106411);
}

std::vector<float>
WindowFunction::blackman_nuttal(int ntaps)
{
	return nuttall(ntaps);
}

std::vector<float>
WindowFunction::blackman_nuttall(int ntaps)
{
	return nuttall(ntaps);
}

std::vector<float>
WindowFunction::nuttal_cfd(int ntaps)
{
	return nuttall_cfd(ntaps);
}

std::vector<float>
WindowFunction::nuttall_cfd(int ntaps)
{
	return coswindow(ntaps, 0.355768, 0.487396, 0.144232, 0.012604);
}

std::vector<float>
WindowFunction::flattop(int ntaps)
{
	double scale = 4.63867;
	return coswindow(ntaps, 1.0 / scale, 1.93 / scale, 1.29 / scale, 0.388 / scale, 0.028 / scale);
}

std::vector<float>
WindowFunction::kaiser(int ntaps, double beta)
{
	if (beta < 0)
		throw std::out_of_range("windowFunction::kaiser: beta must be >= 0");

	std::vector<float> taps(ntaps);

	double IBeta = 1.0 / Izero(beta);
	double inm1 = 1.0 / ((double)(ntaps - 1));
	double temp;

	/* extracting first and last element out of the loop, since
	   sqrt(1.0-temp*temp) might trigger unexpected floating point behaviour
	   if |temp| = 1.0+epsilon, which can happen for i==0 and
	   1/i==1/(ntaps-1)==inm1 ; compare
	   https://github.com/gnuradio/gnuradio/issues/1348 .
	   In any case, the 0. Bessel function of first kind is 1 at point 0.
	 */
	taps[0] = IBeta;
	for (int i = 1; i < ntaps - 1; i++) {
		temp = 2 * i * inm1 - 1;
		taps[i] = Izero(beta * sqrt(1.0 - temp * temp)) * IBeta;
	}
	taps[ntaps - 1] = IBeta;
	return taps;
}

std::vector<float>
WindowFunction::bartlett(int ntaps)
{
	std::vector<float> taps(ntaps);
	float M = static_cast<float>(ntaps - 1);

	for (int n = 0; n < ntaps / 2; n++)
		taps[n] = 2 * n / M;
	for (int n = ntaps / 2; n < ntaps; n++)
		taps[n] = 2 - 2 * n / M;

	return taps;
}

std::vector<float>
WindowFunction::welch(int ntaps)
{
	std::vector<float> taps(ntaps);
	double m1 = midm1(ntaps);
	double p1 = midp1(ntaps);
	for (int i = 0; i < midn(ntaps) + 1; i++) {
		taps[i] = 1.0 - pow((i - m1) / p1, 2);
		taps[ntaps - i - 1] = taps[i];
	}
	return taps;
}

std::vector<float>
WindowFunction::parzen(int ntaps)
{
	std::vector<float> taps(ntaps);
	double m1 = midm1(ntaps);
	double m = midn(ntaps);
	int i;
	for (i = ntaps / 4; i < 3 * ntaps / 4; i++) {
		taps[i] = 1.0 - 6.0 * pow((i - m1) / m, 2.0) * (1.0 - fabs(i - m1) / m);
	}
	for (; i < ntaps; i++) {
		taps[i] = 2.0 * pow(1.0 - fabs(i - m1) / m, 3.0);
		taps[ntaps - i - 1] = taps[i];
	}
	return taps;
}

std::vector<float>
WindowFunction::exponential(int ntaps, double d)
{
	if (d < 0)
		throw std::out_of_range("windowFunction::exponential: d must be >= 0");

	double m1 = midm1(ntaps);
	double p = 1.0 / (8.69 * ntaps / (2.0 * d));
	std::vector<float> taps(ntaps);
	for (int i = 0; i < midn(ntaps) + 1; i++) {
		taps[i] = exp(-fabs(i - m1) * p);
		taps[ntaps - i - 1] = taps[i];
	}
	return taps;
}

std::vector<float>
WindowFunction::riemann(int ntaps)
{
	double cx;
	double sr1 = freq(ntaps);
	double mid = midn(ntaps);
	std::vector<float> taps(ntaps);
	for (int i = 0; i < mid; i++) {
		if (i == midn(ntaps)) {
			taps[i] = 1.0;
			taps[ntaps - i - 1] = 1.0;
		}
		else {
			cx = sr1 * (i - mid);
			taps[i] = sin(cx) / cx;
			taps[ntaps - i - 1] = sin(cx) / cx;
		}
	}
	return taps;
}

std::vector<float>
WindowFunction::build(win_type type, int ntaps, double beta)
{
	switch (type) {
	case WIN_RECTANGULAR: return rectangular(ntaps);
	case WIN_HAMMING: return hamming(ntaps);
	case WIN_HANN: return hann(ntaps);
	case WIN_BLACKMAN: return blackman(ntaps);
	case WIN_BLACKMAN_HARRIS: return blackman_harris(ntaps);
	case WIN_KAISER: return kaiser(ntaps, beta);
	case WIN_BARTLETT: return bartlett(ntaps);
	case WIN_FLATTOP: return flattop(ntaps);
	default:
		throw std::out_of_range("windowFunction::build: type out of range");
	}
}