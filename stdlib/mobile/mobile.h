#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>

namespace Sapphire {
namespace Mobile {

// ===== Enums =====

enum class Platform { IOS, ANDROID, CROSS_PLATFORM };
enum class Orientation { PORTRAIT, LANDSCAPE, BOTH };
enum class ComponentType {
    VIEW, BUTTON, TEXT, TEXT_INPUT, IMAGE,
    LIST, SCROLL_VIEW, MODAL, NAVIGATION_BAR,
    TAB_BAR, SWITCH, SLIDER, PROGRESS_BAR, ICON
};
enum class LayoutType { STACK_VERTICAL, STACK_HORIZONTAL, GRID, FLEX };
enum class Alignment { START, CENTER, END, STRETCH, SPACE_BETWEEN, SPACE_AROUND };
enum class PermissionStatus { GRANTED, DENIED, NOT_DETERMINED };

// ===== App Config =====

struct AppConfig {
    std::string name;
    std::string bundle_id;       // e.g. "com.example.myapp"
    std::string version;
    std::string build_number;
    Platform    platform;
    Orientation orientation;
    std::string icon_path;
    std::string splash_path;
    std::unordered_map<std::string, std::string> permissions; // "camera" -> "reason"
};

// ===== UI Component =====

struct UIStyle {
    std::string background_color;  // hex "#RRGGBB"
    std::string text_color;
    std::string font_family;
    int         font_size;
    int         padding;
    int         margin;
    int         border_radius;
    std::string border_color;
    int         border_width;
    double      opacity;
    int         width;   // 0 = auto
    int         height;  // 0 = auto
    UIStyle();
};

struct UIEvent {
    std::string type;    // "tap", "change", "submit", "scroll", "focus", "blur"
    std::string value;   // for input changes
    double      x, y;   // for tap events
};

struct UIComponent {
    std::string   id;
    ComponentType type;
    std::string   text;       // label, placeholder, src
    UIStyle       style;
    std::vector<UIComponent> children;
    std::unordered_map<std::string, std::string> props; // extra props
    bool          visible;
    bool          enabled;
    bool          checked;    // for Switch
    double        value;      // for Slider/Progress
    double        min_value, max_value;

    UIComponent();
    std::string to_json() const;
    std::string type_name() const;
};

// ===== Layout =====

struct UILayout {
    LayoutType  type;
    Alignment   main_axis;
    Alignment   cross_axis;
    int         spacing;
    int         columns;     // for GRID
    bool        wrap;
    std::vector<UIComponent> items;

    UILayout();
    std::string render() const;
    std::string to_json() const;
};

// ===== Screen =====

struct Screen {
    std::string  name;
    std::string  title;
    UILayout     layout;
    std::string  background_color;
    bool         has_nav_bar;
    bool         has_tab_bar;
    std::string  to_json() const;
};

// ===== Navigation =====

struct NavigationStack {
    std::vector<std::string> stack;  // screen names
    void push(const std::string& screen);
    void pop();
    std::string current() const;
    int depth() const { return static_cast<int>(stack.size()); }
};

// ===== MobileApp =====

class MobileApp {
public:
    explicit MobileApp(const AppConfig& config);
    void add_screen(const Screen& screen);
    Screen* get_screen(const std::string& name);
    std::vector<std::string> screen_names() const;
    int screen_count() const { return static_cast<int>(screens_.size()); }
    NavigationStack& navigation() { return nav_; }
    const AppConfig& config() const { return config_; }
    std::string build_ios() const;
    std::string build_android() const;
    std::string export_manifest() const;
    std::string platform_name() const;

private:
    AppConfig config_;
    std::vector<Screen> screens_;
    NavigationStack nav_;
};

// ===== Device API =====

struct Location {
    double latitude;
    double longitude;
    double altitude;
    double accuracy;
    std::string to_string() const;
};

struct AccelerometerData {
    double x, y, z;
    std::string to_string() const;
};

struct StorageEntry {
    std::string key;
    std::string value;
    uint64_t    timestamp_ms;
};

struct Notification {
    std::string id;
    std::string title;
    std::string body;
    std::string icon;
    int         badge;
    uint64_t    scheduled_ms;  // 0 = immediate
    std::string to_string() const;
};

class DeviceAPI {
public:
    DeviceAPI(Platform platform);
    // Camera
    std::string capture_photo(const std::string& quality = "high");
    std::string record_video(int duration_sec = 10);
    bool        has_camera() const { return true; }
    // GPS
    Location    get_location();
    bool        request_location_permission();
    // Accelerometer
    AccelerometerData get_accelerometer();
    // Storage
    bool        storage_set(const std::string& key, const std::string& value);
    std::string storage_get(const std::string& key) const;
    bool        storage_delete(const std::string& key);
    std::vector<std::string> storage_keys() const;
    // Notifications
    std::string schedule_notification(const Notification& n);
    bool        cancel_notification(const std::string& id);
    int         pending_notification_count() const;
    // Network
    bool        is_connected() const { return true; }
    std::string network_type() const;  // "wifi", "cellular", "none"
    // Device info
    std::string device_model() const;
    std::string os_version() const;
    std::string device_id() const;
    int         battery_level() const;  // 0-100
    bool        is_charging() const;
    // Permissions
    PermissionStatus check_permission(const std::string& permission) const;
    bool             request_permission(const std::string& permission);

private:
    Platform platform_;
    std::unordered_map<std::string, std::string> storage_;
    std::vector<Notification> pending_notifications_;
    std::unordered_map<std::string, PermissionStatus> permissions_;
    int sim_counter_;
};

} // namespace Mobile
} // namespace Sapphire

// ===== C API =====
extern "C" {

// AppConfig
void* mobile_app_config_create(const char* name, const char* bundle_id,
                                const char* version, int platform);
void  mobile_app_config_destroy(void* cfg);
void  mobile_app_config_set_orientation(void* cfg, int orientation);
void  mobile_app_config_add_permission(void* cfg, const char* perm, const char* reason);

// MobileApp
void* mobile_app_create(void* config);
void  mobile_app_destroy(void* app);
int   mobile_app_screen_count(void* app);
const char* mobile_app_platform_name(void* app);
const char* mobile_app_build_ios(void* app);
const char* mobile_app_build_android(void* app);
const char* mobile_app_export_manifest(void* app);

// Screen
void* mobile_screen_create(const char* name, const char* title);
void  mobile_screen_destroy(void* screen);
void  mobile_screen_set_background(void* screen, const char* color);
void  mobile_screen_set_nav_bar(void* screen, int has_nav);
void  mobile_app_add_screen(void* app, void* screen);
const char* mobile_screen_to_json(void* screen);

// UIComponent
void* mobile_component_create(int type, const char* id, const char* text);
void  mobile_component_destroy(void* comp);
void  mobile_component_set_text(void* comp, const char* text);
void  mobile_component_set_visible(void* comp, int visible);
void  mobile_component_set_enabled(void* comp, int enabled);
void  mobile_component_set_value(void* comp, double value);
void  mobile_component_set_prop(void* comp, const char* key, const char* value);
void  mobile_component_set_style_color(void* comp, const char* bg, const char* fg);
void  mobile_component_set_style_font(void* comp, const char* family, int size);
void  mobile_component_set_style_size(void* comp, int width, int height);
void  mobile_component_set_style_padding(void* comp, int padding);
void  mobile_component_add_child(void* comp, void* child);
const char* mobile_component_to_json(void* comp);
const char* mobile_component_type_name(void* comp);
const char* mobile_component_get_text(void* comp);
double      mobile_component_get_value(void* comp);

// UILayout
void* mobile_layout_create(int type);
void  mobile_layout_destroy(void* layout);
void  mobile_layout_set_spacing(void* layout, int spacing);
void  mobile_layout_set_columns(void* layout, int cols);
void  mobile_layout_add_item(void* layout, void* comp);
void  mobile_screen_set_layout(void* screen, void* layout);
const char* mobile_layout_to_json(void* layout);

// Navigation
void* mobile_nav_create();
void  mobile_nav_destroy(void* nav);
void  mobile_nav_push(void* nav, const char* screen);
void  mobile_nav_pop(void* nav);
const char* mobile_nav_current(void* nav);
int   mobile_nav_depth(void* nav);

// DeviceAPI
void* mobile_device_create(int platform);
void  mobile_device_destroy(void* dev);
const char* mobile_device_capture_photo(void* dev, const char* quality);
const char* mobile_device_record_video(void* dev, int duration_sec);
void* mobile_device_get_location(void* dev);
void  mobile_location_destroy(void* loc);
double mobile_location_latitude(void* loc);
double mobile_location_longitude(void* loc);
double mobile_location_altitude(void* loc);
double mobile_location_accuracy(void* loc);
void* mobile_device_get_accelerometer(void* dev);
void  mobile_accel_destroy(void* accel);
double mobile_accel_x(void* accel);
double mobile_accel_y(void* accel);
double mobile_accel_z(void* accel);
int   mobile_device_storage_set(void* dev, const char* key, const char* value);
const char* mobile_device_storage_get(void* dev, const char* key);
int   mobile_device_storage_delete(void* dev, const char* key);
int   mobile_device_is_connected(void* dev);
const char* mobile_device_network_type(void* dev);
const char* mobile_device_model(void* dev);
const char* mobile_device_os_version(void* dev);
const char* mobile_device_id(void* dev);
int   mobile_device_battery_level(void* dev);
int   mobile_device_is_charging(void* dev);
const char* mobile_device_check_permission(void* dev, const char* perm);
int   mobile_device_request_permission(void* dev, const char* perm);
const char* mobile_device_schedule_notification(void* dev,
    const char* title, const char* body, int badge, uint64_t delay_ms);
int   mobile_device_cancel_notification(void* dev, const char* id);
int   mobile_device_pending_notifications(void* dev);

} // extern "C"
