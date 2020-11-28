#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <string>

#include "physical_device.h"

namespace owl::vulkan::helpers
{
    void handle_result(VkResult result, const std::string& error_message, bool throw_on_error = true);
    bool has_operation_succeeded(VkResult result);
    void handle_result_error(VkResult result, const std::string& error_message);
    void log_result_error(VkResult result, const std::string& error_message);

    std::string vk_result_to_string(VkResult result);

    VkFormat find_supported_format(const std::shared_ptr<physical_device>& physical_device,
                                   const std::vector<VkFormat>& candidates,
                                   VkImageTiling tiling,
                                   VkFormatFeatureFlags features);
    VkFormat find_depth_format(const std::shared_ptr<physical_device>& physical_device);
    bool has_stencil_component(VkFormat format);
} // namespace owl::vulkan::helpers
