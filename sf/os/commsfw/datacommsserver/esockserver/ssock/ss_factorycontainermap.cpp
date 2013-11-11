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
*/

#include "ss_factorycontainermap.h"

#include <ss_std.h>
#include <comms-infras/ss_log.h>

using namespace ESock;
using namespace Messages;
using namespace Den;
using namespace CommsFW;

void RPlaneFCMap::Close()
	{
	iMap.Close();
	}

TInt RPlaneFCMap::AddPlaneFC(const Den::TPlayerRole& aPlane, const TNodeId& aFC)
	{
	TPlaneFCEntry entry;
	entry.iPlane = aPlane;
	entry.iFC = aFC;
	return iMap.InsertInOrder(entry, TLinearOrder<TPlaneFCEntry>(TPlaneFCEntry::Cmp));
	}

TNodeId RPlaneFCMap::GetPlaneFC(const Den::TPlayerRole& aPlane)
	{
	TNodeId fc;
	TPlaneFCEntry entry;
	entry.iPlane = aPlane;
	TInt idx = iMap.FindInOrder(entry, TLinearOrder<TPlaneFCEntry>(TPlaneFCEntry::Cmp));
	if(idx >= 0)
		{
		fc = iMap[idx].iFC;
		}
	else
		{
		LOG(ESockLog::Printf(KESockBootingTag,_L8("RPlaneFCMap::GetPlaneFC(plane %x, kindex %d) no FC found"), aPlane.Role(), aPlane.Kindex()));
		}
	return fc;
	}

void RPlaneFCMap::DropFCOfPeer(const Den::TPlayerRole& aPlane, TWorkerId aPeerWorker)
	{
	for(int i = iMap.Count() - 1; i >= 0; --i)
		{
		const TPlaneFCEntry& entry(iMap[i]);
		if(aPlane.HasRole(entry.iPlane.Role()) && entry.iFC.Thread() == aPeerWorker)
			{
			iMap.Remove(i);
			}
		}
	}

TInt RPlaneFCMap::TPlaneFCEntry::Cmp(const TPlaneFCEntry& aLHS, const TPlaneFCEntry& aRHS)
	{
	TInt diff = aLHS.iPlane.Role() - aRHS.iPlane.Role();
	if(diff == 0)
		{
		diff = aLHS.iPlane.Kindex() - aRHS.iPlane.Kindex();
		}
	return diff;
	}

