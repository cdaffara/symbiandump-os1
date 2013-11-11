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


#ifndef LBSNETSIMSERVER_H
#define LBSNETSIMSERVER_H

// Includes
#include "lbsnetsim.h"
#include "lbsnetsimhandlerretriever.h"

// Forwards
class MLbsNetSimSessionHandler;
class CLbsNetSimTestHandler;
class CLbsNetSimGatewayHandler;
class CLbsNetSimAssistanceDataProvider;

class CLbsNetSimServer : 
	public CPolicyServer,
	public MLbsNetSimHandlerRetriever
	{
	public:
		virtual CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
		static TInt StartServer();
		~CLbsNetSimServer();
		
		void IncClientCount();
		void DecClientCount();
		
		// MLbsNetSimHandlerRetreiver
		virtual MLbsNetSimSessionHandler* GetGatewayHandler();
		virtual MLbsNetSimSessionHandler* GetTestHandler();
	private:
		// Constructors
		static CLbsNetSimServer* NewL();
		void ConstructL();
		CLbsNetSimServer(TInt aPriority);
		
		
		static void StartServerL();
	private:
		CLbsNetSimAssistanceDataProvider*	iAssistanceDataProvider;
		CLbsNetSimTestHandler*				iTestHandler;
		CLbsNetSimGatewayHandler*			iGatewayHandler;
		TInt								iClientCount;
	};

#endif // LBSNETSIMSERVER_H