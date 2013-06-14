/** @mainpage DooFit fitting framework
 *
 * \tableofcontents
 *
 * @section mp_introduction Introduction
 *
 * DooFit is a RooFit-based fitting framework intended to assist in fitting with
 * RooFit by providing high level functions for generating complex PDFs, fitting
 * these, plotting or conducting toy studies.
 *
 * @section mp_topics What DooFit can be used for
 *
 * Here is an overview of the current DooFit functionality:
 *
 * @li @link doofit::toy doofit::toy: Automated toy sample generation, toy study assistance and analysis @endlink
 * @li @link doofit::plotting doofit::plotting: Easy plotting without all the fuss @endlink
 * @li @link doofit::builder::EasyPdf doofit::builder::EasyPdf: Simple and efficient on-demand PDF building @endlink
 *
 * @section mp_demo Demonstration and examples
 *
 * Usage examples and demonstration can be found on the @ref demo page.
 *
 * @section mp_simplesetup Simple setup via global DooSoftware installation
 *
 * On all machines with a DooSoftware installation setup of DooFit is very
 * simple. In this case you can skip the next building step. Just call
 * dooSoftwareSetup after setting up LHCb software and DaVinci/ROOT and the
 * DooSoftware magic will do the rest.
 *
 * @section mp_installation Manual Installation
 *
 * The intended usage of DooFit is as a framework and therefore @a not to build
 * custom user code inside of DooFit. However, DooFit can be easily installed
 * and interfaced from other external projects. FindDooFit CMake modules are
 * supplied which allow easy integration of DooFit in any other CMake project.
 *
 * @subsection mp_building Building DooFit
 *
 * First set the environment variable @c $DOOFITSYS to the install location
 * as set above. If not installing into a standard location like @c /usr/local
 * or similar, you also have to set @c $LD_LIBRARY_PATH (on Linux) or
 * @c $DYLD_LIBRARY_PATH (on Mac OS) to include @c $DOOFITSYS/lib. Best practice
 * probably is to set these in your <tt>~/.bashrc</tt> or globally (see remark
 * about DooSoftware below to skip these steps)..
 *
 * @code
 * export DOOFITSYS=/path/to/install/to
 * export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DOOFITSYS/lib
 * export PATH=$PATH:$DOOFITSYS/bin
 * @endcode
 *
 * If setting in your <tt>~/.bashrc</tt>, remember to log out and in once or
 * copy the corresponding lines also into your terminal (in case of DooSoftware
 * installation you can just use ~/LocalDooSoftware.sh, see
 * https://wiki.e5.physik.tu-dortmund.de/LHCb/Software/DooSoftwareUsage).
 *
 * To build DooFit, download it either as a release version or checkout from
 * Git. Create a @c build directory where you invoke CMake and install DooFit.
 *
 * @code
 * /path/of/doofit/source/scripts/rebuild_cmake.sh
 * make
 * make install
 * @endcode
 *
 *
 * @subsection mp_externalusage Using DooFit from an external project
 *
 * In the external project, adapt your <tt>CMakeLists.txt</tt> file to find
 * DooFit and copy the provided <tt>FindDooFit.cmake</tt> (can be found in
 * <tt>external/</tt> directory of DooFit) into your CMake modules path (which
 * can be configured in CMakeLists.txt). The following example shows this
 * procedure:
 *
 * @code
 * # Append local directory cmake/Modules to $CMAKE_MODULE_PATH.
 * # This is where you want to put the provided FindDooFit.cmake.
 * set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
 *
 * # Try to find DooFit (this requires $DOOFITSYS to be set)
 * find_package(DooFit)
 *
 * # Tell CMake to also use DooFit includes and libraries in its search path for
 * # includes and libraries.
 * include_directories(${DooFit_INCLUDE_DIR})
 * link_directories(${DooFit_LIBRARY_DIR})
 *
 * # Only if DooFit is found build this executable and link it.
 * # this will protect the complete project from not compiling for anyone not
 * # having DooFit installed.
 * if (DooFit_FOUND)
 *   add_executable(MyBinary MainBinaryMain.cpp)
 *   target_link_libraries(MyBinary ${DooFit_LIBRARIES} ${MY_OTHER_LIBRARIES})
 * endif (DooFit_FOUND)
 * @endcode
 *
 * @section mp_usage How to use DooFit objects
 *
 * Using DooFit from external projects is simple. However, one should get an
 * idea of the design philosophy to understand the way, DooFit should be used.
 * An integral part is the doofit::Config system which relies on having a
 * distinct config object for each sub-module of DooFit. Config objects are the
 * interface between user and module. They are needed at initialization of a
 * module object and can be configured via setter functions, command line
 * arguments and config files. One always requires a doofit::CommonConfig
 * instance and the specific instance of the module to use.
 *
 * Take this example:
 *
 * @code
 * #include "doofit/Config/CommonConfig.h"
 * #include "doofit/Toy/ToyFactoryStd/ToyFactoryStd.h"
 * #include "doofit/Toy/ToyFactoryStd/ToyFactoryStdConfig.h"
 * #include "doofit/Toy/ToyStudyStd/ToyStudyStd.h"
 * #include "doofit/Toy/ToyStudyStd/ToyStudyStdConfig.h"
 *
 * int main(int argc, char* argv[]) {
 *   // Initialize config objects (mandatory).
 *   // Notice how the first object gets initialized via argc/argv from command
 *   // line. All following config objects get the former config object passed
 *   // on to take over all previously unrecognized options.
 *   // Parsing a config file will be handled automatically and can be
 *   // configured via -c or --config-file command line option.
 *   // See examples/toys directory for config file examples.
 *   doofit::Config::CommonConfig cfg_com("common");
 *   cfg_com.InitializeOptions(argc, argv);
 *   doofit::Toy::ToyFactoryStdConfig cfg_tfac("toyfac");
 *   cfg_tfac.InitializeOptions(cfg_com);
 *
 *   // Check for a set --help flag and if so, print help and exit gracefully
 *   // (recommended).
 *   cfg_com.CheckHelpFlagAndPrintHelp();
 *
 *   // More custom code, e.g. to set options internally.
 *   // Not required as configuration via command line/config file is enough.
 *   ...
 *
 *   // Print overview of all options (optional)
 *   cfg_com.PrintAll();
 *
 *   // Initialize the toy factory module with the config objects and start
 *   // generating toy samples.
 *   doofit::Toy::ToyFactoryStd tfac(cfg_com, cfg_tfac);
 *   RooDataSet* data = tfac.Generate();
 * }
 * @endcode
 *
 */


/**
 * @page demo DooFit demonstration
 *
 * \tableofcontents
 *
 * @section demo_example1 Example 1: Toy studies, EasyPdf building, Plotting
 *
 * This complete program demonstrates a sceanario for toy studies, utilizing 
 * doofit::builder::EasyPdf for PDF building, doofit::plotting for plots and
 * doofit::toy for toy generation and fit result storage as well as evaluation.
 *
 * The following program shows the functionality in three functions.
 *
 * @verbinclude ToyTestMain.cpp
 *
 * To run, you might also need the following parameter file as @c parameters.txt
 * which initializes parameters to sane values.
 *
 * @verbinclude parameters.txt
 *
 * The following file (e.g. @c config.cfg) can be used to configure the toy 
 * factory and study. It can be supplied to the binary via @c -c @c config.cfg.
 *
 * @verbinclude config.cfg
 */

// Global namespace documentation

/** @namespace doofit
 *
 *  @brief Global DooFit namespace
 *
 *  This namespace by definition contains all DooFit namespaces, classes and
 *  functions to ensure that DooFit can be used without naming conflicts inside
 *  other projects.
 *
 */

/** @namespace doofit::builder
 *  @brief Everything that creates and combines PDFs
 *
 *  This namespace contains all PDF building utilities of DooFit.
 *
 *  @section mp_topics Which DooFit builders are available?
 *
 *  Here is an overview of the working DooFit builders:
 *
 *  @li @link doofit::builder::EasyPdf doofit::builder::EasyPdf: Simple and efficient on-demand PDF building @endlink
 */

/** @namespace doofit::toy
 *
 *  @brief DooFit Toy subsystem.
 *
 *  This namespace is responsible for toy sample creation and toy studies. Main
 *  objects of interest are Toy::ToyFactoryStd and Toy::ToyStudyStd and their
 *  accompanying Config objects Toy::ToyFactoryStdConfig and
 *  Toy::ToyStudyStdConfig.
 *
 *  Take the following usage example:
 *
 * @code
 * #include "doofit/config/CommonConfig.h"
 * #include "doofit/toy/ToyFactoryStd/ToyFactoryStd.h"
 * #include "doofit/toy/ToyFactoryStd/ToyFactoryStdConfig.h"
 * #include "doofit/toy/ToyStudyStd/ToyStudyStd.h"
 * #include "doofit/toy/ToyStudyStd/ToyStudyStdConfig.h"
 *
 * int main(int argc, char* argv[]) {
 *   // Initialize config objects (mandatory).
 *   // Notice how the first object gets initialized via argc/argv from command
 *   // line. All following config objects get the former config object passed
 *   // on to take over all previously unrecognized options.
 *   // Parsing a config file will be handled automatically and can be
 *   // configured via -c or --config-file command line option.
 *   // See examples/toys directory for config file examples.
 *   doofit::config::CommonConfig cfg_com("common");
 *   cfg_com.InitializeOptions(argc, argv);
 *   doofit::toy::ToyFactoryStdConfig cfg_tfac("toyfac");
 *   cfg_tfac.InitializeOptions(cfg_com);
 *   ToyStudyStdConfig cfg_tstudy("toystudy");
 *   cfg_tstudy.InitializeOptions(cfg_tfac);
 *
 *   // set a previously defined workspace to get PDF from (not mandatory, but convenient)
 *   cfg_tfac.set_workspace(my_workspace);
 *
 *   // Check for a set --help flag and if so, print help and exit gracefully
 *   // (recommended).
 *   cfg_com.CheckHelpFlagAndPrintHelp();
 *
 *   // More custom code, e.g. to set options internally.
 *   // Not required as configuration via command line/config file is enough.
 *   ...
 *
 *   // Print overview of all options (optional)
 *   cfg_com.PrintAll();
 *
 *   // Initialize the toy factory module with the config objects and start
 *   // generating toy samples.
 *   doofit::toy::ToyFactoryStd tfac(cfg_com, cfg_tfac);
 *   RooDataSet* data = tfac.Generate();
 *
 *   // fitting on the generated dataset is your responsibility
 *   RooFitResult* fit_result = my_workspace->pdf("mypdf")->fitTo(*data);
 *
 *   // Store the fit result of the toy fit
 *   doofit::toy::ToyStudyStd tstudy(cfg_com, cfg_tstudy);
 *   tstudy.StoreFitResult(fit_result);
 *
 *   // assume, we're in another program now, analyzing toy fits
 *   // the next line is only necessary if storing and reading fit results in
 *   // one program
 *   tstudy.FinishFitResultSaving();
 *
 *   // do the complete automated analysis of toy fits
 *   tstudy.ReadFitResults();
 *   tstudy.EvaluateFitResults();
 *   tstudy.PlotEvaluatedParameters();
 * }
 * @endcode
 *
 *  Notice the mandatory doofit::config objects being created before usage. They
 *  are initialized via the command line arguments argc and argv. These also
 *  give the possibility to pass a config file for easy configuration. Calling
 *  your program with the "--help" parameter will print available options. All
 *  configuration is handled via these Config objects and nearly everything can
 *  be configured via the command line or config file (except setting of
 *  internal objects like RooWorkspaces etc.).
 *
 *  The interface of doofit::toy::ToyFactoryStd is very minimal. It only offers
 *  to generate a dataset.
 *
 *  doofit::toy::ToyStudyStd is the interface for storing fit results of toy
 *  fits and also for the analysis of fit results after all toy fits are
 *  completed. Storing is automatic and collision-safe even in case of several
 *  instances writing into the same file (e.g. on a network filesystem). Notice
 *  that some cluster filesystems like FhGFS might not be able to handle the
 *  load of several processes trying to access one file, a moderate NFS server
 *  seems to be able to perfectly handle the load.
 *
 *  In the analysis afterwards, doofit::toy::ToyStudyStd can automatically read
 *  and analyse toy fits and produce plots of distributions of interest
 *  (pulls, residuals, ...) for all used parameters. Accessing individual fit
 *  results is also possible.
 *
 *  For submitting toy jobs via PBS, see PBSJobBuilder and PBSJobScheduler.
 *
 *  @author Florian Kruse
 */

