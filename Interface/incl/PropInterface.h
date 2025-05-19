#pragma once

#ifdef WIN32
#ifdef PROPINF_EXPORTS
#define PROPINF_EXPORT_CLASS __declspec(dllexport)
#define PROPINF_EXPORT_FUNC extern __declspec(dllexport)
#else
#define PROPINF_EXPORT_CLASS __declspec(dllimport)
#define PROPINF_EXPORT_FUNC extern __declspec(dllimport)
#endif
#else
#define PROPINF_EXPORT_CLASS
#define PROPINF_EXPORT_FUNC extern
#endif

#include "LishuttleCtl.h"
#include "UPlanCommonExport.h"
