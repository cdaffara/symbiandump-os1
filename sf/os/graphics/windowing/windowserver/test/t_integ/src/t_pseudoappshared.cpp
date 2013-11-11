// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#include <e32debug.h> //RDebug

#include "t_pseudoappshared.h"
#include "t_pseudoappscreen.h"
#include "t_pseudoappwindow.h"

CTPseudoAppShared::TSurfaceCollection::TSurfaceCollection()
:iScreenNo(-1),iWindowNo(-1),iSurfaceId(TSurfaceId::CreateNullId())
	{
	}

CTPseudoAppShared::TSurfaceCollection::TSurfaceCollection(TInt aScreenNo, TInt aWindowNo, const TSurfaceId& aSurfaceId)
:iScreenNo(aScreenNo),iWindowNo(aWindowNo),iSurfaceId(aSurfaceId)
	{
	}

CTPseudoAppShared::TSurfaceCollection::TSurfaceCollection(TInt aScreenNo, TInt aWindowNo)
:iScreenNo(aScreenNo),iWindowNo(aWindowNo),iSurfaceId(TSurfaceId::CreateNullId())
	{
	}

TBool CTPseudoAppShared::TSurfaceCollection::IndicesEqual(const TSurfaceCollection& aFirst, const TSurfaceCollection& aSecond)
	{
	return (aFirst.iScreenNo==aSecond.iScreenNo && aFirst.iWindowNo==aSecond.iWindowNo);
	}

TBool CTPseudoAppShared::TSurfaceCollection::AnotherWindowWithSameSurfaceId(const TSurfaceCollection& aFirst, const TSurfaceCollection& aSecond)
	{
	return (aFirst.iSurfaceId==aSecond.iSurfaceId && (aFirst.iScreenNo!=aSecond.iScreenNo || aFirst.iWindowNo!=aSecond.iWindowNo));
	}

EXPORT_C CTPseudoAppShared::CTPseudoAppShared()
	{
	}

EXPORT_C void CTPseudoAppShared::AddTestScreenL(TInt aScreenNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize,
												TGceTestResults* aGceTestResults, const TDesC& aConfigFileName)
	{
	CTestScreen* newScreen = CTestScreen::NewL(aScreenNo, aMode, aFrameDuration, aScreenSize, aGceTestResults, aConfigFileName, *this);
	CleanupStack::PushL(newScreen);
	iTestScreens.AppendL(newScreen);
	CleanupStack::Pop(newScreen);
	}

CTPseudoAppShared::~CTPseudoAppShared()
	{
	iTestScreens.ResetAndDestroy();
	iTestScreens.Close();
	iSurfaceCollection.Close();
	}

TSurfaceId CTPseudoAppShared::GetSurfaceId(TInt aScreenNo, TInt aWindowNo)
	{
	TSurfaceCollection matchCollection(aScreenNo, aWindowNo);
	
	TInt pos = iSurfaceCollection.Find(matchCollection, TIdentityRelation<TSurfaceCollection>(TSurfaceCollection::IndicesEqual));
	if(pos == KErrNotFound)
		{
		return TSurfaceId::CreateNullId();
		}
	else
		{
		return iSurfaceCollection[pos].iSurfaceId;
		}
	}

void CTPseudoAppShared::SetSurfaceIdL(TInt aScreenNo, TInt aWindowNo, const TSurfaceId& aSurfaceId)
	{
	TSurfaceCollection newCollection(aScreenNo, aWindowNo, aSurfaceId);
	
	TInt pos = iSurfaceCollection.Find(newCollection, TIdentityRelation<TSurfaceCollection>(TSurfaceCollection::IndicesEqual));
	if(pos >= 0)
		{
		// Replace SurfaceId for window as already in array
		iSurfaceCollection[pos].iSurfaceId = aSurfaceId;
		}
	else if(pos == KErrNotFound)
		{
		// Add new entry to array
		iSurfaceCollection.AppendL(newCollection);
		}
	else
		{
		User::Leave(pos);
		}
	}

void CTPseudoAppShared::RemoveSurfaceIdEntry(TInt aScreenNo, TInt aWindowNo)
	{
	TSurfaceCollection matchCollection(aScreenNo, aWindowNo);
	
	TInt pos = 0;
	while(pos >= 0)
		{
		pos = iSurfaceCollection.Find(matchCollection, TIdentityRelation<TSurfaceCollection>(TSurfaceCollection::IndicesEqual));
		if(pos == KErrNotFound)
			{
			return;
			}
		else
			{
			iSurfaceCollection.Remove(pos);
			}
		}
	}

TBool CTPseudoAppShared::RotationSupported(TInt aScreenNo, TInt aWindowNo, const TSurfaceId& aSurfaceId) const
	{
	TSurfaceCollection matchCollection(aScreenNo, aWindowNo, aSurfaceId);
	
	TInt pos = iSurfaceCollection.Find(matchCollection, TIdentityRelation<TSurfaceCollection>(TSurfaceCollection::AnotherWindowWithSameSurfaceId));
	
	return (pos == KErrNotFound);
	}
