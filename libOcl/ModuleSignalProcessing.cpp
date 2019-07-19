#include "ModuleSignalProcessing.h"



#include "../libFFT/AllignedBufferI16C.h"
#include "../libFFT/AllignedBufferF.h"
#include "../libFFT/AllignedBufferFC.h"

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120

#include <CL/cl2.hpp>
#include <clFFT.h>

namespace ocl
{
	static std::string vectorMultiplicationCode{
		R"CLC(
		//https://gist.github.com/knotman90/83b004a8bfdc98107d4d068ab44fca63
		typedef short2 cl_short_complex;
		typedef float2 cl_float_complex;
		typedef cl_float_complex cl_complex;

        __kernel void vectorMultiplication(
		__global const  cl_short_complex* a,
		__global const  float* b,
		__global cl_complex* c)
		{
			int threadId = get_global_id(0); // OpenCL function
			c[threadId].x = a[threadId].x * b[threadId];
			c[threadId].y = a[threadId].y * b[threadId];
		}
		)CLC" };

// 	static std::string PostProcessCode{
// 		R"CLC(
//         __kernel void PostProcessCode(
// 		__global const  float2* input,
// 		__global float* output)
// 		{
// 			const int threadId = get_global_id(0);
// 			const int count = get_global_size(0);
// 			const int half_count = count /2;
// 			float ratio_power = 1.0f / count;
// 
// 			if( threadId < half_count)
// 			{
// 				float2 sample1 = input[threadId] *ratio_power ;
// 				output[threadId + half_count] = 20.0f * log10( sqrt( sample1.x *sample1.x +  sample1.y * sample1.y));
// 			
// 				float2 sample2 = input[threadId+half_count] *ratio_power;
// 				output[threadId ] = 20.0f * log10( sqrt( sample2.x *sample2.x +  sample2.y * sample2.y));
// 			}
// 		}
// 		)CLC" };


	static std::string PostProcessCode{
	R"CLC(
        __kernel void PostProcessCode(
		__global const  float2* input,
		__global float* output)
		{
			const int threadId = get_global_id(0);
			const int count = get_global_size(0);
			float ratio_power = .5f / count;
			
			float2 sample1 = input[threadId] *ratio_power ;
			output[threadId + count] = 20.0f * log10( sqrt( sample1.x *sample1.x +  sample1.y * sample1.y));
			
			float2 sample2 = input[threadId + count] *ratio_power;
			output[threadId ] = 20.0f * log10( sqrt( sample2.x *sample2.x +  sample2.y * sample2.y));
			
		}
		)CLC" };

	ModuleSignalProcessing::~ModuleSignalProcessing()
	{
		cl_int ret;

		/* Release clFFT library. */
		ret = clfftDestroyPlan(&planHandle_);
		clfftTeardown();
		delete fftSetup_;

		ret = clFlush(command_queue_);
		ret = clFinish(command_queue_);
		ret = clReleaseKernel(kernel_postprocess_);
		ret = clReleaseKernel(kernel_preprocess_);
		ret = clReleaseProgram(program_);

		ret = clReleaseMemObject(mem_obj_input_);
		ret = clReleaseMemObject(mem_obj_window_);
		ret = clReleaseMemObject(mem_obj_fft_);
		ret = clReleaseMemObject(signal_power_out_);

		ret = clReleaseCommandQueue(command_queue_);
		ret = clReleaseContext(context_);
	}

	auto ModuleSignalProcessing::create(size_t sample_count, const WindowFunction::win_type win_type) ->std::shared_ptr<ModuleSignalProcessing>
	{

		auto window_vec = WindowFunction::build(win_type, int(sample_count), 0);
		for (int i = 0; i < int(sample_count); ++i)
		{
			window_vec[i] = window_vec[i] / 32768.0f;
		}



		// Filter for a 2.0 platform and set it as the default
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		cl::Platform plat;

		for (auto& p : platforms)
		{
			std::string pl_profile = p.getInfo<CL_PLATFORM_PROFILE>();
			std::string pl_version = p.getInfo<CL_PLATFORM_VERSION>();
			std::string pl_name = p.getInfo<CL_PLATFORM_NAME>();
			std::string pl_vendor = p.getInfo<CL_PLATFORM_VENDOR>();


			if (pl_vendor.find("NVIDIA ") != std::string::npos)
			{
				plat = p;
				break;
			}
		}

		std::vector<cl::Device> device;
		plat.getDevices(CL_DEVICE_TYPE_GPU, &device);

		if (device.empty() )
		{
			plat.getDevices(CL_DEVICE_TYPE_CPU, &device);
		}
		if (device.size() == 0)
		{
			return {};
		}

		//std::string platver = p.getInfo<CL_PLATFORM_VERSION>();


		//device[0].
		//////////////////////////////////////////////////////////////////////////

		auto device_id = device[0].get();

		cl_int ret;

		// Create an OpenCL context
		cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
		if (ret != CL_SUCCESS)
		{
			return {};
		}

		// Create a command queue
		cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);
		if (ret != CL_SUCCESS)
		{
			clReleaseContext(context);
			return {};
		}

		// Create memory buffers on the device for each vector
		cl_mem mem_obj_input = clCreateBuffer(context, CL_MEM_READ_ONLY, sample_count * sizeof(std::complex<int16_t>), NULL, &ret);
		cl_mem mem_obj_window = clCreateBuffer(context, CL_MEM_READ_ONLY, sample_count * sizeof(float), NULL, &ret);
		cl_mem mem_obj_fft = clCreateBuffer(context, CL_MEM_READ_WRITE, sample_count * sizeof(std::complex<float>), NULL, &ret);
		cl_mem signal_power_out = clCreateBuffer(context, CL_MEM_READ_WRITE, sample_count * sizeof(float), NULL, &ret);

		// Create a program from the kernel source
		char* source_str[] = { vectorMultiplicationCode.data() ,PostProcessCode.data() };
		size_t source_size[] = { vectorMultiplicationCode.size()  , PostProcessCode.size() };

		cl_program program = clCreateProgramWithSource(context, 2, (const char**)source_str, source_size, &ret);
		if (ret != CL_SUCCESS)
		{
			ret = clReleaseMemObject(mem_obj_input);
			ret = clReleaseMemObject(mem_obj_window);
			ret = clReleaseMemObject(mem_obj_fft);
			clReleaseCommandQueue(command_queue);
			clReleaseContext(context);
			return {};
		}
		// Build the program
		ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			clReleaseProgram(program);
			clReleaseMemObject(mem_obj_input);
			clReleaseMemObject(mem_obj_window);
			clReleaseMemObject(mem_obj_fft);
			clReleaseCommandQueue(command_queue);
			clReleaseContext(context);
			return {};
		}

		// Create the OpenCL kernel
		cl_kernel kernel_preprocess = clCreateKernel(program, "vectorMultiplication", &ret);
		if (ret != CL_SUCCESS)
		{
			ret = clReleaseKernel(kernel_preprocess);
			ret = clReleaseProgram(program);
			ret = clReleaseMemObject(mem_obj_input);
			ret = clReleaseMemObject(mem_obj_window);
			ret = clReleaseMemObject(mem_obj_fft);
			clReleaseCommandQueue(command_queue);
			clReleaseContext(context);
			return {};
		}

		// Create the OpenCL kernel
		cl_kernel kernel_postprocess = clCreateKernel(program, "PostProcessCode", &ret);
		if (ret != CL_SUCCESS)
		{
			ret = clReleaseKernel(kernel_preprocess);
			ret = clReleaseProgram(program);
			ret = clReleaseMemObject(mem_obj_input);
			ret = clReleaseMemObject(mem_obj_window);
			ret = clReleaseMemObject(mem_obj_fft);
			clReleaseCommandQueue(command_queue);
			clReleaseContext(context);
			return {};
		}

		/* Setup clFFT. */
		clfftSetupData* fftSetup = new clfftSetupData;
		ret = clfftInitSetupData(fftSetup);
		ret = clfftSetup(fftSetup);

		clfftPlanHandle planHandle;
		clfftDim dim = CLFFT_1D;
		size_t clLengths[1] = { sample_count };
		/* Create a default plan for a complex FFT. */
		ret = clfftCreateDefaultPlan(&planHandle, context, dim, clLengths);

		/* Set plan parameters. */
		ret = clfftSetPlanPrecision(planHandle, CLFFT_SINGLE);
		ret = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);
		ret = clfftSetResultLocation(planHandle, CLFFT_INPLACE);

		/* Bake the plan. */
		ret = clfftBakePlan(planHandle, 1, &command_queue, NULL, NULL);

		std::shared_ptr<ModuleSignalProcessing> obj = std::shared_ptr<ModuleSignalProcessing>(new ModuleSignalProcessing);
		obj->context_ = context;
		obj->command_queue_ = command_queue;
		obj->program_ = program;
		obj->kernel_preprocess_ = kernel_preprocess;
		obj->kernel_postprocess_ = kernel_postprocess;
		obj->mem_obj_input_ = mem_obj_input;
		obj->mem_obj_window_ = mem_obj_window;
		obj->mem_obj_fft_ = mem_obj_fft;
		obj->signal_power_out_ = signal_power_out;

		ret = clEnqueueWriteBuffer(obj->command_queue_, obj->mem_obj_window_, CL_TRUE, 0, sample_count * sizeof(float), window_vec.data(), 0, NULL, NULL);


		obj->fftSetup_ = fftSetup;
		obj->planHandle_ = planHandle;

		return obj;
	}

	auto ModuleSignalProcessing::perform(const std::shared_ptr<AllignedBufferI16C>& rawData ) ->std::shared_ptr<AllignedBufferF>
	{
		size_t sample_count = rawData->size();
		cl_int ret;
		// Set the arguments of the kernel
		{
			ret = clSetKernelArg(kernel_preprocess_, 0, sizeof(cl_mem), (void*)& mem_obj_input_);
			ret = clSetKernelArg(kernel_preprocess_, 1, sizeof(cl_mem), (void*)& mem_obj_window_);
			ret = clSetKernelArg(kernel_preprocess_, 2, sizeof(cl_mem), (void*)& mem_obj_fft_);

			// Copy the lists A and B to their respective memory buffers
			ret = clEnqueueWriteBuffer(command_queue_, mem_obj_input_, CL_TRUE, 0, sample_count * sizeof(std::complex<int16_t>), rawData->data(), 0, NULL, NULL);

			// Execute the OpenCL kernel on the list
			size_t global_item_size = sample_count; // Process the entire lists
			size_t local_item_size = 64; // Divide work items into groups of 64
			ret = clEnqueueNDRangeKernel(command_queue_, kernel_preprocess_, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
		}

		std::shared_ptr<AllignedBufferFC> retBufferf = std::make_shared<AllignedBufferFC>(sample_count);
		{

			//////////////////////////////////////////////////////////////////////////
		/* Execute the plan. */
			ret = clfftEnqueueTransform(planHandle_, CLFFT_FORWARD, 1, &command_queue_, 0, NULL, NULL, &mem_obj_fft_, NULL, NULL);

			ret = clEnqueueReadBuffer(command_queue_, mem_obj_fft_, CL_TRUE, 0, sample_count * sizeof(std::complex<float>), retBufferf->data(), 0, NULL, NULL);
		}

		std::shared_ptr<AllignedBufferF> retBuffer = std::make_shared<AllignedBufferF>(sample_count);

		//////////////////////////////////////////////////////////////////////////
		{
			ret = clSetKernelArg(kernel_postprocess_, 0, sizeof(cl_mem), (void*)& mem_obj_fft_);
			ret = clSetKernelArg(kernel_postprocess_, 1, sizeof(cl_mem), (void*)& signal_power_out_);
			// Execute the OpenCL kernel on the list
			size_t global_item_size = sample_count/2; // Process the entire lists
			size_t local_item_size = 64; // Divide work items into groups of 64
			ret = clEnqueueNDRangeKernel(command_queue_, kernel_postprocess_, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

			ret = clEnqueueReadBuffer(command_queue_, signal_power_out_, CL_TRUE, 0, sample_count * sizeof(float), retBuffer->data(), 0, NULL, NULL);
		}


		ret = clFlush(command_queue_);
		ret = clFinish(command_queue_);

		return retBuffer;
	}
}