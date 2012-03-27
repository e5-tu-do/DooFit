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
blueprint/elements/factory.cpp blueprint/elements/factory.h
blueprint/pdfs/pdf.cpp blueprint/pdfs/pdf.h
blueprint/pdfs/acceptance.cpp blueprint/pdfs/acceptance.h
blueprint/pdfs/effpdf.cpp blueprint/pdfs/effpdf.h
blueprint/pdfs/basicpdf.cpp blueprint/pdfs/basicpdf.h
blueprint/pdfs/resolpdf.cpp blueprint/pdfs/resolpdf.h
blueprint/pdfs/genwresolpdf.cpp blueprint/pdfs/genwresolpdf.h
blueprint/pdfs/genpdf.cpp blueprint/pdfs/genpdf.h
blueprint/pdfs/datapdf.cpp blueprint/pdfs/datapdf.h
blueprint/pdfs/gaussresol.cpp blueprint/pdfs/gaussresol.h
blueprint/pdfs/decaypdf.cpp blueprint/pdfs/decaypdf.h
blueprint/pdfs/gausspdf.cpp blueprint/pdfs/gausspdf.h
)
