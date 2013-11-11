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
// cstep.h
// 
//


#ifndef __RSDbgMbufTotalSpace1_H_
#define __RSDbgMbufTotalSpace1_H_

//Include the suite header
#include "CSuite.h"



class CRSDbgMbufTotalSpace1Step: public CCapabilityTestStep
	{
public:

	//Get the version of the server to be called
	TVersion Version()
		{
		return TVersion(RootServer::KRS32MajorVersionNumber, RootServer::KRS32MinorVersionNumber, RootServer::KRS32BuildVersionNumber);
		}	
	
	//Constructor called from the respective Suite.cpp from their "AddTestStep" function
	CRSDbgMbufTotalSpace1Step();

	//Always clean your mess
	~CRSDbgMbufTotalSpace1Step()
		{
		tChildThread.Close();
		}
	
	//This is the Function called from "doTestStepL" by the test Suite,and it creates an 
	//child thread which internally calls the corresponding Exec_SendReceive_SERVERNAME fn.
	TVerdict MainThread();

	//Here's where the connection and testing the message takes place
	TInt Exec_SendReceive();
	};

#endif // __RSDbgMbufTotalSpace1_H_

