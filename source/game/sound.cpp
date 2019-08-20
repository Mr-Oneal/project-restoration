#include "game/sound.h"

#include <tuple>

#include "common/utils.h"
#include "game/actor.h"

namespace game::sound {

class StreamMgr;

static StreamMgr& GetStreamMgr() {
  return rst::util::GetInstance<StreamMgr>(std::tuple{0x7CB49C}, std::tuple{0x6B0A3C},
                                           std::tuple{0x1E11F8});
}

bool PlayEffect(EffectId id) {
  return rst::util::GetPointer<bool(EffectId)>(0x2006E4)(id);
}

bool PlayEffect(const Vec3& position, EffectId id) {
  return rst::util::GetPointer<bool(const Vec3&, EffectId)>(0x226218)(position, id);
}

bool PlayEffect(const act::Actor& actor, EffectId id) {
  return PlayEffect(actor.position, id);
}

StreamId GetCurrentStreamId(StreamPlayer player) {
  return rst::util::GetPointer<StreamId(StreamMgr&, StreamPlayer)>(0x1E1194)(GetStreamMgr(),
                                                                             player);
}

bool PlayStream(StreamId id, StreamPlayer player) {
  return rst::util::GetPointer<bool(StreamMgr&, StreamId, StreamPlayer, u32)>(0x239228)(
      GetStreamMgr(), id, player, 0xffffffff);
}

void ControlStream(StreamPlayer player, int a, int b) {
  rst::util::GetPointer<void(StreamMgr&, StreamPlayer, int, int)>(0x1DC2F0)(GetStreamMgr(), player,
                                                                            a, b);
}

}  // namespace game::sound
