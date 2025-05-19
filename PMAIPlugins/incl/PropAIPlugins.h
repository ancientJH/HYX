#pragma once

#ifdef WIN32
#ifdef PMAIPLUGINS_EXPORTS
#define PROPAIPLUGINS_EXPORT_CLASS __declspec(dllexport)
#define PROPAIPLUGINS_EXPORT_FUNC extern __declspec(dllexport)
#else
#define PROPAIPLUGINS_EXPORT_CLASS __declspec(dllimport)
#define PROPAIPLUGINS_EXPORT_FUNC extern __declspec(dllimport)
#endif
#else
#define PROPAIPLUGINS_EXPORT_CLASS
#define PROPAIPLUGINS_EXPORT_FUNC extern
#endif

#include "LishuttleCtl.h"
#include "UPlanCommonExport.h"
