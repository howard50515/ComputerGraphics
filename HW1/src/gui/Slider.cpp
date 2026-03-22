#include "gui/Slider.h"

#include <utility>

namespace hw1 {

Slider::Slider(std::string id, std::string label, float minValue, float maxValue, float stepValue, float value)
	: id_(std::move(id)),
	  label_(std::move(label)),
	  minValue_(minValue),
	  maxValue_(maxValue),
	  stepValue_(stepValue),
	  value_(value) {}

std::string Slider::renderHtml(const std::string& extraInputAttributes) const {
	std::string html;
	html.reserve(256);
	html += "<div class='sliderRow'><label for='";
	html += id_;
	html += "'>";
	html += label_;
	html += "</label><input type='range' id='";
	html += id_;
	html += "' min='";
	html += std::to_string(minValue_);
	html += "' max='";
	html += std::to_string(maxValue_);
	html += "' step='";
	html += std::to_string(stepValue_);
	html += "' value='";
	html += std::to_string(value_);
	html += "'";
	if (!extraInputAttributes.empty()) {
		html += ' ';
		html += extraInputAttributes;
	}
	html += "><span class='sliderValue' id='";
	html += id_;
	html += "_value'>";
	html += std::to_string(value_);
	html += "</span></div>";
	return html;
}

}  // namespace hw1

