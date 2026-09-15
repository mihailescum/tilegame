#pragma once

namespace engine
{
    /**
     * @brief Timing snapshot passed to update()/draw() calls each frame.
     */
    struct GameTime
    {
        float elapsed_time; ///< Time elapsed since the previous update/draw call, in seconds.
        float total_elapsed_time; ///< Total time elapsed since the game started running, in seconds.
    };
}