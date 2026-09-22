#pragma once

/**
 * @file
 * @brief Umbrella header that pulls in the entire engine public API (core, graphics and tilemap modules).
 */

#include "core/circle.hpp"
#include "core/color.hpp"
#include "core/game.hpp"
#include "core/gametime.hpp"
#include "core/log.hpp"
#include "core/point.hpp"
#include "core/ray.hpp"
#include "core/rectangle.hpp"
#include "core/resource.hpp"
#include "core/resourcemanager.hpp"
#include "core/scene.hpp"
#include "core/scenemanager.hpp"
#include "core/shader.hpp"
#include "core/shape.hpp"
#include "core/shapevariant.hpp"
#include "core/texture2d.hpp"
#include "core/texture2dcontainer.hpp"
#include "core/viewport.hpp"
#include "core/window.hpp"

#include "graphics/graphicsdevice.hpp"
#include "graphics/postprocessingeffect.hpp"
#include "graphics/postprocessor.hpp"
#include "graphics/sprite.hpp"
#include "graphics/spriteframe.hpp"
#include "graphics/spritefont.hpp"
#include "graphics/spritesheet.hpp"
#include "graphics/spritestate.hpp"
#include "graphics/spritebatch.hpp"
#include "graphics/vertexpositiontexturecolor.hpp"

#include "tilemap/tile.hpp"
#include "tilemap/tileset.hpp"