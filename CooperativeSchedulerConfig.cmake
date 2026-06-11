get_filename_component(_cs_dir "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(COOPERATIVE_SCHEDULER_INCLUDE_DIRS "${_cs_dir}/include")

if(NOT TARGET cooperative_scheduler::cooperative_scheduler)
  add_library(cooperative_scheduler::cooperative_scheduler INTERFACE IMPORTED)
  set_target_properties(cooperative_scheduler::cooperative_scheduler PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${COOPERATIVE_SCHEDULER_INCLUDE_DIRS}")
endif()
