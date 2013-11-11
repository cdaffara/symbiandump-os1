// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PARAM_MESS_NAME_H_
#define PARAM_MESS_NAME_H_

//Include the suite header
#include "CSuite.h"

PARAM_FOREACH_LIST_BEGIN
#include "PARAM_SVR_LISTHEADERFILE"
PARAM_FOREACH_LIST_END


class CPARAM_MESS_NAMEStep: public CCapabilityTestStep
	{

public:

	//Get the version of the server to be called
	TVersion Version()
	{
		return TVersion(PARAM_SVR_MAJORNAME, PARAM_SVR_MINORNAME, PARAM_SVR_BUILDNAME);
	}	
	
	
	//Constructor called from the respective Suite.cpp from their "AddTestStep" function
	CPARAM_MESS_NAMEStep() ;

	//Always clean your mess
	~CPARAM_MESS_NAMEStep()
	{
		tChildThread.Close();
	}		
	
	//This is the Function called from "doTestStepL" by the test Suite,and it creates an 
	//child thread which internally calls the corresponding Exec_SendReceive_SERVERNAME fn.
	TVerdict MainThread();

	//Here's where the connection and testing the message takes place
	TInt Exec_SendReceive();
	
	};

#endif
