cmake_minimum_required(VERSION 3.22)
project(CallAndResponseMIDI VERSION 2.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "" FORCE)
    set(PLUGIN_FORMATS VST3 AU Standalone)
else()
    set(PLUGIN_FORMATS VST3 Standalone)
endif()

# Add JUCE from vendor subdirectory
add_subdirectory(vendor/JUCE)

juce_add_plugin(CallAndResponseMIDI
    COMPANY_NAME "Viral Samples"
    BUNDLE_ID "com.viralsamples.callresponsemidi"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    PLUGIN_MANUFACTURER_CODE Virl
    PLUGIN_CODE Crgm
    FORMATS ${PLUGIN_FORMATS}
    PRODUCT_NAME "Call & Response MIDI"
    VST3_AUTO_MANIFEST FALSE
)

target_sources(CallAndResponseMIDI PRIVATE
    Source/GeneratorEngine.h
    Source/GeneratorEngine.cpp
    Source/SynthEngine.h
    Source/PresetManager.h
    Source/PresetManager.cpp
    Source/CustomLookAndFeel.h
    Source/CustomLookAndFeel.cpp
    Source/StepperComponent.h
    Source/StepperComponent.cpp
    Source/InspectorPanel.h
    Source/InspectorPanel.cpp
    Source/StepGridComponent.h
    Source/StepGridComponent.cpp
    Source/PianoRollComponent.h
    Source/PianoRollComponent.cpp
    Source/MidiDragComponent.h
    Source/MidiDragComponent.cpp
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
)

target_compile_definitions(CallAndResponseMIDI PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_USE_WINDOWS_MEDIA_FORMAT=0
)

target_link_libraries(CallAndResponseMIDI PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

if(WIN32)
    target_link_libraries(CallAndResponseMIDI PRIVATE
        d2d1
        d3d11
        dcomp
        dxgi
        dxguid
        dbghelp
        sapi
        winmm
        ksuser
        uuid
        ole32
        oleaut32
        comctl32
        dwmapi
        imm32
        version
        shlwapi
        ws2_32
        wininet
    )
endif()

if(APPLE)
    # Automatically install VST3 to /Library/Audio/Plug-Ins/VST3 and ~/Library/Audio/Plug-Ins/VST3
    if(TARGET CallAndResponseMIDI_VST3)
        add_custom_command(TARGET CallAndResponseMIDI_VST3 POST_BUILD
            COMMAND /usr/bin/ditto
                "$<TARGET_BUNDLE_DIR:CallAndResponseMIDI_VST3>"
                "/Library/Audio/Plug-Ins/VST3/Call & Response MIDI.vst3"
            COMMAND /usr/bin/ditto
                "$<TARGET_BUNDLE_DIR:CallAndResponseMIDI_VST3>"
                "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/Call & Response MIDI.vst3"
            COMMENT "Auto-installing VST3 to /Library/Audio/Plug-Ins/VST3 and ~/Library/Audio/Plug-Ins/VST3"
            VERBATIM
        )
    endif()

    # Automatically install AU to /Library/Audio/Plug-Ins/Components and ~/Library/Audio/Plug-Ins/Components
    if(TARGET CallAndResponseMIDI_AU)
        add_custom_command(TARGET CallAndResponseMIDI_AU POST_BUILD
            COMMAND /usr/bin/ditto
                "$<TARGET_BUNDLE_DIR:CallAndResponseMIDI_AU>"
                "/Library/Audio/Plug-Ins/Components/Call & Response MIDI.component"
            COMMAND /usr/bin/ditto
                "$<TARGET_BUNDLE_DIR:CallAndResponseMIDI_AU>"
                "$ENV{HOME}/Library/Audio/Plug-Ins/Components/Call & Response MIDI.component"
            COMMENT "Auto-installing AU to /Library/Audio/Plug-Ins/Components and ~/Library/Audio/Plug-Ins/Components"
            VERBATIM
        )
    endif()
endif()
