#include "gui/GUIWindow.h"

#include <charconv>

namespace hw1 {

namespace {

bool ParseInt(const std::string& text, int& out) {
	const char* begin = text.data();
	const char* end = begin + text.size();
	const auto result = std::from_chars(begin, end, out);
	return result.ec == std::errc() && result.ptr == end;
}

bool ParseFloat(const std::string& text, float& out) {
	try {
		out = std::stof(text);
		return true;
	} catch (...) {
		return false;
	}
}

float ClampFloat(float value, float minValue, float maxValue) {
	if (value < minValue) {
		return minValue;
	}
	if (value > maxValue) {
		return maxValue;
	}
	return value;
}

}  // namespace

GUIWindow::GUIWindow()
	: shadingMenu_(
		"shading",
		"Shading:",
		{
			{"flat", "Flat"},
			{"gouraud", "Gouraud"},
			{"phong", "Phong"},
			{"unlit", "Unlit"},
		},
		"gouraud"),
	  cameraMenu_(
		"camera",
		"Camera:",
		{
			{"0", "Object Camera"},
			{"1", "Side Camera"},
		},
		"0"),
	  shadingMode_(static_cast<int>(ShadingMode::Gouraud)),
	  activeCameraIndex_(0),
	  objectScale_(1.0f),
	  scene_(nullptr),
	  lastAppliedShadingMode_(ShadingMode::Gouraud),
	  lastAppliedCameraIndex_(static_cast<std::size_t>(-1)),
	  lastAppliedObjectScale_(-1.0f) {}

void GUIWindow::bindScene(Scene* scene) {
	scene_ = scene;
	if (!scene_) {
		return;
	}

	std::lock_guard<std::mutex> lock(pendingMutex_);
	pendingCameraPositions_.assign(scene_->cameras().size(), PendingVector3Update{false, Vector3()});
	pendingLightPositions_.assign(scene_->lights().size(), PendingVector3Update{false, Vector3()});
	pendingLightColors_.assign(scene_->lights().size(), PendingColorUpdate{false, Color()});
}

void GUIWindow::bindPrimaryObject(const std::shared_ptr<SceneObject>& object) {
	primaryObject_ = object;
}

void GUIWindow::bindShadingMaterials(
	const std::shared_ptr<Material>& flat,
	const std::shared_ptr<Material>& gouraud,
	const std::shared_ptr<Material>& phong,
	const std::shared_ptr<Material>& unlit) {
	flatMaterial_ = flat;
	gouraudMaterial_ = gouraud;
	phongMaterial_ = phong;
	unlitMaterial_ = unlit;
}

void GUIWindow::applyRuntimeBindings() {
	const ShadingMode mode = shadingMode();
	if (primaryObject_ && mode != lastAppliedShadingMode_) {
		switch (mode) {
		case ShadingMode::Flat:
			if (flatMaterial_) {
				primaryObject_->setMaterial(flatMaterial_);
			}
			break;
		case ShadingMode::Gouraud:
			if (gouraudMaterial_) {
				primaryObject_->setMaterial(gouraudMaterial_);
			}
			break;
		case ShadingMode::Phong:
			if (phongMaterial_) {
				primaryObject_->setMaterial(phongMaterial_);
			}
			break;
		case ShadingMode::Unlit:
			if (unlitMaterial_) {
				primaryObject_->setMaterial(unlitMaterial_);
			}
			break;
		}
		lastAppliedShadingMode_ = mode;
	}

	if (scene_) {
		const std::size_t requestedCameraIndex = activeCameraIndex();
		if (requestedCameraIndex != lastAppliedCameraIndex_ && requestedCameraIndex < scene_->cameras().size()) {
			scene_->setActiveCameraIndex(requestedCameraIndex);
			lastAppliedCameraIndex_ = requestedCameraIndex;
		}
	}

	if (primaryObject_) {
		const float scaleValue = objectScale();
		if (scaleValue != lastAppliedObjectScale_) {
			primaryObject_->setScale(Vector3(scaleValue, scaleValue, scaleValue));
			lastAppliedObjectScale_ = scaleValue;
		}
	}

	if (!scene_) {
		return;
	}

	std::lock_guard<std::mutex> lock(pendingMutex_);
	for (std::size_t i = 0; i < pendingCameraPositions_.size() && i < scene_->cameras().size(); ++i) {
		if (!pendingCameraPositions_[i].dirty || !scene_->cameras()[i]) {
			continue;
		}
		scene_->cameras()[i]->setPosition(pendingCameraPositions_[i].value);
		pendingCameraPositions_[i].dirty = false;
	}

	for (std::size_t i = 0; i < pendingLightPositions_.size() && i < scene_->lights().size(); ++i) {
		if (!scene_->lights()[i]) {
			continue;
		}
		if (pendingLightPositions_[i].dirty) {
			scene_->lights()[i]->position = pendingLightPositions_[i].value;
			pendingLightPositions_[i].dirty = false;
		}
		if (pendingLightColors_[i].dirty) {
			scene_->lights()[i]->color = pendingLightColors_[i].value;
			pendingLightColors_[i].dirty = false;
		}
	}
}

void GUIWindow::setShadingMode(ShadingMode mode) {
	shadingMode_.store(static_cast<int>(mode));
}

GUIWindow::ShadingMode GUIWindow::shadingMode() const {
	const int value = shadingMode_.load();
	switch (value) {
	case static_cast<int>(ShadingMode::Flat):
		return ShadingMode::Flat;
	case static_cast<int>(ShadingMode::Gouraud):
		return ShadingMode::Gouraud;
	case static_cast<int>(ShadingMode::Phong):
		return ShadingMode::Phong;
	case static_cast<int>(ShadingMode::Unlit):
		return ShadingMode::Unlit;
	default:
		return ShadingMode::Gouraud;
	}
}

void GUIWindow::setActiveCameraIndex(std::size_t index) {
	activeCameraIndex_.store(static_cast<int>(index));
}

std::size_t GUIWindow::activeCameraIndex() const {
	const int index = activeCameraIndex_.load();
	return (index >= 0) ? static_cast<std::size_t>(index) : 0u;
}

void GUIWindow::setObjectScale(float uniformScale) {
	objectScale_.store(uniformScale);
}

float GUIWindow::objectScale() const {
	return objectScale_.load();
}

const char* GUIWindow::modeToString(ShadingMode mode) {
	switch (mode) {
	case ShadingMode::Flat:
		return "flat";
	case ShadingMode::Gouraud:
		return "gouraud";
	case ShadingMode::Phong:
		return "phong";
	case ShadingMode::Unlit:
		return "unlit";
	}
	return "gouraud";
}

bool GUIWindow::tryParseMode(const std::string& text, ShadingMode& outMode) {
	if (text == "flat") {
		outMode = ShadingMode::Flat;
		return true;
	}
	if (text == "gouraud") {
		outMode = ShadingMode::Gouraud;
		return true;
	}
	if (text == "phong") {
		outMode = ShadingMode::Phong;
		return true;
	}
	if (text == "unlit") {
		outMode = ShadingMode::Unlit;
		return true;
	}
	return false;
}

std::string GUIWindow::extractQueryValue(const std::string& requestLine, const std::string& prefix) {
	if (requestLine.rfind(prefix, 0) != 0) {
		return {};
	}

	const std::size_t begin = prefix.size();
	std::size_t end = requestLine.find_first_of(" &", begin);
	if (end == std::string::npos) {
		end = requestLine.size();
	}
	return requestLine.substr(begin, end - begin);
}

std::string GUIWindow::extractModeValue(const std::string& requestLine) {
	return extractQueryValue(requestLine, "GET /set_shading?mode=");
}

std::string GUIWindow::shadingModeJson() const {
	const char* modeText = modeToString(shadingMode());
	return std::string("{\"mode\":\"") + modeText + "\"}";
}

std::string GUIWindow::cameraJson() const {
	return std::string("{\"index\":") + std::to_string(activeCameraIndex()) + "}";
}

std::string GUIWindow::objectJson() const {
	return std::string("{\"scale\":") + std::to_string(objectScale()) + "}";
}

std::string GUIWindow::lightsJson() const {
	return std::string("{\"count\":") + std::to_string(scene_ ? scene_->lights().size() : 0u) + "}";
}

bool GUIWindow::trySetShadingModeFromRequest(const std::string& requestLine, std::string& outJsonPayload) {
	const std::string modeText = extractModeValue(requestLine);
	ShadingMode parsedMode = shadingMode();
	if (!tryParseMode(modeText, parsedMode)) {
		return false;
	}

	setShadingMode(parsedMode);
	outJsonPayload = shadingModeJson();
	return true;
}

bool GUIWindow::trySetCameraFromRequest(const std::string& requestLine, std::string& outJsonPayload) {
	const std::string value = extractQueryValue(requestLine, "GET /set_camera?index=");
	if (value.empty()) {
		return false;
	}

	int parsed = 0;
	if (!ParseInt(value, parsed) || parsed < 0) {
		return false;
	}

	setActiveCameraIndex(static_cast<std::size_t>(parsed));
	outJsonPayload = cameraJson();
	return true;
}

bool GUIWindow::trySetObjectScaleFromRequest(const std::string& requestLine, std::string& outJsonPayload) {
	const std::string value = extractQueryValue(requestLine, "GET /set_object_scale?value=");
	if (value.empty()) {
		return false;
	}

	float parsed = 1.0f;
	if (!ParseFloat(value, parsed)) {
		return false;
	}
	setObjectScale(ClampFloat(parsed, 0.1f, 3.0f));
	outJsonPayload = objectJson();
	return true;
}

bool GUIWindow::trySetCameraPositionFromRequest(const std::string& requestLine, std::string& outJsonPayload) {
	const std::string indexText = extractQueryValue(requestLine, "GET /set_camera_position?index=");
	if (indexText.empty()) {
		return false;
	}

	int index = 0;
	if (!ParseInt(indexText, index) || index < 0) {
		return false;
	}

	const std::string axis = extractQueryValue(requestLine, "GET /set_camera_position?index=" + indexText + "&axis=");
	const std::string valueText = extractQueryValue(requestLine, "GET /set_camera_position?index=" + indexText + "&axis=" + axis + "&value=");
	if (axis.empty() || valueText.empty()) {
		return false;
	}

	float value = 0.0f;
	if (!ParseFloat(valueText, value)) {
		return false;
	}
	value = ClampFloat(value, -100.0f, 100.0f);

	std::lock_guard<std::mutex> lock(pendingMutex_);
	if (static_cast<std::size_t>(index) >= pendingCameraPositions_.size()) {
		return false;
	}

	Vector3 current = pendingCameraPositions_[static_cast<std::size_t>(index)].value;
	if (scene_ && static_cast<std::size_t>(index) < scene_->cameras().size() && scene_->cameras()[static_cast<std::size_t>(index)]) {
		current = scene_->cameras()[static_cast<std::size_t>(index)]->getPosition();
	}

	if (axis == "x") {
		current.x = value;
	} else if (axis == "y") {
		current.y = value;
	} else if (axis == "z") {
		current.z = value;
	} else {
		return false;
	}

	auto& pending = pendingCameraPositions_[static_cast<std::size_t>(index)];
	pending.value = current;
	pending.dirty = true;
	outJsonPayload = "{\"ok\":true}";
	return true;
}

bool GUIWindow::trySetLightPositionFromRequest(const std::string& requestLine, std::string& outJsonPayload) {
	const std::string indexText = extractQueryValue(requestLine, "GET /set_light_position?index=");
	if (indexText.empty()) {
		return false;
	}

	int index = 0;
	if (!ParseInt(indexText, index) || index < 0) {
		return false;
	}

	const std::string axis = extractQueryValue(requestLine, "GET /set_light_position?index=" + indexText + "&axis=");
	const std::string valueText = extractQueryValue(requestLine, "GET /set_light_position?index=" + indexText + "&axis=" + axis + "&value=");
	if (axis.empty() || valueText.empty()) {
		return false;
	}

	float value = 0.0f;
	if (!ParseFloat(valueText, value)) {
		return false;
	}
	value = ClampFloat(value, -100.0f, 100.0f);

	std::lock_guard<std::mutex> lock(pendingMutex_);
	if (static_cast<std::size_t>(index) >= pendingLightPositions_.size()) {
		return false;
	}

	Vector3 current = pendingLightPositions_[static_cast<std::size_t>(index)].value;
	if (scene_ && static_cast<std::size_t>(index) < scene_->lights().size() && scene_->lights()[static_cast<std::size_t>(index)]) {
		current = scene_->lights()[static_cast<std::size_t>(index)]->position;
	}

	if (axis == "x") {
		current.x = value;
	} else if (axis == "y") {
		current.y = value;
	} else if (axis == "z") {
		current.z = value;
	} else {
		return false;
	}

	auto& pending = pendingLightPositions_[static_cast<std::size_t>(index)];
	pending.value = current;
	pending.dirty = true;
	outJsonPayload = "{\"ok\":true}";
	return true;
}

bool GUIWindow::trySetLightColorFromRequest(const std::string& requestLine, std::string& outJsonPayload) {
	const std::string indexText = extractQueryValue(requestLine, "GET /set_light_color?index=");
	if (indexText.empty()) {
		return false;
	}

	int index = 0;
	if (!ParseInt(indexText, index) || index < 0) {
		return false;
	}

	const std::string channel = extractQueryValue(requestLine, "GET /set_light_color?index=" + indexText + "&channel=");
	const std::string valueText = extractQueryValue(requestLine, "GET /set_light_color?index=" + indexText + "&channel=" + channel + "&value=");
	if (channel.empty() || valueText.empty()) {
		return false;
	}

	float value = 0.0f;
	if (!ParseFloat(valueText, value)) {
		return false;
	}
	value = ClampFloat(value, 0.0f, 1.0f);

	std::lock_guard<std::mutex> lock(pendingMutex_);
	if (static_cast<std::size_t>(index) >= pendingLightColors_.size()) {
		return false;
	}

	Color current = pendingLightColors_[static_cast<std::size_t>(index)].value;
	if (scene_ && static_cast<std::size_t>(index) < scene_->lights().size() && scene_->lights()[static_cast<std::size_t>(index)]) {
		current = scene_->lights()[static_cast<std::size_t>(index)]->color;
	}

	if (channel == "r") {
		current.r = value;
	} else if (channel == "g") {
		current.g = value;
	} else if (channel == "b") {
		current.b = value;
	} else {
		return false;
	}

	auto& pending = pendingLightColors_[static_cast<std::size_t>(index)];
	pending.value = current;
	pending.dirty = true;
	outJsonPayload = "{\"ok\":true}";
	return true;
}

std::vector<std::uint8_t> GUIWindow::htmlPage() const {
	const std::string toolbarShading = shadingMenu_.renderHtml(modeToString(shadingMode()));
	const std::string toolbarCamera = cameraMenu_.renderHtml(std::to_string(activeCameraIndex()));
	std::string cameraSliders;
	std::string lightSliders;

	if (scene_) {
		for (std::size_t i = 0; i < scene_->cameras().size(); ++i) {
			if (!scene_->cameras()[i]) {
				continue;
			}
			const Vector3 p = scene_->cameras()[i]->getPosition();
			Slider sx("cam_" + std::to_string(i) + "_x", "Cam " + std::to_string(i) + " X", -100.0f, 100.0f, 0.1f, p.x);
			Slider sy("cam_" + std::to_string(i) + "_y", "Cam " + std::to_string(i) + " Y", -100.0f, 100.0f, 0.1f, p.y);
			Slider sz("cam_" + std::to_string(i) + "_z", "Cam " + std::to_string(i) + " Z", -100.0f, 100.0f, 0.1f, p.z);
			cameraSliders += sx.renderHtml("class='cam-pos' data-index='" + std::to_string(i) + "' data-axis='x'");
			cameraSliders += sy.renderHtml("class='cam-pos' data-index='" + std::to_string(i) + "' data-axis='y'");
			cameraSliders += sz.renderHtml("class='cam-pos' data-index='" + std::to_string(i) + "' data-axis='z'");
		}

		for (std::size_t i = 0; i < scene_->lights().size(); ++i) {
			if (!scene_->lights()[i]) {
				continue;
			}
			const Vector3 p = scene_->lights()[i]->position;
			const Color c = scene_->lights()[i]->color;
			Slider lpx("light_" + std::to_string(i) + "_x", "Light " + std::to_string(i) + " X", -100.0f, 100.0f, 0.1f, p.x);
			Slider lpy("light_" + std::to_string(i) + "_y", "Light " + std::to_string(i) + " Y", -100.0f, 100.0f, 0.1f, p.y);
			Slider lpz("light_" + std::to_string(i) + "_z", "Light " + std::to_string(i) + " Z", -100.0f, 100.0f, 0.1f, p.z);
			Slider lcr("light_" + std::to_string(i) + "_r", "Light " + std::to_string(i) + " R", 0.0f, 1.0f, 0.01f, c.r);
			Slider lcg("light_" + std::to_string(i) + "_g", "Light " + std::to_string(i) + " G", 0.0f, 1.0f, 0.01f, c.g);
			Slider lcb("light_" + std::to_string(i) + "_b", "Light " + std::to_string(i) + " B", 0.0f, 1.0f, 0.01f, c.b);
			lightSliders += lpx.renderHtml("class='light-pos' data-index='" + std::to_string(i) + "' data-axis='x'");
			lightSliders += lpy.renderHtml("class='light-pos' data-index='" + std::to_string(i) + "' data-axis='y'");
			lightSliders += lpz.renderHtml("class='light-pos' data-index='" + std::to_string(i) + "' data-axis='z'");
			lightSliders += lcr.renderHtml("class='light-color' data-index='" + std::to_string(i) + "' data-channel='r'");
			lightSliders += lcg.renderHtml("class='light-color' data-index='" + std::to_string(i) + "' data-channel='g'");
			lightSliders += lcb.renderHtml("class='light-color' data-index='" + std::to_string(i) + "' data-channel='b'");
		}
	}

	std::string html;
	html.reserve(12000);
	html += "<!doctype html><html><head><meta charset='utf-8'><title>HW1 Framebuffer</title>";
	html += "<style>body{margin:0;background:#111;color:#eee;font-family:Segoe UI,Arial,sans-serif;}";
	html += "#wrap{display:flex;flex-direction:column;align-items:center;gap:10px;padding:12px;}";
	html += "#toolbar{display:flex;align-items:center;gap:10px;flex-wrap:wrap;}";
	html += "#controls{width:min(95vw,1100px);display:grid;grid-template-columns:1fr 1fr;gap:12px;}";
	html += ".panel{border:1px solid #333;background:#171717;padding:10px;border-radius:8px;}";
	html += ".sliderRow{display:grid;grid-template-columns:120px 1fr 64px;gap:8px;align-items:center;margin:4px 0;}";
	html += "#fps{position:fixed;top:10px;left:10px;background:rgba(0,0,0,.65);padding:6px 10px;border:1px solid #333;border-radius:6px;}";
	html += "#perf{position:fixed;top:48px;left:10px;background:rgba(0,0,0,.65);padding:6px 10px;border:1px solid #333;border-radius:6px;white-space:pre-line;}";
	html += "canvas{max-width:95vw;max-height:70vh;border:1px solid #333;image-rendering:pixelated;}";
	html += "select,input[type=range]{padding:6px 8px;border:1px solid #444;background:#222;color:#eee;}";
	html += "button{padding:8px 12px;border:1px solid #444;background:#222;color:#eee;cursor:pointer;}button:hover{background:#333;}";
	html += "</style></head><body>";
	html += "<div id='fps'>FPS: --</div><div id='perf'>geometry: -- ms\\nraster: -- ms\\nrender: -- ms\\nupload: -- ms\\nencode: -- ms</div>";
	html += "<div id='wrap'><div>Live Framebuffer</div><div id='toolbar'>";
	html += toolbarShading;
	html += toolbarCamera;
	html += "<label for='scale'>Scale:</label><input id='scale' type='range' min='0.1' max='3.0' step='0.1' value='";
	html += std::to_string(objectScale());
	html += "'><span id='scaleValue'>";
	html += std::to_string(objectScale());
	html += "</span></div>";
	html += "<canvas id='fb' width='1' height='1'></canvas>";
	html += "<div id='controls'><div class='panel'><h3>Cameras</h3>";
	html += cameraSliders;
	html += "</div><div class='panel'><h3>Lights</h3>";
	html += lightSliders;
	html += "</div></div><button id='stop'>Terminate Renderer</button></div>";
	html += "<script>const canvas=document.getElementById('fb');const ctx2d=canvas.getContext('2d');let imgData=null;";
	html += "const fpsText=document.getElementById('fps');const perfText=document.getElementById('perf');";
	html += "const shadingSelect=document.getElementById('shading');const cameraSelect=document.getElementById('camera');";
	html += "const scaleSlider=document.getElementById('scale');const scaleValue=document.getElementById('scaleValue');let running=true;";
	html += "function bindSliderValue(el){const v=document.getElementById(el.id+'_value');if(v){v.textContent=Number(el.value).toFixed(2);}}";
	html += "async function tick(){if(!running)return;try{const r=await fetch('/frame.rgba?t='+performance.now());if(r.ok){const buf=await r.arrayBuffer();if(buf.byteLength>=8){const v=new DataView(buf);const w=v.getUint32(0,true);const h=v.getUint32(4,true);const bytes=new Uint8ClampedArray(buf,8);if(canvas.width!==w||canvas.height!==h||!imgData){canvas.width=w;canvas.height=h;imgData=ctx2d.createImageData(w,h);}imgData.data.set(bytes);ctx2d.putImageData(imgData,0,0);}}}catch(e){}setTimeout(tick,33);}tick();";
	html += "async function pollFps(){if(!running)return;try{const r=await fetch('/fps?t='+performance.now());if(r.ok){const d=await r.json();fpsText.textContent='FPS: '+Number(d.fps).toFixed(2);}}catch(e){}setTimeout(pollFps,500);}pollFps();";
	html += "async function pollPerf(){if(!running)return;try{const r=await fetch('/perf?t='+performance.now());if(r.ok){const d=await r.json();perfText.textContent='geometry: '+Number(d.geometryMs).toFixed(3)+' ms\\nraster: '+Number(d.rasterMs).toFixed(3)+' ms\\nrender: '+Number(d.renderMs).toFixed(3)+' ms\\nupload: '+Number(d.uploadMs).toFixed(3)+' ms\\nencode: '+Number(d.encodeMs).toFixed(3)+' ms';}}catch(e){}setTimeout(pollPerf,500);}pollPerf();";
	html += "shadingSelect.onchange=async()=>{try{await fetch('/set_shading?mode='+encodeURIComponent(shadingSelect.value));}catch(e){}};";
	html += "cameraSelect.onchange=async()=>{try{await fetch('/set_camera?index='+encodeURIComponent(cameraSelect.value));}catch(e){}};";
	html += "scaleSlider.oninput=()=>{scaleValue.textContent=Number(scaleSlider.value).toFixed(1);};";
	html += "scaleSlider.onchange=async()=>{try{await fetch('/set_object_scale?value='+encodeURIComponent(scaleSlider.value));}catch(e){}};";
	html += "document.querySelectorAll('.cam-pos').forEach((el)=>{bindSliderValue(el);el.oninput=()=>bindSliderValue(el);el.onchange=async()=>{try{await fetch('/set_camera_position?index='+encodeURIComponent(el.dataset.index)+'&axis='+encodeURIComponent(el.dataset.axis)+'&value='+encodeURIComponent(el.value));}catch(e){}};});";
	html += "document.querySelectorAll('.light-pos').forEach((el)=>{bindSliderValue(el);el.oninput=()=>bindSliderValue(el);el.onchange=async()=>{try{await fetch('/set_light_position?index='+encodeURIComponent(el.dataset.index)+'&axis='+encodeURIComponent(el.dataset.axis)+'&value='+encodeURIComponent(el.value));}catch(e){}};});";
	html += "document.querySelectorAll('.light-color').forEach((el)=>{bindSliderValue(el);el.oninput=()=>bindSliderValue(el);el.onchange=async()=>{try{await fetch('/set_light_color?index='+encodeURIComponent(el.dataset.index)+'&channel='+encodeURIComponent(el.dataset.channel)+'&value='+encodeURIComponent(el.value));}catch(e){}};});";
	html += "document.getElementById('stop').onclick=async()=>{running=false;try{await fetch('/shutdown');}catch(e){};document.body.insertAdjacentHTML('beforeend','<div style=\\\"padding:8px\\\">Shutdown requested. You can close this tab.</div>');};";
	html += "</script></body></html>";

	return std::vector<std::uint8_t>(html.begin(), html.end());
}

}  // namespace hw1
