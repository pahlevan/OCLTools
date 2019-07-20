#pragma once

#include "../libFFT/WindowFunction.h"

#include <memory>
#include <vector>

class AllignedBufferF;
class AllignedBufferFC;
class AllignedBufferI16C;

typedef struct _cl_context* cl_context;
typedef struct _cl_kernel* cl_kernel;
typedef struct _cl_program* cl_program;
typedef struct _cl_command_queue* cl_command_queue;
typedef struct _cl_mem* cl_mem;

typedef struct clfftSetupData_ clfftSetupData;
typedef size_t clfftPlanHandle;


namespace ocl
{
	class ModuleSignalProcessing
	{
		ModuleSignalProcessing() = default;
	public:
		~ModuleSignalProcessing();
		static auto create(size_t sample_count , const WindowFunction::win_type win_type) ->std::shared_ptr<ModuleSignalProcessing>;

		auto perform(const std::shared_ptr<AllignedBufferI16C>& rawData) ->std::shared_ptr<AllignedBufferF>;

	private:
		cl_context context_;
		cl_command_queue command_queue_;
		cl_program program_;
		cl_kernel kernel_preprocess_;
		cl_kernel kernel_postprocess_;

		cl_mem mem_obj_input_;
		cl_mem mem_obj_window_;
		cl_mem mem_obj_fft_;
		cl_mem signal_power_out_;

		clfftSetupData* fftSetup_;
		clfftPlanHandle planHandle_;

	};


}