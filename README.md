# assault-cube-cheat


A small Assault Cube cheat for Windows that gives the user "infinite" health and ammunition (integer limit).

Searches the `ac_client.exe` process and reads it's memory via `ReadProcessMemory()` to then write via `WriteProcessMemory()`.
Hard-coded addresses where found using Cheat Engine Version 7.7.


This project was made to learn Windows API and understand low-level concepts.
