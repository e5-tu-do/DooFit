# --------------------------------------------------------------------------------------------------------------
# Extra RooPdfs from Gerhard Raven, see:
# link: https://indico.cern.ch/conferenceDisplay.py?confId=258524
# link: https://cds.cern.ch/record/1563169
# from PhysFit/P2VV
# ______________________________________________________________________________________________________________
root_generate_dictionaries_local(splineroopdfsDict SplineRooAbsEffResModel.h SplineRooAbsGaussModelEfficiency.h SplineRooCubicSplineFun.h SplineRooCubicSplineKnot.h SplineRooEffConvGenContext.h SplineRooEffResModel.h SplineRooGaussEfficiencyModel.h)

add_library(dfsplineroopdfs SHARED 
SplineRooAbsEffResModel.h SplineRooAbsEffResModel_dict.cc SplineRooAbsEffResModel_dict.h 
SplineRooAbsGaussModelEfficiency.cxx SplineRooAbsGaussModelEfficiency.h SplineRooAbsGaussModelEfficiency_dict.cc SplineRooAbsGaussModelEfficiency_dict.h 
SplineRooCubicSplineFun.cxx SplineRooCubicSplineFun.h SplineRooCubicSplineFun_dict.cc SplineRooCubicSplineFun_dict.h 
SplineRooCubicSplineKnot.cxx SplineRooCubicSplineKnot.h SplineRooCubicSplineKnot_dict.cc SplineRooCubicSplineKnot_dict.h 
SplineRooEffConvGenContext.cxx SplineRooEffConvGenContext.h SplineRooEffConvGenContext_dict.cc SplineRooEffConvGenContext_dict.h 
SplineRooEffResModel.cxx SplineRooEffResModel.h SplineRooEffResModel_dict.cc SplineRooEffResModel_dict.h 
SplineRooGaussEfficiencyModel.cxx SplineRooGaussEfficiencyModel.h SplineRooGaussEfficiencyModel_dict.cc SplineRooGaussEfficiencyModel_dict.h
)

target_link_libraries(dfsplineroopdfs ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfsplineroopdfs dfsplineroopdfsDict)

install(TARGETS dfsplineroopdfs DESTINATION lib/splinepdfs)
install(FILES SplineRooAbsEffResModel.h SplineRooAbsGaussModelEfficiency.h SplineRooCubicSplineFun.h SplineRooCubicSplineKnot.h SplineRooEffConvGenContext.h SplineRooEffResModel.h SplineRooGaussEfficiencyModel.h DESTINATION include/doofit/extraroopdfs/splinepdfs)
