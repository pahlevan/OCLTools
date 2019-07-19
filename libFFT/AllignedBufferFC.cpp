#include "AllignedBufferFC.h"


#include <fftw3.h>


AllignedBufferFC::AllignedBufferFC(const size_t sample_count)
	: sample_count_(sample_count)
{
	buffer_ = (std::complex<float>*)fftwf_malloc(sizeof(std::complex<float>) * sample_count);

}

AllignedBufferFC::~AllignedBufferFC()
{
	fftwf_free(buffer_);
}

void AllignedBufferFC::set(const size_t pos, const std::complex<float>& data)
{ // subscript mutable sequence
#if _DEBUG
	assert(pos < sample_count_ && "vector subscript out of range");
#endif

	buffer_[pos] = data;
}

[[nodiscard]] const std::complex<float>& AllignedBufferFC::get(const size_t pos)const
{ // subscript mutable sequence
#if _DEBUG
	assert(pos < sample_count_ && "vector subscript out of range");
#endif

	return buffer_[pos];
}