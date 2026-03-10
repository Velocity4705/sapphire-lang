#ifndef SAPPHIRE_STDLIB_VULKAN_H
#define SAPPHIRE_STDLIB_VULKAN_H

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <string>
#include <vector>

namespace sapphire {
namespace stdlib {
namespace graphics {

/**
 * Vulkan Graphics Library for Sapphire
 * 
 * Provides Vulkan API for high-performance 3D graphics.
 * This is a simplified wrapper - for full control, use the C++ implementation directly.
 */

struct VulkanVertex {
    float pos[2];
    float color[3];
};

class VulkanContext {
private:
    SDL_Window* window_;
    VkInstance instance_;
    VkSurfaceKHR surface_;
    VkPhysicalDevice physical_device_;
    VkDevice device_;
    VkQueue graphics_queue_;
    VkQueue present_queue_;
    VkSwapchainKHR swapchain_;
    std::vector<VkImage> swapchain_images_;
    VkFormat swapchain_image_format_;
    VkExtent2D swapchain_extent_;
    std::vector<VkImageView> swapchain_image_views_;
    VkRenderPass render_pass_;
    VkPipelineLayout pipeline_layout_;
    VkPipeline graphics_pipeline_;
    std::vector<VkFramebuffer> swapchain_framebuffers_;
    VkCommandPool command_pool_;
    std::vector<VkCommandBuffer> command_buffers_;
    VkSemaphore image_available_semaphore_;
    VkSemaphore render_finished_semaphore_;
    VkBuffer vertex_buffer_;
    VkDeviceMemory vertex_buffer_memory_;
    
    uint32_t graphics_family_index_;
    uint32_t present_family_index_;
    
    bool initialized_;
    
public:
    VulkanContext(const std::string& title, int width, int height);
    ~VulkanContext();
    
    // Initialization
    bool init();
    
    // Rendering
    void begin_frame();
    void end_frame();
    void draw_triangle();
    
    // Window management
    bool should_close() const;
    void poll_events();
    
private:
    bool create_instance();
    bool create_surface();
    bool pick_physical_device();
    bool create_logical_device();
    bool create_swapchain();
    bool create_image_views();
    bool create_render_pass();
    bool create_graphics_pipeline();
    bool create_framebuffers();
    bool create_command_pool();
    bool create_vertex_buffer();
    bool create_command_buffers();
    bool create_semaphores();
    
    void cleanup();
};

} // namespace graphics
} // namespace stdlib
} // namespace sapphire

// C API for Sapphire code generation
extern "C" {
    // Context management
    void* sapphire_vulkan_create_context(const char* title, int width, int height);
    void sapphire_vulkan_destroy_context(void* context);
    bool sapphire_vulkan_init(void* context);
    
    // Rendering
    void sapphire_vulkan_begin_frame(void* context);
    void sapphire_vulkan_end_frame(void* context);
    void sapphire_vulkan_draw_triangle(void* context);
    
    // Window management
    bool sapphire_vulkan_should_close(void* context);
    void sapphire_vulkan_poll_events(void* context);
}

#endif // SAPPHIRE_STDLIB_VULKAN_H
