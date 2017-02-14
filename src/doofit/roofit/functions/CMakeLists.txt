# --------------------------------------------------------------------------------------------------------------
# Authors: Christophe Cauet, Frank Meier, Florian Kruse
# 2014-05-16
# 2014-08-06 (updated: removed deprecated functions)
# ______________________________________________________________________________________________________________

root_generate_dictionaries_local(dfFunctionsDict 
CategoryToReal.h
CPCoefficient.h
Sum.h
Product.h
TripleProduct.h
LinearFunctionWithOffset.h
QuadraticFunctionWithOffset.h
TauCorrectionWithBetaFactor.h 
AtanAcceptanceIncludingBeta.h 
AtanAcceptance.h 
DooDecRateCoeff.h
####DEPRECATED##################
#CosCoeffWithProdAsymm.h
#CosCoeffCombo.h
#CoshCoeff.h
#CoshCoeffTagAsReal.h
#CoshCoeffWithAsymms.h 
#CoshCoeffCombo.h
#SinhCoeff.h
#SinCoeffSimple.h
#SinCoeffWithProdAsymm.h
#SinCoeffWithProdAsymmTagAsReal.h
#SinCoeffWithTaggingCalib.h
#SinCoeffWithAsymms.h
#SinCoeffCombo.h
#PolyTest.h 
#PowerLaw.h 
#SimpleSignedTaggedCPCoefficient.h 
#SingleMistagCalibrationNoAsymmetries.h
#SingleMistagCalibrationWithAsymmetries.h
#TwoTaggerMistagCombination.h
#TwoTaggerTagCombination.h
#CombinationOfCalibratedMistags.h
#CombinationOfUncalibratedMistags.h
#CombinationOfMistagsWithAsymmetries.h
#CombinedTagCalibrated.h
#CombinedTagUncalibrated.h
####DEPRECATED##################
)

add_library(dfFunctions SHARED 
CategoryToReal.h CategoryToReal.cxx CategoryToReal_dict.h CategoryToReal_dict.cc
CPCoefficient.h CPCoefficient.cxx CPCoefficient_dict.h CPCoefficient_dict.cc
Sum.h Sum.cxx Sum_dict.h Sum_dict.cc
Product.h Product.cxx Product_dict.h Product_dict.cc
TripleProduct.h TripleProduct.cxx TripleProduct_dict.h TripleProduct_dict.cc
LinearFunctionWithOffset.h LinearFunctionWithOffset.cxx LinearFunctionWithOffset_dict.h LinearFunctionWithOffset_dict.cc
QuadraticFunctionWithOffset.h QuadraticFunctionWithOffset.cxx QuadraticFunctionWithOffset_dict.h QuadraticFunctionWithOffset_dict.cc
TauCorrectionWithBetaFactor.h TauCorrectionWithBetaFactor.cxx TauCorrectionWithBetaFactor_dict.h TauCorrectionWithBetaFactor_dict.cc
AtanAcceptanceIncludingBeta.h AtanAcceptanceIncludingBeta.cxx AtanAcceptanceIncludingBeta_dict.h AtanAcceptanceIncludingBeta_dict.cc
AtanAcceptance.h AtanAcceptance.cxx AtanAcceptance_dict.h AtanAcceptance_dict.cc
DooDecRateCoeff.h DooDecRateCoeff.cxx DooDecRateCoeff_dict.h DooDecRateCoeff_dict.cc
####DEPRECATED##################
#CoshCoeff.h CoshCoeff.cxx CoshCoeff_dict.h CoshCoeff_dict.cc
#CosCoeffWithProdAsymm.h CosCoeffWithProdAsymm.cxx CosCoeffWithProdAsymm_dict.h CosCoeffWithProdAsymm_dict.cc
#CosCoeffCombo.h CosCoeffCombo.cxx CosCoeffCombo_dict.h CosCoeffCombo_dict.cc
#CoshCoeffTagAsReal.h CoshCoeffTagAsReal.cxx CoshCoeffTagAsReal_dict.h CoshCoeffTagAsReal_dict.cc
#CoshCoeffWithAsymms.h CoshCoeffWithAsymms.cxx CoshCoeffWithAsymms_dict.h CoshCoeffWithAsymms_dict.cc
#CoshCoeffCombo.h CoshCoeffCombo.cxx CoshCoeffCombo_dict.h CoshCoeffCombo_dict.cc
#SinhCoeff.h SinhCoeff.cxx SinhCoeff_dict.h SinhCoeff_dict.cc
#SinCoeffSimple.h SinCoeffSimple.cxx SinCoeffSimple_dict.h SinCoeffSimple_dict.cc
#SinCoeffWithProdAsymm.h SinCoeffWithProdAsymm.cxx SinCoeffWithProdAsymm_dict.h SinCoeffWithProdAsymm_dict.cc
#SinCoeffWithProdAsymmTagAsReal.h SinCoeffWithProdAsymmTagAsReal.cxx SinCoeffWithProdAsymmTagAsReal_dict.h SinCoeffWithProdAsymmTagAsReal_dict.cc
#SinCoeffWithTaggingCalib.h SinCoeffWithTaggingCalib.cxx SinCoeffWithTaggingCalib_dict.h SinCoeffWithTaggingCalib_dict.cc
#SinCoeffWithAsymms.h SinCoeffWithAsymms.cxx SinCoeffWithAsymms_dict.h SinCoeffWithAsymms_dict.cc
#SinCoeffCombo.h SinCoeffCombo.cxx SinCoeffCombo_dict.h SinCoeffCombo_dict.cc
#PolyTest.h PolyTest.cxx PolyTest_dict.h PolyTest_dict.cc
#PowerLaw.h PowerLaw.cxx PowerLaw_dict.h PowerLaw_dict.cc
#SimpleSignedTaggedCPCoefficient.h SimpleSignedTaggedCPCoefficient.cxx SimpleSignedTaggedCPCoefficient_dict.h SimpleSignedTaggedCPCoefficient_dict.cc
#SingleMistagCalibrationNoAsymmetries.h SingleMistagCalibrationNoAsymmetries.cxx SingleMistagCalibrationNoAsymmetries_dict.h SingleMistagCalibrationNoAsymmetries_dict.cc
#SingleMistagCalibrationWithAsymmetries.h SingleMistagCalibrationWithAsymmetries.cxx SingleMistagCalibrationWithAsymmetries_dict.h SingleMistagCalibrationWithAsymmetries_dict.cc
#TwoTaggerMistagCombination.h TwoTaggerMistagCombination.cxx TwoTaggerMistagCombination_dict.h TwoTaggerMistagCombination_dict.cc
#TwoTaggerTagCombination.h TwoTaggerTagCombination.cxx TwoTaggerTagCombination_dict.h TwoTaggerTagCombination_dict.cc
#CombinationOfCalibratedMistags.h CombinationOfCalibratedMistags.cxx CombinationOfCalibratedMistags_dict.h CombinationOfCalibratedMistags_dict.cc
#CombinationOfUncalibratedMistags.h CombinationOfUncalibratedMistags.cxx CombinationOfUncalibratedMistags_dict.h CombinationOfUncalibratedMistags_dict.cc
#CombinationOfMistagsWithAsymmetries.h CombinationOfMistagsWithAsymmetries.cxx CombinationOfMistagsWithAsymmetries_dict.h CombinationOfMistagsWithAsymmetries_dict.cc
#CombinedTagCalibrated.h CombinedTagCalibrated.cxx CombinedTagCalibrated_dict.h CombinedTagCalibrated_dict.cc
#CombinedTagUncalibrated.h CombinedTagUncalibrated.cxx CombinedTagUncalibrated_dict.h CombinedTagUncalibrated_dict.cc
####DEPRECATED##################
)

target_link_libraries(dfFunctions ${ALL_LIBRARIES})

# see above, not sure if it works
add_dependencies(dfFunctions dfFunctionsDict)

install(TARGETS dfFunctions DESTINATION lib)
install(FILES 
CategoryToReal.h
Sum.h
Product.h 
TripleProduct.h
CPCoefficient.h
LinearFunctionWithOffset.h 
QuadraticFunctionWithOffset.h 
AtanAcceptanceIncludingBeta.h 
AtanAcceptance.h
TauCorrectionWithBetaFactor.h 
DooDecRateCoeff.h
####DEPRECATED##################
#SinhCoeff.h 
#CosCoeffWithProdAsymm.h
#CosCoeffCombo.h
#CoshCoeff.h
#CoshCoeffTagAsReal.h
#CoshCoeffWithAsymms.h
#CoshCoeffCombo.h
#SinCoeffSimple.h 
#SinCoeffWithProdAsymm.h
#SinCoeffWithProdAsymmTagAsReal.h
#SinCoeffWithTaggingCalib.h
#SinCoeffWithAsymms.h
#SinCoeffCombo.h
#PolyTest.h 
#PowerLaw.h 
#SimpleSignedTaggedCPCoefficient.h
#SingleMistagCalibrationNoAsymmetries.h
#SingleMistagCalibrationWithAsymmetries.h
#TwoTaggerMistagCombination.h
#TwoTaggerTagCombination.h
#CombinationOfCalibratedMistags.h
#CombinationOfUncalibratedMistags.h
#CombinationOfMistagsWithAsymmetries.h
#CombinedTagCalibrated.h
#CombinedTagUncalibrated.h
####DEPRECATED##################
DESTINATION include/doofit/roofit/functions)

install(FILES
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/CategoryToReal_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/Sum_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/Product_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/TripleProduct_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/CPCoefficient_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/LinearFunctionWithOffset_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/QuadraticFunctionWithOffset_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/AtanAcceptanceIncludingBeta_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/AtanAcceptance_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/TauCorrectionWithBetaFactor_dict_rdict.pcm
${CMAKE_BINARY_DIR}/src/doofit/roofit/functions/DooDecRateCoeff_dict_rdict.pcm
DESTINATION lib)
add_subdirectory(bdecay)
