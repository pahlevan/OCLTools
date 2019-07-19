#include "AllignedBufferF.h"

#include <fftw3.h>

#include <assert.h>
#include <cstdlib>

AllignedBufferF::AllignedBufferF(const size_t sample_count)
	: sample_count_(sample_count)
{
	buffer_ = (float*)fftwf_malloc(sizeof(float) * sample_count);

}

AllignedBufferF::~AllignedBufferF()
{
	fftwf_free(buffer_);
}


[[nodiscard]] 
float AllignedBufferF::get(const size_t pos)const
{ 
	// subscript mutable sequence
#if _DEBUG
	assert(pos < sample_count_ && "vector subscript out of range");
#endif

	return buffer_[pos];
}

void AllignedBufferF::set(const size_t pos, float data)
{
	// subscript mutable sequence
#if _DEBUG
	assert(pos < sample_count_ && "vector subscript out of range");
#endif

	buffer_[pos] = data;
}
