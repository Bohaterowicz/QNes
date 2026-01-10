#pragma once

#include "renderer.hpp"

namespace QNes::renderer {

void InterfaceNewFrame(Renderer& renderer);

void InterfaceRender();

void InterfaceDrawBuild(Renderer& renderer);

}  // namespace QNes::renderer