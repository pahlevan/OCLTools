#pragma once
#include <cstdint>
#include <complex>

class AllignedBufferI16C
{
	friend class FFTCpu;
public:
	AllignedBufferI16C(size_t sample_count);
	~AllignedBufferI16C();

	[[nodiscard]]
	const std::complex<int16_t>& get(const size_t pos)const;
	void set(const size_t pos, const std::complex<int16_t>& data);
	size_t size() const { return sample_count_; }
	std::complex<int16_t>* data() { return buffer_; }

private:

	std::complex<int16_t>* buffer_{ nullptr };
	const size_t sample_count_{ 0 };
};
