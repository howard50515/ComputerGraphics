#include "gui/ObjectControlPanel.h"

#include "config/PropertyLimits.h"
#include "gui/Slider.h"

namespace hw1 {

std::string ObjectControlPanel::renderHtml(
	const SceneObject& object,
	const std::string& selectedMaterial,
	const std::vector<std::string>& modelOptions,
	const std::string& selectedModel) {
	const std::string idText = std::to_string(object.objectId());
	const Vector3 scale = object.scale();
	const Vector3 translation = object.translation();
	const Vector3 rotation = object.rotation();

	std::string html;
	html.reserve(2400);
	html += "<div class='panel'><div class='panelTitleRow'><h3>Object #" + idText + "</h3><label class='toggleInline' for='obj_" + idText + "_visible'>Visible <input type='checkbox' id='obj_" + idText + "_visible' class='obj-visibility' data-object-id='" + idText + "' data-prop='visibility'";
	if (object.visible()) {
		html += " checked";
	}
	html += "></label></div>";

	Slider s(
		"obj_" + idText + "_scale",
		"Scale",
		PropertyLimits::ObjectScale.min,
		PropertyLimits::ObjectScale.max,
		PropertyLimits::ObjectScale.step,
		scale.x);
	html += s.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='scale'");

	Slider tx(
		"obj_" + idText + "_tx",
		"Translate X",
		PropertyLimits::ObjectTranslation.min,
		PropertyLimits::ObjectTranslation.max,
		PropertyLimits::ObjectTranslation.step,
		translation.x);
	Slider ty(
		"obj_" + idText + "_ty",
		"Translate Y",
		PropertyLimits::ObjectTranslation.min,
		PropertyLimits::ObjectTranslation.max,
		PropertyLimits::ObjectTranslation.step,
		translation.y);
	Slider tz(
		"obj_" + idText + "_tz",
		"Translate Z",
		PropertyLimits::ObjectTranslation.min,
		PropertyLimits::ObjectTranslation.max,
		PropertyLimits::ObjectTranslation.step,
		translation.z);
	html += tx.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='translation' data-axis='x'");
	html += ty.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='translation' data-axis='y'");
	html += tz.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='translation' data-axis='z'");

	Slider rx(
		"obj_" + idText + "_rx",
		"Rotate X",
		PropertyLimits::ObjectRotation.min,
		PropertyLimits::ObjectRotation.max,
		PropertyLimits::ObjectRotation.step,
		rotation.x);
	Slider ry(
		"obj_" + idText + "_ry",
		"Rotate Y",
		PropertyLimits::ObjectRotation.min,
		PropertyLimits::ObjectRotation.max,
		PropertyLimits::ObjectRotation.step,
		rotation.y);
	Slider rz(
		"obj_" + idText + "_rz",
		"Rotate Z",
		PropertyLimits::ObjectRotation.min,
		PropertyLimits::ObjectRotation.max,
		PropertyLimits::ObjectRotation.step,
		rotation.z);
	html += rx.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='rotation' data-axis='x'");
	html += ry.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='rotation' data-axis='y'");
	html += rz.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='rotation' data-axis='z'");

	html += "<details class='subSection'><summary>Shear</summary>";
	Slider sxy(
		"obj_" + idText + "_sxy",
		"X + cY",
		PropertyLimits::ObjectShear.min,
		PropertyLimits::ObjectShear.max,
		PropertyLimits::ObjectShear.step,
		object.shearXY());
	Slider sxz(
		"obj_" + idText + "_sxz",
		"X + cZ",
		PropertyLimits::ObjectShear.min,
		PropertyLimits::ObjectShear.max,
		PropertyLimits::ObjectShear.step,
		object.shearXZ());
	Slider syx(
		"obj_" + idText + "_syx",
		"Y + cX",
		PropertyLimits::ObjectShear.min,
		PropertyLimits::ObjectShear.max,
		PropertyLimits::ObjectShear.step,
		object.shearYX());
	Slider syz(
		"obj_" + idText + "_syz",
		"Y + cZ",
		PropertyLimits::ObjectShear.min,
		PropertyLimits::ObjectShear.max,
		PropertyLimits::ObjectShear.step,
		object.shearYZ());
	Slider szx(
		"obj_" + idText + "_szx",
		"Z + cX",
		PropertyLimits::ObjectShear.min,
		PropertyLimits::ObjectShear.max,
		PropertyLimits::ObjectShear.step,
		object.shearZX());
	Slider szy(
		"obj_" + idText + "_szy",
		"Z + cY",
		PropertyLimits::ObjectShear.min,
		PropertyLimits::ObjectShear.max,
		PropertyLimits::ObjectShear.step,
		object.shearZY());
	html += sxy.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='shear' data-axis='xy'");
	html += sxz.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='shear' data-axis='xz'");
	html += syx.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='shear' data-axis='yx'");
	html += syz.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='shear' data-axis='yz'");
	html += szx.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='shear' data-axis='zx'");
	html += szy.renderHtml("class='obj-prop' data-object-id='" + idText + "' data-prop='shear' data-axis='zy'");
	html += "</details>";

	html += "<div class='rowInline'><label for='obj_" + idText + "_material'>Material</label><select id='obj_" + idText + "_material' class='obj-material' data-object-id='" + idText + "' data-prop='material'>";
	html += "<option value='flat'" + std::string(selectedMaterial == "flat" ? " selected" : "") + ">Flat</option>";
	html += "<option value='gouraud'" + std::string(selectedMaterial == "gouraud" ? " selected" : "") + ">Gouraud</option>";
	html += "<option value='phong'" + std::string(selectedMaterial == "phong" ? " selected" : "") + ">Phong</option>";
	html += "<option value='unlit'" + std::string(selectedMaterial == "unlit" ? " selected" : "") + ">Unlit</option>";
	html += "</select></div>";

	html += "<div class='rowInline'><label for='obj_" + idText + "_model'>Model</label><select id='obj_" + idText + "_model' class='obj-model' data-object-id='" + idText + "' data-prop='model'>";
	for (const std::string& option : modelOptions) {
		html += "<option value='" + option + "'" + std::string(selectedModel == option ? " selected" : "") + ">" + option + "</option>";
	}
	html += "</select></div>";

	html += "</div>";

	return html;
}

}  // namespace hw1
