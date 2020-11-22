#pragma once

#include <vulkan/vulkan.h>

#include <string>

namespace owl::vulkan::helpers
{
    void handle_result(VkResult result, const std::string& error_message, bool throw_on_error = true);
    bool has_operation_succeeded(VkResult result);
    void handle_result_error(VkResult result, const std::string& error_message);
    void log_result_error(VkResult result, const std::string& error_message);

    std::string vk_result_to_string(VkResult result);
} // namespace owl::vulkan::helpers
