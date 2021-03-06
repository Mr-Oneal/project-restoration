#include "game/camera.h"

#include "common/context.h"
#include "common/debug.h"
#include "common/utils.h"
#include "game/context.h"
#include "game/pad.h"

namespace game {

u32 Camera::ChangeMode(CameraMode mode, u32 unknown) {
  return rst::util::GetPointer<u32(Camera*, CameraMode, u32)>(0x18B31C)(this, mode, unknown);
}

static bool IsMovingCStick(const pad::State& state) {
  return (state.c_stick.x_raw_last * state.c_stick.x_raw_last +
          state.c_stick.y_raw_last * state.c_stick.y_raw_last) >= 5.198f;
}

void CalcCamera() {
  // Add an extra free cam switch trigger for the ocarina
  auto* states = rst::util::GetPointer<CameraStateInfo>(0x667894);
  auto& ocarina_state = states[size_t(CameraState::ITEM1)];

  static bool s_initialised = false;
  if (!s_initialised) {
    auto& free_cam_mode = (*ocarina_state.modes)[size_t(CameraMode::FREECAMERA)];
    free_cam_mode.handler_fn_idx = 0x3D;
    free_cam_mode.field_2 = 2;
    free_cam_mode.field_4 = 0x6645C8;
    s_initialised = true;
  }

  auto* gctx = rst::GetContext().gctx;
  auto* camera = &gctx->main_camera + gctx->camera_idx;
  const bool using_ocarina = gctx->camera_idx == 1 && camera->state == CameraState::ITEM1;
  if (using_ocarina) {
    // Only enable free camera mode when needed to ensure that the camera goes into
    // mode NORMAL when starting to play the ocarina.
    ocarina_state.allowed_modes.Set(CameraMode::FREECAMERA);
    if (camera->mode == CameraMode::NORMAL && IsMovingCStick(gctx->pad_state))
      camera->ChangeMode(CameraMode::FREECAMERA);
  } else {
    ocarina_state.allowed_modes.Clear(CameraMode::FREECAMERA);
  }
}

}  // namespace game

extern "C" {
using namespace game;
RST_HOOK bool rst_ShouldSwitchToFreeCam(Camera* self, CameraMode mode, u32) {
  constexpr CameraMode extra_freecam_modes[] = {
      CameraMode::FREECAMERA,
      CameraMode::TALK,
      CameraMode::KEEPON,
      CameraMode::PARALLEL,
  };
  if (!rst::util::Contains(extra_freecam_modes, mode))
    return false;

  // Keep free camera mode.
  // ...but not if the player wants to reset the camera.
  const pad::State& state = rst::GetContext().gctx->pad_state;
  if (state.input.new_buttons.IsSet(pad::Button::L))
    return false;
  if (self->mode == CameraMode::FREECAMERA)
    return true;

  // Otherwise, enter free camera mode only if the C stick is moved.
  return IsMovingCStick(state);
}
}
