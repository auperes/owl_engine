#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "vulkan_object.h"

namespace owl::vulkan::core
{
    class debug_messenger : public vulkan_object<VkDebugUtilsMessengerEXT>
    {
    public:
        explicit debug_messenger(const VkInstance& vk_instance);
        ~debug_messenger();

        static void configure_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                             VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                             void* user_data);

    private:
        const VkInstance& _vk_instance;
    };
} // namespace owl::vulkan