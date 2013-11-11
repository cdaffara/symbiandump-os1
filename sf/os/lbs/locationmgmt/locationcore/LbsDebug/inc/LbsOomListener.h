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

#ifndef __LBSOOMLISTENER_H__
#define __LBSOOMLISTENER_H__

//****************************************************************
// Includes
//****************************************************************
// System
#include <e32std.h>
#include <e32base.h>


// LBS

// Component
#include "LbsOomTest.h"

//****************************************************************
// Classes
//****************************************************************
/** 
The Oom listener is responsible for setting the Oom property and
enable/disable heap allocation failure accordingly
@see CActive
@internalComponent
@released
*/
class CLbsOomListener : public CActive
	{
public:
	IMPORT_C static CLbsOomListener* NewL();
	IMPORT_C virtual ~CLbsOomListener();
	IMPORT_C void StartGettingRequests();
	
protected:
	void ConstructL();
	CLbsOomListener();

	//from CActive	
	void RunL();
	void DoCancel();

private:
 	ROomRequest iRequestOom;
	TBool 		iFlag; // flag to state whether OOM Test has been done before or not
	};
	
#endif //__LBSOOMLISTENER_H__
