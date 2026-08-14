# desktop

Below are some utilities I edit to make my desktop comfortable.

1. I set up gui-wm/hyprland, a window manager that uses the
Wayland display protocol.
2. I set up the Blackbriar-theme.
3. I set up Aylur's GTK Shell, which handles notifications and the bar
with little RAM and easy TypeScript.
4. I set up file managers and thumbnailing: app-misc/nnn and gnome-extra/nemo.

The main Gentoo-specific advantageous change I make is the integration of
an internal hyprland gesture patch I use.

## 1. hyprland

I use gui-wm/hyprland (uses the Wayland secure display protocol),
since animations make tiling WMs more accessible
(i.e. visual responsivity). One important such feature is the
progress-of-the-gesture-is-distance-in-the-animation. But I like to
have both swiping up and swiping left/right, in a row-major basically-mod-20
workspace setup (a diy grid).

<details>
<summary>Click to enlarge. It's a bit long.</summary>

```bash path=/etc/portage/patches/gui-wm/hyprland/grid-swipe.patch
diff --git a/src/config/lua/bindings/LuaBindingsConfigRules.cpp b/src/config/lua/bindings/LuaBindingsConfigRules.cpp
index 8095b170..011c25fb 100644
--- a/src/config/lua/bindings/LuaBindingsConfigRules.cpp
+++ b/src/config/lua/bindings/LuaBindingsConfigRules.cpp
@@ -877,6 +877,20 @@ static int hlGesture(lua_State* L) {
 
 #undef GET_ACTION_STRING
 
+    // "workspace" action: how many workspace IDs a full swipe jumps (grid row/column size). Default 1 = adjacent.
+    int step = 1;
+    lua_getfield(L, 1, "step");
+    if (!lua_isnil(L, -1)) {
+        CLuaConfigInt stepParser(1, 1, 1000);
+        auto          stepErr = stepParser.parse(L);
+        if (stepErr.errorCode != PARSE_ERROR_OK) {
+            lua_pop(L, 1);
+            return Internal::configError(L, std::format("hl.gesture: field \"step\": {}", stepErr.message));
+        }
+        step = stepParser.parsed();
+    }
+    lua_pop(L, 1);
+
     uint32_t modMask = 0;
     lua_getfield(L, 1, "mods");
     if (!lua_isnil(L, -1)) {
@@ -935,7 +949,7 @@ static int hlGesture(lua_State* L) {
         const auto& action = actionParser.parsed();
 
         if (action == "workspace")
-            result = g_pTrackpadGestures->addGesture(makeUnique<CWorkspaceSwipeGesture>(), fingerCount, direction, modMask, deltaScale, disableInhibit);
+            result = g_pTrackpadGestures->addGesture(makeUnique<CWorkspaceSwipeGesture>(step), fingerCount, direction, modMask, deltaScale, disableInhibit);
         else if (action == "resize")
             result = g_pTrackpadGestures->addGesture(makeUnique<CResizeTrackpadGesture>(), fingerCount, direction, modMask, deltaScale, disableInhibit);
         else if (action == "move")
diff --git a/src/managers/input/UnifiedWorkspaceSwipeGesture.cpp b/src/managers/input/UnifiedWorkspaceSwipeGesture.cpp
index f58c1869..681c522e 100644
--- a/src/managers/input/UnifiedWorkspaceSwipeGesture.cpp
+++ b/src/managers/input/UnifiedWorkspaceSwipeGesture.cpp
@@ -1,5 +1,7 @@
 #include "UnifiedWorkspaceSwipeGesture.hpp"
 
+#include <format>
+
 #include "../../Compositor.hpp"
 #include "../../state/WorkspaceState.hpp"
 #include "../../desktop/state/FocusState.hpp"
@@ -13,7 +15,7 @@ bool CUnifiedWorkspaceSwipeGesture::isGestureInProgress() {
     return !!m_workspaceBegin;
 }
 
-void CUnifiedWorkspaceSwipeGesture::begin() {
+void CUnifiedWorkspaceSwipeGesture::begin(int step, std::optional<bool> vertical) {
     if (isGestureInProgress())
         return;
 
@@ -21,6 +23,8 @@ void CUnifiedWorkspaceSwipeGesture::begin() {
 
     Log::logger->log(Log::DEBUG, "CUnifiedWorkspaceSwipeGesture::begin: Starting a swipe from {}", PWORKSPACE->m_name);
 
+    m_step           = std::max(1, step);
+    m_vertical       = vertical;
     m_workspaceBegin = PWORKSPACE;
     m_delta          = 0;
     m_monitor        = Desktop::focusState()->monitor();
@@ -53,15 +57,19 @@ void CUnifiedWorkspaceSwipeGesture::update(double delta) {
     const auto   XDISTANCE     = m_monitor->m_size.x + *PWORKSPACEGAP;
     const auto   YDISTANCE     = m_monitor->m_size.y + *PWORKSPACEGAP;
     const auto   ANIMSTYLE     = m_workspaceBegin->m_renderOffset->getStyle();
-    const bool   VERTANIMS     = ANIMSTYLE == "slidevert" || ANIMSTYLE.starts_with("slidefadevert");
+    const bool   VERTANIMS     = m_vertical.value_or(ANIMSTYLE == "slidevert" || ANIMSTYLE.starts_with("slidefadevert"));
     const double d             = m_delta - delta;
     m_delta                    = delta;
 
     m_avgSpeed = (m_avgSpeed * m_speedPoints + abs(d)) / (m_speedPoints + 1);
     m_speedPoints++;
 
-    auto workspaceIDLeft  = getWorkspaceIDNameFromString((*PSWIPEUSER ? "r-1" : "m-1")).id;
-    auto workspaceIDRight = getWorkspaceIDNameFromString((*PSWIPEUSER ? "r+1" : "m+1")).id;
+    // For grid jumps (m_step > 1) we address workspaces by numeric id (r+-step); otherwise keep the classic
+    // monitor-relative adjacent behavior (m±1) unless workspace_swipe_use_r is set.
+    const bool USEREAL              = *PSWIPEUSER || m_step != 1;
+    auto       workspaceIDLeft      = getWorkspaceIDNameFromString(std::format("{}-{}", USEREAL ? "r" : "m", m_step)).id;
+    auto       workspaceIDRight     = getWorkspaceIDNameFromString(std::format("{}+{}", USEREAL ? "r" : "m", m_step)).id;
+    const bool OVERSHOOTSLOWERBOUND = m_step > 1 && m_workspaceBegin->m_id > 0 && m_step >= m_workspaceBegin->m_id;
 
     if ((workspaceIDLeft == WORKSPACE_INVALID || workspaceIDRight == WORKSPACE_INVALID || workspaceIDLeft == m_workspaceBegin->m_id) && !*PSWIPENEW) {
         m_workspaceBegin = nullptr; // invalidate the swipe
@@ -73,7 +81,9 @@ void CUnifiedWorkspaceSwipeGesture::update(double delta) {
     m_delta = std::clamp(m_delta, sc<double>(-SWIPEDISTANCE), sc<double>(SWIPEDISTANCE));
 
     if ((m_workspaceBegin->m_id == workspaceIDLeft && *PSWIPENEW && (m_delta < 0)) ||
-        (m_delta > 0 && m_workspaceBegin->getWindowCount() == 0 && workspaceIDRight <= m_workspaceBegin->m_id) || (m_delta < 0 && m_workspaceBegin->m_id <= workspaceIDLeft)) {
+        (m_delta > 0 && m_workspaceBegin->getWindowCount() == 0 && workspaceIDRight <= m_workspaceBegin->m_id) || (m_delta < 0 && m_workspaceBegin->m_id <= workspaceIDLeft) ||
+        // r-N resolves an overshoot to workspace 1, so compare the requested step with the starting ID instead.
+        (m_delta < 0 && OVERSHOOTSLOWERBOUND)) {
 
         m_delta = 0;
         g_pHyprRenderer->damageMonitor(m_monitor.lock());
@@ -193,17 +203,19 @@ void CUnifiedWorkspaceSwipeGesture::end() {
     static auto PSWIPEUSER    = CConfigValue<Config::INTEGER>("gestures:workspace_swipe_use_r");
     static auto PWORKSPACEGAP = CConfigValue<Config::INTEGER>("general:gaps_workspaces");
     const auto  ANIMSTYLE     = m_workspaceBegin->m_renderOffset->getStyle();
-    const bool  VERTANIMS     = ANIMSTYLE == "slidevert" || ANIMSTYLE.starts_with("slidefadevert");
+    const bool  VERTANIMS     = m_vertical.value_or(ANIMSTYLE == "slidevert" || ANIMSTYLE.starts_with("slidefadevert"));
 
     // commit
-    auto       workspaceIDLeft  = getWorkspaceIDNameFromString((*PSWIPEUSER ? "r-1" : "m-1")).id;
-    auto       workspaceIDRight = getWorkspaceIDNameFromString((*PSWIPEUSER ? "r+1" : "m+1")).id;
-    const auto SWIPEDISTANCE    = std::clamp(*PSWIPEDIST, sc<int64_t>(1LL), sc<int64_t>(UINT32_MAX));
+    const bool USEREAL              = *PSWIPEUSER || m_step != 1;
+    auto       workspaceIDLeft      = getWorkspaceIDNameFromString(std::format("{}-{}", USEREAL ? "r" : "m", m_step)).id;
+    auto       workspaceIDRight     = getWorkspaceIDNameFromString(std::format("{}+{}", USEREAL ? "r" : "m", m_step)).id;
+    const auto SWIPEDISTANCE        = std::clamp(*PSWIPEDIST, sc<int64_t>(1LL), sc<int64_t>(UINT32_MAX));
+    const bool OVERSHOOTSLOWERBOUND = m_step > 1 && m_workspaceBegin->m_id > 0 && m_step >= m_workspaceBegin->m_id;
 
     // If we've been swiping off the right end with PSWIPENEW enabled, there is
     // no workspace there yet, and we need to choose an ID for a new one now.
     if (workspaceIDRight <= m_workspaceBegin->m_id && *PSWIPENEW)
-        workspaceIDRight = getWorkspaceIDNameFromString("r+1").id;
+        workspaceIDRight = getWorkspaceIDNameFromString(std::format("r+{}", m_step)).id;
 
     auto         PWORKSPACER = State::workspaceState()->query().id(workspaceIDRight).run(); // not guaranteed if PSWIPENEW || PSWIPENUMBER
     auto         PWORKSPACEL = State::workspaceState()->query().id(workspaceIDLeft).run();  // not guaranteed if PSWIPENUMBER
@@ -214,7 +226,9 @@ void CUnifiedWorkspaceSwipeGesture::end() {
 
     PHLWORKSPACE pSwitchedTo = nullptr;
 
-    if ((abs(m_delta) < SWIPEDISTANCE * *PSWIPEPERC && (*PSWIPEFORC == 0 || (*PSWIPEFORC != 0 && m_avgSpeed < *PSWIPEFORC))) || abs(m_delta) < 2) {
+    // r-N resolves an overshoot to workspace 1, so force a revert based on the requested step instead.
+    if ((m_delta < 0 && OVERSHOOTSLOWERBOUND) || (abs(m_delta) < SWIPEDISTANCE * *PSWIPEPERC && (*PSWIPEFORC == 0 || (*PSWIPEFORC != 0 && m_avgSpeed < *PSWIPEFORC))) ||
+        abs(m_delta) < 2) {
         // revert
         if (abs(m_delta) < 2) {
             if (PWORKSPACEL)
diff --git a/src/managers/input/UnifiedWorkspaceSwipeGesture.hpp b/src/managers/input/UnifiedWorkspaceSwipeGesture.hpp
index 4dbb6c5d..73609572 100644
--- a/src/managers/input/UnifiedWorkspaceSwipeGesture.hpp
+++ b/src/managers/input/UnifiedWorkspaceSwipeGesture.hpp
@@ -1,25 +1,33 @@
 #pragma once
 
+#include <optional>
+
 #include "../../helpers/memory/Memory.hpp"
 #include "../../desktop/DesktopTypes.hpp"
 
 class CUnifiedWorkspaceSwipeGesture {
   public:
-    void begin();
+    // step: how many workspace IDs a full swipe jumps (1 = classic adjacent swipe, >1 = grid row/column jump).
+    // vertical: force the swipe axis (true = vertical, false = horizontal). If unset, falls back to the global
+    //           `workspaces` animation style (slidevert -> vertical), preserving legacy behavior.
+    void begin(int step = 1, std::optional<bool> vertical = std::nullopt);
     void update(double delta);
     void end();
 
     bool isGestureInProgress();
 
   private:
-    PHLWORKSPACE  m_workspaceBegin = nullptr;
-    PHLMONITORREF m_monitor;
-
-    double        m_delta            = 0;
-    int           m_initialDirection = 0;
-    float         m_avgSpeed         = 0;
-    int           m_speedPoints      = 0;
-    int           m_touchID          = 0;
+    PHLWORKSPACE        m_workspaceBegin = nullptr;
+    PHLMONITORREF       m_monitor;
+
+    int                 m_step     = 1;
+    std::optional<bool> m_vertical = std::nullopt;
+
+    double              m_delta            = 0;
+    int                 m_initialDirection = 0;
+    float               m_avgSpeed         = 0;
+    int                 m_speedPoints      = 0;
+    int                 m_touchID          = 0;
 
     friend class CWorkspaceSwipeGesture;
     friend class CInputManager;
diff --git a/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.cpp b/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.cpp
index f807e773..fc50a11f 100644
--- a/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.cpp
+++ b/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.cpp
@@ -1,5 +1,7 @@
 #include "WorkspaceSwipeGesture.hpp"
 
+#include <optional>
+
 #include "../../../../Compositor.hpp"
 #include "../../../../state/WorkspaceState.hpp"
 #include "../../../../desktop/state/FocusState.hpp"
@@ -24,7 +26,15 @@ void CWorkspaceSwipeGesture::begin(const ITrackpadGesture::STrackpadGestureBegin
     if (onMonitor < 2 && !*PSWIPENEW)
         return; // disallow swiping when there's 1 workspace on a monitor
 
-    g_pUnifiedWorkspaceSwipe->begin();
+    // Force the animation axis to match the actual swipe direction, so a vertical grid swipe slides
+    // vertically regardless of the global `workspaces` animation style (and horizontal stays horizontal).
+    std::optional<bool> vertical;
+    if (e.direction == TRACKPAD_GESTURE_DIR_UP || e.direction == TRACKPAD_GESTURE_DIR_DOWN)
+        vertical = true;
+    else if (e.direction == TRACKPAD_GESTURE_DIR_LEFT || e.direction == TRACKPAD_GESTURE_DIR_RIGHT)
+        vertical = false;
+
+    g_pUnifiedWorkspaceSwipe->begin(m_step, vertical);
 }
 
 void CWorkspaceSwipeGesture::update(const ITrackpadGesture::STrackpadGestureUpdate& e) {
diff --git a/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.hpp b/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.hpp
index 203fc329..8d826296 100644
--- a/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.hpp
+++ b/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.hpp
@@ -5,7 +5,8 @@
 
 class CWorkspaceSwipeGesture : public ITrackpadGesture {
   public:
-    CWorkspaceSwipeGesture()          = default;
+    // step: how many workspace IDs a full swipe jumps. 1 = classic adjacent swipe, >1 = grid row/column jump.
+    CWorkspaceSwipeGesture(int step = 1) : m_step(step) {}
     virtual ~CWorkspaceSwipeGesture() = default;
 
     virtual void begin(const ITrackpadGesture::STrackpadGestureBegin& e);
@@ -13,4 +14,7 @@ class CWorkspaceSwipeGesture : public ITrackpadGesture {
     virtual void end(const ITrackpadGesture::STrackpadGestureEnd& e);
 
     virtual bool isDirectionSensitive();
+
+  private:
+    int m_step = 1;
 };
```

</details>

I use unstable hyprland cuz I'd rather risk having new bugs than having old ones.

```bash path=/etc/portage/package.accept_keywords/hyprland
gui-wm/hyprland ~amd64
dev-libs/wayland-protocols ~amd64
# required by xdg-desktop-portal-hyprland
dev-cpp/sdbus-c++ ~amd64
```

Btw make sure you're on gcc 15 to avoid `class std::vector<unsigned char> has no member named append_range`

## 2. Blackbriar theme

The only Gentoo-specific switch I need to hit is

```bash path=/etc/portage/package.accept_keywords/kvantum
x11-themes/kvantum ~amd64
```

since it makes QT6 apps look more standardized to my theme.
Other than that, everything in
[Blackbriar-theme ⇗](https://github.com/swomf/Blackbriar-theme)
is distro-agnostic.

## 3. aylur's gtk shell

TODO: mention that i patched ags at
https://github.com/swomf/overlay-funroll/blob/main/gui-libs/astal-hyprland/files/hyprwm-v0.55-lua.patch

For a while I'd used ags v2.3.0 way past the due date.
Just to use ags v2.3.0 after the Hyprland v0.55 breaking Lua changes, I'd even
[written a basic ags backport ⇗](https://github.com/swomf/funroll.swomf.com/blob/6caa5ab39177ea652a3f045cc0574d42482b14b7/content/conf/desktop.md?plain=1#L1642-L1728).
I want to make my bar and leave it there forever. But I finally
got around to updating and fixing the package management of
ags in the ::funroll overlay. Harhar.

Also I don't use quickshell; here's the
[boring comparison ⇗](https://github.com/swomf/dotfiles/commit/c0f895a82b015cb4a215d883f20bb44e6f51eb1f)
that made me use ags instead.

Aylur's GTK Shell covers my bar and notification daemon, and will
probably eventually subsume my app launchers and dmenu-like apps (i.e. anyrun).
It's expressive, minimal, doesn't need a domain-specific language (ewww),
and has pleasant TypeScript GTK support.

```bash path=/etc/portage/package.use/ags
# I use ::funroll libcava since guru doesn't have
# libcava 1.0 at the time of writing (2026-07-25)
gui-libs/astal-meta apps auth battery bluetooth brightness
gui-libs/astal-meta cava gjs greetd gtk3 gtk4 hyprland mpris
gui-libs/astal-meta network notifd powerprofiles river tray wireplumber
gui-libs/astal-gjs gtk4
dev-lang/vala valadoc
# network USE flag deps
net-libs/libnma vala
net-misc/networkmanager vala
# gtk USE flag deps
gui-libs/gtk4-layer-shell vala
gui-libs/gtk-layer-shell vala
```

```bash path=/etc/portage/package.accept_keywords/ags
gui-libs/gtk4-layer-shell ~amd64
```

## 4. file manager

<div class="ml-3">
  <span class="bright">Aside.</span> Frankly I don't really use these.
  If I ever have to click and drag things, I use `ripdrag`, which I
  installed via `cargo install`.
</div>


For a file manager I use
app-misc/nnn (terminal-based) and
gnome-extra/nemo (gui-based) over competitors:

* xfce-base/thunar needs xfce-base/tumbler for thumbnails, which
turns a minimal app into a non-minimal megabyte eater
* x11-misc/pcmanfm isn't very maintained -- e.g. no
wayland trackpad scroll. [bug 1131 ⇗](https://sourceforge.net/p/pcmanfm/bugs/1131/)

Setting up app-misc/nnn thumbnailing is done via
a [single-file plugin ⇗](https://github.com/swomf/dotfiles/blob/gentoo/gentoo/config/nnn/plugins/preview-tui)
and requires the Kitty terminal (or any terminal supporting
the Kitty image protocol).

Setting up good Nemo thumbnailing is a bit manual.

- To thumbnail media larger than 1MB on Nemo, you
need to go into Preferences -> Preview -> Only for files smaller than...?
and increase the limit.
- media-video/ffmpegthumbnailer is responsible for video thumbnails.
- x11-libs/gdk-pixbuf and gui-libs/gdk-pixbuf-loader-webp
are responsible for image thumbnails.

```bash path=/etc/portage/package.use/filemanager
# nnn
app-misc/nnn nerdfonts
# nemo and dependencies
gnome-extra/nemo exif
dev-libs/libdbusmenu gtk3
# nemo thumbnailing
media-video/ffmpegthumbnailer gnome gtk jpeg png
x11-libs/gdk-pixbuf jpeg gif tiff
```
