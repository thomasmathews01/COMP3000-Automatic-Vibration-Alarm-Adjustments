add_library(Model Crow/crow.h Server.cpp Server.h SerialisableTypes/MatrixInputs.h SerialisableTypes/MatrixResponse.h SerialisableTypes/Generators.h SerialisableTypes/helper.h CudaAdder.cu CudaAdder.h)

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()

