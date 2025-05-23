cmake_minimum_required(VERSION 3.13)

if(APPLE)
    project(YumLife LANGUAGES CXX OBJC)
else()
    project(YumLife LANGUAGES CXX)
endif()

set(CMAKE_CXX_STANDARD 14)

set(MINORGEMS_COMMON_SOURCE_FILES
    minorGems/graphics/openGL/ScreenGL_SDL.cpp
    minorGems/graphics/openGL/SingleTextureGL.cpp
    minorGems/util/stringUtils.cpp
    minorGems/util/StringBufferOutputStream.cpp
    minorGems/util/ByteBufferInputStream.cpp
    minorGems/util/TranslationManager.cpp
    minorGems/network/NetworkFunctionLocks.cpp
    minorGems/network/LookupThread.cpp
    minorGems/network/web/WebRequest.cpp
    minorGems/network/web/WebRequestCompletionThread.cpp
    minorGems/network/web/URLUtils.cpp
    minorGems/util/SettingsManager.cpp
    minorGems/system/FinishedSignalThread.cpp
    minorGems/system/StopSignalThread.cpp
    minorGems/crypto/hashes/sha1.cpp
    minorGems/formats/encodingUtils.cpp
    minorGems/util/log/Log.cpp
    minorGems/util/log/AppLog.cpp
    minorGems/util/log/FileLog.cpp
    minorGems/util/log/PrintLog.cpp
    minorGems/util/printUtils.cpp
    minorGems/game/platforms/SDL/gameSDL.cpp
    minorGems/game/platforms/openGL/gameGraphicsGL.cpp
    minorGems/game/platforms/openGL/SpriteGL.cpp
    minorGems/game/doublePair.cpp
    minorGems/game/Font.cpp
    minorGems/game/drawUtils.cpp
    minorGems/game/platforms/SDL/DemoCodeChecker.cpp
    minorGems/sound/formats/aiff.cpp
    minorGems/sound/audioNoClip.cpp
    minorGems/sound/filters/SoundSamples.cpp
    minorGems/sound/filters/ReverbSoundFilter.cpp
    minorGems/sound/filters/coefficientFilters.cpp
    minorGems/crypto/keyExchange/curve25519.cpp
    minorGems/crypto/cryptoRandom.cpp
    minorGems/game/diffBundle/client/diffBundleClient.cpp
    minorGems/util/StringTree.cpp
    minorGems/util/crc32.cpp
)

if (WIN32)
    set(MINORGEMS_PLATFORM_SOURCE_FILES
        minorGems/io/win32/TypeIOWin32.cpp
        minorGems/io/file/win32/PathWin32.cpp
        minorGems/system/win32/TimeWin32.cpp
        minorGems/system/win32/ThreadWin32.cpp
        minorGems/system/win32/MutexLockWin32.cpp
        minorGems/system/win32/BinarySemaphoreWin32.cpp
        minorGems/network/win32/SocketWin32.cpp
        minorGems/network/win32/HostAddressWin32.cpp
        minorGems/network/win32/SocketClientWin32.cpp
        minorGems/network/win32/SocketServerWin32.cpp
        minorGems/io/file/win32/DirectoryWin32.cpp
    )
else()
    set(MINORGEMS_PLATFORM_SOURCE_FILES
        minorGems/io/linux/TypeIOLinux.cpp
        minorGems/io/file/linux/PathLinux.cpp
        minorGems/system/linux/ThreadLinux.cpp
        minorGems/system/linux/MutexLockLinux.cpp
        minorGems/system/linux/BinarySemaphoreLinux.cpp
        minorGems/network/linux/SocketLinux.cpp
        minorGems/network/linux/HostAddressLinux.cpp
        minorGems/network/linux/SocketClientLinux.cpp
        minorGems/network/linux/SocketServerLinux.cpp
        minorGems/system/unix/TimeUnix.cpp
        minorGems/io/file/unix/DirectoryUnix.cpp
    )
endif()

set(MINORGEMS_SOURCE_FILES
    ${MINORGEMS_COMMON_SOURCE_FILES}
    ${MINORGEMS_PLATFORM_SOURCE_FILES}
)

set(CLIENT_SOURCE_FILES
    gameSource/minitech.cpp
    gameSource/hetuwmod.cpp
    gameSource/hetuwFont.cpp
    gameSource/hetuwTCPConnection.cpp
    gameSource/yumBlob.cpp
    gameSource/yumConfig.cpp
    gameSource/yumRebirthComponent.cpp
    gameSource/game.cpp
    gameSource/spriteBank.cpp
    gameSource/objectBank.cpp
    gameSource/transitionBank.cpp
    gameSource/animationBank.cpp
    gameSource/whiteSprites.cpp
    gameSource/message.cpp
    gameSource/serialWebRequests.cpp
    gameSource/accountHmac.cpp
    gameSource/PageComponent.cpp
    gameSource/GamePage.cpp
    gameSource/LivingLifePage.cpp
    gameSource/pathFind.cpp
    gameSource/ageControl.cpp
    gameSource/ExtendedMessagePage.cpp
    gameSource/buttonStyle.cpp
    gameSource/Button.cpp
    gameSource/TextButton.cpp
    gameSource/RebirthChoicePage.cpp
    gameSource/TextField.cpp
    gameSource/LoadingPage.cpp
    gameSource/folderCache.cpp
    gameSource/binFolderCache.cpp
    gameSource/liveObjectSet.cpp
    commonSource/fractalNoise.cpp
    commonSource/sayLimit.cpp
    gameSource/ExistingAccountPage.cpp
    gameSource/KeyEquivalentTextButton.cpp
    gameSource/ServerActionPage.cpp
    gameSource/FinalMessagePage.cpp
    gameSource/AutoUpdatePage.cpp
    gameSource/soundBank.cpp
    gameSource/convolution.cpp
    gameSource/fft.cpp
    gameSource/ogg.cpp
    gameSource/musicPlayer2.cpp
    gameSource/groundSprites.cpp
    gameSource/SettingsPage.cpp
    gameSource/CheckboxButton.cpp
    gameSource/ValueSlider.cpp
    gameSource/SpriteButton.cpp
    gameSource/SpriteToggleButton.cpp
    gameSource/categoryBank.cpp
    gameSource/liveAnimationTriggers.cpp
    gameSource/ReviewPage.cpp
    gameSource/TextArea.cpp
    gameSource/RadioButtonSet.cpp
    gameSource/spellCheck.cpp
    gameSource/SoundUsage.cpp
    gameSource/TwinPage.cpp
    gameSource/objectMetadata.cpp
    gameSource/emotion.cpp
    gameSource/Picker.cpp
    gameSource/PickableStatics.cpp
    gameSource/photos.cpp
    gameSource/lifeTokens.cpp
    gameSource/PollPage.cpp
    gameSource/fitnessScore.cpp
    gameSource/GeneticHistoryPage.cpp
    gameSource/ServicesPage.cpp
    gameSource/photoCache.cpp
    gameSource/exporter.cpp
    gameSource/importer.cpp
    gameSource/settingsToggle.cpp
    gameSource/AHAPResultPage.cpp
    gameSource/AHAPSettingsPage.cpp
    gameSource/rocketAnimation.cpp
)

# These are the compile options from the original build system. Note that -O0
# in particular is sadly necessary to prevent some OHOL crashes.
set(CLIENT_COMPILE_OPTIONS -Wall -Wwrite-strings -Wchar-subscripts -Wparentheses -g -O0)

include_directories(.)

if(WIN32)
    add_definitions(-DWIN32 -DWIN_32 -DSTATICLIB)

    set(SDL_DIR "SDL-1.2.15")
    include_directories("${SDL_DIR}/include")
    link_directories("${SDL_DIR}/lib")

    get_filename_component(SDL_ABS_DIR "${CMAKE_SOURCE_DIR}/SDL-1.2.15" ABSOLUTE)
    if (NOT EXISTS ${SDL_ABS_DIR})
        message(FATAL_ERROR "SDL directory not found: ${SDL_DIR}")
    endif()

    add_executable(YumLife_windows ${CLIENT_SOURCE_FILES} ${MINORGEMS_SOURCE_FILES} icon.rc)
    target_compile_options(YumLife_windows PRIVATE ${CLIENT_COMPILE_OPTIONS})
    target_link_libraries(YumLife_windows opengl32 glu32 mingw32 SDLmain SDL wsock32 winmm)
    target_link_options(YumLife_windows PRIVATE -mwindows -static-libstdc++ -static-libgcc)
elseif(APPLE)
    add_definitions(-DMACOSX -D__mac__)

    add_executable(YumLife_macos ${CLIENT_SOURCE_FILES} ${MINORGEMS_SOURCE_FILES})
    target_compile_options(YumLife_macos PRIVATE ${CLIENT_COMPILE_OPTIONS})

    find_library(SDL_LIBRARY NAMES SDL-1.2 SDL12-compat SDL PATHS /opt/homebrew/opt/sdl12-compat/lib PATH_SUFFIXES SDL12-compat)
    find_path(SDL_INCLUDE_DIR SDL.h PATHS /opt/homebrew/opt/sdl12-compat/include/SDL)
    if(NOT SDL_LIBRARY OR NOT SDL_INCLUDE_DIR)
        message(FATAL_ERROR "libsdl12-compat not found. Please install with Homebrew: brew install sdl12-compat")
    endif()
    include_directories(${SDL_INCLUDE_DIR})
    target_link_libraries(YumLife_macos ${SDL_LIBRARY})

    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(YumLife_macos ${OPENGL_LIBRARIES})

    target_link_libraries(YumLife_macos "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" "-framework Carbon")

    # Symlink OpenGL headers into the build directory so we don't have to change
    # the include paths in the source files
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink /Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers ${CMAKE_BINARY_DIR}/GL)
    include_directories(${CMAKE_BINARY_DIR})

    set(MACOS_EXTRA_SOURCES ${CMAKE_SOURCE_DIR}/minorGems/game/platforms/SDL/mac/SDLMain.m)
    set_source_files_properties(${MACOS_EXTRA_SOURCES} PROPERTIES LANGUAGE OBJC)
    target_sources(YumLife_macos PRIVATE ${MACOS_EXTRA_SOURCES})
    set_property(TARGET YumLife_macos APPEND_STRING PROPERTY LINK_FLAGS " -ObjC")
else()
    add_definitions(-DLINUX)

    add_executable(YumLife_linux ${CLIENT_SOURCE_FILES} ${MINORGEMS_SOURCE_FILES})
    target_compile_options(YumLife_linux PRIVATE ${CLIENT_COMPILE_OPTIONS})

    find_package(SDL REQUIRED)
    include_directories(${SDL_INCLUDE_DIR})
    target_link_libraries(YumLife_linux ${SDL_LIBRARY})

    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(YumLife_linux ${OPENGL_LIBRARIES})
endif()
