function(enable_compiler_warnings target)
    target_compile_options(${target} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall>
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4>)
endfunction()
