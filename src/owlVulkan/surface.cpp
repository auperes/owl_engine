#include "surface.h"

namespace owl::vulkan
{
    surface::surface(const std::shared_ptr<instance>& instance, VkSurfaceKHR vk_surface)
        : _instance(instance), _vk_surface(vk_surface)
    {
    }

    surface::~surface() { vkDestroySurfaceKHR(_instance->get_vk_instance(), _vk_surface, nullptr); }
} // namespace owl::vulkan