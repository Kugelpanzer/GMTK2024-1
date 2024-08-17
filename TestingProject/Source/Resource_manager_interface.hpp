#pragma once

#include "Engine.hpp"

#include <string>

class ResourceManagerInterface : public spe::ContextComponent {
public:
    ~ResourceManagerInterface() override = default;

private:
    SPEMPE_CTXCOMP_TAG("ResourceManagerInterface");
};

using MResource = ResourceManagerInterface;
