#include "node/param.h"

namespace node {

ParamDesc::ParamDesc(util::StringRef name, util::StringRef friendlyName,
                     util::StringRef help, util::Value::Type baseType,
                     int numChannels, ParamHint paramHint,
                     util::Value                     defaultValue,
                     util::ArrayRef<ParamFloatRange> channelRanges)
    : name{name}, friendlyName{friendlyName}, help{help}, baseType{baseType},
      numChannels{numChannels}, paramHint{paramHint},
      channelRanges{channelRanges.begin(), channelRanges.end()} {}

} // namespace node