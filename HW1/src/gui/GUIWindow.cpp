#include "gui/GUIWindow.h"

#include <algorithm>
#include <filesystem>

#include "config/LightingSettings.h"
#include "gui/CameraControlPanel.h"
#include "gui/LightControlPanel.h"
#include "gui/LightSettingsPanel.h"
#include "gui/ObjectControlPanel.h"

namespace hw1 {

namespace {

std::vector<std::string> ListExampleModels() {
    std::vector<std::string> models;
    const std::filesystem::path examplesDir("examples");

    std::error_code ec;
    if (!std::filesystem::exists(examplesDir, ec) || ec) {
        return models;
    }

    for (const auto& entry : std::filesystem::directory_iterator(examplesDir, ec)) {
        if (ec || !entry.is_regular_file()) {
            continue;
        }
        const std::string ext = entry.path().extension().string();
        if (ext == ".json" || ext == ".JSON") {
            models.push_back(entry.path().filename().string());
        }
    }

    std::sort(models.begin(), models.end());
    return models;
}

}  // namespace

GUIWindow::GUIWindow()
    : activeCameraIndex_(0),
      scene_(nullptr),
      lastAppliedCameraIndex_(static_cast<std::size_t>(-1)) {}

void GUIWindow::bindScene(Scene* scene) {
    scene_ = scene;
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
    if (!scene_) {
        return;
    }

    const std::size_t requestedCameraIndex = activeCameraIndex();
    if (requestedCameraIndex != lastAppliedCameraIndex_ && requestedCameraIndex < scene_->cameras().size()) {
        scene_->setActiveCameraIndex(requestedCameraIndex);
        lastAppliedCameraIndex_ = requestedCameraIndex;
    }
}

void GUIWindow::setActiveCameraIndex(std::size_t index) {
    activeCameraIndex_.store(static_cast<int>(index));
}

std::size_t GUIWindow::activeCameraIndex() const {
    const int index = activeCameraIndex_.load();
    return (index >= 0) ? static_cast<std::size_t>(index) : 0u;
}

std::string GUIWindow::shadingModeJson() const {
    std::string mode = "gouraud";
    if (scene_) {
        for (const auto& object : scene_->objects()) {
            if (!object) {
                continue;
            }
            mode = selectedMaterialName(object);
            break;
        }
    }
    return std::string("{\"mode\":\"") + mode + "\"}";
}

std::string GUIWindow::cameraJson() const {
    const std::size_t index = scene_ ? scene_->activeCameraIndex() : activeCameraIndex();
    return std::string("{\"index\":") + std::to_string(index) + "}";
}

std::string GUIWindow::objectJson() const {
    return std::string("{\"count\":") + std::to_string(scene_ ? scene_->objectCount() : 0u) + "}";
}

std::string GUIWindow::lightsJson() const {
    return std::string("{\"count\":") + std::to_string(scene_ ? scene_->lights().size() : 0u) + "}";
}

std::string GUIWindow::selectedMaterialName(const std::shared_ptr<SceneObject>& object) const {
    if (!object || !object->material()) {
        return "gouraud";
    }
    if (flatMaterial_ && object->material() == flatMaterial_) {
        return "flat";
    }
    if (gouraudMaterial_ && object->material() == gouraudMaterial_) {
        return "gouraud";
    }
    if (phongMaterial_ && object->material() == phongMaterial_) {
        return "phong";
    }
    if (unlitMaterial_ && object->material() == unlitMaterial_) {
        return "unlit";
    }
    return "gouraud";
}

std::vector<std::uint8_t> GUIWindow::htmlPage() const {
    const std::vector<std::string> modelOptions = ListExampleModels();
    const LightingSettings& lighting = GlobalLightingSettingsConst();

    const std::size_t active = scene_ ? scene_->activeCameraIndex() : activeCameraIndex();

    std::string objectPanels;
    std::string cameraPanels;
    std::string lightPanels;
    std::string lightSettingsPanel;

    if (scene_) {
        for (std::size_t i = 0; i < scene_->cameras().size(); ++i) {
            if (!scene_->cameras()[i]) {
                continue;
            }
            cameraPanels += CameraControlPanel::renderHtml(i, scene_->cameras()[i]->getPosition(), i == active);
        }

        for (std::size_t i = 0; i < scene_->lights().size(); ++i) {
            if (!scene_->lights()[i]) {
                continue;
            }
            lightPanels += LightControlPanel::renderHtml(i, scene_->lights()[i]->position, scene_->lights()[i]->color, scene_->lights()[i]->intensity, scene_->lights()[i]->visible);
        }

        for (const auto& object : scene_->objects()) {
            if (!object) {
                continue;
            }
            objectPanels += ObjectControlPanel::renderHtml(*object, selectedMaterialName(object), modelOptions, object->modelFile());
        }
    }

    lightSettingsPanel = LightSettingsPanel::renderHtml(lighting);

    std::string html;
    html.reserve(22000);
    html += "<!doctype html><html><head><meta charset='utf-8'><title>HW1</title>";
    html += "<style>body{margin:0;background:#111;color:#eee;font-family:Segoe UI,Arial,sans-serif;}";
    html += "#wrap{display:flex;flex-direction:column;gap:10px;padding:12px;min-height:100vh;box-sizing:border-box;}";
    html += "#main{display:grid;grid-template-columns:auto 1fr;gap:12px;width:100%;align-items:start;min-height:0;box-sizing:border-box;}";
    html += "#viewer{position:sticky;top:200px;left:10px;display:inline-block;border:1px solid #333;background:#0f0f0f;border-radius:8px;padding:8px;justify-self:start;align-self: start;}";
    html += "#side{display:flex;flex-direction:column;gap:12px;border:1px solid #333;background:#101010;border-radius:8px;padding:10px;}";
    html += "#controls{display:grid;grid-template-columns:1fr;gap:12px;}";
    html += "#objectPanels,#cameraPanels,#lightPanels{display:flex;flex-wrap:wrap;gap:10px;justify-content:flex-start;}";
    html += "#objectPanels>.panel:first-child,#cameraPanels>.panel:first-child,#lightPanels>.panel:first-child{flex-basis:100%}";
    html += ".panel{border:1px solid #333;background:#171717;padding:8px;border-radius:8px;}";
    html += ".panelTitleRow{display:flex;align-items:center;justify-content:space-between;gap:8px;}";
    html += ".panelTitleRow h3{margin:0;}";
    html += ".toggleInline{display:inline-flex;align-items:center;gap:6px;font-size:12px;white-space:nowrap;}";
    html += ".sliderRow{display:grid;grid-template-columns:86px 1fr 50px;gap:6px;align-items:center;margin:3px 0;}";
    html += ".sliderRow>label{font-size:12px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}";
    html += ".sliderRow .sliderValue{font-size:12px;}";
    html += ".rowInline{display:flex;justify-content:space-between;align-items:center;gap:8px;padding:4px 0;}";
    html += ".subSection{margin-top:6px;border:1px solid #2a2a2a;border-radius:6px;padding:4px 6px;background:#131313;}";
    html += ".subSection>summary{cursor:pointer;font-size:12px;color:#ddd;list-style:none;}";
    html += ".subSection>summary::-webkit-details-marker{display:none;}";
    html += ".subSection[open]>summary{margin-bottom:4px;}";
    html += ".lightSettingsGrid{display:flex;flex-wrap:wrap;gap:8px;}";
    html += ".lightSettingsGrid .sliderRow{flex:1 1 300px;}";
    html += "#fps{position:fixed;top:10px;left:10px;background:rgba(0,0,0,.65);padding:6px 10px;border:1px solid #333;border-radius:6px;}";
    html += "#perf{position:fixed;top:48px;left:10px;background:rgba(0,0,0,.65);padding:6px 10px;border:1px solid #333;border-radius:6px;white-space:pre-line;}";
    html += "canvas{display: block;border:1px solid #333;image-rendering:pixelated;}";
    html += "select,input[type=range]{padding:6px 8px;border:1px solid #444;background:#222;color:#eee;}";
    html += "button{padding:8px 12px;border:1px solid #444;background:#222;color:#eee;cursor:pointer;}button:hover{background:#333;}";
    html += "@media (max-width: 1100px){#main{grid-template-columns:1fr;}#viewer{position:static;}canvas{max-height:60vh;}#objectPanels>.panel,#cameraPanels>.panel,#lightPanels>.panel{flex-basis:100%;}}";
    html += "</style></head><body>";
    html += "<div id='fps'>FPS: --</div><div id='perf'>geometry: -- ms\\nraster: -- ms\\nrender: -- ms\\nupload: -- ms\\nencode: -- ms</div>";
    html += "<div id='wrap'><div id='main'><div id='viewer'><canvas id='fb' width='1' height='1'></canvas></div><div id='side'><div id='controls'><div class='panel'><h3>Light Settings</h3><div class='lightSettingsGrid'>";
    html += lightSettingsPanel;
    html += "</div></div><div id='objectPanels'><div class='panel'><h3>Objects</h3><div>Total: ";
    html += std::to_string(scene_ ? scene_->objectCount() : 0u);
    html += "</div></div>";
    html += objectPanels;
    html += "</div><div id='cameraPanels'><div class='panel'><h3>Cameras</h3></div>";
    html += cameraPanels;
    html += "</div><div id='lightPanels'><div class='panel'><h3>Lights</h3></div>";
    html += lightPanels;
    html += "</div></div></div></div><button id='stop'>Terminate Renderer</button></div>";
    html += "<script>const canvas=document.getElementById('fb');const ctx2d=canvas.getContext('2d');let imgData=null;";
    html += "const fpsText=document.getElementById('fps');const perfText=document.getElementById('perf');";
    html += "let running=true;";
    html += "function bindSliderValue(el){const v=document.getElementById(el.id+'_value');if(v){v.textContent=Number(el.value).toFixed(2);}}";
    html += "function apiGet(url){fetch(url).catch(()=>{});}";
    html += "function apiSetObject(id,prop,value,axis){let url='/set_object_property?id='+encodeURIComponent(id)+'&prop='+encodeURIComponent(prop)+'&value='+encodeURIComponent(value);if(axis){url+='&axis='+encodeURIComponent(axis);}apiGet(url);}";
    html += "async function tick(){if(!running)return;try{const r=await fetch('/frame.rgba?t='+performance.now());if(r.ok){const buf=await r.arrayBuffer();if(buf.byteLength>=8){const v=new DataView(buf);const w=v.getUint32(0,true);const h=v.getUint32(4,true);const bytes=new Uint8ClampedArray(buf,8);if(canvas.width!==w||canvas.height!==h||!imgData){canvas.width=w;canvas.height=h;imgData=ctx2d.createImageData(w,h);}imgData.data.set(bytes);ctx2d.putImageData(imgData,0,0);}}}catch(e){}setTimeout(tick,33);}tick();";
    html += "async function pollFps(){if(!running)return;try{const r=await fetch('/fps?t='+performance.now());if(r.ok){const d=await r.json();fpsText.textContent='FPS: '+Number(d.fps).toFixed(2);}}catch(e){}setTimeout(pollFps,500);}pollFps();";
    html += "async function pollPerf(){if(!running)return;try{const r=await fetch('/perf?t='+performance.now());if(r.ok){const d=await r.json();perfText.textContent='geometry: '+Number(d.geometryMs).toFixed(3)+' ms\\nraster: '+Number(d.rasterMs).toFixed(3)+' ms\\nrender: '+Number(d.renderMs).toFixed(3)+' ms\\nupload: '+Number(d.uploadMs).toFixed(3)+' ms\\nencode: '+Number(d.encodeMs).toFixed(3)+' ms';}}catch(e){}setTimeout(pollPerf,500);}pollPerf();";
    html += "document.querySelectorAll('.cam-active').forEach((el)=>{el.onchange=()=>{if(!el.checked){el.checked=true;return;}document.querySelectorAll('.cam-active').forEach((other)=>{if(other!==el){other.checked=false;}});apiGet('/set_camera?index='+encodeURIComponent(el.dataset.index));};});";
    html += "document.querySelectorAll('.global-lighting').forEach((el)=>{bindSliderValue(el);const update=()=>{bindSliderValue(el);apiGet('/set_lighting_settings?prop='+encodeURIComponent(el.dataset.prop)+'&value='+encodeURIComponent(el.value));};el.addEventListener('input',update);el.addEventListener('change',update);});";
    html += "document.querySelectorAll('.obj-prop').forEach((el)=>{bindSliderValue(el);const update=()=>{bindSliderValue(el);apiSetObject(el.dataset.objectId,el.dataset.prop,el.value,el.dataset.axis);};el.addEventListener('input',update);el.addEventListener('change',update);});";
    html += "document.querySelectorAll('.obj-material').forEach((el)=>{el.onchange=()=>apiSetObject(el.dataset.objectId,el.dataset.prop,el.value,'');});";
    html += "document.querySelectorAll('.obj-model').forEach((el)=>{el.onchange=()=>apiSetObject(el.dataset.objectId,el.dataset.prop,el.value,'');});";
    html += "document.querySelectorAll('.obj-visibility').forEach((el)=>{el.onchange=()=>apiSetObject(el.dataset.objectId,el.dataset.prop,el.checked?'true':'false','');});";
    html += "document.querySelectorAll('.cam-pos').forEach((el)=>{bindSliderValue(el);const update=()=>{bindSliderValue(el);apiGet('/set_camera_position?index='+encodeURIComponent(el.dataset.index)+'&axis='+encodeURIComponent(el.dataset.axis)+'&value='+encodeURIComponent(el.value));};el.addEventListener('input',update);el.addEventListener('change',update);});";
    html += "document.querySelectorAll('.light-pos').forEach((el)=>{bindSliderValue(el);const update=()=>{bindSliderValue(el);apiGet('/set_light?index='+encodeURIComponent(el.dataset.index)+'&prop=position&axis='+encodeURIComponent(el.dataset.axis)+'&value='+encodeURIComponent(el.value));};el.addEventListener('input',update);el.addEventListener('change',update);});";
    html += "document.querySelectorAll('.light-color').forEach((el)=>{bindSliderValue(el);const update=()=>{bindSliderValue(el);apiGet('/set_light?index='+encodeURIComponent(el.dataset.index)+'&prop=color&channel='+encodeURIComponent(el.dataset.channel)+'&value='+encodeURIComponent(el.value));};el.addEventListener('input',update);el.addEventListener('change',update);});";
    html += "document.querySelectorAll('.light-intensity').forEach((el)=>{bindSliderValue(el);const update=()=>{bindSliderValue(el);apiGet('/set_light?index='+encodeURIComponent(el.dataset.index)+'&prop=intensity&value='+encodeURIComponent(el.value));};el.addEventListener('input',update);el.addEventListener('change',update);});";
    html += "document.querySelectorAll('.light-visibility').forEach((el)=>{el.onchange=()=>apiGet('/set_light?index='+encodeURIComponent(el.dataset.index)+'&prop=visibility&value='+(el.checked?'true':'false'));});";
    html += "document.getElementById('stop').onclick=async()=>{running=false;try{await fetch('/shutdown');}catch(e){};document.body.insertAdjacentHTML('beforeend','<div style=\\\"padding:8px\\\">Shutdown requested. You can close this tab.</div>');};";
    html += "</script></body></html>";

    return std::vector<std::uint8_t>(html.begin(), html.end());
}

}  // namespace hw1
