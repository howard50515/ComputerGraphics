#pragma once

#include <string>

namespace hw1 {

class Slider {
public:
	Slider(std::string id, std::string label, float minValue, float maxValue, float stepValue, float value);

	std::string renderHtml(const std::string& extraInputAttributes = "") const;

private:
	std::string id_;
	std::string label_;
	float minValue_;
	float maxValue_;
	float stepValue_;
	float value_;
};

}  // namespace hw1

