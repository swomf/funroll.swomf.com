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
index cc6108ff..d99c44c6 100644
--- a/src/config/lua/bindings/LuaBindingsConfigRules.cpp
+++ b/src/config/lua/bindings/LuaBindingsConfigRules.cpp
@@ -878,6 +878,20 @@ static int hlGesture(lua_State* L) {
 
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
     Input::ModifierMask modMask = Input::HL_MODIFIER_NONE;
     lua_getfield(L, 1, "mods");
     if (!lua_isnil(L, -1)) {
@@ -942,7 +956,7 @@ static int hlGesture(lua_State* L) {
         const auto& action = actionParser.parsed();
 
         if (action == "workspace")
-            result = g_pTrackpadGestures->addGesture(makeUnique<CWorkspaceSwipeGesture>(), fingerCount, direction, modMask, deltaScale, disableInhibit);
+            result = g_pTrackpadGestures->addGesture(makeUnique<CWorkspaceSwipeGesture>(step), fingerCount, direction, modMask, deltaScale, disableInhibit);
         else if (action == "resize")
             result = g_pTrackpadGestures->addGesture(makeUnique<CResizeTrackpadGesture>(), fingerCount, direction, modMask, deltaScale, disableInhibit);
         else if (action == "move")
diff --git a/src/managers/input/UnifiedWorkspaceSwipeGesture.cpp b/src/managers/input/UnifiedWorkspaceSwipeGesture.cpp
index 2f407790..60d05e93 100644
--- a/src/managers/input/UnifiedWorkspaceSwipeGesture.cpp
+++ b/src/managers/input/UnifiedWorkspaceSwipeGesture.cpp
@@ -1,5 +1,7 @@
 #include "UnifiedWorkspaceSwipeGesture.hpp"
 
+#include <format>
+
 #include "../../Compositor.hpp"
 #include "../../state/WorkspaceState.hpp"
 #include "../../state/workspace/Resolver.hpp"
@@ -22,7 +24,7 @@ bool CUnifiedWorkspaceSwipeGesture::isGestureInProgress() {
     return !!m_workspaceBegin;
 }
 
-void CUnifiedWorkspaceSwipeGesture::begin() {
+void CUnifiedWorkspaceSwipeGesture::begin(int step, std::optional<bool> vertical) {
     if (isGestureInProgress())
         return;
 
@@ -34,6 +36,8 @@ void CUnifiedWorkspaceSwipeGesture::begin() {
 
     LOG(Log::DEBUG, "CUnifiedWorkspaceSwipeGesture::begin: Starting a swipe from {}", PWORKSPACE->displayName());
 
+    m_step           = std::max(1, step);
+    m_vertical       = vertical;
     m_workspaceBegin = PWORKSPACE;
     m_delta          = 0;
     m_monitor        = MONITOR;
@@ -66,15 +70,15 @@ void CUnifiedWorkspaceSwipeGesture::update(double delta) {
     const auto   XDISTANCE     = m_monitor->m_size.x + *PWORKSPACEGAP;
     const auto   YDISTANCE     = m_monitor->m_size.y + *PWORKSPACEGAP;
     const auto   ANIMSTYLE     = m_workspaceBegin->m_renderOffset->getStyle();
-    const bool   VERTANIMS     = ANIMSTYLE == "slidevert" || ANIMSTYLE.starts_with("slidefadevert");
+    const bool   VERTANIMS     = m_vertical.value_or(ANIMSTYLE == "slidevert" || ANIMSTYLE.starts_with("slidefadevert"));
     const double d             = m_delta - delta;
     m_delta                    = delta;
 
     m_avgSpeed = (m_avgSpeed * m_speedPoints + abs(d)) / (m_speedPoints + 1);
     m_speedPoints++;
 
-    auto workspaceLeft  = State::Workspace::resolver()->getWorkspaceTargetFromString((*PSWIPEUSER ? "r-1" : "m-1"));
-    auto workspaceRight = State::Workspace::resolver()->getWorkspaceTargetFromString((*PSWIPEUSER ? "r+1" : "m+1"));
+    auto workspaceLeft  = State::Workspace::resolver()->getWorkspaceTargetFromString(std::format("{}-{}", *PSWIPEUSER || m_step != 1 ? "r" : "m", m_step));
+    auto workspaceRight = State::Workspace::resolver()->getWorkspaceTargetFromString(std::format("{}+{}", *PSWIPEUSER || m_step != 1 ? "r" : "m", m_step));
 
     if (!workspaceLeft.valid() || !workspaceRight.valid() || (sameWorkspaceIdentity(workspaceLeft, m_workspaceBegin) && !*PSWIPENEW)) {
         m_workspaceBegin = nullptr; // invalidate the swipe
@@ -90,7 +94,7 @@ void CUnifiedWorkspaceSwipeGesture::update(double delta) {
     const auto RIGHT_ID = std::get_if<Workspace::SWorkspaceNumberedID>(&*workspaceRight.id);
     if ((sameWorkspaceIdentity(workspaceLeft, m_workspaceBegin) && *PSWIPENEW && (m_delta < 0)) ||
         (m_delta > 0 && m_workspaceBegin->getWindowCount() == 0 && BEGIN_ID && RIGHT_ID && RIGHT_ID->value <= *BEGIN_ID) ||
-        (m_delta < 0 && BEGIN_ID && LEFT_ID && *BEGIN_ID <= LEFT_ID->value)) {
+        (m_delta < 0 && BEGIN_ID && LEFT_ID && *BEGIN_ID <= LEFT_ID->value) || (m_delta < 0 && m_step > 1 && BEGIN_ID && m_step >= *BEGIN_ID)) {
 
         m_delta = 0;
         g_pHyprRenderer->damageMonitor(m_monitor.lock());
@@ -194,7 +198,7 @@ void CUnifiedWorkspaceSwipeGesture::update(double delta) {
     if (*PSWIPEFOREVER) {
         if (abs(m_delta) >= SWIPEDISTANCE) {
             end();
-            begin();
+            begin(m_step, m_vertical);
         }
     }
 }
@@ -210,11 +214,11 @@ void CUnifiedWorkspaceSwipeGesture::end() {
     static auto PSWIPEUSER    = CConfigValue<Config::INTEGER>("gestures:workspace_swipe_use_r");
     static auto PWORKSPACEGAP = CConfigValue<Config::INTEGER>("general:gaps_workspaces");
     const auto  ANIMSTYLE     = m_workspaceBegin->m_renderOffset->getStyle();
-    const bool  VERTANIMS     = ANIMSTYLE == "slidevert" || ANIMSTYLE.starts_with("slidefadevert");
+    const bool  VERTANIMS     = m_vertical.value_or(ANIMSTYLE == "slidevert" || ANIMSTYLE.starts_with("slidefadevert"));
 
     // commit
-    auto       workspaceLeft  = State::Workspace::resolver()->getWorkspaceTargetFromString((*PSWIPEUSER ? "r-1" : "m-1"));
-    auto       workspaceRight = State::Workspace::resolver()->getWorkspaceTargetFromString((*PSWIPEUSER ? "r+1" : "m+1"));
+    auto       workspaceLeft  = State::Workspace::resolver()->getWorkspaceTargetFromString(std::format("{}-{}", *PSWIPEUSER || m_step != 1 ? "r" : "m", m_step));
+    auto       workspaceRight = State::Workspace::resolver()->getWorkspaceTargetFromString(std::format("{}+{}", *PSWIPEUSER || m_step != 1 ? "r" : "m", m_step));
     const auto SWIPEDISTANCE  = std::clamp(*PSWIPEDIST, sc<int64_t>(1LL), sc<int64_t>(UINT32_MAX));
 
     // If we've been swiping off the right end with PSWIPENEW enabled, there is
@@ -222,7 +226,7 @@ void CUnifiedWorkspaceSwipeGesture::end() {
     const auto BEGIN_ID = m_workspaceBegin->numberedID();
     const auto RIGHT_ID = workspaceRight.id ? std::get_if<Workspace::SWorkspaceNumberedID>(&*workspaceRight.id) : nullptr;
     if (BEGIN_ID && RIGHT_ID && RIGHT_ID->value <= *BEGIN_ID && *PSWIPENEW)
-        workspaceRight = State::Workspace::resolver()->getWorkspaceTargetFromString("r+1");
+        workspaceRight = State::Workspace::resolver()->getWorkspaceTargetFromString(std::format("r+{}", m_step));
 
     if (!workspaceLeft.valid() || !workspaceRight.valid()) {
         m_workspaceBegin->m_renderOffset->setValueAndWarp({});
@@ -241,7 +245,9 @@ void CUnifiedWorkspaceSwipeGesture::end() {
 
     PHLWORKSPACE pSwitchedTo = nullptr;
 
-    if ((abs(m_delta) < SWIPEDISTANCE * *PSWIPEPERC && (*PSWIPEFORC == 0 || (*PSWIPEFORC != 0 && m_avgSpeed < *PSWIPEFORC))) || abs(m_delta) < 2) {
+    // Reject grid jumps below workspace 1 instead of accepting the resolver's clamped target.
+    if ((m_delta < 0 && m_step > 1 && BEGIN_ID && m_step >= *BEGIN_ID) ||
+        (abs(m_delta) < SWIPEDISTANCE * *PSWIPEPERC && (*PSWIPEFORC == 0 || (*PSWIPEFORC != 0 && m_avgSpeed < *PSWIPEFORC))) || abs(m_delta) < 2) {
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
index ead116a7..23bcd1f3 100644
--- a/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.cpp
+++ b/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.cpp
@@ -1,5 +1,7 @@
 #include "WorkspaceSwipeGesture.hpp"
 
+#include <optional>
+
 #include "../../../../Compositor.hpp"
 #include "../../../../state/WorkspaceState.hpp"
 #include "../../../../desktop/state/FocusState.hpp"
@@ -7,6 +9,10 @@
 
 #include "../../UnifiedWorkspaceSwipeGesture.hpp"
 
+CWorkspaceSwipeGesture::CWorkspaceSwipeGesture(int step) : m_step(step) {
+    ;
+}
+
 void CWorkspaceSwipeGesture::begin(const ITrackpadGesture::STrackpadGestureBegin& e) {
     ITrackpadGesture::begin(e);
 
@@ -24,7 +30,15 @@ void CWorkspaceSwipeGesture::begin(const ITrackpadGesture::STrackpadGestureBegin
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
index 203fc329..8561367b 100644
--- a/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.hpp
+++ b/src/managers/input/trackpad/gestures/WorkspaceSwipeGesture.hpp
@@ -5,7 +5,8 @@
 
 class CWorkspaceSwipeGesture : public ITrackpadGesture {
   public:
-    CWorkspaceSwipeGesture()          = default;
+    // step: how many workspace IDs a full swipe jumps. 1 = classic adjacent swipe, >1 = grid row/column jump.
+    CWorkspaceSwipeGesture(int step = 1);
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

I use the latest hyprland git cuz I'd rather risk having new bugs than having old ones.
So I added this patch (stuff broke when i updated to 92b82c0c1e4168d93903ec42a2276843bbd84821).

```bash path=/etc/portage/patches/gui-wm/hyprland/gcc15-string-subview.patch
diff --git a/src/ipc/s1/S1.cpp b/src/ipc/s1/S1.cpp
--- a/src/ipc/s1/S1.cpp
+++ b/src/ipc/s1/S1.cpp
@@ -8,6 +8,7 @@
 #include <optional>
 #include <ranges>
 #include <sstream>
+#include <string_view>
 #include <hyprutils/string/String.hpp>
 
 using namespace IPC::Socket1;
@@ -145,7 +146,7 @@
             if (i < request.size() && (request[i + 1] == '\\' || request[i + 1] == ';'))
                 ++i;
             else
-                LOG(Log::ERR, "Malformed socket1 request: invalid escape sequence {} at position {}, using it verbatim", request.subview(i, 2), i);
+                LOG(Log::ERR, "Malformed socket1 request: invalid escape sequence {} at position {}, using it verbatim", std::string_view{request}.substr(i, 2), i);
         }
         parsedCommand << request[i];
     }
```

```bash path=/etc/portage/package.accept_keywords/hyprland
gui-wm/hyprland **
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
