#pragma once

#include "core/color.hpp"
#include "core/game.hpp"
#include "core/graphicsdevice.hpp"
#include "core/log.hpp"
#include "core/rectangle.hpp"
#include "core/resourcemanager.hpp"
#include "core/shader.hpp"
#include "core/spritebatch.hpp"
#include "core/texture2D.hpp"
#include "core/viewport.hpp"
#include "core/window.hpp"
#include "core/spritesheet.hpp"
#include "core/spriteinfo.hpp"

#include "world/map.hpp"
#include "world/tilelayer.hpp"
#include "world/tileset.hpp"
#include "world/tile.hpp"
#include "world/mapobjects.hpp"
#include "world/tilesetcomponent.hpp"
#include "world/tilelayercomponent.hpp"
#include "world/objectlayer.hpp"
#include "world/character.hpp"

#include "scene/scene.hpp"
#include "scene/entity.hpp"
#include "scene/observer.hpp"

#include "scene/components/childcomponent.hpp"
#include "scene/components/movecomponent.hpp"
#include "scene/components/positioncomponent.hpp"
#include "scene/components/tagcomponent.hpp"
#include "scene/components/cameracomponent.hpp"
#include "scene/components/rendercomponent.hpp"
#include "scene/components/inputcomponent.hpp"
#include "scene/components/spritecomponent.hpp"
#include "scene/components/spriteinfocomponent.hpp"
#include "scene/components/visibilitycomponent.hpp"
#include "scene/components/animationcomponent.hpp"
#include "scene/components/collisioncomponent.hpp"
#include "scene/components/boundingboxcomponent.hpp"