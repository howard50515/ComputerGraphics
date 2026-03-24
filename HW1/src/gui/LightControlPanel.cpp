#include "gui/LightControlPanel.h"

#include "config/PropertyLimits.h"
#include "gui/Slider.h"

namespace hw1 {

std::string LightControlPanel::renderHtml(std::size_t index, const Vector3& position, const Color& color, float intensity, bool visible) {
	const std::string idx = std::to_string(index);
	std::string html;
	html.reserve(1400);
	html += "<div class='panel'><div class='panelTitleRow'><h3>Light " + idx + "</h3><label class='toggleInline' for='light_" + idx + "_visible'>Visible <input type='checkbox' id='light_" + idx + "_visible' class='light-visibility' data-index='" + idx + "'";
	if (visible) {
		html += " checked";
	}
	html += "></label></div>";
	Slider lpx(
		"light_" + idx + "_x",
		"Position X",
		PropertyLimits::LightPosition.min,
		PropertyLimits::LightPosition.max,
		PropertyLimits::LightPosition.step,
		position.x);
	Slider lpy(
		"light_" + idx + "_y",
		"Position Y",
		PropertyLimits::LightPosition.min,
		PropertyLimits::LightPosition.max,
		PropertyLimits::LightPosition.step,
		position.y);
	Slider lpz(
		"light_" + idx + "_z",
		"Position Z",
		PropertyLimits::LightPosition.min,
		PropertyLimits::LightPosition.max,
		PropertyLimits::LightPosition.step,
		position.z);
	Slider lcr(
		"light_" + idx + "_r",
		"Color R",
		PropertyLimits::LightColor.min,
		PropertyLimits::LightColor.max,
		PropertyLimits::LightColor.step,
		color.r);
	Slider lcg(
		"light_" + idx + "_g",
		"Color G",
		PropertyLimits::LightColor.min,
		PropertyLimits::LightColor.max,
		PropertyLimits::LightColor.step,
		color.g);
	Slider lcb(
		"light_" + idx + "_b",
		"Color B",
		PropertyLimits::LightColor.min,
		PropertyLimits::LightColor.max,
		PropertyLimits::LightColor.step,
		color.b);
	Slider lint(
		"light_" + idx + "_intensity",
		"Intensity",
		PropertyLimits::LightIntensity.min,
		PropertyLimits::LightIntensity.max,
		PropertyLimits::LightIntensity.step,
		intensity);
	html += lpx.renderHtml("class='light-pos' data-index='" + idx + "' data-axis='x'");
	html += lpy.renderHtml("class='light-pos' data-index='" + idx + "' data-axis='y'");
	html += lpz.renderHtml("class='light-pos' data-index='" + idx + "' data-axis='z'");
	html += lcr.renderHtml("class='light-color' data-index='" + idx + "' data-channel='r'");
	html += lcg.renderHtml("class='light-color' data-index='" + idx + "' data-channel='g'");
	html += lcb.renderHtml("class='light-color' data-index='" + idx + "' data-channel='b'");
	html += lint.renderHtml("class='light-intensity' data-index='" + idx + "'");
	html += "</div>";
	return html;
}

}  // namespace hw1
