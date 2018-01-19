CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
PROJECT(facialExpression)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)
find_package(OpenCV REQUIRED PATHS /usr/local/Cellar/opencv/3.3.0_3 NO_DEFAULT_PATH)

#boost options
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
#boost directories
SET(BOOST_ROOT "/usr/local/Cellar/boost/1.65.0")
SET (BOOST_INCLUDEDIR "${BOOST_ROOT}/include/boost")
SET(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

FIND_PACKAGE(Boost 1.65.0 COMPONENTS regex date_time system chrono iostreams thread serialization filesystem REQUIRED)

ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)

SET(SOURCE_DIR "source")
SET(INCLUDE_DIR "include")

INCLUDE_DIRECTORIES(include ${Boost_INCLUDE_DIRS} {OpenCV_INCLUDE_DIRS})
#include opencv lib directory
#LINK_DIRECTORIES(/usr/local/Cellar/opencv/3.3.0_3/lib)

FILE(GLOB SOURCE_FILES ${SOURCE_DIR}/*.cpp)
FILE(GLOB HEADER_FILES ${INCLUDE_DIR}/*.h)

#header filters
SOURCE_GROUP("Header Files" FILES ${HEADER_FILES})
#source filters
SOURCE_GROUP("Source Files" FILES ${SOURCE_FILES})

#create exe
ADD_EXECUTABLE(facialExpression ${SOURCE_FILES} ${HEADER_FILES})
#link libs
TARGET_LINK_LIBRARIES(facialExpression ${Boost_LIBRARIES} ${OpenCV_LIBS})
