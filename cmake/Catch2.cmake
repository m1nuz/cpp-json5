include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.6
)

FetchContent_MakeAvailable(Catch2)

include(${Catch2_SOURCE_DIR}/contrib/Catch.cmake)