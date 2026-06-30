# DLLinjector



A lightweight Windows DLL injection tool that uses `CreateRemoteThread` and `LoadLibraryW` to load a DLL into a target process's address space. 



## Features

- Inject any DLL into a running Windows process

- Supports Unicode (wide-character) DLL paths

- Lightweight and fast



## Disclaimer



**This tool is intended for security research, malware analysis, and educational purposes only.\*\*



- Do not use this tool on systems without explicit permission from the owner.

- The author is not responsible for any misuse or damage caused by this software.



## Usage



Build and execute:

dllinject.exe [ProcessID] [PathToDLL]

