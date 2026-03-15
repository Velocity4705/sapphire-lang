#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================
// Phase 1: Graphics Engine
// ============================================================

// Renderer
void* game_renderer_create(const char* title, int width, int height);
void  game_renderer_destroy(void* renderer);
void  game_renderer_clear(void* renderer, float r, float g, float b, float a);
void  game_renderer_present(void* renderer);
int   game_renderer_width(void* renderer);
int   game_renderer_height(void* renderer);
const char* game_renderer_title(void* renderer);

// Texture
void* game_texture_create(void* renderer, int width, int height);
void* game_texture_load(void* renderer, const char* path);
void  game_texture_destroy(void* texture);
int   game_texture_width(void* texture);
int   game_texture_height(void* texture);
const char* game_texture_path(void* texture);

// Sprite
void* game_sprite_create(void* texture, float x, float y);
void  game_sprite_destroy(void* sprite);
void  game_sprite_set_position(void* sprite, float x, float y);
void  game_sprite_set_scale(void* sprite, float sx, float sy);
void  game_sprite_set_rotation(void* sprite, float angle);
void  game_sprite_set_color(void* sprite, float r, float g, float b, float a);
void  game_sprite_set_visible(void* sprite, int visible);
float game_sprite_x(void* sprite);
float game_sprite_y(void* sprite);
float game_sprite_scale_x(void* sprite);
float game_sprite_scale_y(void* sprite);
float game_sprite_rotation(void* sprite);
const char* game_sprite_to_string(void* sprite);

// Shader
void* game_shader_create(const char* name, const char* vert_src, const char* frag_src);
void  game_shader_destroy(void* shader);
void  game_shader_set_float(void* shader, const char* uniform, float value);
void  game_shader_set_int(void* shader, const char* uniform, int value);
void  game_shader_set_vec2(void* shader, const char* uniform, float x, float y);
void  game_shader_set_vec3(void* shader, const char* uniform, float x, float y, float z);
void  game_shader_set_vec4(void* shader, const char* uniform, float x, float y, float z, float w);
const char* game_shader_name(void* shader);
int   game_shader_uniform_count(void* shader);
const char* game_shader_uniform_name(void* shader, int index);

// Camera2D
void* game_camera2d_create(float x, float y, float zoom);
void  game_camera2d_destroy(void* camera);
void  game_camera2d_set_position(void* camera, float x, float y);
void  game_camera2d_set_zoom(void* camera, float zoom);
void  game_camera2d_move(void* camera, float dx, float dy);
float game_camera2d_x(void* camera);
float game_camera2d_y(void* camera);
float game_camera2d_zoom(void* camera);

// Camera3D
void* game_camera3d_create(float px, float py, float pz, float tx, float ty, float tz);
void  game_camera3d_destroy(void* camera);
void  game_camera3d_set_position(void* camera, float x, float y, float z);
void  game_camera3d_set_target(void* camera, float x, float y, float z);
void  game_camera3d_set_fov(void* camera, float fov);
float game_camera3d_pos_x(void* camera);
float game_camera3d_pos_y(void* camera);
float game_camera3d_pos_z(void* camera);
float game_camera3d_fov(void* camera);

// ============================================================
// Phase 2: Physics Engine
// ============================================================

// PhysicsWorld
void* game_physics_world_create(float gx, float gy);
void  game_physics_world_destroy(void* world);
void  game_physics_world_step(void* world, float dt);
void  game_physics_world_set_gravity(void* world, float gx, float gy);
int   game_physics_world_body_count(void* world);

// RigidBody
void* game_rigidbody_create(void* world, float x, float y, float mass, int is_static);
void  game_rigidbody_destroy(void* body);
void  game_rigidbody_set_position(void* body, float x, float y);
void  game_rigidbody_set_velocity(void* body, float vx, float vy);
void  game_rigidbody_apply_force(void* body, float fx, float fy);
void  game_rigidbody_apply_impulse(void* body, float ix, float iy);
void  game_rigidbody_set_restitution(void* body, float e);
void  game_rigidbody_set_friction(void* body, float f);
float game_rigidbody_x(void* body);
float game_rigidbody_y(void* body);
float game_rigidbody_vx(void* body);
float game_rigidbody_vy(void* body);
float game_rigidbody_mass(void* body);
int   game_rigidbody_is_static(void* body);
const char* game_rigidbody_to_string(void* body);

// Collider
void* game_collider_box(void* body, float w, float h);
void* game_collider_circle(void* body, float radius);
void  game_collider_destroy(void* collider);
int   game_collider_check(void* a, void* b);
const char* game_collider_type(void* collider);

// ParticleSystem
void* game_particles_create(float x, float y, int max_particles);
void  game_particles_destroy(void* ps);
void  game_particles_emit(void* ps, int count);
void  game_particles_update(void* ps, float dt);
void  game_particles_set_velocity(void* ps, float vx, float vy, float spread);
void  game_particles_set_lifetime(void* ps, float min_life, float max_life);
void  game_particles_set_color(void* ps, float r, float g, float b, float a);
void  game_particles_set_size(void* ps, float start_size, float end_size);
int   game_particles_active_count(void* ps);
int   game_particles_max_count(void* ps);
const char* game_particles_to_string(void* ps);

// ============================================================
// Phase 3: Audio System
// ============================================================

// AudioEngine
void* game_audio_engine_create();
void  game_audio_engine_destroy(void* engine);
void  game_audio_engine_set_master_volume(void* engine, float vol);
float game_audio_engine_master_volume(void* engine);
int   game_audio_engine_channel_count(void* engine);

// Sound
void* game_sound_load(void* engine, const char* path);
void* game_sound_create(void* engine, const char* name, const char* type);
void  game_sound_destroy(void* sound);
void  game_sound_play(void* sound);
void  game_sound_stop(void* sound);
void  game_sound_pause(void* sound);
void  game_sound_resume(void* sound);
void  game_sound_set_volume(void* sound, float vol);
void  game_sound_set_pitch(void* sound, float pitch);
void  game_sound_set_loop(void* sound, int loop);
int   game_sound_is_playing(void* sound);
float game_sound_volume(void* sound);
float game_sound_pitch(void* sound);
const char* game_sound_name(void* sound);
const char* game_sound_type(void* sound);

// Music
void* game_music_load(void* engine, const char* path);
void* game_music_create(void* engine, const char* name);
void  game_music_destroy(void* music);
void  game_music_play(void* music);
void  game_music_stop(void* music);
void  game_music_pause(void* music);
void  game_music_resume(void* music);
void  game_music_set_volume(void* music, float vol);
void  game_music_set_loop(void* music, int loop);
int   game_music_is_playing(void* music);
const char* game_music_name(void* music);

// Audio3D
void* game_audio3d_create(void* engine);
void  game_audio3d_destroy(void* audio3d);
void  game_audio3d_set_listener(void* audio3d, float x, float y, float z);
void  game_audio3d_play_at(void* audio3d, void* sound, float x, float y, float z);
void  game_audio3d_set_rolloff(void* audio3d, float factor);

// AudioMixer
void* game_mixer_create(void* engine, int channels);
void  game_mixer_destroy(void* mixer);
void  game_mixer_set_channel_volume(void* mixer, int ch, float vol);
float game_mixer_channel_volume(void* mixer, int ch);
void  game_mixer_play_on_channel(void* mixer, void* sound, int ch);
int   game_mixer_channels(void* mixer);

// ============================================================
// Phase 4: Game Framework
// ============================================================

// GameLoop
void* game_loop_create(int target_fps);
void  game_loop_destroy(void* loop);
void  game_loop_start(void* loop);
void  game_loop_stop(void* loop);
void  game_loop_tick(void* loop);
int   game_loop_is_running(void* loop);
float game_loop_delta_time(void* loop);
int   game_loop_fps(void* loop);
int   game_loop_target_fps(void* loop);
int   game_loop_frame_count(void* loop);
double game_loop_elapsed(void* loop);

// Scene
void* game_scene_create(const char* name);
void  game_scene_destroy(void* scene);
void  game_scene_add_sprite(void* scene, void* sprite);
void  game_scene_add_body(void* scene, void* body);
void  game_scene_update(void* scene, float dt);
int   game_scene_sprite_count(void* scene);
int   game_scene_body_count(void* scene);
const char* game_scene_name(void* scene);

// SceneManager
void* game_scene_manager_create();
void  game_scene_manager_destroy(void* sm);
void  game_scene_manager_push(void* sm, void* scene);
void  game_scene_manager_pop(void* sm);
void* game_scene_manager_current(void* sm);
int   game_scene_manager_depth(void* sm);

// InputManager
void* game_input_create();
void  game_input_destroy(void* input);
void  game_input_key_press(void* input, const char* key);
void  game_input_key_release(void* input, const char* key);
void  game_input_mouse_move(void* input, float x, float y);
void  game_input_mouse_press(void* input, int button);
void  game_input_mouse_release(void* input, int button);
int   game_input_is_key_down(void* input, const char* key);
int   game_input_is_key_pressed(void* input, const char* key);
int   game_input_is_mouse_down(void* input, int button);
float game_input_mouse_x(void* input);
float game_input_mouse_y(void* input);
void  game_input_update(void* input);

// AssetManager
void* game_assets_create(const char* base_path);
void  game_assets_destroy(void* assets);
void  game_assets_register(void* assets, const char* name, const char* path, const char* type);
const char* game_assets_get_path(void* assets, const char* name);
const char* game_assets_get_type(void* assets, const char* name);
int   game_assets_count(void* assets);
const char* game_assets_name_at(void* assets, int index);
const char* game_assets_to_json(void* assets);

#ifdef __cplusplus
}
#endif
