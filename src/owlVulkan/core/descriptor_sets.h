#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

#include "buffer.h"
#include "descriptor_pool.h"
#include "descriptor_set_layout.h"
#include "image_view.h"
#include "logical_device.h"
#include "sampler.h"

namespace owl::vulkan::core
{
    class descriptor_sets
    {
    public:
        descriptor_sets(const std::shared_ptr<logical_device>& logical_device,
                        const std::shared_ptr<descriptor_set_layout>& layout,
                        const std::shared_ptr<descriptor_pool>& descriptor_pool,
                        const std::vector<std::shared_ptr<buffer>>& uniform_buffers,
                        const std::shared_ptr<image_view>& image_view,
                        const std::shared_ptr<sampler>& sampler,
                        const uint32_t sets_count);
        ~descriptor_sets();

        const std::vector<VkDescriptorSet>& get_vk_descriptor_sets() const { return _vk_descriptor_sets; }

    private:
        std::vector<VkDescriptorSet> _vk_descriptor_sets;
    };
}