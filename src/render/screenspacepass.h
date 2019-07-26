#pragma once
#include <render/node.h>
#include <util/arrayref.h>
#include <util/stringref.h>
#include <string>
#include <vector>
#include <memory>
#include <util/value.h>

namespace render {
	/*
class ImagePassParameter {
public:
};

class Parameter {
public:
protected:
	std::string name_;
};

class Node {
public:
private:
	Node* parent_;
	std::vector<std::shared_ptr<Parameter>> params_;
};


// float, int, vec2,3,4, matrix constant parameter
class ConstantParameter : public Node {
public:
	ConstantParameter(std::string name, float v = 0.0f);

	enum class Type {
		Float
	};

	float value() const;
	void setValue(float val);

private:
	std::string name_;
	float val_;
};

class ScreenSpacePass : public Node {
public:
  std::weak_ptr<ConstantParameter> addParameter(util::StringRef name, float v);
  

  void removeParameter(util::StringRef name);
  util::StringRef source() const;
  void setSource(util::StringRef source);

private:
	std::string source_;
	std::vector<std::shared_ptr<ConstantParameter>> parameters_;
};

// don't store pointer to nodes since they can be deleted and it's hard to track which node is using them
// instead, use weak pointers*/

} // namespace render