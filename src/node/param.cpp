#include "node/param.h"

namespace node {

ParamDesc::ParamDesc(const ParamName &name, util::StringRef help,
                     util::Value::Type baseType, int numChannels,
                     ParamHint paramHint, util::Value defaultValue,
                     util::ArrayRef<ParamFloatRange> channelRanges)
    : name_{name}, help_{help}, baseType_{baseType}, numChannels_{numChannels},
	paramHint_{ paramHint}, channelRanges_{channelRanges} {}


} // namespace node