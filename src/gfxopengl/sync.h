#pragma once
#include <cstdint>
#include <memory>

namespace gfxopengl {

/// Synchronization timelines.
///
/// A timeline is synchronization primitive that contains a monotonically increasing 64-bit integer.
/// 'Signal' operations inside the GPU command stream increase this value.
/// The application can then wait for a specific timeline value to ensure that all commands prior
/// to the corresponding signal operation have finished.
///
/// This is basically an emulation of D3D12 Fences, and of Vulkan's proposed 'timeline semaphores'.
class SyncTimeline {
public:
	/// Creates a new timeline initialized to zero.
  SyncTimeline();

  /// Creates a new timeline with the specified initial value.
  SyncTimeline(uint64_t init_value);
  ~SyncTimeline();

  /// Signals the timeline.
  ///
  /// The timeline value is increased once all GPU operations submitted prior to this call
  /// have completed.
  void signal(uint64_t value);

  /// Waits for the given value to be reached (on the application side).
  ///
  /// Timeout is for a single ClientWaitSync only: there may be more than one.
  /// Returns true if value reached, false if timeout. Panics if wait failed.
  /// Timeout is in nanoseconds.
  bool clientSync(uint64_t value, uint64_t timeoutNs);

  /// Returns the latest reached value. 
  uint64_t value();

private:
  struct Private;
  std::unique_ptr<Private> d_;
};

} // namespace gfxopengl
