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
// A unit test wrapper over the RLbsSystemController class
// 
//

#include "Te_LbsSuplPushLbsSystemController.h"
#include "Te_LbsSuplErrorPattern.h"


/**
Opens the session with the LBS root admin interface.

@param aModuleId [In] The UID of the module opening the connection.

@leave If a error happens, it leaves with one of the system error codes. 

@see RLbsSystemController::OpenL
*/
void RTe_LbsSuplPushLbsSystemController::OpenL(TUid aModuleId)
	{
	iLbsSystemController.OpenL(aModuleId);
	}


/**
Closes the session with the LBS root admin interface.

@see RLbsSystemController::Close
*/
void RTe_LbsSuplPushLbsSystemController::Close()
	{
	iLbsSystemController.Close();
	}

/**
Requests the LBS to start.

@return KErrNone if successful, one of the system error codes otherwise.

@see RLbsSystemController::RequestSystemStartup
*/
TInt RTe_LbsSuplPushLbsSystemController::RequestSystemStartup()
	{
	TInt err = RTe_LbsSuplErrorPattern::CheckNextError(
			Te_LbsSuplErrorId::ERLbsSystemController_RequestSystemStartup);
	if(err==KErrNone)
		{
		return iLbsSystemController.RequestSystemStartup();
		}
	return err;
	}
