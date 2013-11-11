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
// lbsnetsimassitancedataproviderobserver.h
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef MLBSNETSIMASSITANCEDATAPROVIDEROBSERVER_H
#define MLBSNETSIMASSITANCEDATAPROVIDEROBSERVER_H

#include "lbsnetsim.h"


class MLbsNetSimAssistanceDataProviderObserver
/** Observer class for CLbsAssistanceDataProvider class.

@private
*/
	{
	public:
		virtual void ProcessAssistanceDataL(TInt aError,
											RLbsAssistanceDataBuilderSet& aAssistanceData,
										    TPositionInfo& aPosition, 
										    TLbsNetPosRequestQuality& aQuality) = 0;
	};



#endif // MLBSNETSIMASSITANCEDATAPROVIDEROBSERVER_H