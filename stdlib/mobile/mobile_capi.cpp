#include "mobile.h"
#include <string>
#include <sstream>

using namespace Sapphire::Mobile;

static thread_local std::string tl_str;
static const char* ret_str(const std::string& s) { tl_str = s; return tl_str.c_str(); }

// ===== AppConfig =====

void* mobile_app_config_create(const char* name, const char* bundle_id,
                                const char* version, int platform) {
    auto* cfg = new AppConfig();
    cfg->name         = name ? name : "";
    cfg->bundle_id    = bundle_id ? bundle_id : "";
    cfg->version      = version ? version : "1.0.0";
    cfg->build_number = "1";
    cfg->platform     = static_cast<Platform>(platform);
    cfg->orientation  = Orientation::PORTRAIT;
    return cfg;
}
void mobile_app_config_destroy(void* cfg) { delete static_cast<AppConfig*>(cfg); }
void mobile_app_config_set_orientation(void* cfg, int orientation) {
    static_cast<AppConfig*>(cfg)->orientation = static_cast<Orientation>(orientation);
}
void mobile_app_config_add_permission(void* cfg, const char* perm, const char* reason) {
    static_cast<AppConfig*>(cfg)->permissions[perm ? perm : ""] = reason ? reason : "";
}

// ===== MobileApp =====

void* mobile_app_create(void* config) {
    return new MobileApp(*static_cast<AppConfig*>(config));
}
void mobile_app_destroy(void* app) { delete static_cast<MobileApp*>(app); }
int  mobile_app_screen_count(void* app) { return static_cast<MobileApp*>(app)->screen_count(); }
const char* mobile_app_platform_name(void* app) {
    return ret_str(static_cast<MobileApp*>(app)->platform_name());
}
const char* mobile_app_build_ios(void* app) {
    return ret_str(static_cast<MobileApp*>(app)->build_ios());
}
const char* mobile_app_build_android(void* app) {
    return ret_str(static_cast<MobileApp*>(app)->build_android());
}
const char* mobile_app_export_manifest(void* app) {
    return ret_str(static_cast<MobileApp*>(app)->export_manifest());
}
void mobile_app_add_screen(void* app, void* screen) {
    static_cast<MobileApp*>(app)->add_screen(*static_cast<Screen*>(screen));
}

// ===== Screen =====

void* mobile_screen_create(const char* name, const char* title) {
    auto* s = new Screen();
    s->name             = name ? name : "";
    s->title            = title ? title : "";
    s->background_color = "#F5F5F5";
    s->has_nav_bar      = true;
    s->has_tab_bar      = false;
    return s;
}
void mobile_screen_destroy(void* screen) { delete static_cast<Screen*>(screen); }
void mobile_screen_set_background(void* screen, const char* color) {
    static_cast<Screen*>(screen)->background_color = color ? color : "#FFFFFF";
}
void mobile_screen_set_nav_bar(void* screen, int has_nav) {
    static_cast<Screen*>(screen)->has_nav_bar = has_nav != 0;
}
const char* mobile_screen_to_json(void* screen) {
    return ret_str(static_cast<Screen*>(screen)->to_json());
}
void mobile_screen_set_layout(void* screen, void* layout) {
    static_cast<Screen*>(screen)->layout = *static_cast<UILayout*>(layout);
}

// ===== UIComponent =====

void* mobile_component_create(int type, const char* id, const char* text) {
    auto* c = new UIComponent();
    c->type = static_cast<ComponentType>(type);
    c->id   = id ? id : "";
    c->text = text ? text : "";
    return c;
}
void mobile_component_destroy(void* comp) { delete static_cast<UIComponent*>(comp); }
void mobile_component_set_text(void* comp, const char* text) {
    static_cast<UIComponent*>(comp)->text = text ? text : "";
}
void mobile_component_set_visible(void* comp, int visible) {
    static_cast<UIComponent*>(comp)->visible = visible != 0;
}
void mobile_component_set_enabled(void* comp, int enabled) {
    static_cast<UIComponent*>(comp)->enabled = enabled != 0;
}
void mobile_component_set_value(void* comp, double value) {
    static_cast<UIComponent*>(comp)->value = value;
}
void mobile_component_set_prop(void* comp, const char* key, const char* value) {
    static_cast<UIComponent*>(comp)->props[key ? key : ""] = value ? value : "";
}
void mobile_component_set_style_color(void* comp, const char* bg, const char* fg) {
    auto* c = static_cast<UIComponent*>(comp);
    if (bg) c->style.background_color = bg;
    if (fg) c->style.text_color = fg;
}
void mobile_component_set_style_font(void* comp, const char* family, int size) {
    auto* c = static_cast<UIComponent*>(comp);
    if (family) c->style.font_family = family;
    c->style.font_size = size;
}
void mobile_component_set_style_size(void* comp, int width, int height) {
    auto* c = static_cast<UIComponent*>(comp);
    c->style.width = width;
    c->style.height = height;
}
void mobile_component_set_style_padding(void* comp, int padding) {
    static_cast<UIComponent*>(comp)->style.padding = padding;
}
void mobile_component_add_child(void* comp, void* child) {
    static_cast<UIComponent*>(comp)->children.push_back(*static_cast<UIComponent*>(child));
}
const char* mobile_component_to_json(void* comp) {
    return ret_str(static_cast<UIComponent*>(comp)->to_json());
}
const char* mobile_component_type_name(void* comp) {
    return ret_str(static_cast<UIComponent*>(comp)->type_name());
}
const char* mobile_component_get_text(void* comp) {
    return ret_str(static_cast<UIComponent*>(comp)->text);
}
double mobile_component_get_value(void* comp) {
    return static_cast<UIComponent*>(comp)->value;
}

// ===== UILayout =====

void* mobile_layout_create(int type) {
    auto* l = new UILayout();
    l->type = static_cast<LayoutType>(type);
    return l;
}
void mobile_layout_destroy(void* layout) { delete static_cast<UILayout*>(layout); }
void mobile_layout_set_spacing(void* layout, int spacing) {
    static_cast<UILayout*>(layout)->spacing = spacing;
}
void mobile_layout_set_columns(void* layout, int cols) {
    static_cast<UILayout*>(layout)->columns = cols;
}
void mobile_layout_add_item(void* layout, void* comp) {
    static_cast<UILayout*>(layout)->items.push_back(*static_cast<UIComponent*>(comp));
}
const char* mobile_layout_to_json(void* layout) {
    return ret_str(static_cast<UILayout*>(layout)->to_json());
}

// ===== Navigation =====

void* mobile_nav_create() { return new NavigationStack(); }
void  mobile_nav_destroy(void* nav) { delete static_cast<NavigationStack*>(nav); }
void  mobile_nav_push(void* nav, const char* screen) {
    static_cast<NavigationStack*>(nav)->push(screen ? screen : "");
}
void  mobile_nav_pop(void* nav) { static_cast<NavigationStack*>(nav)->pop(); }
const char* mobile_nav_current(void* nav) {
    return ret_str(static_cast<NavigationStack*>(nav)->current());
}
int mobile_nav_depth(void* nav) { return static_cast<NavigationStack*>(nav)->depth(); }

// ===== DeviceAPI =====

void* mobile_device_create(int platform) {
    return new DeviceAPI(static_cast<Platform>(platform));
}
void mobile_device_destroy(void* dev) { delete static_cast<DeviceAPI*>(dev); }

const char* mobile_device_capture_photo(void* dev, const char* quality) {
    return ret_str(static_cast<DeviceAPI*>(dev)->capture_photo(quality ? quality : "high"));
}
const char* mobile_device_record_video(void* dev, int duration_sec) {
    return ret_str(static_cast<DeviceAPI*>(dev)->record_video(duration_sec));
}

void* mobile_device_get_location(void* dev) {
    return new Location(static_cast<DeviceAPI*>(dev)->get_location());
}
void   mobile_location_destroy(void* loc) { delete static_cast<Location*>(loc); }
double mobile_location_latitude(void* loc)  { return static_cast<Location*>(loc)->latitude; }
double mobile_location_longitude(void* loc) { return static_cast<Location*>(loc)->longitude; }
double mobile_location_altitude(void* loc)  { return static_cast<Location*>(loc)->altitude; }
double mobile_location_accuracy(void* loc)  { return static_cast<Location*>(loc)->accuracy; }

void* mobile_device_get_accelerometer(void* dev) {
    return new AccelerometerData(static_cast<DeviceAPI*>(dev)->get_accelerometer());
}
void   mobile_accel_destroy(void* accel) { delete static_cast<AccelerometerData*>(accel); }
double mobile_accel_x(void* accel) { return static_cast<AccelerometerData*>(accel)->x; }
double mobile_accel_y(void* accel) { return static_cast<AccelerometerData*>(accel)->y; }
double mobile_accel_z(void* accel) { return static_cast<AccelerometerData*>(accel)->z; }

int mobile_device_storage_set(void* dev, const char* key, const char* value) {
    return static_cast<DeviceAPI*>(dev)->storage_set(key?key:"", value?value:"") ? 1 : 0;
}
const char* mobile_device_storage_get(void* dev, const char* key) {
    return ret_str(static_cast<DeviceAPI*>(dev)->storage_get(key?key:""));
}
int mobile_device_storage_delete(void* dev, const char* key) {
    return static_cast<DeviceAPI*>(dev)->storage_delete(key?key:"") ? 1 : 0;
}
int mobile_device_is_connected(void* dev) {
    return static_cast<DeviceAPI*>(dev)->is_connected() ? 1 : 0;
}
const char* mobile_device_network_type(void* dev) {
    return ret_str(static_cast<DeviceAPI*>(dev)->network_type());
}
const char* mobile_device_model(void* dev) {
    return ret_str(static_cast<DeviceAPI*>(dev)->device_model());
}
const char* mobile_device_os_version(void* dev) {
    return ret_str(static_cast<DeviceAPI*>(dev)->os_version());
}
const char* mobile_device_id(void* dev) {
    return ret_str(static_cast<DeviceAPI*>(dev)->device_id());
}
int mobile_device_battery_level(void* dev) {
    return static_cast<DeviceAPI*>(dev)->battery_level();
}
int mobile_device_is_charging(void* dev) {
    return static_cast<DeviceAPI*>(dev)->is_charging() ? 1 : 0;
}
const char* mobile_device_check_permission(void* dev, const char* perm) {
    auto status = static_cast<DeviceAPI*>(dev)->check_permission(perm?perm:"");
    switch (status) {
        case PermissionStatus::GRANTED:         return "granted";
        case PermissionStatus::DENIED:          return "denied";
        case PermissionStatus::NOT_DETERMINED:  return "not_determined";
    }
    return "unknown";
}
int mobile_device_request_permission(void* dev, const char* perm) {
    return static_cast<DeviceAPI*>(dev)->request_permission(perm?perm:"") ? 1 : 0;
}
const char* mobile_device_schedule_notification(void* dev,
    const char* title, const char* body, int badge, uint64_t delay_ms) {
    Notification n;
    n.id           = "notif_" + std::to_string(delay_ms);
    n.title        = title ? title : "";
    n.body         = body ? body : "";
    n.badge        = badge;
    n.scheduled_ms = delay_ms;
    return ret_str(static_cast<DeviceAPI*>(dev)->schedule_notification(n));
}
int mobile_device_cancel_notification(void* dev, const char* id) {
    return static_cast<DeviceAPI*>(dev)->cancel_notification(id?id:"") ? 1 : 0;
}
int mobile_device_pending_notifications(void* dev) {
    return static_cast<DeviceAPI*>(dev)->pending_notification_count();
}
