#pragma once
#include "gfxopengl/glcore45.h"
#include <utility>	// std::swap

namespace gfxopengl {
/// Wrapper to use gl::GLuint as a unique_ptr handle type
/// http://stackoverflow.com/questions/6265288/unique-ptr-custom-storage-type-example/6272139#6272139
/// This type follows the same semantics as unique_ptr.
/// Deleter is a functor type with a static operator()(GLuint) member, that is
/// in charge of deleting the OpenGL object by calling glDeleteX The deleter is
/// automatically called when obj != 0 and:
///		- the GLHandle goes out of scope
///		- the GLHandle is move-assigned another GLHandle
/// This is useful to avoid specifying move constructors for all wrapper types.
template <typename Deleter> struct GLHandle {
  unsigned int obj;
  GLHandle(gl::GLuint obj_) : obj{obj_} {}

  /// default and nullptr constructors folded together
  GLHandle(std::nullptr_t = nullptr) : obj{0} {}

  /// Move-constructor: take ownership of the GL resource
  GLHandle(GLHandle &&rhs) : obj{rhs.obj} { rhs.obj = 0; }
  /// Move-assignment operator: take ownership of the GL resource
  GLHandle &operator=(GLHandle &&rhs) {
    std::swap(obj, rhs.obj);
    return *this;
  }

  // GL resources are not copyable, so delete the copy ctors
  GLHandle(const GLHandle &) = delete;
  GLHandle &operator=(const GLHandle &) = delete;

  ~GLHandle() {
    if (obj) {
      Deleter{}(obj);
      obj = 0;
    }
  }

  unsigned int get() const { return obj; }

  explicit operator bool() { return obj != 0; }
  friend bool operator==(const GLHandle &l, const GLHandle &r) {
    return l.obj == r.obj;
  }
  friend bool operator!=(const GLHandle &l, const GLHandle &r) {
    return !(l == r);
  }
  // default copy ctor and operator= are fine
  // explicit nullptr assignment and comparison unneeded
  // because of implicit nullptr constructor
  // swappable requirement fulfilled by std::swap
};
} // namespace gfxopengl