// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_TESTSURFACERASTERIZER_H__
#define __T_TESTSURFACERASTERIZER_H__

#include <e32base.h>
#include <e32std.h>
#include <graphics/surface.h>		//TSurfaceId
#include <graphics/surfacemanager.h>	//RSurfaceManager
#include <graphics/surfaceupdateclient.h>

#include "t_pseudoapptestresults.h"

typedef struct SurfaceDetails
	{
	TSurfaceId aSurfaceId;
	TSize aSurfaceSize;
	TInt aBufferNumber;
	}SurfaceDetails;

class CTestSurfaceRasterizer : public CBase
	{
public:
	IMPORT_C static CTestSurfaceRasterizer* NewL(const TSurfaceId& aSurfaceId, TInt aSurfaceScreenNumber, RSurfaceManager* aSurfaceManager);
	IMPORT_C void DrawRect(TUint32* aBuffer, SurfaceDetails& aSurfDetails);
 	IMPORT_C void PerfTestL(TInt aFrameNumber);
	~CTestSurfaceRasterizer();

protected:

private:
	CTestSurfaceRasterizer();
	TBool ConstructL(const TSurfaceId& aSurfaceId, TInt aSurfaceScreenNumber, RSurfaceManager* aSurfaceManager);
	void SendUpdateRequest(const TPoint aPoint, const TSize aSurfaceSize);

private:
	TSurfaceId iSurfaceId;
	TInt iSurfaceScreenNumber;
	TInt iSurfaceCurrentBuffer;
	RChunk iChunk;
	RSurfaceManager* iSurfaceManager;
	RSurfaceManager::TInfoBuf iInfoBuf;
	};

#endif	//__T_TESTSURFACERASTERIZER_H__
