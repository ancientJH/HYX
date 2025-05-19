#pragma once

#ifdef WIN32
	#ifdef PROPFRAME_EXPORTS
		#define PROPFRAME_EXPORT_CLASS __declspec(dllexport)
		#define PROPFRAME_EXPORT_FUNC extern __declspec(dllexport)
	#else
		#define PROPFRAME_EXPORT_CLASS __declspec(dllimport)
		#define PROPFRAME_EXPORT_FUNC extern __declspec(dllimport)
	#endif
#else
	#define PROPFRAME_EXPORT_CLASS
	#define PROPFRAME_EXPORT_FUNC extern
#endif

#include "LishuttleCtl.h"
#include "UPlanCommonExport.h"
