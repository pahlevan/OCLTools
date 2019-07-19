#pragma once
#include <memory>

class AllignedBufferF
{
	friend class FFTCpu;
	friend class FFTOpenCL;
public:
	AllignedBufferF(const size_t sample_count);
	~AllignedBufferF();

	[[nodiscard]]
	float get(const size_t pos)const;
	void set(const size_t pos, float data);
	size_t size() const { return sample_count_; }
	float* data() { return buffer_; }

private:

	float* buffer_{ nullptr };
	const size_t sample_count_;
};
