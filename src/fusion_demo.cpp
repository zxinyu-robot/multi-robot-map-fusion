#include "map_fusion/shared_frame_slot.h"
#include "map_fusion/submap_registry.h"

#include <iostream>
#include <span>
#include <string>
#include <unistd.h>

int main() {
    using namespace map_fusion;

    const std::string name =
        "/map_fusion_demo_" + std::to_string(::getpid());
    SharedFrameSlot producer(name, 4096, SharedFrameSlot::Mode::Create, true);
    SharedFrameSlot consumer(name, 4096, SharedFrameSlot::Mode::Open);

    const std::string payload = "compressed-keyframe:robot-1:submap-7";
    const std::uint64_t sequence = producer.publish(std::as_bytes(
        std::span(payload.data(), payload.size())));
    const auto view = consumer.view_latest();

    SubmapRegistry registry;
    const SubmapId first{1, 1, 7};
    const SubmapId second{2, 1, 3};
    registry.register_submap({first, 100, "robot_1/submap_7", sequence, 11, 0.91});
    registry.register_submap({second, 120, "robot_2/submap_3", 1, 22, 0.88});
    const RegistryResult fused = registry.add_constraint({
        1,
        first,
        second,
        {1.2, 0.1, 0.0, 0.0, 0.0, 0.05, 0.9987},
        {0.1, 0.1, 0.1, 0.05, 0.05, 0.05},
        0.86,
    });

    std::cout << "shared frame sequence=" << view.sequence()
              << " bytes=" << view.bytes().size() << '\n'
              << "submaps=" << registry.submap_count()
              << " constraints=" << registry.constraint_count()
              << " graph_version=" << fused.graph_version << '\n';
    return fused ? 0 : 1;
}
