# --------------------------------------------------------------------------------------------------------------
# Authors: Ulrich Eitschberger (RooBMixAsymDecay), Frank Meier (RooBCPGenUt), Julian Wishahi (BiasDelta),
# Christophe Cauet (DooCubicSplineKnot, DooCubicSplinePdf, RectangularPdf)
# 2014-05-16
# ______________________________________________________________________________________________________________

root_generate_dictionaries_local(dfPdfsDict
BiasDelta.h
DooCubicSplineKnot.h
DooCubicSplinePdf.h
RectangularPdf.h
RooBCPGenDecayUt.h
RooBMixAsymDecay.h)

add_library(dfPdfs SHARED 
BiasDelta.h BiasDelta.cxx BiasDelta_dict.h BiasDelta_dict.cc
DooCubicSplineKnot.h DooCubicSplineKnot.cxx DooCubicSplineKnot_dict.h DooCubicSplineKnot_dict.cc
DooCubicSplinePdf.h DooCubicSplinePdf.cxx DooCubicSplinePdf_dict.h DooCubicSplinePdf_dict.cc
RectangularPdf.h RectangularPdf.cxx RectangularPdf_dict.h RectangularPdf_dict.cc
RooBCPGenDecayUt.h RooBCPGenDecayUt.cxx RooBCPGenDecayUt_dict.h RooBCPGenDecayUt_dict.cc
RooBMixAsymDecay.h RooBMixAsymDecay.cxx RooBMixAsymDecay_dict.h RooBMixAsymDecay_dict.cc)

target_link_libraries(dfPdfs dfp2vvpdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfPdfs dfPdfsDict)

install(TARGETS dfPdfs DESTINATION lib)
install(FILES 
BiasDelta.h
DooCubicSplineKnot.h
DooCubicSplinePdf.h
RectangularPdf.h
RooBCPGenDecayUt.h
RooBMixAsymDecay.h
DESTINATION include/doofit/roofit/pdfs)
