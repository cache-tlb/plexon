//
// MATLAB Compiler: 6.0 (R2015a)
// Date: Fri Jun 26 16:53:41 2015
// Arguments: "-B" "macro_default" "-W" "cpplib:MatLib" "-T" "link:lib"
// "ImplElec_Test.m" "ImplElec_Train.m" "SaveCapturedData.m" "Entry_Train.m"
// "Entry_Test.m" "Mat_ECOG_Test.m" "Mat_ECOG_Train.m" 
//

#include <stdio.h>
#define EXPORTING_MatLib 1
#include "MatLib.h"

static HMCRINSTANCE _mcr_inst = NULL;


#if defined( _MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__LCC__)
#ifdef __LCC__
#undef EXTERN_C
#endif
#include <windows.h>

static char path_to_dll[_MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetModuleFileName(hInstance, path_to_dll, _MAX_PATH) == 0)
            return FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_MatLib_C_API
#define LIB_MatLib_C_API /* No special import/export declaration */
#endif

LIB_MatLib_C_API 
bool MW_CALL_CONV MatLibInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("MatLib"), path_to_dll, _MAX_PATH))
    return false;
    {
        mclCtfStream ctfStream = 
            mclGetEmbeddedCtfStream(path_to_dll);
        if (ctfStream) {
            bResult = mclInitializeComponentInstanceEmbedded(   &_mcr_inst,
                                                                error_handler, 
                                                                print_handler,
                                                                ctfStream);
            mclDestroyStream(ctfStream);
        } else {
            bResult = 0;
        }
    }  
    if (!bResult)
    return false;
  return true;
}

LIB_MatLib_C_API 
bool MW_CALL_CONV MatLibInitialize(void)
{
  return MatLibInitializeWithHandlers(mclDefaultErrorHandler, mclDefaultPrintHandler);
}

LIB_MatLib_C_API 
void MW_CALL_CONV MatLibTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_MatLib_C_API 
void MW_CALL_CONV MatLibPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(&stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_MatLib_C_API 
bool MW_CALL_CONV mlxImplElec_Test(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "ImplElec_Test", nlhs, plhs, nrhs, prhs);
}

LIB_MatLib_C_API 
bool MW_CALL_CONV mlxImplElec_Train(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "ImplElec_Train", nlhs, plhs, nrhs, prhs);
}

LIB_MatLib_C_API 
bool MW_CALL_CONV mlxSaveCapturedData(int nlhs, mxArray *plhs[], int nrhs, mxArray 
                                      *prhs[])
{
  return mclFeval(_mcr_inst, "SaveCapturedData", nlhs, plhs, nrhs, prhs);
}

LIB_MatLib_C_API 
bool MW_CALL_CONV mlxEntry_Train(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "Entry_Train", nlhs, plhs, nrhs, prhs);
}

LIB_MatLib_C_API 
bool MW_CALL_CONV mlxEntry_Test(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "Entry_Test", nlhs, plhs, nrhs, prhs);
}

LIB_MatLib_C_API 
bool MW_CALL_CONV mlxMat_ECOG_Test(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "Mat_ECOG_Test", nlhs, plhs, nrhs, prhs);
}

LIB_MatLib_C_API 
bool MW_CALL_CONV mlxMat_ECOG_Train(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "Mat_ECOG_Train", nlhs, plhs, nrhs, prhs);
}

LIB_MatLib_CPP_API 
void MW_CALL_CONV ImplElec_Test(int nargout, mwArray& vecMov, const mwArray& 
                                matSpikeCount, const mwArray& strFileName)
{
  mclcppMlfFeval(_mcr_inst, "ImplElec_Test", nargout, 1, 2, &vecMov, &matSpikeCount, &strFileName);
}

LIB_MatLib_CPP_API 
void MW_CALL_CONV ImplElec_Train(int nargout, mwArray& fErr, const mwArray& 
                                 matSpikeCount, const mwArray& matMov, const mwArray& 
                                 nEpo, const mwArray& strFileName, const mwArray& 
                                 nWorkMode)
{
  mclcppMlfFeval(_mcr_inst, "ImplElec_Train", nargout, 1, 5, &fErr, &matSpikeCount, &matMov, &nEpo, &strFileName, &nWorkMode);
}

LIB_MatLib_CPP_API 
void MW_CALL_CONV SaveCapturedData(int nargout, mwArray& res, const mwArray& 
                                   matSpikeCount, const mwArray& matMov, const mwArray& 
                                   strFileName)
{
  mclcppMlfFeval(_mcr_inst, "SaveCapturedData", nargout, 1, 3, &res, &matSpikeCount, &matMov, &strFileName);
}

LIB_MatLib_CPP_API 
void MW_CALL_CONV Entry_Train(int nargout, mwArray& fErr, const mwArray& data_name, const 
                              mwArray& nEpo, const mwArray& nWorkMode)
{
  mclcppMlfFeval(_mcr_inst, "Entry_Train", nargout, 1, 3, &fErr, &data_name, &nEpo, &nWorkMode);
}

LIB_MatLib_CPP_API 
void MW_CALL_CONV Entry_Test(int nargout, mwArray& predict_res, const mwArray& 
                             matSpikeCount, const mwArray& strFileName)
{
  mclcppMlfFeval(_mcr_inst, "Entry_Test", nargout, 1, 2, &predict_res, &matSpikeCount, &strFileName);
}

LIB_MatLib_CPP_API 
void MW_CALL_CONV Mat_ECOG_Test(int nargout, mwArray& nLabel, const mwArray& matECOGData, 
                                const mwArray& strFileName)
{
  mclcppMlfFeval(_mcr_inst, "Mat_ECOG_Test", nargout, 1, 2, &nLabel, &matECOGData, &strFileName);
}

LIB_MatLib_CPP_API 
void MW_CALL_CONV Mat_ECOG_Train(int nargout, mwArray& nRtn, const mwArray& 
                                 vecStrFileName, const mwArray& MAXROUTES, const mwArray& 
                                 indTriger, const mwArray& spRateOri, const mwArray& 
                                 nDown, const mwArray& strResultFileName)
{
  mclcppMlfFeval(_mcr_inst, "Mat_ECOG_Train", nargout, 1, 6, &nRtn, &vecStrFileName, &MAXROUTES, &indTriger, &spRateOri, &nDown, &strResultFileName);
}

