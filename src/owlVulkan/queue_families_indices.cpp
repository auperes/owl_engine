#include "queue_families_indices.h"

#include <vector>

namespace owl::vulkan
{
    bool queue_families_indices::is_complete() { return graphics_family.has_value() && presentation_family.has_value(); }
} // namespace owl::vulkan
