#include "FFTCpu.h"

#include "AllignedBufferF.h"
#include "AllignedBufferI16C.h"
#include "WindowFunction.h"

#include <fftw3.h>

#include <assert.h>
#include <omp.h>

FFTCpu::FFTCpu(const FFTPointCount fft_point, const WindowFunction::win_type win_type)
	: fft_point_(fft_point)
{
	window_vec_ = WindowFunction::build(win_type, int(fft_point),0);

	in_ = (std::complex<float>*)fftwf_malloc(sizeof(fftwf_complex) * int(fft_point_));
	out_ = (std::complex<float>*)fftwf_malloc(sizeof(fftwf_complex) * int(fft_point_));

	handle_ = fftwf_plan_dft_1d(int(fft_point_), (fftwf_complex*)in_, (fftwf_complex*)out_, FFTW_FORWARD, FFTW_ESTIMATE);
}

FFTCpu::~FFTCpu(void)
{
	fftwf_free(in_);
	fftwf_free(out_);
	fftwf_destroy_plan(handle_);
	handle_ = nullptr;
}

auto FFTCpu::forward(std::shared_ptr<AllignedBufferI16C>& input_buffer) ->std::shared_ptr<AllignedBufferF>
{
	/*int fft_pt = int(fft_point_);
	int half_fft_pt = fft_pt / 2;
	float  invpower = 1.0f / float(fft_point_);

	auto fffrrr = omp_get_wtime();

	std::shared_ptr<AllignedBufferF> out_buffer = std::make_shared<AllignedBufferF>(input_buffer->sample_count_);
	auto fft_buffsss = opll_->perform(input_buffer, window_vec_);

	for (int i = 0; i < half_fft_pt; i += 4)
	{
		//out_buffer->set(half_fft_pt + i + 0, 20 * std::log10f(std::abs(out_[i + 0] * invpower)));
		//out_buffer->set(half_fft_pt + i + 1, 20 * std::log10f(std::abs(out_[i + 1] * invpower)));
		//out_buffer->set(half_fft_pt + i + 2, 20 * std::log10f(std::abs(out_[i + 2] * invpower)));
		//out_buffer->set(half_fft_pt + i + 3, 20 * std::log10f(std::abs(out_[i + 3] * invpower)));
		//
		//out_buffer->set(i + 0, 20 * std::log10f(std::abs(out_[half_fft_pt + i + 0] * invpower)));
		//out_buffer->set(i + 1, 20 * std::log10f(std::abs(out_[half_fft_pt + i + 1] * invpower)));
		//out_buffer->set(i + 2, 20 * std::log10f(std::abs(out_[half_fft_pt + i + 2] * invpower)));
		//out_buffer->set(i + 3, 20 * std::log10f(std::abs(out_[half_fft_pt + i + 3] * invpower)));
	}

	fffrrr = omp_get_wtime() - fffrrr;
	return {};
	//return out_buffer;*/

	auto fffrrrssss = omp_get_wtime();
	int fft_pt = int(fft_point_);
	int half_fft_pt = fft_pt / 2;

	for (int i = 0; i < input_buffer->sample_count_; ++i)
	{
		auto sample = input_buffer->get(i);

		in_[i].real(input_buffer->get(i + 0).real() * (window_vec_[i]) / 32768.0f);
		in_[i].imag(input_buffer->get(i + 0).imag() * (window_vec_[i]) / 32768.0f);
	}

	fftwf_execute(handle_);

	fffrrrssss = omp_get_wtime() - fffrrrssss;

	//printf("FFTCpu: GPU=%f , CPU=%f\n", fffrrr, fffrrrssss);

	std::shared_ptr<AllignedBufferF> out_buffer = std::make_shared<AllignedBufferF>(input_buffer->sample_count_);

	float  invpower = 1.0f / float(fft_point_);

	for (int i = 0; i < half_fft_pt; i += 4)
	{
		out_buffer->set(half_fft_pt + i + 0, 20 * std::log10f(std::abs(out_[i + 0] * invpower)));
		out_buffer->set(half_fft_pt + i + 1, 20 * std::log10f(std::abs(out_[i + 1] * invpower)));
		out_buffer->set(half_fft_pt + i + 2, 20 * std::log10f(std::abs(out_[i + 2] * invpower)));
		out_buffer->set(half_fft_pt + i + 3, 20 * std::log10f(std::abs(out_[i + 3] * invpower)));

		out_buffer->set(i + 0, 20 * std::log10f(std::abs(out_[half_fft_pt + i + 0] * invpower)));
		out_buffer->set(i + 1, 20 * std::log10f(std::abs(out_[half_fft_pt + i + 1] * invpower)));
		out_buffer->set(i + 2, 20 * std::log10f(std::abs(out_[half_fft_pt + i + 2] * invpower)));
		out_buffer->set(i + 3, 20 * std::log10f(std::abs(out_[half_fft_pt + i + 3] * invpower)));
	}

	return out_buffer;
}