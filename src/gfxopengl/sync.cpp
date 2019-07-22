#include "gfxopengl/sync.h"
#include "gfxopengl/glcore45.h"
#include <deque>

namespace gfxopengl {

struct SyncPoint {
  gl::GLsync sync;
  uint64_t value;
};

struct SyncTimeline::Private {
  std::deque<SyncPoint> syncPoints;
  uint64_t currentValue;
};

SyncTimeline::SyncTimeline() : d_{std::make_unique<SyncTimeline::Private>()} {
  d_->currentValue = 0;
}

SyncTimeline::SyncTimeline(uint64_t initValue)
    : d_{std::make_unique<SyncTimeline::Private>()} {
  d_->currentValue = initValue;
}

SyncTimeline::~SyncTimeline() {
  // should we wait?
}

void SyncTimeline::signal(uint64_t value) {
  auto sync = gl::FenceSync(gl::SYNC_GPU_COMMANDS_COMPLETE, 0);
  d_->syncPoints.push_back(SyncPoint{sync, value});
}

bool SyncTimeline::clientSync(uint64_t value, uint64_t timeoutNs) {
  while (d_->currentValue < value) {
    if (d_->syncPoints.empty()) {
      // There's no sync in the wait queue, and the value has not been reached.
      // Since we don't support signalling from another thread (yet), we reached
      // a deadlock.
      throw std::logic_error{"deadlock waiting for timeline"};
    }
    auto waitResult = gl::ClientWaitSync(
        d_->syncPoints.front().sync, gl::SYNC_FLUSH_COMMANDS_BIT, timeoutNs);
    switch (waitResult) {
    case gl::CONDITION_SATISFIED:
    case gl::ALREADY_SIGNALED:
      d_->currentValue = d_->syncPoints.front().value;
      break;
    case gl::WAIT_FAILED_:
      throw std::runtime_error{"glClientWaitSync returned WAIT_FAILED"};
    default:
      // timeout
      return false;
    }
    // successful wait, pop the sync off the list
    gl::DeleteSync(d_->syncPoints.front().sync);
    d_->syncPoints.pop_front();
  }
  return true;
}

uint64_t SyncTimeline::value() {
  clientSync(UINT64_MAX, 0);
  return d_->currentValue;
}

} // namespace gfxopengl