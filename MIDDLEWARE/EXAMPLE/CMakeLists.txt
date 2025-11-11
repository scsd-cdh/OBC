# First you declare that you wish to create a new library
add_library(MIDDLEWARE_EXAMPLE)

# Now you tell cmake what files are part of the library, you are going to need to come back here when adding new files
target_sources(MIDDLEWARE_EXAMPLE
    # This first block is for .h files located within the include/ directory, this is technically optional, but it
    # helps CLion give you a better autocomplete, so keep this updated even if it doesn't break your build.
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/example/life.h

    # This block is for .h files that are NOT in the include/ directory, this is also technically optional.
    PRIVATE
        FILE_SET private_headers
        TYPE HEADERS
        BASE_DIRS .
        FILES
            supercomputer.h
            life_private.h

    # This block is the most important one, its for files that will be processed by the C compiler. Only .c files
    # should be added.
    PRIVATE
        life.c
        supercomputer.c
)

# This line tells cmake that everything in include can be used by other modules/middlewares
target_include_directories(MIDDLEWARE_EXAMPLE PUBLIC include)
# In our case, we don't actually use the drivers in here, but for the sake of compleness in the example, we added it
# here to demonstrate how to use other modules/middlewares in your own. You can choose between PRIVATE and PUBLIC as
# needed, PRIVATE means only you are allowed to use the library, PUBLIC means that anyone using you can also use the
# library transitively.
target_link_libraries(MIDDLEWARE_EXAMPLE PRIVATE DRIVER_CDH_COMMON)