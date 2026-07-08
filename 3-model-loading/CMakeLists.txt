get_filename_component(CHAPTER_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB CHAPTER_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_chapter_target(${CHAPTER_NAME} "${CHAPTER_SOURCES}")

target_link_libraries(
  ${CHAPTER_NAME} PRIVATE
  lgl-common
  assimp::assimp
)
