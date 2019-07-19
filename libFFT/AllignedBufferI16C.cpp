#include "AllignedBufferI16C.h"

#include <fftw3.h>
#include <assert.h>


AllignedBufferI16C::AllignedBufferI16C(size_t sample_count)
	:sample_count_(sample_count)
{
	buffer_ = (std::complex<int16_t>*)fftwf_malloc(sizeof(std::complex<int16_t>) * sample_count_);

}


AllignedBufferI16C::~AllignedBufferI16C()
{
	fftwf_free(buffer_);
}

void AllignedBufferI16C::set(const size_t pos, const std::complex<int16_t>& data)
{ // subscript mutable sequence
#if _DEBUG
	assert(pos < sample_count_ && "vector subscript out of range");
#endif

	buffer_[pos] = data;
}

[[nodiscard]] 
const std::complex<int16_t>& AllignedBufferI16C::get(const size_t pos)const
{ // subscript mutable sequence
#if _DEBUG
	assert(pos < sample_count_ && "vector subscript out of range");
#endif

	return buffer_[pos];
}