#pragma once

#include <complex>
#include <vector>
#include <cassert>


class AllignedBufferFC
{
	friend class FFTCpu;
	friend class FFTMulti;
	friend class FFTOpenCL;
public:
	AllignedBufferFC(const size_t sample_count);
	~AllignedBufferFC();

	[[nodiscard]]
	const std::complex<float>& get(const size_t pos)const;
	void set(const size_t pos, const std::complex<float>& data);
	size_t size() const { return sample_count_; }
	std::complex<float>* data() { return buffer_; }


private:

	std::complex<float>* buffer_{ nullptr };
	const size_t sample_count_;
};
