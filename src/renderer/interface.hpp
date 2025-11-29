#pragma once

#include "renderer.hpp"

namespace QNES::renderer {

void InterfaceNewFrame(RendererPlatformBackend &backend);

void InterfaceRender();

void InterfaceDrawDemoWindow();

}  // namespace QNES::renderer