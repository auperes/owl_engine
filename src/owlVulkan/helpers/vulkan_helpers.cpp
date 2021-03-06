#include "../helpers/vulkan_helpers.h"

#include <iostream>
#include <vector>

namespace owl::vulkan::helpers
{
    void handle_result(VkResult result, const std::string& error_message, bool throw_on_error)
    {
        if (has_operation_succeeded(result))
            return;

        log_result_error(result, error_message);
        if (throw_on_error)
            handle_result_error(result, error_message);
    }

    bool has_operation_succeeded(VkResult result) { return result == VK_SUCCESS; }

    void handle_result_error(VkResult result, const std::string& error_message)
    {
        throw std::logic_error(error_message + " (" + vk_result_to_string(result) + ")");
    }

    void log_result_error(VkResult result, const std::string& error_message)
    {
        std::cerr << error_message << " (" << vk_result_to_string(result) << ")" << std::endl;
    }

    std::string vk_result_to_string(VkResult result) { return std::to_string(static_cast<int>(result)); }

    bool has_stencil_component(VkFormat format) { return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT; }
} // namespace owl::vulkan::helpers
