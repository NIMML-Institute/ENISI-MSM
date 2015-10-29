if (NOT EXISTS .mpich_install_done)
  message(STATUS "Installing MPICH...")
  file(REMOVE_RECURSE ${REPAST_EXT_DIR}/MPICH)
  execute_process(
    COMMAND bash ./install.sh mpich
    WORKING_DIRECTORY ${WORK_DIR}
    OUTPUT_FILE ${LOG_DIR}/mpich_install.log
    ERROR_FILE ${LOG_DIR}/mpich_install_errors.log
  )
  execute_process(COMMAND touch .mpich_install_done)
endif()

if (NOT EXISTS .boost_install_done)
  message(STATUS "Installing Boost...")
  file(REMOVE_RECURSE ${REPAST_EXT_DIR}/Boost)
  execute_process(COMMAND bash ./install.sh boost 
    WORKING_DIRECTORY ${WORK_DIR}
    OUTPUT_FILE ${LOG_DIR}/boost_install.log
    ERROR_FILE ${LOG_DIR}/boost_install_errors.log
  )
  execute_process(COMMAND touch .boost_install_done)
endif()

if (NOT EXISTS .netcdf_install_done)
  message(STATUS "Installing NetCDF...")
  file(REMOVE_RECURSE ${REPAST_EXT_DIR}/NetCDF)
  execute_process(COMMAND bash ./install.sh netcdf-netcpp 
    WORKING_DIRECTORY ${WORK_DIR}
    OUTPUT_FILE ${LOG_DIR}/netcdf_install.log
    ERROR_FILE ${LOG_DIR}/netcdf_install_errors.log
  )
  execute_process(COMMAND touch .netcdf_install_done)
endif()

if (NOT EXISTS .curl_install_done)
  message(STATUS "Installing Curl...")
  file(REMOVE_RECURSE ${REPAST_EXT_DIR}/CURL)
  execute_process(COMMAND bash ./install.sh lcurl
    WORKING_DIRECTORY ${WORK_DIR}
    OUTPUT_FILE ${LOG_DIR}/lcurl_install.log
    ERROR_FILE ${LOG_DIR}/lcurl_install_errors.log
  )
  execute_process(COMMAND touch .curl_install_done)
endif()

if (NOT EXISTS ${REPAST_DIR}/lib/librepast_hpc-2.0.a)
  message(STATUS "Installing Repast HPC libraries...")
  execute_process(COMMAND bash ./install.sh rhpc
    WORKING_DIRECTORY ${WORK_DIR}
    OUTPUT_FILE ${LOG_DIR}/rhpc_install.log
    ERROR_FILE ${LOG_DIR}/rhpc_install_errors.log
  )
endif()
