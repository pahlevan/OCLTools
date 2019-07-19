#pragma once

#include "FFTpointCount.h"
#include "WindowFunction.h"

#include <complex>
#include <vector>

class AllignedBufferF;
class AllignedBufferI16C;



typedef struct fftwf_plan_s* fftwf_plan;

class FFTCpu
{
public:
	FFTCpu(const FFTPointCount fft_point ,const WindowFunction::win_type win_type);
	~FFTCpu();

	auto forward(std::shared_ptr<AllignedBufferI16C>& input_buffer) ->std::shared_ptr<AllignedBufferF>;
private:

	fftwf_plan  handle_;
	std::complex<float>* in_;
	std::complex<float>* out_;
	std::vector<float>	window_vec_;
	FFTPointCount fft_point_;
};
