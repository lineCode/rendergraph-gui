#pragma once
#include <cstdint>
#include <memory>

namespace gfxopengl {

class SyncTimeline {
public:
  SyncTimeline();

  SyncTimeline(uint64_t init_value);
  ~SyncTimeline();

  void signal(uint64_t value);
  bool clientSync(uint64_t value);
  uint64_t value();

private:
  struct Private;
  std::unique_ptr<Private> d_;
};

} // namespace gfxopengl
