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

#ifndef __T_PSEUDOAPPSHARED_H__
#define __T_PSEUDOAPPSHARED_H__

#include "t_pseudoapptestresults.h"
#include <e32base.h>
#include <graphics/surface.h>		//TSurfaceId
#include <gdi.h>			//Display modes

class CTestScreen;
class CTestSurfacedWindow;

class CTPseudoAppShared : public CBase
	{
private:
	class TSurfaceCollection
		{
	public:
		TSurfaceCollection();
		TSurfaceCollection(TInt aScreenNo, TInt aWindowNo, const TSurfaceId& aSurfaceId);
		TSurfaceCollection(TInt aScreenNo, TInt aWindowNo);

		// TIdentityRelation comparison functions
		static TBool IndicesEqual(const TSurfaceCollection& aFirst, const TSurfaceCollection& aSecond);
		static TBool AnotherWindowWithSameSurfaceId(const TSurfaceCollection& aFirst, const TSurfaceCollection& aSecond);
		
	public:
		TInt iScreenNo;
		TInt iWindowNo;
		TSurfaceId iSurfaceId;
		};

public:
	IMPORT_C CTPseudoAppShared();
	IMPORT_C void AddTestScreenL(TInt aScreenNo, TDisplayMode aMode, TInt aFrameDuration, const TSize& aScreenSize,
								 TGceTestResults* aGceTestResults, const TDesC& aConfigFileName);

	~CTPseudoAppShared();
	TSurfaceId GetSurfaceId(TInt aScreenNo, TInt aWindowNo);
	void SetSurfaceIdL(TInt aScreenNo, TInt aWindowNo, const TSurfaceId& aSurfaceId);
	void RemoveSurfaceIdEntry(TInt aScreenNo, TInt aWindowNo);
	TBool RotationSupported(TInt aScreenNo, TInt aWindowNo, const TSurfaceId& aSurfaceId) const;

public:
	RPointerArray<CTestScreen> iTestScreens;

private:
	RArray<TSurfaceCollection> iSurfaceCollection;
	};

#endif //__T_PSEUDOAPPSHARED_H__
