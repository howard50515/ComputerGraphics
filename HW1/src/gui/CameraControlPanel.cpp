#include "gui/CameraControlPanel.h"

#include "config/PropertyLimits.h"
#include "gui/Slider.h"

namespace hw1 {

std::string CameraControlPanel::renderHtml(std::size_t index, const Vector3& position, bool active) {
	const std::string idx = std::to_string(index);
	std::string html;
	html.reserve(700);
	html += "<div class='panel'><div class='panelTitleRow'><h3>Camera " + idx + "</h3><label class='toggleInline' for='cam_" + idx + "_active'>Active <input type='checkbox' id='cam_" + idx + "_active' class='cam-active' data-index='" + idx + "'";
	if (active) {
		html += " checked";
	}
	html += "></label></div>";
	Slider sx(
		"cam_" + idx + "_x",
		"Position X",
		PropertyLimits::CameraPosition.min,
		PropertyLimits::CameraPosition.max,
		PropertyLimits::CameraPosition.step,
		position.x);
	Slider sy(
		"cam_" + idx + "_y",
		"Position Y",
		PropertyLimits::CameraPosition.min,
		PropertyLimits::CameraPosition.max,
		PropertyLimits::CameraPosition.step,
		position.y);
	Slider sz(
		"cam_" + idx + "_z",
		"Position Z",
		PropertyLimits::CameraPosition.min,
		PropertyLimits::CameraPosition.max,
		PropertyLimits::CameraPosition.step,
		position.z);
	html += sx.renderHtml("class='cam-pos' data-index='" + idx + "' data-axis='x'");
	html += sy.renderHtml("class='cam-pos' data-index='" + idx + "' data-axis='y'");
	html += sz.renderHtml("class='cam-pos' data-index='" + idx + "' data-axis='z'");
	html += "</div>";
	return html;
}

}  // namespace hw1
