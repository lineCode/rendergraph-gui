#pragma once
#include "gfx/color.h"
#include "node/node.h"
#include "util/value.h"

namespace node {

struct ParamName {
  ParamName(util::StringRef name, util::StringRef friendlyName = "")
      : name{name}, friendlyName{friendlyName} {}
  util::StringRef name;
  util::StringRef friendlyName;
};

enum class ParamHint {
  None,
  MinMax,
  Angle,
  ColorRGBA, // Float x 4, channels r,g,b,a
  ColorRGB,  // Float x 3  channels r,g,b
  FileName,  // String
  Ramp,
  UiSlider, // Unspecified, but display with a slider
};

//------------------------------------------------------------------------------------
// parameter descs have a base type (the actual data), a component counts (how
// many components are there) and "interpretation bits" (e.g. whether it's a
// color, etc.)

struct ParamFloatRange {
  bool   hasMin;
  bool   hasMax;
  double min;
  double max;
};

/// Description of a parameter.
///
/// This object does NOT acquire ownership of strings and arrays passed in the
/// constructor, so be careful that any pointer that you pass to the constructor
/// lives long enough.
class ParamDesc {
public:
  /// Constructor.
  /// name: name and 'friendly name' of the param
  /// help: help tooltip
  /// baseType: the data type of the parameter (int, float, string, array...)
  /// numChannels: number of channels (for array base types)
  /// typeHint: type interpretation hint (how should the value inside the
  /// parameter be interpreted). Useful for determining what UI to show.
  ParamDesc(const ParamName &name, util::StringRef help,
            util::Value::Type baseType, int numChannels, ParamHint paramHint,
            util::Value                     defaultValue,
            util::ArrayRef<ParamFloatRange> channelRanges);

  ParamName                       name;
  util::StringRef                 help;
  util::Value::Type               baseType;
  int                             numChannels;
  ParamHint                       paramHint;
  util::ArrayRef<ParamFloatRange> channelRanges;
};

class FloatParamDesc : public ParamDesc {
public:
  using ParamDesc::ParamDesc;

  double getValue(Node *node) const { return node->evalParam(*this).asReal(); }
};

class ColorParamDesc : public ParamDesc {
public:
  using ParamDesc::ParamDesc;

  gfx::ColorF getValue(Node *node) const {
    gfx::ColorF c;
    auto &&     value = node->evalParam(*this);
    auto &&     array = value.asRealArray();
    return gfx::ColorF{array[0], array[1], array[2], array[3]};
  }
};

static FloatParamDesc paramFloat(const ParamName &name, util::StringRef help,
                                 double v = 0.0) {
  return FloatParamDesc(name, help, util::Value::Type::Real, 1, ParamHint::None,
                        util::Value{v}, nullptr);
}

static FloatParamDesc paramFloatSlider(const ParamName &name,
                                       util::StringRef help, double v = 0.0) {
  return FloatParamDesc(name, help, util::Value::Type::Real, 1, ParamHint::None,
                        util::Value{v}, nullptr);
}

static ColorParamDesc paramColorRGBA(const ParamName &name,
                                     util::StringRef help, double r = 0.0,
                                     double g = 0.0, double b = 0.0,
                                     double a = 1.0) {
  double rgba[] = {r, g, b, a};
  return ColorParamDesc(name, help, util::Value::Type::Real, 1,
                        ParamHint::ColorRGBA,
                        util::Value{util::makeArrayRef(rgba)}, nullptr);
}

//=======================================================================================
class Param {
public:
  Param(Node *owner, const ParamDesc &desc) : owner_{owner}, desc_{desc} {}

  util::StringRef  name() const { return desc_.name.name; }
  util::StringRef  friendlyName() const { return desc_.name.friendlyName; }
  const ParamDesc &desc() const { return desc_; }
  util::Value &    value() { return value_; }

private:
  Node *           owner_;
  const ParamDesc &desc_;
  util::Value      value_;
};

} // namespace node