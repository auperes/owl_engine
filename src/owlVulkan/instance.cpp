#include "instance.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "debug_messenger.h"
#include "helpers/vulkan_helpers.h"
#include "vulkan_collections_helpers.h"

namespace owl::vulkan
{
    instance::instance(bool enable_validation_layers,
                       const std::vector<const char*>& validation_layers,
                       const std::vector<const char*>& required_extensions)
    {
        if (enable_validation_layers && !check_validation_layer_support(validation_layers))
            throw std::runtime_error("Validation layers requested but some are not available.");

        VkApplicationInfo application_info{};
        application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.pApplicationName = "Owl Engine";
        application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.pEngineName = "No engine";
        application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        application_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &application_info;
        create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
        create_info.ppEnabledExtensionNames = required_extensions.data();

        if (enable_validation_layers)
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledLayerNames = validation_layers.data();

            VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT};
            VkValidationFeaturesEXT validation_features{};
            validation_features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
            validation_features.enabledValidationFeatureCount = 1;
            validation_features.pEnabledValidationFeatures = enables;

            VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
            debug_create_info.pNext = &validation_features;

            debug_messenger::configure_debug_create_info(debug_create_info);
            create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
        }
        else
        {
            create_info.enabledLayerCount = 0;
            create_info.pNext = nullptr;
        }

        VkResult result = vkCreateInstance(&create_info, nullptr, &_vk_handle);
        vulkan::helpers::handle_result(result, "Failed to create instance");
    }

    instance::~instance() { vkDestroyInstance(_vk_handle, nullptr); }

    bool instance::check_validation_layer_support(const std::vector<const char*>& validation_layers)
    {
        std::vector<VkLayerProperties> available_layers = vulkan::helpers::get_instance_layer_properties();

        bool all_layers_found = true;
        for (const auto layer_name : validation_layers)
        {
            auto result = std::find_if(available_layers.begin(), available_layers.end(), [layer_name](const auto& layer_properties) {
                return strcmp(layer_name, layer_properties.layerName) == 0;
            });

            if (result == available_layers.end())
            {
                std::cout << "Layer " << layer_name << " not found." << std::endl; // TODO log
                all_layers_found = false;
            }
        }

        return all_layers_found;
    }
} // namespace owl::vulkan