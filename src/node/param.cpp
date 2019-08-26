#include "node/param.h"

namespace node {

ParamDesc::ParamDesc(const ParamName &name, util::StringRef help,
                     util::Value::Type baseType, int numChannels,
                     ParamHint paramHint, util::Value defaultValue,
                     util::ArrayRef<ParamFloatRange> channelRanges)
    : name{name}, help{help}, baseType{baseType}, numChannels{numChannels},
	paramHint{ paramHint}, channelRanges{channelRanges} {}


} // namespace node