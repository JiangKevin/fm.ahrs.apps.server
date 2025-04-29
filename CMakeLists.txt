cmake_minimum_required(VERSION 3.15)

set(app_name "ahrs_server")

# Set project name
project(${app_name})

# 设置 C++ 标准版本为 C++17
set(CMAKE_CXX_STANDARD 17)
set(Boost_NO_BOOST_CMAKE ON)

# 设置所有可执行文件的运行时输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $ENV{FmDev}/bin)

#
include_directories($ENV{FmDev}/libs/urho3d/include)
include_directories($ENV{FmDev}/libs/urho3d/include/Urho3D/ThirdParty)
include_directories($ENV{FmDev}/source)

#
# add_definitions(-DENABLE_LOGGING)
find_package(OpenGL REQUIRED)
find_path(RAPIDCSV_INCLUDE_DIRS "rapidcsv.h")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-concurrentqueue CONFIG REQUIRED)

# 查找 Boost.Beast 及其依赖库
find_package(Boost REQUIRED)

#
# 查找源文件
file(GLOB app_src
    $ENV{FmDev}/source/*.cpp
    $ENV{FmDev}/source/*.inl
    $ENV{FmDev}/source/base/*.cpp
    $ENV{FmDev}/source/base/*.inl
    $ENV{FmDev}/source/websocket/*.cpp
)

# 添加可执行文件
add_executable(${app_name}
    ${app_src}
)

# 链接库到可执行文件
target_link_libraries(${app_name} PUBLIC

    #
    OpenGL::GL
    unofficial::concurrentqueue::concurrentqueue
    nlohmann_json::nlohmann_json
    ${Boost_LIBRARIES}

    #
    $ENV{FmDev}/libs/urho3d/libs/libAngelScript.a
    $ENV{FmDev}/libs/urho3d/libs/libBox2D.a
    $ENV{FmDev}/libs/urho3d/libs/libBullet.a
    $ENV{FmDev}/libs/urho3d/libs/libCivetweb.a
    $ENV{FmDev}/libs/urho3d/libs/libDetour.a
    $ENV{FmDev}/libs/urho3d/libs/libDetourCrowd.a
    $ENV{FmDev}/libs/urho3d/libs/libDetourTileCache.a
    $ENV{FmDev}/libs/urho3d/libs/libETCPACK.a
    $ENV{FmDev}/libs/urho3d/libs/libFreeType.a
    $ENV{FmDev}/libs/urho3d/libs/libik.a
    $ENV{FmDev}/libs/urho3d/libs/libLZ4.a
    $ENV{FmDev}/libs/urho3d/libs/libPugiXml.a
    $ENV{FmDev}/libs/urho3d/libs/libRecast.a
    $ENV{FmDev}/libs/urho3d/libs/libSDL.a
    $ENV{FmDev}/libs/urho3d/libs/libSLikeNet.a
    $ENV{FmDev}/libs/urho3d/libs/libStanHull.a
    $ENV{FmDev}/libs/urho3d/libs/libUrho3D.a
    $ENV{FmDev}/libs/urho3d/libs/libWebP.a
)