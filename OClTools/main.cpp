#include "../../pato-com-asia/TacticalMonitoring/TacticalMonitoring/libBuffer/AllignedBufferI16C.h"
#include "../libFFT/FFTPointCount.h"
#include "../libMath/MathConst.h"

#include <random>
#include "../../pato-com-asia/TacticalMonitoring/TacticalMonitoring/libOcl/ModuleSignalProcessing.h"
#include "../libFFT/WindowFunction.h"
#include "../libFFT/FFTCpu.h"
#include <omp.h>

#ifdef _DEBUG
#pragma comment( lib , "../3rdparties/debug/lib/clFFT")
#pragma comment( lib , "../3rdparties/debug/lib/OpenCL")
//#pragma comment( lib , "../3rdparties/debug/lib/fmtd")
#pragma comment( lib , "../3rdparties/debug/lib/fftw3f")
#else
#pragma comment( lib , "../3rdparties/lib/clFFT")
#pragma comment( lib , "../3rdparties/lib/OpenCL")
//#pragma comment( lib , "../3rdparties/lib/fmt")
#pragma comment( lib , "../3rdparties/lib/fftw3f")
#endif

int main()
{
	std::random_device rd;
	std::minstd_rand gen(rd());
	std::normal_distribution<float> dis(-1, 1);

	auto fftp = FFTPointCount::Point_512K;

	int fft_point = int(fftp);

	auto ocl = ocl::ModuleSignalProcessing::create(fft_point, WindowFunction::blackman_harris(fft_point));

	auto cpu = std::make_shared<FFTCpu>(fftp, WindowFunction::win_type::WIN_BLACKMAN_HARRIS);
		auto buff = std::make_shared<AllignedBufferI16C>(fft_point);

		// fill data
		for (size_t index = 0; index < fft_point; ++index)
		{
			//auto vvvv = dis(gen);

			std::complex<int16_t> sff; //(32768 * vvvv, 32768 * vvvv);
			sff.real(32768 * dis(gen) * (sin((200000 * pi * (index) / fft_point))));
			sff.imag(32768 * dis(gen) * (cos((200000 * pi * (index) / fft_point))));

			buff->set(index, sff);
		}
	while (true)
	{


		double time_ocl = omp_get_wtime();

		auto ret_ocl = ocl->perform(buff);

		time_ocl = omp_get_wtime() - time_ocl;

		double time_cpu = omp_get_wtime();
		auto ret_cpu = cpu->forward(buff);
		time_cpu = omp_get_wtime() - time_cpu;

		printf(" col=%f  , cpu=%f \n", time_ocl, time_cpu);
	}

	return 0;
}