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
 @internalComponent - Internal Symbian test code
*/

#if !defined(__TSURFACEMANAGER_H__)
#define __TSURFACEMANAGER_H__

#include <test/tgraphicsharness.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include "tsmgmultprocessshared.h"
#include "../testdriver/d_sharedchunk.h"

class CTSurfaceManager : public CTGraphicsBase
	{
public:
	CTSurfaceManager(CTestStep* aStep);
	~CTSurfaceManager();
protected:
	//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	void TestCreateSurfaceNewChunk1L();
	void TestCreateSurfaceNewChunk2L();
	void TestCreateSurfaceExisitingChunkL();
	void TestCreateSurfaceChunkWithHolesL();
	void TestCreateSurfaceExisitingChunk2L();
	void TestCreateSurfaceExisitingChunk3L();
	void TestCreateSurfaceExisitingChunk4L();
	void TestCreateSurfaceAssociatedWithHintPairsL();
	void TestCreateSurfaceSetReferenceCountL();
	void TestSharedChunkClosedL();
	void TestMultipleChannelsL();
	void TestClosingChannelsL();
	void TestSynchronizeCacheL();
	void TestSharedChunkClosed2L();
	void TestCreateSurfaceInvalidParamsNewChunkL();
	void TestCreateSurfaceInvalidParamsExisitingChunkL();
	void TestCreateSurfaceInvalidParamsExisitingChunk2L();
	void TestInvalidContiguousPropertyL();
	void TestInvalidCachePropertyL();
	void TestInvalidSharedChunkL();
	void TestSynchronizeCacheInvalidParamsL();
	void TestCreateSurfaceAlignmentsMemoryTypeL();
	void TestOpenSurfaceL();
	void TestWriteToTwoBuffersL(); 
	void TestQuerySurfaceInfoWithoutMappingL();
	void TestQuerySurfaceInfoAfterMappingL();
	void TestMapSurfaceL();
	void TestClosingSurfaceL();  
	void TestClosingSurfaceDecrementsReferenceCountL();
	void TestCreateVeryLargeSurfacesL();
	void TestSurfaceInfoChangedTypeL();
	void TestMapSurfaceInvalidParamsL();
	void TestCloseSurfaceInvalidParamsL();
	void TestCloseSurfaceSameIDTwiceL();
	void TestCreateSurfaceOomL();
	void TestGetSurfaceHintInFalseConditionsL();
	void TestSetSurfaceHintInFalseConditionsL();
	void TestAddSurfaceHintInFalseConditionsL();
	void TestGetSurfaceHintL();
	void TestSetSurfaceHintL();
	void TestAddSurfaceHintL();
	void TestDuplicatedHintKeyL();
	void TestSurfaceLimitL();
	void TestPerformanceOfSurfaceHintAndCacheL();
	void TestPerformanceOfSurfaceCreatingAndClosingL();
	void TestZerosInHintsArrayL();
	void TestCreateSurfaceWithTooManyHintsL();
	void TestCreateSurfaceUnMappableL();

    void testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine); 
private:
	void GetSharedChunkHandleL(RSharedChunkLdd& aDevice, RChunk& aHandle, TUint aChunkSize, TUint aChunkAttribs, TUint aContiguousAttrib);
	void SetupAttributesL(RSurfaceManager::TSurfaceCreationAttributes& attributesParams);
	void SetAttributesForHintTestL(RSurfaceManager::TSurfaceCreationAttributes& attributesParamsForHintTest);
	void SetupAttributesForNegativeTestL(RSurfaceManager::TSurfaceCreationAttributes& attributesForNegativeTest);
	void SetAttributesForNegativeHintTestL(RSurfaceManager::TSurfaceCreationAttributes& attributesForNegativeHintTest);
	void ChangeAttributes(RSurfaceManager::TSurfaceCreationAttributes* aAttributes, TInt aAlignment, TInt aOffsetToFirstBuffer, TInt aOffsetBetweenBuffers, RSurfaceManager::TCacheAttribute aCacheAttrib);
	void SurfaceOperation(RSurfaceManager::TSurfaceCreationAttributesBuf& aAttributes);
	void CheckAttributesL(RSurfaceManager::TSurfaceCreationAttributes& aAttributes);
	void CheckSizeL(TInt aOffsetToFirstBuffer, TInt aOffsetBetweenBuffers);
	void CheckHintPair(TSurfaceId& aSurfaceId, RSurfaceManager::THintPair& aHintPairToCheck, RSurfaceManager::THintPair& aHintPairCompareWith);
	void CheckAlignment(TSurfaceId& aId, TInt aAlign, RSurfaceManager::TCacheAttribute aCached);
	void ReAllocHintsArrayL(TUint aNumberOfHints);
	
private:
	RSurfaceManager iSurfaceManager;
	TSurfaceId iSurfaceId;
	RChunk iHandle;
	RSurfaceManager::TInfoBuf iInfoBuf;
	RSurfaceManager::TSurfaceInfoV01 iInfo;
	TInt	iFreq;	
	RSurfaceManager::THintPair *iHintsArray;
	};


class CTSurfaceManagerStep : public CTGraphicsStep
	{
public:
	CTSurfaceManagerStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};	

_LIT(KTSurfaceManagerStep,"TSurfaceManager");

#endif
