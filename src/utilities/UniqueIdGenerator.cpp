#include "UniqueIdGenerator.h"

#include <atomic>
#include <chrono>

namespace lvt {
namespace utilities {

std::string UniqueIdGenerator::generateUniqueId() {
    static std::atomic_uint64_t idGenerator{0};
    
    auto newId = idGenerator.fetch_add(1);
    
    auto timeSinceEpoch = std::chrono::system_clock::now().time_since_epoch().count();
    
    return std::to_string(timeSinceEpoch) + "-" + std::to_string(newId);
}

} // namespace utilities
} // namespace lvt
