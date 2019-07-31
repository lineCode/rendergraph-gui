#pragma once
#include <utility>
#include <memory>

namespace gfx {
	class GraphicsBackend;

	template <typename T, typename Deleter> class Handle {
	public:
		Handle(gfx::GraphicsBackend *backend, T raw) : backend_{ backend }, raw_{ raw } {}

		/// default and nullptr constructors folded together
		Handle(std::nullptr_t = nullptr) : backend_{ nullptr }, raw_{ 0 } {}

		/// Move-constructor: take ownership
		Handle(Handle &&rhs) : backend_{ rhs.backend_ }, raw_{ rhs.raw_ } {
			rhs.backend_ = nullptr;
			rhs.raw_ = 0;
		}

		/// Move-assignment operator: take ownership
		Handle &operator=(Handle &&rhs) {
			std::swap(backend_, rhs.backend_);
			std::swap(raw_, rhs.raw_);
			return *this;
		}

		// not copyable
		Handle(const Handle &) = delete;
		Handle &operator=(const Handle &) = delete;

		~Handle() {
			if (raw_) {
				Deleter{}(backend_, raw_);
			}
		}

		GraphicsBackend *backend() const { return backend_; }
		T get() const { return raw_; }

		explicit operator bool() { return obj != 0; }
		friend bool operator==(const Handle &l, const Handle &r) {
			return l.backend_ == r.backend_ && l.raw_ == r.raw_;
		}
		friend bool operator!=(const Handle &l, const Handle &r) { return !(l == r); }

		// default copy ctor and operator= are fine
		// explicit nullptr assignment and comparison unneeded
		// because of implicit nullptr constructor
		// swappable requirement fulfilled by std::swap

	private:
		gfx::GraphicsBackend *backend_;
		T raw_;
	};
	/*
	template <typename T, typename Deleter> class SharedHandle {
	public:
		SharedHandle(gfx::GraphicsBackend *backend, T raw) : backend_{ backend }, raw_{ raw } {}



	private:
		std::shared_ptr<> ptr;
		gfx::GraphicsBackend *backend_;
		T raw_;
	};*/
}