#include "gui/Menu.h"

#include <utility>

namespace hw1 {

Menu::Menu(std::string id, std::string label, std::vector<Option> options, std::string selectedValue)
	: id_(std::move(id)),
	  label_(std::move(label)),
	  options_(std::move(options)) {
	(void)selectedValue;
}

std::string Menu::renderHtml(const std::string& selectedValue) const {
	std::string html;
	html.reserve(256);
	html += "<label for='";
	html += id_;
	html += "'>";
	html += label_;
	html += "</label><select id='";
	html += id_;
	html += "'>";

	for (const auto& option : options_) {
		html += "<option value='";
		html += option.value;
		html += "'";
		if (option.value == selectedValue) {
			html += " selected";
		}
		html += ">";
		html += option.label;
		html += "</option>";
	}

	html += "</select>";
	return html;
}

}  // namespace hw1
