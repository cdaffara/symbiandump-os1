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


#ifndef LBSNETSIMHANDLERRETRIEVER_H
#define LBSNETSIMHANDLERRETRIEVER_H

#include "lbsnetsim.h"


class MLbsNetSimSessionHandler;
class CTestHandler;


class MLbsNetSimHandlerRetriever
	{
	public:
		virtual MLbsNetSimSessionHandler* GetGatewayHandler() = 0;
		virtual MLbsNetSimSessionHandler* GetTestHandler() = 0;
	};
#endif // LBSNETSIMHANDLERRETRIEVER_H
