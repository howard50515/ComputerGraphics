#pragma once

#include <string>
#include <vector>

namespace hw1 {

class Menu {
public:
	struct Option {
		std::string value;
		std::string label;
	};

	Menu(std::string id, std::string label, std::vector<Option> options, std::string selectedValue);

	std::string renderHtml(const std::string& selectedValue) const;

private:
	std::string id_;
	std::string label_;
	std::vector<Option> options_;
};

}  // namespace hw1
