#include "gui/LightSettingsPanel.h"

#include "config/PropertyLimits.h"
#include "gui/Slider.h"

namespace hw1 {

std::string LightSettingsPanel::renderHtml(const LightingSettings& settings) {
	std::string html;
	html.reserve(1200);

	Slider ambient(
		"global_ambient",
		"Ambient",
		PropertyLimits::GlobalAmbient.min,
		PropertyLimits::GlobalAmbient.max,
		PropertyLimits::GlobalAmbient.step,
		settings.ambientStrength);
	Slider specular(
		"global_specular",
		"Specular",
		PropertyLimits::GlobalSpecular.min,
		PropertyLimits::GlobalSpecular.max,
		PropertyLimits::GlobalSpecular.step,
		settings.specularStrength);
	Slider shininess(
		"global_shininess",
		"Shininess",
		PropertyLimits::GlobalShininess.min,
		PropertyLimits::GlobalShininess.max,
		PropertyLimits::GlobalShininess.step,
		settings.shininess);

	html += ambient.renderHtml("class='global-lighting' data-prop='ambient'");
	html += specular.renderHtml("class='global-lighting' data-prop='specular'");
	html += shininess.renderHtml("class='global-lighting' data-prop='shininess'");

	return html;
}

}  // namespace hw1
