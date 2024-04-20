cmake_minimum_required(VERSION 3.10)

project(YumLife)

set(CMAKE_CXX_STANDARD 14)

set(MINORGEMS_SOURCE_FILES
    minorGems/graphics/openGL/ScreenGL_SDL.cpp
    minorGems/graphics/openGL/SingleTextureGL.cpp
    minorGems/io/linux/TypeIOLinux.cpp
    minorGems/util/stringUtils.cpp
    minorGems/util/StringBufferOutputStream.cpp
    minorGems/util/ByteBufferInputStream.cpp
    minorGems/io/file/linux/PathLinux.cpp
    minorGems/system/unix/TimeUnix.cpp
    minorGems/system/linux/ThreadLinux.cpp
    minorGems/system/linux/MutexLockLinux.cpp
    minorGems/system/linux/BinarySemaphoreLinux.cpp
    minorGems/util/TranslationManager.cpp
    minorGems/network/linux/SocketLinux.cpp
    minorGems/network/linux/HostAddressLinux.cpp
    minorGems/network/linux/SocketClientLinux.cpp
    minorGems/network/linux/SocketServerLinux.cpp
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
    minorGems/io/file/unix/DirectoryUnix.cpp
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

set(CLIENT_SOURCE_FILES
    gameSource/minitech.cpp
    gameSource/hetuwmod.cpp
    gameSource/hetuwFont.cpp
    gameSource/hetuwTCPConnection.cpp
    gameSource/phex.cpp
    gameSource/yumBlob.cpp
    gameSource/yumConfig.cpp
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
)

include_directories(.)

add_executable(YumLife ${CLIENT_SOURCE_FILES} ${MINORGEMS_SOURCE_FILES})

if(WIN32)
    add_definitions(-DWIN32)

    set(SDL_DIR "SDL-1.2.15")
    include_directories("${SDL_DIR}/include")
    link_directories("${SDL_DIR}/lib")
    
    target_link_libraries(YumLife SDL)
else()
    add_definitions(-DLINUX)

    find_package(SDL REQUIRED)
    include_directories(${SDL_INCLUDE_DIR})
    target_link_libraries(YumLife ${SDL_LIBRARY})

    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(YumLife ${OPENGL_LIBRARIES})
endif()
