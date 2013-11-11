/**
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file CSDPROG.H
 @internalComponent
*/

#ifndef __CSDPROG_H__
#define __CSDPROG_H__

#include <nifvar.h>

/**
Stages in establishing a circuit switched data connection
*/
enum TCsdProgress
	{
	ECsdStartingDialling			= KCsdStartingDialling,
	ECsdFinishedDialling			= KCsdFinishedDialling,
	ECsdScanningScript				= KCsdScanningScript,
	ECsdScannedScript				= KCsdScannedScript,
	ECsdGettingLoginInfo			= KCsdGettingLoginInfo,
	ECsdGotLoginInfo				= KCsdGotLoginInfo,
	ECsdStartingConnect				= KCsdStartingConnect,
	ECsdFinishedConnect				= KCsdFinishedConnect,
	ECsdStartingLogIn				= KCsdStartingLogIn,
	ECsdFinishedLogIn				= KCsdFinishedLogIn,
	ECsdStartingAnswer				= KCsdStartingAnswer,
	ECsdAnswered					= KCsdAnswered,
	ECsdConnectionOpen              = KCsdConnectionOpen,
	ECsdStartingHangUp				= KCsdStartingHangUp,
	ECsdFinishedHangUp              = KCsdFinishedHangUp
	};

#endif
