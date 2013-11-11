/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file te_TransportServer.h
*/
 
#if (!defined TE_TRANSPORT_SERVER_H)
#define TE_TRANSPORT_SERVER_H

#include <test/testexecuteserverbase.h>
#include "te_TransportStep.h" 



class CCommsTransportTestServer: public CTestServer
	{
	public:
	static CCommsTransportTestServer* NewL();
	~CCommsTransportTestServer();
		virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	
	private:

	};



#endif
