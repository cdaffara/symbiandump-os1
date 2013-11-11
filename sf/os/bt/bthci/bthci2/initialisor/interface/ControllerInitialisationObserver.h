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
 @publishedPartner
 @released
*/

#ifndef CONTROLLERINITIALISATIONOBSERVER_H
#define CONTROLLERINITIALISATIONOBSERVER_H

#include <e32def.h>

class MControllerInitialisationObserver
	{
public:
	/**	
	Indicates that pre-reset is complete.  Corresponds to
	MControllerInitialisationInterface::MciiPreResetCommand
	
	@param aError indicates any error that occurred during pre-reset
	*/
	virtual void McioPreResetCommandComplete(TInt aError) = 0;

	/**	
	Indicates that post-reset is complete.  Corresponds to
	MControllerInitialisationInterface::MciiPostResetCommand
	
	@param aError indicates any error that occurred during post-reset
	*/
	virtual void McioPostResetCommandComplete(TInt aError) = 0;
	};

#endif // CONTROLLERINITIALISATIONOBSERVER_H
