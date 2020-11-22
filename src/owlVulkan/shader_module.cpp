#include "shader_module.h"

#include <stdexcept>

#include "file_helpers.h"

namespace owl::vulkan
{
    shader_module::shader_module(const std::string& filename, const std::shared_ptr<logical_device>& logical_device)
        : _logical_device(logical_device)
    {
        auto shader_code = read_file(filename);
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = shader_code.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

        auto result = vkCreateShaderModule(_logical_device->get_vk_handle(), &create_info, nullptr, &_vk_handle);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module: " + std::to_string(result));
        }
    }

    shader_module::~shader_module() { vkDestroyShaderModule(_logical_device->get_vk_handle(), _vk_handle, nullptr); }
} // namespace owl::vulkan