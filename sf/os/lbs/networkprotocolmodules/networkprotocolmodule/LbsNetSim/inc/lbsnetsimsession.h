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


#ifndef LBSNETSIMSESSION_H
#define LBSNETSIMSESSION_H

#include "lbsnetsim.h"

// Forwards
class MLbsNetSimHandlerRetriever;
class MLbsNetSimSessionHandler;
class CLbsNetSimServer;

class CLbsNetSimSession : public CSession2
	{
	public:
		// Constructor
		CLbsNetSimSession(MLbsNetSimHandlerRetriever* aRetriever);
		virtual ~CLbsNetSimSession();
		
		// CSession2
		void ServiceL(const RMessage2& aMessage);
		void Disconnect(const RMessage2& aMessage);
	protected:
		CLbsNetSimServer* Server();
	private:
		void RetrieveTestHandler();
		void RetrieveGatewayHandler();
		void ForwardToHandlerL(const RMessage2& aMessage);
	private:
#ifdef _DEBUG
		TBufC<256>								iWhich;
#endif
		
		MLbsNetSimSessionHandler*				iHandler; // Session handler
		MLbsNetSimHandlerRetriever*				iRetriever; // Used to retrieve handler
	};


#endif // LBSNETSIMSESSION_H


