#pragma once

static constexpr bool renderable =
#ifdef RENDERABLE
  true
#else
  false
#endif
;