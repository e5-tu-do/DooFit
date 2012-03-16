add_library(Builder_Numerobis numerobis.cpp numerobis.h numerobis_config.cpp numerobis_config.h)

add_library(Builder_Numerobis_Blueprint 
blueprint/blueprint.cpp blueprint/blueprint.h 
blueprint/elements/element.cpp blueprint/elements/element.h 
blueprint/elements/dimelement.cpp blueprint/elements/dimelement.h 
blueprint/elements/realvalelement.cpp blueprint/elements/realvalelement.h
blueprint/elements/paramelement.cpp blueprint/elements/paramelement.h 
blueprint/elements/parambasic.cpp blueprint/elements/parambasic.h 
blueprint/elements/formula.cpp blueprint/elements/formula.h
blueprint/elements/dimreal.cpp blueprint/elements/dimreal.h
blueprint/elements/registrar.cpp blueprint/elements/registrar.h
)
