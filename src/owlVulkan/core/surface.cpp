#include "surface.h"

namespace owl::vulkan::core
{
    surface::surface(const std::shared_ptr<instance>& instance, VkSurfaceKHR vk_surface)
        : _instance(instance)
    {
        _vk_handle = vk_surface;
    }

    surface::~surface() { vkDestroySurfaceKHR(_instance->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan