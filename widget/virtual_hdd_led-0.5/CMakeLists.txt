
project(plasma-virtual_hdd_led)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

add_subdirectory(icons)

set(virtual_hdd_led_SRCS virtual_hdd_led.cpp)


kde4_add_ui_files(virtual_hdd_led_SRCS virtual_hdd_ledConfig.ui )
kde4_add_plugin(plasma_applet_virtual_hdd_led ${virtual_hdd_led_SRCS})
target_link_libraries(plasma_applet_virtual_hdd_led 
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
 
install(TARGETS plasma_applet_virtual_hdd_led
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-virtual_hdd_led.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
