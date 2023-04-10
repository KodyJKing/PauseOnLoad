A little DLL that pauses all threads until the user decides to proceed.

Intended to allow debuggers to attach to programs before early library loading and initialization. I use this to inject RenderDoc into games that need to be launched through Steam before they load D3D libraries.

Use with MSDetours' setdll.exe.