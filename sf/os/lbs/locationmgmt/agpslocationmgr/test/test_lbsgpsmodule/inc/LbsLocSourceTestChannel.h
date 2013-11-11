// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Header file for the api for talking to the stub AGPS Module
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef LBSAGPSMODULETESTAPI_H_
#define LBSAGPSMODULETESTAPI_H_

#include <e32base.h>
#include <e32property.h>


/**
@internalComponent
@released

The interface is unidirectional (from AGPS Module to a Test Suite)
and does not pass parameters.
*/
class RAgpsModuleTestChannel
	{
public:
	/**
	The message types correspond to methods of the CLbsLocationSourceGpsBase interface.
	*/
	enum TTestMsgType
		{
		ETestMsgUnknown 				= 0,
		ETestMsgNewL					= 1,
		ETestMsgRequestLocationUpdate	= 2,
		ETestMsgCancelLocationRequest	= 3,
		ETestMsgAdvisePowerMode			= 4,
		ETestMsgSetGpsOptions			= 5,
		ETestMsgAssistanceDataEvent		= 6,
		ETestMsgShutdownEvent			= 7
		};

	IMPORT_C RAgpsModuleTestChannel();
	
	IMPORT_C static void InitialiseL();
	IMPORT_C static void Shutdown();
	
	IMPORT_C void OpenL();
	IMPORT_C void Close();

	IMPORT_C void Subscribe(TRequestStatus &aRequest);
	IMPORT_C void Cancel();
	
	IMPORT_C TInt SetMsg(TTestMsgType aMsgType);
	IMPORT_C TInt GetMsg(TTestMsgType& aMsgType);

private:
	RProperty iPropFromModule;
	//RProperty iPropToMdoule;
	};
	
	
#endif // LBSAGPSMODULETESTAPI_H_
