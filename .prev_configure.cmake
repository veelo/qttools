

#### Inputs



#### Libraries



#### Tests

# libclang
qt_find_package(WrapLibClang PROVIDED_TARGETS WrapLibClang::WrapLibClang)

if(TARGET WrapLibClang::WrapLibClang)
    set(TEST_libclang "ON" CACHE BOOL "Required libclang version found." FORCE)
endif()



#### Features

qt_feature("assistant" PRIVATE
    LABEL "Qt Assistant"
    PURPOSE "Qt Assistant is a tool for viewing on-line documentation in Qt help file format."
)
qt_feature("clang" PRIVATE
    LABEL "QDoc"
    CONDITION TEST_libclang
)
qt_feature("clangcpp" PRIVATE
    LABEL "Clang-based lupdate parser"
    CONDITION QT_FEATURE_clang AND TEST_libclang
)
qt_feature("designer" PRIVATE
    LABEL "Qt Designer"
    PURPOSE "Qt Designer is the Qt tool for designing and building graphical user interfaces (GUIs) with Qt Widgets. You can compose and customize your windows or dialogs in a what-you-see-is-what-you-get (WYSIWYG) manner, and test them using different styles and resolutions."
)
qt_feature("distancefieldgenerator" PRIVATE
    LABEL "Qt Distance Field Generator"
    PURPOSE "The Qt Distance Field Generator tool can be used to pregenerate the font cache in order to optimize startup performance."
)
qt_feature("kmap2qmap" PRIVATE
    LABEL "kmap2qmap"
    PURPOSE "kmap2qmap is a tool to generate keymaps for use on Embedded Linux. The source files have to be in standard Linux kmap format that is e.g. understood by the kernel's loadkeys command."
)
qt_feature("linguist" PRIVATE
    LABEL "Qt Linguist"
    PURPOSE "Qt Linguist can be used by translator to translate text in Qt applications."
)
qt_feature("macdeployqt" PRIVATE
    LABEL "Mac Deployment Tool"
    PURPOSE "The Mac deployment tool automates the process of creating a deployable application bundle that contains the Qt libraries as private frameworks."
    CONDITION APPLE
)
qt_feature("pixeltool" PRIVATE
    LABEL "pixeltool"
    PURPOSE "The Qt Pixel Zooming Tool is a graphical application that magnifies the screen around the mouse pointer so you can look more closely at individual pixels."
)
qt_feature("qdbus" PRIVATE
    LABEL "qdbus"
    PURPOSE "qdbus is a communication interface for Qt-based applications."
)
qt_feature("qev" PRIVATE
    LABEL "qev"
    PURPOSE "qev allows introspection of incoming events for a QWidget, similar to the X11 xev tool."
)
qt_feature("qtattributionsscanner" PRIVATE
    LABEL "Qt Attributions Scanner"
    PURPOSE "Qt Attributions Scanner generates attribution documents for third-party code in Qt."
)
qt_feature("qtdiag" PRIVATE
    LABEL "qtdiag"
    PURPOSE "qtdiag outputs information about the Qt installation it was built with."
)
qt_feature("qtpaths" PRIVATE
    LABEL "qtpaths"
    PURPOSE "qtpaths is a command line client to QStandardPaths."
)
qt_feature("qtplugininfo" PRIVATE
    LABEL "qtplugininfo"
    PURPOSE "qtplugininfo dumps metadata about Qt plugins in JSON format."
)
qt_feature("windeployqt" PRIVATE
    LABEL "Windows deployment tool"
    PURPOSE "The Windows deployment tool is designed to automate the process of creating a deployable folder containing the Qt-related dependencies (libraries, QML imports, plugins, and translations) required to run the application from that folder. It creates a sandbox for Universal Windows Platform (UWP) or an installation tree for Windows desktop applications, which can be easily bundled into an installation package."
    CONDITION WIN32
)
qt_configure_add_summary_section(NAME "Qt Tools")
qt_configure_add_summary_entry(ARGS "assistant")
qt_configure_add_summary_entry(ARGS "clang")
qt_configure_add_summary_entry(ARGS "clangcpp")
qt_configure_add_summary_entry(ARGS "designer")
qt_configure_add_summary_entry(ARGS "distancefieldgenerator")
qt_configure_add_summary_entry(ARGS "kmap2qmap")
qt_configure_add_summary_entry(ARGS "linguist")
qt_configure_add_summary_entry(ARGS "macdeployqt")
qt_configure_add_summary_entry(ARGS "pixeltool")
qt_configure_add_summary_entry(ARGS "qdbus")
qt_configure_add_summary_entry(ARGS "qev")
qt_configure_add_summary_entry(ARGS "qtattributionsscanner")
qt_configure_add_summary_entry(ARGS "qtdiag")
qt_configure_add_summary_entry(ARGS "qtpaths")
qt_configure_add_summary_entry(ARGS "qtplugininfo")
qt_configure_add_summary_entry(ARGS "windeployqt")
qt_configure_end_summary_section() # end of "Qt Tools" section
qt_configure_add_report_entry(
    TYPE WARNING
    MESSAGE "QDoc will not be compiled, probably because libclang could not be located. This means that you cannot build the Qt documentation.
 Either set CMAKE_PREFIX_PATH or LLVM_INSTALL_DIR to the location of your llvm installation. On Linux systems, you may be able to install libclang by installing the libclang-dev or libclang-devel package, depending on your distribution. On macOS, you can use Homebrew's llvm package."
    CONDITION NOT QT_FEATURE_clang
)
qt_configure_add_report_entry(
    TYPE WARNING
    MESSAGE "Clang-based lupdate parser will not be available. LLVM and Clang C++ libraries have not been found."
    CONDITION NOT QT_FEATURE_clangcpp
)
