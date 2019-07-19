#include "PlatformDeviceEnum.h"

#include <CL/cl.h>

namespace ocl
{
	auto loadPlatformInfo(const cl_platform_id pl, uint32_t name)-> std::string
	{
		cl_int errNum;
		std::size_t paramValueSize;

		errNum = clGetPlatformInfo(pl, name, 0, nullptr, &paramValueSize);
		if (errNum != CL_SUCCESS)
		{
			//std::cerr << "Failed to find OpenCL platform " << str << "." << std::endl;
			return "";
		}

		std::string info; info.resize(paramValueSize);

		errNum = clGetPlatformInfo(pl, name, paramValueSize, &info[0], nullptr);
		if (errNum != CL_SUCCESS)
		{
			//std::cerr << "Failed to find OpenCL platform " << str << "." << std::endl;
			return "";
		}

		return info;
	}

	template<typename T>
	void appendBitfield(T info, T value, std::string name, std::string& str)
	{
		if (info & value)
		{
			if (str.length() > 0)
			{
				str.append(" | ");
			}
			str.append(name);
		}
	}

	///
	// Display information for a particular device.
	// As different calls to clGetDeviceInfo may return
	// values of different types a template is used.
	// As some values returned are arrays of values, a templated class is
	// used so it can be specialized for this case, see below.
	//
	template <typename T>
	class InfoDevice
	{
	public:
		static std::pair<std::string, std::string> display(
			const cl_device_id id,
			const cl_device_info name,
			const std::string& str)
		{
			cl_int errNum;
			std::size_t paramValueSize;
			std::string ret_val;
			errNum = clGetDeviceInfo(id, name, 0, NULL, &paramValueSize);
			if (errNum != CL_SUCCESS)
			{
				//std::cerr << "Failed to find OpenCL device info " << str << "." << std::endl;
				return { "","" };
			}

			auto info = (T*)alloca((sizeof T) * paramValueSize);
			errNum = clGetDeviceInfo(id, name, paramValueSize, info, NULL);
			if (errNum != CL_SUCCESS)
			{
				//std::cerr << "Failed to find OpenCL device info " << str << "." << std::endl;
				return { "","" };
			}

			// Handle a few special cases
			switch (name)
			{
			case CL_DEVICE_TYPE:
			{
				std::string deviceType;
				appendBitfield<cl_device_type>(*(reinterpret_cast<cl_device_type*>(info)), CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU", deviceType);
				appendBitfield<cl_device_type>(*(reinterpret_cast<cl_device_type*>(info)), CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU", deviceType);
				appendBitfield<cl_device_type>(*(reinterpret_cast<cl_device_type*>(info)), CL_DEVICE_TYPE_ACCELERATOR, "CL_DEVICE_TYPE_ACCELERATOR", deviceType);
				appendBitfield<cl_device_type>(*(reinterpret_cast<cl_device_type*>(info)), CL_DEVICE_TYPE_DEFAULT, "CL_DEVICE_TYPE_DEFAULT", deviceType);
				//std::cout << "\t\t" << str << ":\t" << deviceType << std::endl;
				ret_val = deviceType;
			}
			break;
			case CL_DEVICE_SINGLE_FP_CONFIG:
			{
				std::string fpType;
				appendBitfield<cl_device_fp_config>(*(reinterpret_cast<cl_device_fp_config*>(info)), CL_FP_DENORM, "CL_FP_DENORM", fpType);
				appendBitfield<cl_device_fp_config>(*(reinterpret_cast<cl_device_fp_config*>(info)), CL_FP_INF_NAN, "CL_FP_INF_NAN", fpType);
				appendBitfield<cl_device_fp_config>(*(reinterpret_cast<cl_device_fp_config*>(info)), CL_FP_ROUND_TO_NEAREST, "CL_FP_ROUND_TO_NEAREST", fpType);
				appendBitfield<cl_device_fp_config>(*(reinterpret_cast<cl_device_fp_config*>(info)), CL_FP_ROUND_TO_ZERO, "CL_FP_ROUND_TO_ZERO", fpType);
				appendBitfield<cl_device_fp_config>(*(reinterpret_cast<cl_device_fp_config*>(info)), CL_FP_ROUND_TO_INF, "CL_FP_ROUND_TO_INF", fpType);
				appendBitfield<cl_device_fp_config>(*(reinterpret_cast<cl_device_fp_config*>(info)), CL_FP_FMA, "CL_FP_FMA", fpType);
#ifdef CL_FP_SOFT_FLOAT
				appendBitfield<cl_device_fp_config>(*(reinterpret_cast<cl_device_fp_config*>(info)), CL_FP_SOFT_FLOAT, "CL_FP_SOFT_FLOAT", fpType);
#endif

				//std::cout << "\t\t" << str << ":\t" << fpType << std::endl;
				ret_val = fpType;
			}
			case CL_DEVICE_GLOBAL_MEM_CACHE_TYPE:
			{
				std::string memType;
				appendBitfield<cl_device_mem_cache_type>(*(reinterpret_cast<cl_device_mem_cache_type*>(info)), CL_NONE, "CL_NONE", memType);
				appendBitfield<cl_device_mem_cache_type>(*(reinterpret_cast<cl_device_mem_cache_type*>(info)), CL_READ_ONLY_CACHE, "CL_READ_ONLY_CACHE", memType);
				appendBitfield<cl_device_mem_cache_type>(*(reinterpret_cast<cl_device_mem_cache_type*>(info)), CL_READ_WRITE_CACHE, "CL_READ_WRITE_CACHE", memType);
				//std::cout << "\t\t" << str << ":\t" << memType << std::endl;
				ret_val = memType;
			}
			break;
			case CL_DEVICE_LOCAL_MEM_TYPE:
			{
				std::string memType;
				appendBitfield<cl_device_local_mem_type>(*(reinterpret_cast<cl_device_local_mem_type*>(info)), CL_GLOBAL, "CL_LOCAL", memType);
				appendBitfield<cl_device_local_mem_type>(*(reinterpret_cast<cl_device_local_mem_type*>(info)), CL_GLOBAL, "CL_GLOBAL", memType);
				//std::cout << "\t\t" << str << ":\t" << memType << std::endl;
				ret_val = memType;
			}
			break;
			case CL_DEVICE_EXECUTION_CAPABILITIES:
			{
				std::string memType;
				appendBitfield<cl_device_exec_capabilities>(*(reinterpret_cast<cl_device_exec_capabilities*>(info)), CL_EXEC_KERNEL, "CL_EXEC_KERNEL", memType);
				appendBitfield<cl_device_exec_capabilities>(*(reinterpret_cast<cl_device_exec_capabilities*>(info)), CL_EXEC_NATIVE_KERNEL, "CL_EXEC_NATIVE_KERNEL", memType);
				//std::cout << "\t\t" << str << ":\t" << memType << std::endl;
				ret_val = memType;
			}
			break;
			case CL_DEVICE_QUEUE_PROPERTIES:
			{
				std::string memType;
				appendBitfield<cl_device_exec_capabilities>(*(reinterpret_cast<cl_device_exec_capabilities*>(info)), CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE", memType);
				appendBitfield<cl_device_exec_capabilities>(*(reinterpret_cast<cl_device_exec_capabilities*>(info)), CL_QUEUE_PROFILING_ENABLE, "CL_QUEUE_PROFILING_ENABLE", memType);
				//std::cout << "\t\t" << str << ":\t" << memType << std::endl;
				ret_val = memType;
			}
			break;
			default:
				//std::cout << "\t\t" << str << ":\t" << *info << std::endl;
				if constexpr (std::is_pointer<T>::value)
				{
					ret_val = std::to_string((uint64_t)((uint64_t*)(info[0])));
				}
				else
				{
					ret_val = std::to_string(*info);
				}

				break;
			}
			return { str, ret_val };
		}
	};

	///
	// Simple trait class used to wrap base types.
	//
	template <typename T>
	class ArrayType
	{
	public:
		static bool isChar() { return false; }
	};

	///
	// Specialized for the char (i.e. null terminated string case).
	//
	template<>
	class ArrayType<char>
	{
	public:
		static bool isChar() { return true; }
	};

	///
	// Specialized instance of class InfoDevice for array types.
	//
	template <typename T>
	class InfoDevice<ArrayType<T> >
	{
	public:
		static std::pair<std::string, std::string>  display(
			cl_device_id id,
			cl_device_info name,
			const std::string& str)
		{
			cl_int errNum;
			std::size_t paramValueSize;

			std::string ret_val;

			errNum = clGetDeviceInfo(id, name, 0, NULL, &paramValueSize);
			if (errNum != CL_SUCCESS)
			{
				//std::cerr
					// 					<< "Failed to find OpenCL device info "
					// 					<< str
					// 					<< "."
					// 					<< std::endl;
				return { "","" };
			}

			T* info = (T*)alloca(sizeof(T) * paramValueSize);
			errNum = clGetDeviceInfo(
				id,
				name,
				paramValueSize,
				info,
				NULL);
			if (errNum != CL_SUCCESS)
			{
				// 				std::cerr
				// 					<< "Failed to find OpenCL device info "
				// 					<< str
				// 					<< "."
				// 					<< std::endl;
				return { "","" };
			}

			if (ArrayType<T>::isChar())
			{
				//	std::cout << "\t" << str << ":\t" << info << std::endl;
				ret_val = (char*)info;

				int gggg = 0;
			}
			else if (name == CL_DEVICE_MAX_WORK_ITEM_SIZES)
			{
				cl_uint maxWorkItemDimensions;

				errNum = clGetDeviceInfo(
					id,
					CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
					sizeof(cl_uint),
					&maxWorkItemDimensions,
					NULL);
				if (errNum != CL_SUCCESS)
				{
					//std::cerr
					//	<< "Failed to find OpenCL device info "
					//	<< "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS."
					//	<< std::endl;
					return { "","" };
				}

				//std::cout << "\t" << str << ":\t";
				for (cl_uint i = 0; i < maxWorkItemDimensions; i++)
				{
					//std::cout <<  << " ";

					appendBitfield(1, 1, std::to_string(info[i]), ret_val);
				}
				//	std::cout << std::endl;
			}

			return { str,ret_val };
		}
	};

	auto PlatformDeviceEnum::enumPlatform() -> std::vector<std::shared_ptr<PlatformDeviceEnum>>
	{
		std::vector<std::shared_ptr<PlatformDeviceEnum>> ret_pl_dev_vec;

		cl_int errNum;
		cl_uint numPlatforms;
		// First, query the total number of platforms
		errNum = clGetPlatformIDs(0, nullptr, &numPlatforms);
		if (errNum != CL_SUCCESS || numPlatforms <= 0)
		{
			//std::cerr << "Failed to find any OpenCL platform." << std::endl;
			return {};
		}

		// Next, allocate memory for the installed platforms, and query to get the list.
		std::vector<cl_platform_id> platformIds;	platformIds.resize(numPlatforms);

		// First, query the total number of platforms
		errNum = clGetPlatformIDs(numPlatforms, &platformIds[0], nullptr);
		if (errNum != CL_SUCCESS)
		{
			//std::cerr << "Failed to find any OpenCL platforms." << std::endl;
			return {};
		}

		for (const auto& pl : platformIds)
		{
			std::vector<std::pair<std::string, std::string> > platform_info_vec;
			platform_info_vec.emplace_back("CL_PLATFORM_PROFILE", loadPlatformInfo(pl, CL_PLATFORM_PROFILE));
			platform_info_vec.emplace_back("CL_PLATFORM_VERSION", loadPlatformInfo(pl, CL_PLATFORM_VERSION));
			platform_info_vec.emplace_back("CL_PLATFORM_VENDOR", loadPlatformInfo(pl, CL_PLATFORM_VENDOR));
			platform_info_vec.emplace_back("CL_PLATFORM_EXTENSIONS", loadPlatformInfo(pl, CL_PLATFORM_EXTENSIONS));

			// Now query the set of devices associated with the platform
			cl_uint numDevices;
			errNum = clGetDeviceIDs(pl, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);
			if (errNum != CL_SUCCESS)
			{
				//std::cerr << "Failed to find OpenCL devices." << std::endl;
				continue;
			}

			std::vector<cl_device_id> devices;	devices.resize(numDevices);
			errNum = clGetDeviceIDs(pl, CL_DEVICE_TYPE_ALL, numDevices, &devices[0], nullptr);
			if (errNum != CL_SUCCESS)
			{
				//std::cerr << "Failed to find OpenCL devices." << std::endl;
				continue;
			}
			for (const auto& dev : devices)
			{
				std::vector<std::pair<std::string, std::string> >device_info_vec;

				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_VENDOR_ID, "CL_DEVICE_VENDOR_ID"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MAX_COMPUTE_UNITS, "CL_DEVICE_MAX_COMPUTE_UNITS"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS"));
				device_info_vec.emplace_back(InfoDevice<ArrayType<std::size_t>>::display(dev, CL_DEVICE_MAX_WORK_ITEM_SIZES, "CL_DEVICE_MAX_WORK_ITEM_SIZES"));
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_MAX_WORK_GROUP_SIZE, "CL_DEVICE_MAX_WORK_GROUP_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE"));
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, "CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, "CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, "CL_DEVICE_NATIVE_VECTOR_WIDTH_INT"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, "CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, "CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, "CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, "CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF"));
#endif
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MAX_CLOCK_FREQUENCY, "CL_DEVICE_MAX_CLOCK_FREQUENCY"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_ADDRESS_BITS, "CL_DEVICE_ADDRESS_BITS"));
				device_info_vec.emplace_back(InfoDevice<cl_ulong>::display(dev, CL_DEVICE_MAX_MEM_ALLOC_SIZE, "CL_DEVICE_MAX_MEM_ALLOC_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_bool>::display(dev, CL_DEVICE_IMAGE_SUPPORT, "CL_DEVICE_IMAGE_SUPPORT"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MAX_READ_IMAGE_ARGS, "CL_DEVICE_MAX_READ_IMAGE_ARGS"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, "CL_DEVICE_MAX_WRITE_IMAGE_ARGS"));
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_IMAGE2D_MAX_WIDTH, "CL_DEVICE_IMAGE2D_MAX_WIDTH"));
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_IMAGE2D_MAX_WIDTH, "CL_DEVICE_IMAGE2D_MAX_WIDTH"));
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_IMAGE2D_MAX_HEIGHT, "CL_DEVICE_IMAGE2D_MAX_HEIGHT"));
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_IMAGE3D_MAX_WIDTH, "CL_DEVICE_IMAGE3D_MAX_WIDTH"));
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_IMAGE3D_MAX_HEIGHT, "CL_DEVICE_IMAGE3D_MAX_HEIGHT"));
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_IMAGE3D_MAX_DEPTH, "CL_DEVICE_IMAGE3D_MAX_DEPTH"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MAX_SAMPLERS, "CL_DEVICE_MAX_SAMPLERS"));
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_MAX_PARAMETER_SIZE, "CL_DEVICE_MAX_PARAMETER_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MEM_BASE_ADDR_ALIGN, "CL_DEVICE_MEM_BASE_ADDR_ALIGN"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_device_fp_config>::display(dev, CL_DEVICE_SINGLE_FP_CONFIG, "CL_DEVICE_SINGLE_FP_CONFIG"));
				device_info_vec.emplace_back(InfoDevice<cl_device_mem_cache_type>::display(dev, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_ulong>::display(dev, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_ulong>::display(dev, CL_DEVICE_GLOBAL_MEM_SIZE, "CL_DEVICE_GLOBAL_MEM_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_ulong>::display(dev, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_uint>::display(dev, CL_DEVICE_MAX_CONSTANT_ARGS, "CL_DEVICE_MAX_CONSTANT_ARGS"));
				device_info_vec.emplace_back(InfoDevice<cl_device_local_mem_type>::display(dev, CL_DEVICE_LOCAL_MEM_TYPE, "CL_DEVICE_LOCAL_MEM_TYPE"));
				device_info_vec.emplace_back(InfoDevice<cl_ulong>::display(dev, CL_DEVICE_LOCAL_MEM_SIZE, "CL_DEVICE_LOCAL_MEM_SIZE"));
				device_info_vec.emplace_back(InfoDevice<cl_bool>::display(dev, CL_DEVICE_ERROR_CORRECTION_SUPPORT, "CL_DEVICE_ERROR_CORRECTION_SUPPORT"));
#ifdef CL_DEVICE_HOST_UNIFIED_MEMORY
				device_info_vec.emplace_back(InfoDevice<cl_bool>::display(dev, CL_DEVICE_HOST_UNIFIED_MEMORY, "CL_DEVICE_HOST_UNIFIED_MEMORY"));
#endif
				device_info_vec.emplace_back(InfoDevice<std::size_t>::display(dev, CL_DEVICE_PROFILING_TIMER_RESOLUTION, "CL_DEVICE_PROFILING_TIMER_RESOLUTION"));
				device_info_vec.emplace_back(InfoDevice<cl_bool>::display(dev, CL_DEVICE_ENDIAN_LITTLE, "CL_DEVICE_ENDIAN_LITTLE"));
				device_info_vec.emplace_back(InfoDevice<cl_bool>::display(dev, CL_DEVICE_AVAILABLE, "CL_DEVICE_AVAILABLE"));
				device_info_vec.emplace_back(InfoDevice<cl_bool>::display(dev, CL_DEVICE_COMPILER_AVAILABLE, "CL_DEVICE_COMPILER_AVAILABLE"));
				device_info_vec.emplace_back(InfoDevice<cl_device_exec_capabilities>::display(dev, CL_DEVICE_EXECUTION_CAPABILITIES, "CL_DEVICE_EXECUTION_CAPABILITIES"));
				device_info_vec.emplace_back(InfoDevice<cl_command_queue_properties>::display(dev, CL_DEVICE_QUEUE_PROPERTIES, "CL_DEVICE_QUEUE_PROPERTIES"));
				device_info_vec.emplace_back(InfoDevice<cl_platform_id>::display(dev, CL_DEVICE_PLATFORM, "CL_DEVICE_PLATFORM"));
				device_info_vec.emplace_back(InfoDevice<ArrayType<char> >::display(dev, CL_DEVICE_NAME, "CL_DEVICE_NAME"));
				device_info_vec.emplace_back(InfoDevice<ArrayType<char> >::display(dev, CL_DEVICE_VENDOR, "CL_DEVICE_VENDOR"));
				device_info_vec.emplace_back(InfoDevice<ArrayType<char> >::display(dev, CL_DRIVER_VERSION, "CL_DRIVER_VERSION"));
				device_info_vec.emplace_back(InfoDevice<ArrayType<char> >::display(dev, CL_DEVICE_PROFILE, "CL_DEVICE_PROFILE"));
				device_info_vec.emplace_back(InfoDevice<ArrayType<char> >::display(dev, CL_DEVICE_VERSION, "CL_DEVICE_VERSION"));
#ifdef CL_DEVICE_OPENCL_C_VERSION
				device_info_vec.emplace_back(InfoDevice<ArrayType<char> >::display(dev, CL_DEVICE_OPENCL_C_VERSION, "CL_DEVICE_OPENCL_C_VERSION"));
#endif
				device_info_vec.emplace_back(InfoDevice<ArrayType<char> >::display(dev, CL_DEVICE_EXTENSIONS, "CL_DEVICE_EXTENSIONS"));

				std::shared_ptr<PlatformDeviceEnum> pldev = std::make_shared<PlatformDeviceEnum>();

				pldev->pl_ = pl;
				pldev->dev_ = dev;
				pldev->platform_info_vec_ = platform_info_vec;
				pldev->device_info_vec_ = device_info_vec;

				ret_pl_dev_vec.push_back(pldev);
			}
		}

		return ret_pl_dev_vec;
	}
}