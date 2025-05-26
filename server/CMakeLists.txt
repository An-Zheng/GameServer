find_package(Boost 1.74 REQUIRED system)

add_executable(server ./echo/main.cpp ./echo/EchoSocket.cpp)

target_include_directories(server
PUBLIC
    ${Boost_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/echo)
    
target_link_libraries(server 
PUBLIC 
    Boost::system
    pthread)