/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* GPRS Connection progress enums
* 
*
*/



/**
 @file GPRSPROG.H
*/



#ifndef __GPRSCONNECTPROG_H__
#define __GPRSCONNECTPROG_H__

#include <e32def.h>

/**
New software should use the PSD progress values defined in psdprog.h

@publishedAll
@deprecated 7.0s - maintained for compatibility with 6.1
*/
enum TGprsProgress
	{
	EStartingConfiguration = 50, 	///< Starting Configuration
	EFinishedConfiguration,			///< Finished Configuration
	EStartingActivation,			///< Starting Activation
	EFinishedActivation,			///< Finished Activation
	EAnsweringIncoming,				///< Answering Incoming
	ESuspended,						///< Suspended
	EStartingDeactivation,			///< Starting Deactivation
	EFinishedDeactivation			///< Finished Deactivation
	};

#endif

