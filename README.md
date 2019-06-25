# qttools: Assistant/WebEngine

This is a fork of the official qttools repository for the sole purpose of compiling Qt Assistant against a stock binary distribution of Qt, without needing to build Qt itself. Assistant is patched to work with Qt WebEngine to give it full HTML browsing capability. The changes in this repository are additive in nature so it should be easy to keep in sync with upstream.

## Compilation using Qt Creator

<ol>
<li> Switch to the <code>5.13_assistant_webengine</code> branch.
<li> Using a stock distribution of the Qt SDK (5.13.0 is known to work) open the file <code>src/assistant/assistant/assistant_local.pro</code> in Qt Creator.
<li> In Projects, select the appropriate Qt Kit if you have multiple installed.
<li> In Build Settings, for Debug and Release individually, Build Steps, add this Custom Process Step (shown for Windows):<br><br>

Field              | Content
------------------ | -------------
Command:           | windeployqt
Arguments:         | assistant.exe
Working directory: | %{buildDir}\bin

This will copy the necessary shared libraries so that Assistant will run from the <code>bin</code> subdirectory inside the build directory.
</ol>
