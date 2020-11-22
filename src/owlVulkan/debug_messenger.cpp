#include "debug_messenger.h"

#include <iostream>

#include "helpers/vulkan_helpers.h"

namespace owl::vulkan
{
    debug_messenger::debug_messenger(const std::shared_ptr<instance>& instance) : _instance(instance)
    {
        VkDebugUtilsMessengerCreateInfoEXT create_info{};
        configure_debug_create_info(create_info);

        auto create_debug_utils_messenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            _instance->get_vk_instance(), "vkCreateDebugUtilsMessengerEXT");

        VkResult result =
            create_debug_utils_messenger != nullptr
                ? create_debug_utils_messenger(_instance->get_vk_instance(), &create_info, nullptr, &_vk_debug_messenger)
                : VK_ERROR_EXTENSION_NOT_PRESENT;

        vulkan::helpers::handle_result(result, "Failed to setup debug messenger");
    }

    debug_messenger::~debug_messenger()
    {
        auto destroy_debug_utils_messenger =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance->get_vk_instance(), "vkDestroyDebugUtilsMessengerEXT");
        if (destroy_debug_utils_messenger != nullptr)
            destroy_debug_utils_messenger(_instance->get_vk_instance(), _vk_debug_messenger, nullptr);
    }

    void debug_messenger::configure_debug_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
    {
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                      |*/
            /*VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |*/ VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = debug_callback;
        create_info.pUserData = nullptr;
    }

    VkBool32 debug_messenger::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                    VkDebugUtilsMessageTypeFlagsEXT message_type,
                                    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                    void* user_data)
    {
        std::cerr << "validation layer:" << callback_data->pMessage << std::endl; // TODO use logger
        return VK_FALSE;
    }
} // namespace owl::vulkan