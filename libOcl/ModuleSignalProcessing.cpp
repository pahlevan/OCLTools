#include "ModuleSignalProcessing.h"

#include "../libBuffer/AllignedBufferI16C.h"
#include "../libBuffer/AllignedBufferF.h"
#include "../libBuffer/AllignedBufferFC.h"

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
			const float  inv_rate = 1/32768.0f;
			int threadId = get_global_id(0); // OpenCL function
			c[threadId].x = a[threadId].x * b[threadId]*inv_rate;
			c[threadId].y = a[threadId].y * b[threadId]*inv_rate;
		}
		)CLC" };

	static std::string PostProcessCode{
		R"CLC(
        __kernel void PostProcessCode(
		__global const  float2* input,
		__global float* output)
		{
			const int threadId = get_global_id(0);
			const int count = get_global_size(0);
			const int half_count = count >> 1;
			float ratio_power = 1.0f / count;

			if( threadId < half_count)
			{
				float2 sample = input[threadId] *ratio_power ;
				output[threadId + half_count] = 20.0f * log10( sqrt( sample.x *sample.x +  sample.y * sample.y));
			}
			else
			{
				float2 sample = input[threadId]*ratio_power ;
				output[threadId] = 20.0f * log10( sqrt( sample.x *sample.x +  sample.y * sample.y));
			}
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
		ret = clReleaseMemObject(a_mem_obj_);
		ret = clReleaseMemObject(b_mem_obj_);
		ret = clReleaseMemObject(c_mem_obj_);
		ret = clReleaseCommandQueue(command_queue_);
		ret = clReleaseContext(context_);
	}

	auto ModuleSignalProcessing::create(size_t sample_count) ->std::shared_ptr<ModuleSignalProcessing>
	{
		// Filter for a 2.0 platform and set it as the default
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		cl::Platform plat;

		for (auto& p : platforms)
		{
			std::string platver = p.getInfo<CL_PLATFORM_VERSION>();


			if (platver.find("OpenCL 2.") != std::string::npos)
			{
				plat = p;
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
		cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sample_count * sizeof(std::complex<int16_t>), NULL, &ret);
		cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sample_count * sizeof(float), NULL, &ret);
		cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, sample_count * sizeof(std::complex<float>), NULL, &ret);
		cl_mem signal_power_out = clCreateBuffer(context, CL_MEM_READ_WRITE, sample_count * sizeof(float), NULL, &ret);

		// Create a program from the kernel source
		char* source_str[] = { vectorMultiplicationCode.data() ,PostProcessCode.data() };
		size_t source_size[] = { vectorMultiplicationCode.size()  , PostProcessCode.size() };

		cl_program program = clCreateProgramWithSource(context, 2, (const char**)source_str, source_size, &ret);
		if (ret != CL_SUCCESS)
		{
			ret = clReleaseMemObject(a_mem_obj);
			ret = clReleaseMemObject(b_mem_obj);
			ret = clReleaseMemObject(c_mem_obj);
			clReleaseCommandQueue(command_queue);
			clReleaseContext(context);
			return {};
		}
		// Build the program
		ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			clReleaseProgram(program);
			clReleaseMemObject(a_mem_obj);
			clReleaseMemObject(b_mem_obj);
			clReleaseMemObject(c_mem_obj);
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
			ret = clReleaseMemObject(a_mem_obj);
			ret = clReleaseMemObject(b_mem_obj);
			ret = clReleaseMemObject(c_mem_obj);
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
			ret = clReleaseMemObject(a_mem_obj);
			ret = clReleaseMemObject(b_mem_obj);
			ret = clReleaseMemObject(c_mem_obj);
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
		obj->a_mem_obj_ = a_mem_obj;
		obj->b_mem_obj_ = b_mem_obj;
		obj->c_mem_obj_ = c_mem_obj;
		obj->signal_power_out_ = signal_power_out;

		obj->fftSetup_ = fftSetup;
		obj->planHandle_ = planHandle;

		return obj;
	}

	auto ModuleSignalProcessing::perform(const std::shared_ptr<AllignedBufferI16C>& rawData, const std::vector<float>& window /*, const std::shared_ptr<AllignedBufferF>& window*/) ->std::shared_ptr<AllignedBufferF>
	{
		size_t sample_count = rawData->size();
		cl_int ret;
		// Set the arguments of the kernel
		{
			ret = clSetKernelArg(kernel_preprocess_, 0, sizeof(cl_mem), (void*)& a_mem_obj_);
			ret = clSetKernelArg(kernel_preprocess_, 1, sizeof(cl_mem), (void*)& b_mem_obj_);
			ret = clSetKernelArg(kernel_preprocess_, 2, sizeof(cl_mem), (void*)& c_mem_obj_);

			// Copy the lists A and B to their respective memory buffers
			ret = clEnqueueWriteBuffer(command_queue_, a_mem_obj_, CL_TRUE, 0, sample_count * sizeof(std::complex<int16_t>), rawData->data(), 0, NULL, NULL);
			ret = clEnqueueWriteBuffer(command_queue_, b_mem_obj_, CL_TRUE, 0, sample_count * sizeof(float), window.data(), 0, NULL, NULL);

			// Execute the OpenCL kernel on the list
			size_t global_item_size = sample_count; // Process the entire lists
			size_t local_item_size = 64; // Divide work items into groups of 64
			ret = clEnqueueNDRangeKernel(command_queue_, kernel_preprocess_, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
		}

		std::shared_ptr<AllignedBufferFC> retBufferf  = std::make_shared<AllignedBufferFC>(sample_count);
		{	

			//////////////////////////////////////////////////////////////////////////
		/* Execute the plan. */
			ret = clfftEnqueueTransform(planHandle_, CLFFT_FORWARD, 1, &command_queue_, 0, NULL, NULL, &c_mem_obj_, NULL, NULL);

			ret = clEnqueueReadBuffer(command_queue_, c_mem_obj_, CL_TRUE, 0, sample_count * sizeof(std::complex<float>), retBufferf->data(), 0, NULL, NULL);
		}

		std::shared_ptr<AllignedBufferF> retBuffer = std::make_shared<AllignedBufferF>(sample_count);

		//////////////////////////////////////////////////////////////////////////
		{
			ret = clSetKernelArg(kernel_postprocess_, 0, sizeof(cl_mem), (void*)& c_mem_obj_);
			ret = clSetKernelArg(kernel_postprocess_, 1, sizeof(cl_mem), (void*)& signal_power_out_);
			// Execute the OpenCL kernel on the list
			size_t global_item_size = sample_count; // Process the entire lists
			size_t local_item_size = 64; // Divide work items into groups of 64
			ret = clEnqueueNDRangeKernel(command_queue_, kernel_postprocess_, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

			ret = clEnqueueReadBuffer(command_queue_, signal_power_out_, CL_TRUE, 0, sample_count * sizeof(float), retBuffer->data(), 0, NULL, NULL);


		}


		ret = clFlush(command_queue_);
		ret = clFinish(command_queue_);

		return retBuffer;
	}
}