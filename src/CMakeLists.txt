target_sources(granite
    PRIVATE
        main.cpp
        chipvm.cpp
        instructions.cpp
        common_impl.cpp
        windows_impl.cpp

        ${GRANITE_INCLUDE_DIR}/chipvm.hpp
        ${GRANITE_INCLUDE_DIR}/common_impl.hpp
        ${GRANITE_INCLUDE_DIR}/windows_impl.hpp
        ${GRANITE_INCLUDE_DIR}/utils.hpp)
