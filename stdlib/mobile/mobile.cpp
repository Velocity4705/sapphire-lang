#include "mobile.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace Sapphire {
namespace Mobile {

static uint64_t now_ms() {
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
}

static std::string json_str(const std::string& s) {
    std::string r = "\"";
    for (char c : s) {
        if (c == '"') r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else r += c;
    }
    return r + "\"";
}

// ===== UIStyle =====

UIStyle::UIStyle()
    : background_color("#FFFFFF"), text_color("#000000"),
      font_family("System"), font_size(16),
      padding(8), margin(4), border_radius(4),
      border_color("#CCCCCC"), border_width(0),
      opacity(1.0), width(0), height(0) {}

// ===== UIComponent =====

UIComponent::UIComponent()
    : type(ComponentType::VIEW), visible(true), enabled(true),
      checked(false), value(0.0), min_value(0.0), max_value(100.0) {}

std::string UIComponent::type_name() const {
    switch (type) {
        case ComponentType::VIEW:           return "View";
        case ComponentType::BUTTON:         return "Button";
        case ComponentType::TEXT:           return "Text";
        case ComponentType::TEXT_INPUT:     return "TextInput";
        case ComponentType::IMAGE:          return "Image";
        case ComponentType::LIST:           return "List";
        case ComponentType::SCROLL_VIEW:    return "ScrollView";
        case ComponentType::MODAL:          return "Modal";
        case ComponentType::NAVIGATION_BAR: return "NavigationBar";
        case ComponentType::TAB_BAR:        return "TabBar";
        case ComponentType::SWITCH:         return "Switch";
        case ComponentType::SLIDER:         return "Slider";
        case ComponentType::PROGRESS_BAR:   return "ProgressBar";
        case ComponentType::ICON:           return "Icon";
    }
    return "Unknown";
}

std::string UIComponent::to_json() const {
    std::ostringstream ss;
    ss << "{\"id\":" << json_str(id)
       << ",\"type\":" << json_str(type_name())
       << ",\"text\":" << json_str(text)
       << ",\"visible\":" << (visible ? "true" : "false")
       << ",\"enabled\":" << (enabled ? "true" : "false")
       << ",\"value\":" << value
       << ",\"style\":{\"bg\":" << json_str(style.background_color)
       << ",\"fg\":" << json_str(style.text_color)
       << ",\"font_size\":" << style.font_size
       << ",\"padding\":" << style.padding
       << ",\"width\":" << style.width
       << ",\"height\":" << style.height << "}"
       << ",\"children\":[";
    for (size_t i = 0; i < children.size(); i++) {
        if (i) ss << ",";
        ss << children[i].to_json();
    }
    ss << "]}";
    return ss.str();
}

// ===== UILayout =====

UILayout::UILayout()
    : type(LayoutType::STACK_VERTICAL),
      main_axis(Alignment::START), cross_axis(Alignment::STRETCH),
      spacing(8), columns(2), wrap(false) {}

std::string UILayout::to_json() const {
    std::string tname;
    switch (type) {
        case LayoutType::STACK_VERTICAL:   tname = "VStack"; break;
        case LayoutType::STACK_HORIZONTAL: tname = "HStack"; break;
        case LayoutType::GRID:             tname = "Grid"; break;
        case LayoutType::FLEX:             tname = "Flex"; break;
    }
    std::ostringstream ss;
    ss << "{\"type\":" << json_str(tname)
       << ",\"spacing\":" << spacing
       << ",\"columns\":" << columns
       << ",\"items\":[";
    for (size_t i = 0; i < items.size(); i++) {
        if (i) ss << ",";
        ss << items[i].to_json();
    }
    ss << "]}";
    return ss.str();
}

std::string UILayout::render() const { return to_json(); }

// ===== Screen =====

std::string Screen::to_json() const {
    std::ostringstream ss;
    ss << "{\"name\":" << json_str(name)
       << ",\"title\":" << json_str(title)
       << ",\"background\":" << json_str(background_color)
       << ",\"nav_bar\":" << (has_nav_bar ? "true" : "false")
       << ",\"tab_bar\":" << (has_tab_bar ? "true" : "false")
       << ",\"layout\":" << layout.to_json() << "}";
    return ss.str();
}

// ===== NavigationStack =====

void NavigationStack::push(const std::string& screen) { stack.push_back(screen); }
void NavigationStack::pop() { if (!stack.empty()) stack.pop_back(); }
std::string NavigationStack::current() const {
    return stack.empty() ? "" : stack.back();
}

// ===== MobileApp =====

MobileApp::MobileApp(const AppConfig& config) : config_(config) {}

void MobileApp::add_screen(const Screen& screen) { screens_.push_back(screen); }

Screen* MobileApp::get_screen(const std::string& name) {
    for (auto& s : screens_) if (s.name == name) return &s;
    return nullptr;
}

std::vector<std::string> MobileApp::screen_names() const {
    std::vector<std::string> names;
    for (auto& s : screens_) names.push_back(s.name);
    return names;
}

std::string MobileApp::platform_name() const {
    switch (config_.platform) {
        case Platform::IOS:             return "iOS";
        case Platform::ANDROID:         return "Android";
        case Platform::CROSS_PLATFORM:  return "Cross-Platform";
    }
    return "Unknown";
}

std::string MobileApp::build_ios() const {
    std::ostringstream ss;
    ss << "=== iOS Build Report ===\n"
       << "App: " << config_.name << " v" << config_.version << "\n"
       << "Bundle ID: " << config_.bundle_id << "\n"
       << "Build: " << config_.build_number << "\n"
       << "Screens: " << screens_.size() << "\n"
       << "Target: iOS 16.0+\n"
       << "Architecture: ARM64\n"
       << "SDK: iOS SDK 17.0\n"
       << "Status: Build successful (simulated)\n"
       << "Output: " << config_.name << ".ipa\n"
       << "Size: " << (1024 + screens_.size() * 128) << " KB\n";
    return ss.str();
}

std::string MobileApp::build_android() const {
    std::ostringstream ss;
    ss << "=== Android Build Report ===\n"
       << "App: " << config_.name << " v" << config_.version << "\n"
       << "Package: " << config_.bundle_id << "\n"
       << "Version Code: " << config_.build_number << "\n"
       << "Screens: " << screens_.size() << "\n"
       << "Target SDK: 34 (Android 14)\n"
       << "Min SDK: 24 (Android 7.0)\n"
       << "Architectures: arm64-v8a, armeabi-v7a, x86_64\n"
       << "Status: Build successful (simulated)\n"
       << "Output: " << config_.name << ".apk\n"
       << "Size: " << (896 + screens_.size() * 96) << " KB\n";
    return ss.str();
}

std::string MobileApp::export_manifest() const {
    std::ostringstream ss;
    ss << "{\"app\":{\"name\":" << json_str(config_.name)
       << ",\"bundle_id\":" << json_str(config_.bundle_id)
       << ",\"version\":" << json_str(config_.version)
       << ",\"platform\":" << json_str(platform_name())
       << ",\"screens\":[";
    auto names = screen_names();
    for (size_t i = 0; i < names.size(); i++) {
        if (i) ss << ",";
        ss << json_str(names[i]);
    }
    ss << "],\"permissions\":[";
    bool first = true;
    for (auto& [k, v] : config_.permissions) {
        if (!first) ss << ",";
        ss << json_str(k);
        first = false;
    }
    ss << "]}}";
    return ss.str();
}

// ===== Location =====

std::string Location::to_string() const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(6)
       << "{\"lat\":" << latitude << ",\"lng\":" << longitude
       << ",\"alt\":" << altitude << ",\"acc\":" << accuracy << "}";
    return ss.str();
}

std::string AccelerometerData::to_string() const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(4)
       << "{\"x\":" << x << ",\"y\":" << y << ",\"z\":" << z << "}";
    return ss.str();
}

std::string Notification::to_string() const {
    std::ostringstream ss;
    ss << "{\"id\":" << json_str(id)
       << ",\"title\":" << json_str(title)
       << ",\"body\":" << json_str(body)
       << ",\"badge\":" << badge << "}";
    return ss.str();
}

// ===== DeviceAPI =====

DeviceAPI::DeviceAPI(Platform platform)
    : platform_(platform), sim_counter_(0) {
    permissions_["camera"]   = PermissionStatus::NOT_DETERMINED;
    permissions_["location"] = PermissionStatus::NOT_DETERMINED;
    permissions_["storage"]  = PermissionStatus::GRANTED;
    permissions_["notifications"] = PermissionStatus::NOT_DETERMINED;
}

std::string DeviceAPI::capture_photo(const std::string& quality) {
    sim_counter_++;
    return "photo_" + std::to_string(sim_counter_) + "_" + quality + ".jpg";
}

std::string DeviceAPI::record_video(int duration_sec) {
    sim_counter_++;
    return "video_" + std::to_string(sim_counter_) + "_" + std::to_string(duration_sec) + "s.mp4";
}

Location DeviceAPI::get_location() {
    // Simulated GPS — deterministic based on counter
    Location loc;
    loc.latitude  = 37.7749 + (sim_counter_ % 100) * 0.001;
    loc.longitude = -122.4194 + (sim_counter_ % 100) * 0.001;
    loc.altitude  = 10.0 + sim_counter_ % 50;
    loc.accuracy  = 5.0;
    sim_counter_++;
    return loc;
}

bool DeviceAPI::request_location_permission() {
    permissions_["location"] = PermissionStatus::GRANTED;
    return true;
}

AccelerometerData DeviceAPI::get_accelerometer() {
    AccelerometerData d;
    double t = sim_counter_ * 0.1;
    d.x = std::sin(t) * 0.5;
    d.y = std::cos(t) * 0.3;
    d.z = 9.81 + std::sin(t * 2) * 0.1;
    sim_counter_++;
    return d;
}

bool DeviceAPI::storage_set(const std::string& key, const std::string& value) {
    storage_[key] = value;
    return true;
}

std::string DeviceAPI::storage_get(const std::string& key) const {
    auto it = storage_.find(key);
    return it != storage_.end() ? it->second : "";
}

bool DeviceAPI::storage_delete(const std::string& key) {
    return storage_.erase(key) > 0;
}

std::vector<std::string> DeviceAPI::storage_keys() const {
    std::vector<std::string> keys;
    for (auto& [k, v] : storage_) keys.push_back(k);
    return keys;
}

std::string DeviceAPI::network_type() const {
    return (sim_counter_ % 3 == 0) ? "cellular" : "wifi";
}

std::string DeviceAPI::device_model() const {
    return platform_ == Platform::IOS ? "iPhone 15 Pro (simulated)"
                                      : "Pixel 8 Pro (simulated)";
}

std::string DeviceAPI::os_version() const {
    return platform_ == Platform::IOS ? "iOS 17.2" : "Android 14";
}

std::string DeviceAPI::device_id() const {
    return platform_ == Platform::IOS
        ? "AAAA-BBBB-CCCC-DDDD-EEEE"
        : "android_sim_device_001";
}

int DeviceAPI::battery_level() const { return 78; }
bool DeviceAPI::is_charging() const { return false; }

PermissionStatus DeviceAPI::check_permission(const std::string& permission) const {
    auto it = permissions_.find(permission);
    return it != permissions_.end() ? it->second : PermissionStatus::NOT_DETERMINED;
}

bool DeviceAPI::request_permission(const std::string& permission) {
    permissions_[permission] = PermissionStatus::GRANTED;
    return true;
}

std::string DeviceAPI::schedule_notification(const Notification& n) {
    pending_notifications_.push_back(n);
    return n.id;
}

bool DeviceAPI::cancel_notification(const std::string& id) {
    auto it = std::remove_if(pending_notifications_.begin(), pending_notifications_.end(),
        [&](const Notification& n) { return n.id == id; });
    bool found = it != pending_notifications_.end();
    pending_notifications_.erase(it, pending_notifications_.end());
    return found;
}

int DeviceAPI::pending_notification_count() const {
    return static_cast<int>(pending_notifications_.size());
}

} // namespace Mobile
} // namespace Sapphire
