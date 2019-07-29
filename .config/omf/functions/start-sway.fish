set -gx XDG_SESSION_TYPE wayland

# Enable QT Wayland
# set -gx QT_QPA_PLATFORM xcb
set -gx QT_QPA_PLATFORM wayland
set -gx QT_WAYLAND_DISABLE_WINDOWDECORATION 1

# Enable GTK+ Wayland
# set -gx GDK_BACKEND wayland
# set -gx CLUTTER_BACKEND wayland

# Set qt theme (Weird I know)
# set -gx XDG_CURRENT_DESKTOP kde
set -gx QT_QPA_PLATFORMTHEME qt5ct

# Java Weirdness
set -gx _JAVA_AWT_WM_NONREPARENTING 1

exec /usr/bin/sway -d 2> /tmp/sway.log
