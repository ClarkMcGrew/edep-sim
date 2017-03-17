add_executable(edep-sim edepSim.cc)
target_link_libraries(edep-sim LINK_PUBLIC edepsim)
install(TARGETS edep-sim RUNTIME DESTINATION bin)
