# Copilot Instructions

## Project Guidelines
- User prefers classic vcpkg package installation over manifest mode for this C++ project.
- Use classic vcpkg commands and avoid assuming package feature names without verification.
- User prefers not to place generic rendering abstractions like entity rendering helpers inside `PlayState`; if the engine does not support the abstraction properly yet, defer it until a dedicated system is added.
- User prefers movement and controller logic to be kept out of `PlayState::Update` when possible; avoid clunky state update functions with too much gameplay logic.

## User Interface Preferences
- For menu navigation, use mouse movement to keep the current keyboard selection unless the cursor actually hovers a different menu item.