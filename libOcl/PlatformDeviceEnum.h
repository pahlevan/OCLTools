#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

typedef struct _cl_platform_id *    cl_platform_id;
typedef struct _cl_device_id *      cl_device_id;

namespace ocl
{
	class PlatformDeviceEnum
	{
	public:
		PlatformDeviceEnum() = default;
		~PlatformDeviceEnum() = default;

		static auto enumPlatform()->std::vector<std::shared_ptr<PlatformDeviceEnum>>;


		cl_platform_id  getPlatform() const { return pl_;}
		cl_device_id getDevice() const { return dev_; }

		const std::vector<std::pair<std::string, std::string> >& getDeviceInfo() const { return  device_info_vec_; }
		const std::vector<std::pair<std::string, std::string> >& getPlatformInfo() const { return  platform_info_vec_; }

	private:
		cl_platform_id pl_;
		cl_device_id dev_;

		std::vector<std::pair<std::string, std::string> >device_info_vec_;
		std::vector<std::pair<std::string, std::string> >platform_info_vec_;
	};
}