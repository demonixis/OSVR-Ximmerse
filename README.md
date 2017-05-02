# OSVR-Ximmerse
An OSVR plugin for the Ximmerse' Outside/In Controllers.

This plugin provides tracking for 

| Device | Description | Position | Rotation | Buttons |
|--------|-------------|----------|----------|---------|
| X-Hawk | The head blob used for positional tracking | Yes | Yes | No |
| X-Cobra-0 | The left controller | Yes | Yes | Yes |
| X-Cobra-1 | The right controller | Yes | Yes | Yes |

Note: This plugin is still in early stage, pull requests are accepted but I don't provide support for now.

You can build this plugin for Windows (x64) and Mac by following the [standard OSVR plugin build instructions](http://resource.osvr.com/docs/OSVR-Core/TopicWritingDevicePlugin.html).
Linux is not supported because the Ximmerse SDK doesn't provide a library for this system.
