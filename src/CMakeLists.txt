target_sources(granite
    PRIVATE
        main.cpp

        chipvm.cpp
        chipvm.hpp
        instructions.cpp

        common_impl.cpp
        common_impl.hpp

        windows_impl.cpp
        windows_impl.hpp

        sfml_impl.cpp
        sfml_impl.hpp

        utils.hpp)
