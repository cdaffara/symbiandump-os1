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
*
*/



/**
 @file PSDPROG.H
 @internalComponent
*/

#ifndef __PSDPROG_H__
#define __PSDPROG_H__

#include <nifvar.h>

/**
PSD Connection progress enums
*/
enum TPsdProgress
	{
	EPsdStartingConfiguration = KMinAgtProgress,  ///< Starting Psd Configuration
	EPsdFinishedConfiguration,					  ///< Finished Psd Configuration
	EPsdStartingActivation,						  ///< Starting Psd Activation	
	EPsdFinishedActivation = KConnectionOpen,	  ///< Finished Psd Activation
	EPsdAnsweringIncoming,						  ///< Answering Psd Incoming
	EPsdSuspended,								  ///< Psd Suspended
	EPsdStartingDeactivation,					  ///< Starting Psd Deactivation
	EPsdFinishedDeactivation = KConnectionClosed  ///<  Finished Psd Deactivation
	};

#endif

