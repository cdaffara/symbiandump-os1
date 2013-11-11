// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
 @test
*/

#ifndef __LBSPROTOCOLMODULEERRORS_H__
#define __LBSPROTOCOLMODULEERRORS_H__

_LIT(KProtocolModulePanic,"lbsprotocolmodule");

enum TProtocolModulePanic
	{
	/** Unknown */
	EProtocolModuleUnknown				= 1,

	/** State machine cancel source is invalid */
	EProtocolModuleCancelSource			= 2,

	/** State machine transition stage is invalid */
	EProtocolModuleTransitionStage		= 3,

	/** Assistance Data Action is invalid */
	EProtocolModuleAssistDataAction		= 4,

	/** Session ID provided by LBS is invalid */
	EProtocolModuleSessionId			= 5,

	/** Queued request ID is invalid */
	EProtocolModuleQueueRequestId		= 6,

	/** MO-LR state is invalid */
	EProtocolModuleMoLrState			= 7,

	/** MT-LR state is invalid */
	EProtocolModuleMtLrState			= 8,

	/** X3P state is invalid */
	EProtocolModuleX3pState				= 9,

	/** X3P destination too large */
	EProtocolModuleX3PDestination		= 10,

	/** LBS request when no active state machine */
	EProtocolModuleLbsNoActiveMachine	= 11,

	/** Network request when no active state machine */
	EProtocolModuleNetNoActiveMachine	= 12,

	/** Invalid type of Assistance data specified */
	EProtocolModuleAssistDataType		= 13,

	/** Network Based Location state is invalid */
	EProtocolModuleNetLocState			= 14
	};


#endif //__LBSPROTOCOLMODULEERRORS_H__
