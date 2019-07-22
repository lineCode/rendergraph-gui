#pragma once
#include "gfxopengl/buffer.h"
#include "gfxopengl/glcore45.h"
#include <cassert>
#include <cstring>
#include <vector>

namespace gfxopengl {

class MappedBuffer {
public:
  MappedBuffer(size_t size)
      : buffer_{size, gl::MAP_WRITE_BIT | gl::MAP_PERSISTENT_BIT |
                          gl::MAP_COHERENT_BIT} {

    ptr_ = gl::MapNamedBufferRange(
        buffer_.object(), 0, size,
        gl::MAP_UNSYNCHRONIZED_BIT | gl::MAP_WRITE_BIT |
            gl::MAP_PERSISTENT_BIT | gl::MAP_COHERENT_BIT);
  }

  gl::GLuint object() const {
	  return buffer_.object();
  }

  size_t size() const { return buffer_.size(); }

  void writeAt(size_t offset, const void *data, size_t len) {
    assert(offset + len < size_);
    std::memcpy((char *)ptr_ + offset, data, len);
  }

private:
  Buffer buffer_;
  void *ptr_;
};

size_t alignOffset(size_t size, size_t align, size_t start, size_t end) {
	//assert(align.is_power_of_two(), "alignment must be a power of two");
	auto off = start & (align - 1);
	if (off > 0) {
		off = align - off;
	}
	if (start + off + size > end) {
		throw std::out_of_range{ "could not fit buffer in provided range" };
	}
	return start + off;
}

class UploadBufferAdapter {
public:
  /// Borrows the mapped buffer, so it must not move or be deleted.
  UploadBufferAdapter(MappedBuffer &buffer, size_t regionOffset,
                      size_t regionSize);

  gl::GLuint object() const {
	  buffer_.object();
  }

  size_t write(const void *data, size_t len, size_t align) {
	  auto offset = alignOffset(len, align, regionCurrentOffset_, regionStartOffset_ + regionSize_);
	  buffer_.writeAt(offset, data, len);
	  regionCurrentOffset_ = offset + len;
	  return offset;
  }

private:
  MappedBuffer &buffer_;
  size_t regionStartOffset_;
  size_t regionSize_;
  size_t regionCurrentOffset_;
};

} // namespace gfxopengl