add_executable(det-sim detSim.cc)
target_link_libraries(det-sim LINK_PUBLIC edep)
install(TARGETS det-sim RUNTIME DESTINATION bin)

add_executable(edep-sim detSim.cc)
target_link_libraries(edep-sim LINK_PUBLIC edep)
install(TARGETS edep-sim RUNTIME DESTINATION bin)
