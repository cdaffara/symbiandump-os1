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
 @internalTechnology
*/

#ifndef __SS_FACTORYCONTAINERMAP_H__
#define __SS_FACTORYCONTAINERMAP_H__

#include <comms-infras/ss_common.h>

namespace ESock
	{

NONSHARABLE_CLASS(RPlaneFCMap)
	{
public:
	void Close();
	TInt AddPlaneFC(const Den::TPlayerRole& aPlane, const Messages::TNodeId& aFC);
	Messages::TNodeId GetPlaneFC(const Den::TPlayerRole& aPlane);
	void DropFCOfPeer(const Den::TPlayerRole& aPlane, CommsFW::TWorkerId aPeerWorker);

private:
	class TPlaneFCEntry
		{
	public:
		static TInt Cmp(const TPlaneFCEntry& aLHS, const TPlaneFCEntry& aRHS);

		Den::TPlayerRole iPlane;		//< OR'ing together not valid; each FC concerns a single plane
		Messages::TNodeId iFC;	//< Factory Container cookie
		};

	typedef RArray<TPlaneFCEntry> TPlaneFCMap;
	TPlaneFCMap iMap;
	};

	}	// namespace ESock

#endif	// __SS_FACTORYCONTAINERMAP_H__

