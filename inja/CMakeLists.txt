# Copyright 2020-present Yeolar

file(GLOB INJA_HDRS *.h *.hpp)
file(GLOB INJA_NLOHMANN_HDRS nlohmann/*.hpp)

install(FILES ${INJA_HDRS} DESTINATION include/inja)
install(FILES ${INJA_NLOHMANN_HDRS} DESTINATION include/inja/nlohmann)
