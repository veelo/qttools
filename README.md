# qttools: Assistant/WebEngine

This is a fork of the official qttools repository for the sole purpose of compiling Qt Assistant against a stock binary distribution of Qt, without needing to build Qt itself. Assistant is patched to work with Qt WebEngine to give it full HTML browsing capability. The changes in this repository are additive in nature, but due to changes upstream it may be difficult to maintain in the future.

## Compilation using Qt Creator

<ol>
<li> Switch to the <code>6.1.3_compile_against_6.2.4_assistant_webengine</code> branch.
  <li> Using a stock distribution of the Qt SDK <i>with WebEngine support</i> (6.2.4 is known to work) open the file <code>CMakeLists.txt</code> in Qt Creator.
<li> In Projects, select the appropriate Qt Kit if you have multiple installed.
<li> In Build Settings, for Debug and Release individually, Build Steps, add this Custom Process Step (shown for Windows):<br><br>

Field              | Content
------------------ | -------------
Command:           | windeployqt
Arguments:         | assistant.exe
Working directory: | %{buildDir}\bin

This will copy the necessary shared libraries so that Assistant will run from the <code>bin</code> subdirectory inside the build directory.
</ol>

## Compilation using cmake
<ol>
<li> Switch to the <code>6.1.3_compile_against_6.2.4_assistant_webengine</code> branch.
<li> Ensure you have appropriate cmake version (3.15+).
<li> Ensure you have the correct version of Qt configured, or pass in on cmake command line.
<li> Set up to use appropriate compiler (e.g. scl enable, or vcvars64.bat).
<li> Create build directory and change to it. (E.g. as a subdirectory of qttools)
<li> For Windows: 
<ol>
  <li> Run cmake: cmake .. -DCMAKE_PREFIX_PATH=p:\qt\qt-opensource-6.2.4\win64_vc142 -DCMAKE_GENERATOR=Ninja -DCMAKE_BUILD_TYPE=Release
  <li> ninja.exe assistant
</ol>
<li> For Linux: 
<ol>
  <li> Run cmake: cmake ..
  <li> make assistant
</ol>

</ol>