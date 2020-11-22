#include "physical_device.h"

#include <set>

#include "queue_families_indices.h"
#include "swapchain.h"
#include "vulkan_collections_helpers.h"

namespace owl::vulkan
{
    physical_device::physical_device(const std::shared_ptr<instance>& instance,
                                     const std::shared_ptr<surface>& surface,
                                     const std::vector<const char*>& required_device_extensions)
        : _instance(instance)
        , _surface(surface)
    {
        std::vector<VkPhysicalDevice> devices = vulkan::helpers::get_physical_devices(_instance->get_vk_handle());

        if (devices.size() == 0)
            throw std::runtime_error("No GPUs with Vulkan support found.");

        for (const auto& device : devices)
        {
            if (is_device_suitable(device, required_device_extensions))
            {
                _vk_handle = device;
                break;
            }
        }

        if (_vk_handle == VK_NULL_HANDLE)
            throw std::runtime_error("No suitable device found.");
    }

    physical_device::~physical_device() {}

    bool physical_device::is_device_suitable(const VkPhysicalDevice& device, const std::vector<const char*>& required_device_extensions)
    {
        vulkan::queue_families_indices indices = vulkan::find_queue_families(device, _surface->get_vk_handle());
        bool are_required_extensions_supported = check_device_extension_support(device, required_device_extensions);

        bool is_swapchain_suitable = false;
        if (are_required_extensions_supported)
        {
            vulkan::swapchain_support swapchain_support = vulkan::query_swapchain_support(device, _surface->get_vk_handle());
            is_swapchain_suitable = !swapchain_support.formats.empty() && !swapchain_support.presentation_modes.empty();
        }

        return indices.is_complete() && are_required_extensions_supported && is_swapchain_suitable;
    }

    bool physical_device::check_device_extension_support(const VkPhysicalDevice& device,
                                                         const std::vector<const char*>& required_device_extensions)
    {
        std::vector<VkExtensionProperties> available_extensions = vulkan::helpers::get_device_extension_properties(device);
        std::set<std::string> required_extensions(required_device_extensions.begin(), required_device_extensions.end());

        for (const auto& extension : available_extensions)
            required_extensions.erase(extension.extensionName);

        return required_extensions.empty();
    }
} // namespace owl::vulkan