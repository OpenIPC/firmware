PROJECT(logwrapper)

add_definitions("-DHAVE_SYS_UIO_H -DHAVE_IOCTL")
add_definitions('-DLOGGER_LOG_MAIN="log_main"')
add_definitions('-DLOGGER_LOG_RADIO="log_radio"')
add_definitions('-DLOGGER_LOG_EVENTS="log_events"')
add_definitions('-DLOGGER_LOG_SYSTEM="log_system"')

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
AUX_SOURCE_DIRECTORY(. SRC_LIST)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../include)
ADD_EXECUTABLE(logwrapper ${SRC_LIST})

install(TARGETS logwrapper
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
