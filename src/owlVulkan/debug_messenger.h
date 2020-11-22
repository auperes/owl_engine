#pragma once

#include <vulkan/vulkan.h>

#include <memory>

#include "instance.h"

namespace owl::vulkan
{
    class debug_messenger
    {
    public:
        explicit debug_messenger(const std::shared_ptr<instance>& instance);
        ~debug_messenger();

        static void configure_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                             VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                             const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                             void* user_data);

    private:
        VkDebugUtilsMessengerEXT _vk_debug_messenger;
        std::shared_ptr<instance> _instance;
    };
} // namespace owl::vulkan