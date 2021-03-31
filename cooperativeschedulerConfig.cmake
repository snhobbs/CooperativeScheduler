# - Config file for the cooperative_scheduler package
# It defines the following variables
#  COOPERATIVE_SCHEDULER_INCLUDE_DIRS - include directories for cooperative_scheduler
#  COOPERATIVE_SCHEDULER_LIBRARIES    - libraries to link against
#  COOPERATIVE_SCHEDULER_EXECUTABLE   - the bar executable

# Compute paths
get_filename_component("COOPERATIVE_SCHEDULER_CMAKE_DIR" "${CMAKE_CURRENT_LIST_FILE}" PATH)
set("COOPERATIVE_SCHEDULER_INCLUDE_DIRS" "/home/simon/software/cooperative_scheduler;/home/simon/software/cooperative_scheduler/build")
