// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if !defined (__SUSPANICCODES_H__)
#define __SUSPANICCODES_H__

#include <e32std.h>

_LIT(KPanicSsmSus, "Ssm-Sus");

enum TSusPanicCodes
	{
	//SusPluginLoader errors
	EPluginLoaderError1 = 1,
	
	//CSusPluginFrame errors
	EPluginFrameError1,
	EPluginFrameError2,
	
	//CSusUtilServer errors
	EUtilServerError1,
	EUtilServerError2,
	
	//CSusAdaptionServer errors
	EAdaptionServerError1,
	EAdaptionServerError2,
	ELibraryAlreadyOpen,
	
	//Emergency Adaptation panic codes
	EEmergencyCallRfAdaptationNullPtrError1,
	EEmergencyCallRfAdaptationNullPtrError2,
	EEmergencyAdaptationMessageNullPtrError
	};

#endif // __SUSPANICCODES_H__
