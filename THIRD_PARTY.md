# Third-Party Libraries Used

This project uses the following open-source libraries:

1. [cpp-httplib](https://github.com/yhirose/cpp-httplib) — MIT License  
   - Copyright (c) 2016-2023 Yuta Yoshioka  
   - Purpose: Provides a simple, single-header HTTP/HTTPS client and server for networking features in the interpreter.

2. [nlohmann/json](https://github.com/nlohmann/json) — MIT License  
   - Copyright (c) 2013-2023 Niels Lohmann  
   - Purpose: Provides JSON serialization and parsing support for data exchange and configuration.

3. [Dear ImGui](https://github.com/ocornut/imgui) — MIT License  
   - Copyright (c) 2014-2023 Omar Cornut  
   - Files included in this project:
     - Core: `imgui.h`, `imgui.cpp`, `imgui_draw.cpp`, `imgui_widgets.cpp`, `imgui_tables.cpp`, `imgui_internal.h`, `imconfig.h`
     - Backends: `imgui_impl_glfw.h`, `imgui_impl_glfw.cpp`, `imgui_impl_opengl3.h`, `imgui_impl_opengl3.cpp`
   - Purpose: Provides the GUI framework for the interpreter, including windows, buttons, labels, sliders, checkboxes, and other UI elements.
   - Notes: 
     - `imconfig.h` is required by ImGui but can be empty.  
     - Backends are configured for GLFW + OpenGL3.  
     - To update, replace the above files with the latest versions from the ImGui repository and ensure `imconfig.h` exists.