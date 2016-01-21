//
// MATLAB Compiler: 6.0 (R2015a)
// Date: Fri Jun 26 16:53:41 2015
// Arguments: "-B" "macro_default" "-W" "cpplib:MatLib" "-T" "link:lib"
// "ImplElec_Test.m" "ImplElec_Train.m" "SaveCapturedData.m" "Entry_Train.m"
// "Entry_Test.m" "Mat_ECOG_Test.m" "Mat_ECOG_Train.m" 
//

#ifndef __MatLib_h
#define __MatLib_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_MatLib
#define PUBLIC_MatLib_C_API __global
#else
#define PUBLIC_MatLib_C_API /* No import statement needed. */
#endif

#define LIB_MatLib_C_API PUBLIC_MatLib_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_MatLib
#define PUBLIC_MatLib_C_API __declspec(dllexport)
#else
#define PUBLIC_MatLib_C_API __declspec(dllimport)
#endif

#define LIB_MatLib_C_API PUBLIC_MatLib_C_API


#else

#define LIB_MatLib_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_MatLib_C_API 
#define LIB_MatLib_C_API /* No special import/export declaration */
#endif

extern LIB_MatLib_C_API 
bool MW_CALL_CONV MatLibInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_MatLib_C_API 
bool MW_CALL_CONV MatLibInitialize(void);

extern LIB_MatLib_C_API 
void MW_CALL_CONV MatLibTerminate(void);



extern LIB_MatLib_C_API 
void MW_CALL_CONV MatLibPrintStackTrace(void);

extern LIB_MatLib_C_API 
bool MW_CALL_CONV mlxImplElec_Test(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_MatLib_C_API 
bool MW_CALL_CONV mlxImplElec_Train(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_MatLib_C_API 
bool MW_CALL_CONV mlxSaveCapturedData(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                      *prhs[]);

extern LIB_MatLib_C_API 
bool MW_CALL_CONV mlxEntry_Train(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_MatLib_C_API 
bool MW_CALL_CONV mlxEntry_Test(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_MatLib_C_API 
bool MW_CALL_CONV mlxMat_ECOG_Test(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

extern LIB_MatLib_C_API 
bool MW_CALL_CONV mlxMat_ECOG_Train(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_MatLib
#define PUBLIC_MatLib_CPP_API __declspec(dllexport)
#else
#define PUBLIC_MatLib_CPP_API __declspec(dllimport)
#endif

#define LIB_MatLib_CPP_API PUBLIC_MatLib_CPP_API

#else

#if !defined(LIB_MatLib_CPP_API)
#if defined(LIB_MatLib_C_API)
#define LIB_MatLib_CPP_API LIB_MatLib_C_API
#else
#define LIB_MatLib_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_MatLib_CPP_API void MW_CALL_CONV ImplElec_Test(int nargout, mwArray& vecMov, const mwArray& matSpikeCount, const mwArray& strFileName);

extern LIB_MatLib_CPP_API void MW_CALL_CONV ImplElec_Train(int nargout, mwArray& fErr, const mwArray& matSpikeCount, const mwArray& matMov, const mwArray& nEpo, const mwArray& strFileName, const mwArray& nWorkMode);

extern LIB_MatLib_CPP_API void MW_CALL_CONV SaveCapturedData(int nargout, mwArray& res, const mwArray& matSpikeCount, const mwArray& matMov, const mwArray& strFileName);

extern LIB_MatLib_CPP_API void MW_CALL_CONV Entry_Train(int nargout, mwArray& fErr, const mwArray& data_name, const mwArray& nEpo, const mwArray& nWorkMode);

extern LIB_MatLib_CPP_API void MW_CALL_CONV Entry_Test(int nargout, mwArray& predict_res, const mwArray& matSpikeCount, const mwArray& strFileName);

extern LIB_MatLib_CPP_API void MW_CALL_CONV Mat_ECOG_Test(int nargout, mwArray& nLabel, const mwArray& matECOGData, const mwArray& strFileName);

extern LIB_MatLib_CPP_API void MW_CALL_CONV Mat_ECOG_Train(int nargout, mwArray& nRtn, const mwArray& vecStrFileName, const mwArray& MAXROUTES, const mwArray& indTriger, const mwArray& spRateOri, const mwArray& nDown, const mwArray& strResultFileName);

#endif
#endif
