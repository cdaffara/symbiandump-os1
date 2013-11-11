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


#ifndef LBSNETSIMSESSIONHANDLER_H
#define LBSNETSIMSESSIONHANDLER_H

#include "lbsnetsim.h"

class MLbsNetSimSessionHandler
	{
	public:
		virtual void ServiceL(const RMessage2& aMessage) = 0;
		virtual void Connected() = 0;
		virtual void Disconnected() = 0;
	};
	


#endif // LBSNETSIMSESSIONHANDLER_H