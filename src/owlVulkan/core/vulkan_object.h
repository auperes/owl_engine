#pragma once

namespace owl::vulkan::core
{
    template <typename TVulkanHandle>
    class vulkan_object
    {
    public:
        const TVulkanHandle& get_vk_handle() const { return _vk_handle; }

    protected:
        TVulkanHandle _vk_handle;
    };
} // namespace owl::vulkan::core