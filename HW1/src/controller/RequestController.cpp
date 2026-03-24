#include "controller/RequestController.h"

#include <charconv>
#include <string_view>

#include "config/LightingSettings.h"
#include "io/MeshLoader.h"

namespace hw1 {

namespace {

bool ParseFloat(const std::string& text, float& out) {
	try {
		out = std::stof(text);
		return true;
	} catch (...) {
		return false;
	}
}

bool ParseUInt32(const std::string& text, std::uint32_t& out) {
	const char* begin = text.data();
	const char* end = begin + text.size();
	const auto result = std::from_chars(begin, end, out);
	return result.ec == std::errc() && result.ptr == end;
}

std::string MakeJsonSuccess(std::uint32_t id, const std::string& prop, const std::string& value, const std::string& axis = "") {
	std::string json = "{\"ok\":true,\"id\":" + std::to_string(id) + ",\"prop\":\"" + prop + "\"";
	if (!axis.empty()) {
		json += ",\"axis\":\"" + axis + "\"";
	}
	json += ",\"value\":" + value + "}";
	return json;
}

std::string MakeJsonError(const std::string& error, const std::string& message) {
	return "{\"ok\":false,\"error\":\"" + error + "\",\"message\":\"" + message + "\"}";
}

ControllerResult BadRequest(const std::string& error, const std::string& message) {
	return ControllerResult{400, "application/json; charset=utf-8", MakeJsonError(error, message)};
}

ControllerResult NotFound(const std::string& error, const std::string& message) {
	return ControllerResult{404, "application/json; charset=utf-8", MakeJsonError(error, message)};
}

std::string ExtractRequestTarget(const std::string& requestLine) {
	const std::size_t firstSpace = requestLine.find(' ');
	if (firstSpace == std::string::npos) {
		return {};
	}
	const std::size_t secondSpace = requestLine.find(' ', firstSpace + 1);
	if (secondSpace == std::string::npos) {
		return {};
	}
	return requestLine.substr(firstSpace + 1, secondSpace - firstSpace - 1);
}

std::string QueryValue(const std::string& target, const std::string& key) {
	const std::size_t queryPos = target.find('?');
	if (queryPos == std::string::npos || queryPos + 1 >= target.size()) {
		return {};
	}

	const std::string prefix = key + "=";
	std::size_t pos = queryPos + 1;
	while (pos < target.size()) {
		std::size_t amp = target.find('&', pos);
		if (amp == std::string::npos) {
			amp = target.size();
		}
		const std::string_view kv(target.data() + pos, amp - pos);
		if (kv.rfind(prefix, 0) == 0) {
			return std::string(kv.substr(prefix.size()));
		}
		if (amp == target.size()) {
			break;
		}
		pos = amp + 1;
	}

	return {};
}

bool ParseBool(const std::string& text, bool& value) {
	if (text == "true" || text == "1") {
		value = true;
		return true;
	}
	if (text == "false" || text == "0") {
		value = false;
		return true;
	}
	return false;
}

bool ApplyAxisValue(Vector3& target, const std::string& axis, float value) {
	if (axis == "x") {
		target.x = value;
		return true;
	}
	if (axis == "y") {
		target.y = value;
		return true;
	}
	if (axis == "z") {
		target.z = value;
		return true;
	}
	return false;
}

bool ApplyColorChannel(Color& target, const std::string& channel, float value) {
	if (channel == "r") {
		target.r = value;
		return true;
	}
	if (channel == "g") {
		target.g = value;
		return true;
	}
	if (channel == "b") {
		target.b = value;
		return true;
	}
	return false;
}

bool ApplyShearAxis(SceneObject& target, const std::string& axis, float value) {
	if (axis == "xy") {
		target.setShearXY(value);
		return true;
	}
	if (axis == "xz") {
		target.setShearXZ(value);
		return true;
	}
	if (axis == "yx") {
		target.setShearYX(value);
		return true;
	}
	if (axis == "yz") {
		target.setShearYZ(value);
		return true;
	}
	if (axis == "zx") {
		target.setShearZX(value);
		return true;
	}
	if (axis == "zy") {
		target.setShearZY(value);
		return true;
	}
	return false;
}

}  // namespace

RequestController::RequestController() : scene_(nullptr) {}

void RequestController::bindScene(Scene* scene) {
	scene_ = scene;
}

void RequestController::bindShadingMaterials(
	const std::shared_ptr<Material>& flat,
	const std::shared_ptr<Material>& gouraud,
	const std::shared_ptr<Material>& phong,
	const std::shared_ptr<Material>& unlit) {
	flatMaterial_ = flat;
	gouraudMaterial_ = gouraud;
	phongMaterial_ = phong;
	unlitMaterial_ = unlit;
}

bool RequestController::handleSetShading(const std::string& requestLine, ControllerResult& out) const {
	const std::string target = ExtractRequestTarget(requestLine);
	if (target.rfind("/set_shading?", 0) != 0) {
		out = BadRequest("invalid_route", "route must be /set_shading");
		return false;
	}
	if (!scene_) {
		out = ControllerResult{503, "application/json; charset=utf-8", MakeJsonError("scene_not_bound", "scene is not bound")};
		return false;
	}

	const std::string mode = QueryValue(target, "mode");
	std::shared_ptr<Material> selected;
	if (mode == "flat") {
		selected = flatMaterial_;
	} else if (mode == "gouraud") {
		selected = gouraudMaterial_;
	} else if (mode == "phong") {
		selected = phongMaterial_;
	} else if (mode == "unlit") {
		selected = unlitMaterial_;
	} else {
		out = BadRequest("invalid_mode", "mode must be flat|gouraud|phong|unlit");
		return false;
	}

	if (!selected) {
		out = ControllerResult{503, "application/json; charset=utf-8", MakeJsonError("material_not_bound", "target material is not bound")};
		return false;
	}

	for (const auto& object : scene_->objects()) {
		if (object) {
			object->setMaterial(selected);
		}
	}
	out = ControllerResult{200, "application/json; charset=utf-8", std::string("{\"ok\":true,\"mode\":\"") + mode + "\"}"};
	return true;
}

bool RequestController::handleSetLightingSettings(const std::string& requestLine, ControllerResult& out) const {
	const std::string target = ExtractRequestTarget(requestLine);
	if (target.rfind("/set_lighting_settings?", 0) != 0) {
		out = BadRequest("invalid_route", "route must be /set_lighting_settings");
		return false;
	}

	const std::string prop = QueryValue(target, "prop");
	const std::string valueText = QueryValue(target, "value");
	if (prop.empty() || valueText.empty()) {
		out = BadRequest("missing_parameter", "prop and value are required");
		return false;
	}

	float value = 0.0f;
	if (!ParseFloat(valueText, value)) {
		out = BadRequest("invalid_value", "value must be numeric");
		return false;
	}

	LightingSettings& settings = GlobalLightingSettings();
	if (prop == "ambient") {
		settings.ambientStrength = value;
	} else if (prop == "specular") {
		settings.specularStrength = value;
	} else if (prop == "shininess") {
		if (value <= 0.0f) {
			out = BadRequest("invalid_value", "shininess must be > 0");
			return false;
		}
		settings.shininess = value;
	} else {
		out = BadRequest("invalid_property", "prop must be ambient|specular|shininess");
		return false;
	}

	out = ControllerResult{200, "application/json; charset=utf-8", std::string("{\"ok\":true,\"prop\":\"") + prop + "\",\"value\":" + std::to_string(value) + "}"};
	return true;
}

bool RequestController::handleSetCamera(const std::string& requestLine, ControllerResult& out) const {
	const std::string target = ExtractRequestTarget(requestLine);
	if (target.rfind("/set_camera?", 0) != 0) {
		out = BadRequest("invalid_route", "route must be /set_camera");
		return false;
	}
	if (!scene_) {
		out = ControllerResult{503, "application/json; charset=utf-8", MakeJsonError("scene_not_bound", "scene is not bound")};
		return false;
	}

	const std::string indexText = QueryValue(target, "index");
	if (indexText.empty()) {
		out = BadRequest("missing_parameter", "index is required");
		return false;
	}

	std::uint32_t index = 0;
	if (!ParseUInt32(indexText, index)) {
		out = BadRequest("invalid_index", "index must be uint32");
		return false;
	}
	if (index >= scene_->cameras().size()) {
		out = BadRequest("invalid_index", "camera index out of range");
		return false;
	}

	scene_->setActiveCameraIndex(index);
	out = ControllerResult{200, "application/json; charset=utf-8", std::string("{\"ok\":true,\"index\":") + std::to_string(index) + "}"};
	return true;
}

bool RequestController::handleSetCameraPosition(const std::string& requestLine, ControllerResult& out) const {
	const std::string target = ExtractRequestTarget(requestLine);
	if (target.rfind("/set_camera_position?", 0) != 0) {
		out = BadRequest("invalid_route", "route must be /set_camera_position");
		return false;
	}
	if (!scene_) {
		out = ControllerResult{503, "application/json; charset=utf-8", MakeJsonError("scene_not_bound", "scene is not bound")};
		return false;
	}

	const std::string indexText = QueryValue(target, "index");
	const std::string axis = QueryValue(target, "axis");
	const std::string valueText = QueryValue(target, "value");
	if (indexText.empty() || axis.empty() || valueText.empty()) {
		out = BadRequest("missing_parameter", "index, axis, value are required");
		return false;
	}

	std::uint32_t index = 0;
	if (!ParseUInt32(indexText, index)) {
		out = BadRequest("invalid_index", "index must be uint32");
		return false;
	}
	if (index >= scene_->cameras().size() || !scene_->cameras()[index]) {
		out = BadRequest("invalid_index", "camera index out of range");
		return false;
	}

	float value = 0.0f;
	if (!ParseFloat(valueText, value)) {
		out = BadRequest("invalid_value", "value must be numeric");
		return false;
	}
	Vector3 position = scene_->cameras()[index]->getPosition();
	if (!ApplyAxisValue(position, axis, value)) {
		out = BadRequest("invalid_axis", "axis must be x,y,z");
		return false;
	}
	scene_->cameras()[index]->setPosition(position);
	out = ControllerResult{200, "application/json; charset=utf-8", std::string("{\"ok\":true,\"index\":") + std::to_string(index) + ",\"axis\":\"" + axis + "\",\"value\":" + std::to_string(value) + "}"};
	return true;
}

bool RequestController::handleSetLight(const std::string& requestLine, ControllerResult& out) const {
	const std::string target = ExtractRequestTarget(requestLine);
	if (target.rfind("/set_light?", 0) != 0) {
		out = BadRequest("invalid_route", "route must be /set_light");
		return false;
	}
	if (!scene_) {
		out = ControllerResult{503, "application/json; charset=utf-8", MakeJsonError("scene_not_bound", "scene is not bound")};
		return false;
	}

	const std::string indexText = QueryValue(target, "index");
	const std::string prop = QueryValue(target, "prop");
	const std::string valueText = QueryValue(target, "value");
	const std::string axis = QueryValue(target, "axis");
	const std::string channel = QueryValue(target, "channel");
	if (indexText.empty() || prop.empty() || valueText.empty()) {
		out = BadRequest("missing_parameter", "index, prop, value are required");
		return false;
	}

	std::uint32_t index = 0;
	if (!ParseUInt32(indexText, index)) {
		out = BadRequest("invalid_index", "index must be uint32");
		return false;
	}
	if (index >= scene_->lights().size() || !scene_->lights()[index]) {
		out = BadRequest("invalid_index", "light index out of range");
		return false;
	}

	if (prop == "position") {
		if (axis.empty()) {
			out = BadRequest("missing_axis", "axis is required for position");
			return false;
		}
		float value = 0.0f;
		if (!ParseFloat(valueText, value)) {
			out = BadRequest("invalid_value", "value must be numeric");
			return false;
		}
		Vector3 position = scene_->lights()[index]->position;
		if (!ApplyAxisValue(position, axis, value)) {
			out = BadRequest("invalid_axis", "axis must be x,y,z");
			return false;
		}
		scene_->lights()[index]->position = position;
		out = ControllerResult{200, "application/json; charset=utf-8", std::string("{\"ok\":true,\"index\":") + std::to_string(index) + ",\"prop\":\"position\",\"axis\":\"" + axis + "\",\"value\":" + std::to_string(value) + "}"};
		return true;
	}

	if (prop == "color") {
		if (channel.empty()) {
			out = BadRequest("missing_channel", "channel is required for color");
			return false;
		}
		float value = 0.0f;
		if (!ParseFloat(valueText, value)) {
			out = BadRequest("invalid_value", "value must be numeric");
			return false;
		}
		Color color = scene_->lights()[index]->color;
		if (!ApplyColorChannel(color, channel, value)) {
			out = BadRequest("invalid_channel", "channel must be r,g,b");
			return false;
		}
		scene_->lights()[index]->color = color;
		out = ControllerResult{200, "application/json; charset=utf-8", std::string("{\"ok\":true,\"index\":") + std::to_string(index) + ",\"prop\":\"color\",\"channel\":\"" + channel + "\",\"value\":" + std::to_string(value) + "}"};
		return true;
	}

	if (prop == "intensity") {
		float value = 0.0f;
		if (!ParseFloat(valueText, value)) {
			out = BadRequest("invalid_value", "value must be numeric");
			return false;
		}
		scene_->lights()[index]->intensity = value;
		out = ControllerResult{200, "application/json; charset=utf-8", std::string("{\"ok\":true,\"index\":") + std::to_string(index) + ",\"prop\":\"intensity\",\"value\":" + std::to_string(value) + "}"};
		return true;
	}

	if (prop == "visibility") {
		bool visible = true;
		if (!ParseBool(valueText, visible)) {
			out = BadRequest("invalid_value", "value must be true|false|1|0");
			return false;
		}
		scene_->lights()[index]->visible = visible;
		out = ControllerResult{200, "application/json; charset=utf-8", std::string("{\"ok\":true,\"index\":") + std::to_string(index) + ",\"prop\":\"visibility\",\"value\":" + (visible ? "true" : "false") + "}"};
		return true;
	}

	out = BadRequest("invalid_property", "prop must be position|color|intensity|visibility");
	return false;
}

bool RequestController::handleSetObjectProperty(const std::string& requestLine, ControllerResult& out) const {
	const std::string target = ExtractRequestTarget(requestLine);
	if (target.rfind("/set_object_property?", 0) != 0) {
		out = BadRequest("invalid_route", "route must be /set_object_property");
		return false;
	}
	if (!scene_) {
		out = ControllerResult{503, "application/json; charset=utf-8", MakeJsonError("scene_not_bound", "scene is not bound")};
		return false;
	}

	const std::string idText = QueryValue(target, "id");
	const std::string prop = QueryValue(target, "prop");
	const std::string valueText = QueryValue(target, "value");
	const std::string axis = QueryValue(target, "axis");

	if (idText.empty() || prop.empty() || valueText.empty()) {
		out = BadRequest("missing_parameter", "id, prop, value are required");
		return false;
	}

	std::uint32_t id = 0;
	if (!ParseUInt32(idText, id)) {
		out = BadRequest("invalid_id", "id must be uint32");
		return false;
	}

	auto object = scene_->getObjectById(id);
	if (!object) {
		out = NotFound("object_not_found", "no object with this id");
		return false;
	}

	if (prop == "scale") {
		float scale = 1.0f;
		if (!ParseFloat(valueText, scale)) {
			out = BadRequest("invalid_value", "scale must be numeric");
			return false;
		}
		object->setScale(Vector3(scale, scale, scale));
		out = ControllerResult{200, "application/json; charset=utf-8", MakeJsonSuccess(id, "scale", std::to_string(scale))};
		return true;
	}

	if (prop == "translation") {
		if (axis.empty()) {
			out = BadRequest("missing_axis", "axis is required for translation");
			return false;
		}
		float value = 0.0f;
		if (!ParseFloat(valueText, value)) {
			out = BadRequest("invalid_value", "translation value must be numeric");
			return false;
		}
		Vector3 translation = object->translation();
		if (!ApplyAxisValue(translation, axis, value)) {
			out = BadRequest("invalid_axis", "axis must be x,y,z");
			return false;
		}
		object->setTranslation(translation);
		out = ControllerResult{200, "application/json; charset=utf-8", MakeJsonSuccess(id, "translation", std::to_string(value), axis)};
		return true;
	}

	if (prop == "rotation") {
		if (axis.empty()) {
			out = BadRequest("missing_axis", "axis is required for rotation");
			return false;
		}
		float value = 0.0f;
		if (!ParseFloat(valueText, value)) {
			out = BadRequest("invalid_value", "rotation value must be numeric");
			return false;
		}
		Vector3 rotation = object->rotation();
		if (!ApplyAxisValue(rotation, axis, value)) {
			out = BadRequest("invalid_axis", "axis must be x,y,z");
			return false;
		}
		object->setRotation(rotation);
		out = ControllerResult{200, "application/json; charset=utf-8", MakeJsonSuccess(id, "rotation", std::to_string(value), axis)};
		return true;
	}

	if (prop == "shear") {
		if (axis.empty()) {
			out = BadRequest("missing_axis", "axis is required for shear");
			return false;
		}
		float value = 0.0f;
		if (!ParseFloat(valueText, value)) {
			out = BadRequest("invalid_value", "shear value must be numeric");
			return false;
		}
		if (!ApplyShearAxis(*object, axis, value)) {
			out = BadRequest("invalid_axis", "axis must be xy,xz,yx,yz,zx,zy");
			return false;
		}
		out = ControllerResult{200, "application/json; charset=utf-8", MakeJsonSuccess(id, "shear", std::to_string(value), axis)};
		return true;
	}

	if (prop == "material") {
		if (valueText == "flat" && flatMaterial_) {
			object->setMaterial(flatMaterial_);
		} else if (valueText == "gouraud" && gouraudMaterial_) {
			object->setMaterial(gouraudMaterial_);
		} else if (valueText == "phong" && phongMaterial_) {
			object->setMaterial(phongMaterial_);
		} else if (valueText == "unlit" && unlitMaterial_) {
			object->setMaterial(unlitMaterial_);
		} else {
			out = BadRequest("invalid_material", "material must be flat|gouraud|phong|unlit");
			return false;
		}
		out = ControllerResult{200, "application/json; charset=utf-8", MakeJsonSuccess(id, "material", "\"" + valueText + "\"")};
		return true;
	}

	if (prop == "model") {
		if (valueText.empty() || valueText.find('/') != std::string::npos || valueText.find('\\') != std::string::npos) {
			out = BadRequest("invalid_model", "model must be a file name in examples folder");
			return false;
		}
		if (valueText.size() < 5 || (valueText.substr(valueText.size() - 5) != ".json" && valueText.substr(valueText.size() - 5) != ".JSON")) {
			out = BadRequest("invalid_model", "model must be a .json file");
			return false;
		}

		try {
			const Mesh loaded = MeshLoader::loadFromFile(std::string("examples/") + valueText);
			object->mesh() = loaded;
			object->setModelFile(valueText);
		} catch (const std::exception& ex) {
			out = BadRequest("model_load_failed", ex.what());
			return false;
		}

		out = ControllerResult{200, "application/json; charset=utf-8", MakeJsonSuccess(id, "model", "\"" + valueText + "\"")};
		return true;
	}

	if (prop == "visibility") {
		bool visible = true;
		if (!ParseBool(valueText, visible)) {
			out = BadRequest("invalid_value", "visibility must be true|false|1|0");
			return false;
		}
		object->setVisible(visible);
		out = ControllerResult{200, "application/json; charset=utf-8", MakeJsonSuccess(id, "visibility", visible ? "true" : "false")};
		return true;
	}

	out = BadRequest("invalid_property", "prop must be scale|translation|rotation|shear|material|model|visibility");
	return false;
}

}  // namespace hw1
