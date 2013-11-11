// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Surface manager test code
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "tsurfacemanager.h"
#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>
#include <e32std.h>
#include <e32cmn.h>	  
#include <hal.h>
#include <e32def_private.h>
#include <graphics/surface_hints.h>

using namespace surfaceHints;

const TInt KCountLimit = 10000;

CTSurfaceManager::CTSurfaceManager(CTestStep* aStep):
	CTGraphicsBase(aStep)
	{
	}

void CTSurfaceManager::ReAllocHintsArrayL(TUint aNumberOfHints)
	{
	if (iHintsArray)
		{
		User::Free(iHintsArray);
		iHintsArray=NULL;
		}
	iHintsArray=static_cast<RSurfaceManager::THintPair*>(
			User::AllocL(aNumberOfHints * sizeof(RSurfaceManager::THintPair)));
	}

CTSurfaceManager::~CTSurfaceManager()
	{
	if (iHintsArray)
		{
		User::Free(iHintsArray);
		}
	}

void CTSurfaceManager::RunTestCaseL(TInt aCurTestCase)
	{
	TInt procHandles1  =0;
	TInt threadHandles1=0;
	RThread().HandleCount(procHandles1, threadHandles1);
	((CTSurfaceManagerStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);

	switch(aCurTestCase)
		{
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0001
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		For each alignment conditions, create a Surface in a new shared chunk.
 						All the attributes are correct, no rounding is required. 						
@SYMTestStatus			Implemented
@SYMTestActions			For each alignment conditions, call CreateSurface() then SurfaceInfo().  
@SYMTestExpectedResults If the return value is KErrNone , it has suceeded  
*/
	case 1:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0001"));
		TestCreateSurfaceNewChunk1L();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0002-0001
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		For each alignment conditions, create a Surface in a new shared chunk.
 						Rounding is required for iOffsetToFirstBuffer and iOffsetBetweenBuffers. 						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), SurfaceInfo()
@SYMTestExpectedResults If the return value is KErrNone , it has suceeded  
*/
	case 2:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0002-0001"));
		TestCreateSurfaceNewChunk2L();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0002-0002
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test creates a surface in an exisiting shared chunk
						Find a valid shared chunk handle returned by a device driver. 
						Create a surface in this existing shared chunk
						Map the surface in the current process to get a new shared chunk handle						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface(), SurfaceInfo(). 
@SYMTestExpectedResults If the return value is KErrNone , it has suceeded  
*/
	case 3:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0002-0002"));
		TestCreateSurfaceExisitingChunkL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0003
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test creates a surface with distinct creation attributes in an 
						exisiting shared chunk that already contains a surface.
						Create a surface in a new shared chunk with valid creation attributes.
						Map the surface in the current process to get the first chunk handle.
						Create another surface (different attributes) in this existing chunk.
						Check that two surfaceIDs are different.
						Map the second surface in the current process to get the second chunk handle.
						Check that two chunk handles are different but refer to the same shared chunk.
						Write a number in the first buffer of the Second surface and read it from the first Surface (the surfaces share the chunk). 					
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface() and SurfaceInfo().
@SYMTestExpectedResults If the return value is KErrNone , it has suceeded  
*/
	case 4:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0003"));
		TestCreateSurfaceExisitingChunk2L();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0004
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test creates a surface with identical creation attributes in an 
						exisiting shared chunk that already contains a surface.					
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface() and SurfaceInfo().
@SYMTestExpectedResults If the return value is KErrNone , it has suceeded  
*/
	case 5:
        ((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0004"));
		TestCreateSurfaceExisitingChunk3L();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0005
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test creates a surface with rotated orientations in an exisiting 
						shared chunk that already contains a surface at the same address.						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface() and SurfaceInfo().
@SYMTestExpectedResults If the return value is KErrNone , it has suceeded  
*/
	case 6:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0005"));
		TestCreateSurfaceExisitingChunk4L();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0006
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test creates surfaces with Hint pairs (between 0 and the maximum allowed)
@SYMTestStatus			Implemented
@SYMTestActions			Call GetSurfaceManagerAttrib(), CreateSurface(), SurfaceInfo(). 
@SYMTestExpectedResults One surface can hold maximum of 8 hint pairs
*/
	case 7:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0006"));
		TestCreateSurfaceAssociatedWithHintPairsL(); 
		break;
		
		/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0007
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test creates surfaces with all type of alignment in contiguous 
						or fragmented memory.						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(). 
@SYMTestExpectedResults If the return value is KErrNone , it has suceeded
*/
	case 8:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0007"));
		TestCreateSurfaceAlignmentsMemoryTypeL(); 
		break;
		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0008
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test creates a surface and opens the surface
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface() and OpenSurface() 
@SYMTestExpectedResults If the return value is KErrNone , it has suceeded
*/
	case 9:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0008"));
		TestOpenSurfaceL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0009
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This tests CloseSurface (correctly) decrements reference count
						Create a surface (set the reference count to 1)
						Get the SurfaceInfo for the Surface. (don't alter the ref count)
						Close the Surface (ref count =0)
					    Open the Surface (Since the ref count is 0, OpenSurface should fail)					    
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), CloseSurface(), OpenSurface() 
@SYMTestExpectedResults CloseSurface returns KErrNone, OpenSurface returns KErrArgument
*/
	case 10:
        ((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0009"));
		TestClosingSurfaceDecrementsReferenceCountL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0010
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test that the new CreateSurface()API create the surface and set its reference count to one
						Create a surface in a new shared chunk with valid creation attributes. 
						Map the surface in the current process to get the chunk handle.
						Create a new surface in the existing shared chunk with valid creation attributes (ref count is set to 1).
						Close the second surface (decrement the ref count to 0, the surface is deleted)
						Close the second surface again
						Check the return value is KErrArgument as the second surface is already deleted.						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface(), CloseSurface(), OpenSurface() 
@SYMTestExpectedResults The new surface can be created successfully on the existing shared chunk, and the reference count is set to one. 
*/
	case 11:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0010"));
		TestCreateSurfaceSetReferenceCountL();
		break;
		/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0011
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		Check reading from buffers
						Set the ‘iBuffers’ of TSurfaceCreationAttributes to two.
						Create the Surface in the new shared chunk with valid creation attributes
						Map the Surface in the current process to get the shared chunk handle
						Get the address of the pixel data and write a number to that.
						Check that you can read what was written previously.						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface() and MapSurface()  
@SYMTestExpectedResults Upon success, the buffer can be written with valid values
*/
	case 12:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0011"));
		TestWriteToTwoBuffersL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0012
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test gets SurfaceInfo of a Surface without mapping it
						Store the attributes used to create the Surface.
						Create a surface in a new shared chunk with those valid attributes
						Call SurfaceInfo to get the attributes of the new Surface.
						Check if the values are equal to the stored ones. 						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), SurfaceInfo() 
@SYMTestExpectedResults Upon success,the return values are equal to the stored values
*/
	case 13:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0012"));
		TestQuerySurfaceInfoWithoutMappingL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0013
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This test queries SurfaceInfo after mapping it in a memory space
						Store the attributes used to create the Surface.
						Create a surface 
						Map the surface
						Call SurfaceInfo to get the attributes of the Surface
						Check if the values are equal to the stored ones.						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface(), SurfaceInfo() 
@SYMTestExpectedResults Upon success,the values are equal to the stored values.
*/
	case 14:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0013"));
		TestQuerySurfaceInfoAfterMappingL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0014
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This tests the MapSurface method
						Create a surface in a new shared chunk with valid attributes
						Map the surface in the current process to get the chunk handle
						Check that it returns KerrNone						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface()
@SYMTestExpectedResults Upon success,the return value is KErrNone
*/
	case 15:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0014"));
		TestMapSurfaceL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0015
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc		This tests the CloseSurface method
						Create a surface in a new shared chunk with valid attributes
						Close the Surface 
						Check that this returns KErrNone
						Call OpenSurface using the SurfaceID and check it returns KErrArgument as the surface is already deleted.						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), CloseSurface(), OpenSurface()
@SYMTestExpectedResults After closing the surface, Open the same surface again will cause a failure and return value is not KErrNone
*/
	case 16:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0015"));
		TestClosingSurfaceL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0016
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test checks the shared Chunk gets deleted after the surface and handle are both closed
						Create a surface in a new shared chunk with valid attributes
						Map the Surface in the current process to get the chunk handle
						Close the Surface 
						Check that KErrNone is returned
						Call RChunk::Base() to check the chunk is not yet deleted, as the handle is still open
						Close the chunk handle						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), CloseSurface(), MapSurface() 
@SYMTestExpectedResults CloseSurface returns KErrNone, OpenSurface returns KErrArgument
*/
	case 17:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0016"));
		TestSharedChunkClosedL();
		break;

	case 18:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		// Obsolete since cr1649 (it was testing the capabilities of the Surface Manager driver)
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0018
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test creates multiple channels to surface manager LDD in one process and accesses them individually
						Open one channel called SurfaceManager1 to the surface manager LDD 
						Create SurfaceID using SurfaceManager1 (ref count is set to 1)
						Then Open another channel called SurfaceManager2 to LDD in the same process
						Open SurfaceID using SurfaceManager2 (ref count is incremented to 2)
						Close SurfaceManager1 
						Call SurfaceInfo on SurfaceID and check it returns KErrNone, as SurfaceID is still open
						Call CloseSurface on SurfaceID twice and then call SurfaceInfo to check it returns KErrArgument, as the surface is deleted after being closed twice.
						Creating another surface using Surfacemanager2 and check that returns KErrNone too as SurfaceManager2 is still open. 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), OpenSurface(), SurfaceInfo(),CloseSurface()
@SYMTestExpectedResults If multiple connections are created to the driver, closing one surface manager channel will not affect other channels and the surfaces opened in the process. 
*/
	case 19:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0018"));
		TestMultipleChannelsL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0019
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test checks the behaviour of closing driver channels
						Open a channel called as SurfaceManager1 to the LDD by calling RSurfaceManager::Open()
						Create one surface called as SurfaceID1 using SurfaceManager1
						Then open another channel called as SurfaceManager2 to the LDD
						Create another surface called as SurfaceID2 using SurfaceManager2
						Open SurfaceID1 using both SurfaceManager1 and SurfaceManager2
						Open SurfaceID2 using both SurfaceManager1 and SurfaceManager2
						Close SurfaceManager1 and SurfaceManager2
						Reopen a channel to the surface manger LDD 
						Call SurfaceInfo() to access both SurfaceID1 and SurfaceID2 and check both return KErrArgument, as both are deleted.
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), OpenSurface(), SurfaceInfo(),CloseSurface()
@SYMTestExpectedResults If multiple connections are created to the driver, closing one surface manager channel will not affect other channels and the surfaces opened in the process. 
*/
	case 20:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0019"));
		TestClosingChannelsL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0020
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Finding out the maximum number of surfaces to be created
						Create a Surface till error occurs
						Check the maximum number of surfaces created and the last error code
@SYMTestActions			Call CreateSurface()
@SYMTestExpectedResults 	The maximum number varies between platforms(emulator and h4). The last error code is KErrNoMemory.
*/
	case 21:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0020"));
		TestSurfaceLimitL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0021
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test the behaviour of SynchronizeCache()
						Create the surface in a new shared chunk with valid creation attributes. The cache attribute is set to ECached. 
						Map the surface in the current process to get the chunk handle
						Synchronise the cache before the hardware writes to the buffer. Pass TSyncOperation::ESyncBeforeHandwareWrite, valid buffer number and valid surfaceID in SynchronizeCache()
						Synchronise again the cache after the hardware write the buffer. Pass TSyncOperation::ESyncAfterHandwareWrite, valid buffer number and valid surfaceID in SynchronizeCache()
						Synchronise again the cache before the hardware reads the buffer. Pass TSyncOperation::ESyncBeforeHardwareRead, valid buffer number and valid surfaceID in SynchronizeCache()
						Check each calls return KErrNone. 
						We can also visually check the result of SynchronizeCache() by investigating the MBX GCE tests.
@SYMTestActions			Call CreateSurface(), SynchronizeCache()
@SYMTestExpectedResults 	The surface memory is synchronised properly with cached contents. All the synchronize operation returns KErrNone.
*/
	case 22:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0021"));
		TestSynchronizeCacheL();
		break;
		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0022
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test that when two surfaces are created in the same shared chunk,
						closing one surface and chunk handle will not cause the chunk to be deleted.  
@SYMTestActions			Call CreateSurface(), MapSurface(), CloseSurface()
@SYMTestExpectedResults 	If two surfaces are created on the same shared chunk. closing one surface and chunk handle will not cause the chunk to be deleted.
*/
	case 23:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0022"));
		TestSharedChunkClosed2L();
		break;
		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0023
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test that GetSurfaceHint() behaves properly.
@SYMTestActions			Call CreateSurface(), GetSurfaceHint()
@SYMTestExpectedResults 	GetSurfaceHint () returns KErrNone and retrieve THintPair value correctly
*/
	case 24:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0023"));
		TestGetSurfaceHintL();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0024
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test that SetSurfaceHint() behaves properly
@SYMTestActions			Call CreateSurface(), SetSurfaceHint()
@SYMTestExpectedResults 	SetSurfaceHint () can reset value in THintPair if iMutable is ETrue.
*/
	case 25:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0024"));
		TestSetSurfaceHintL();
		break;
		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0025
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test that AddSurfaceHint() behaves properly
@SYMTestActions			Call CreateSurface(), AddSurfaceHint()
@SYMTestExpectedResults 	AddSurfaceHint () can add a new surface hint value for the surface.
*/
	case 26:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0025"));
		TestAddSurfaceHintL();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0026
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test creates a surface with invalid parameters in a new shared chunk
						Set invalid values for the data members of TSurfaceCreationAttributes, which is used to create a Surface.
						Call CreateSurface
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface()
@SYMTestExpectedResults Check that CreateSurface() returns KErrArgument
*/
	case 27: 
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0026"));
		TestCreateSurfaceInvalidParamsNewChunkL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0027
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test creates a surface with invalid parameters in an exisiting shared chunk
						Set invalid values for the data members of TSurfaceCreationAttributes, which is used to create a Surface.
						Call CreateSurface and pass it the shared chunk handle.
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface()
@SYMTestExpectedResults Check that CreateSurface() returns KErrArgument
*/
	case 28: 
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0027"));
		TestCreateSurfaceInvalidParamsExisitingChunkL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0028
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test creates a surface with invalid parameters in an exisiting shared chunk which already contains a surface
						Create a surface in a new shared chunk
						Map the surface in the current process to get the shared chunk handle
						Set invalid values for the data members of TSurfaceCreationAttributes, which is used to create a new Surface in the exising shared chunk.
						Call CreateSurface and pass it the shared chunk handle.
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface()
@SYMTestExpectedResults Check that CreateSurface() returns KErrArgument
*/
	case 29: 
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0028"));
		TestCreateSurfaceInvalidParamsExisitingChunk2L();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0029
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test that Hint keys can not be duplicated						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface()
@SYMTestExpectedResults Check that No duplicated Hint keys are allowed in the TSurfaceHints field
*/
	case 30:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0029"));
		TestDuplicatedHintKeyL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0030
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test checks that iContiguous is indeed ignored when creating the second surface on the existing shared chunk which already contains a surface						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface(), CreateSurface(), SurfaceInfo()
@SYMTestExpectedResults Contiguous attribution in the surface info structure correctly reflects the Contiguous properties of the chunk used.
*/
	case 31: 
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0030"));
		TestInvalidContiguousPropertyL();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0031
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test checks that iCacheAttrib is indeed ignored when creating the second surface on the existing shared chunk which already contains a surface
						
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface, CreateSurface(), SurfaceInfo()
@SYMTestExpectedResults Cache attribution in the surface info structure correctly reflects the caching properties of the chunk used.
*/
	case 32: 
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0031"));
		TestInvalidCachePropertyL();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0032
@SYMPREQ				PREQ 1879,PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test creates a Surface in an invalid chunk
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), Mapsurface, CloseSurface()
@SYMTestExpectedResults CreateSurface should return KErrBadHandle when the chunk handle is invalid
*/
	case 33: 
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0032"));
		TestInvalidSharedChunkL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0033
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This test create enough surfaces to cause KErrNoMemory
						Set surface parameters to create a reasonably large (memory) surface
						Create multiple surfaces using these parameters
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface()
@SYMTestExpectedResults Check that eventually that KErrNoMemory will be returned.
*/
	case 34:
        ((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0033"));
		TestCreateVeryLargeSurfacesL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0034
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This tests the MapSurface method with invalid surfaceId
						Create a Surface 
						Change Surface ID by
							1. making the Surface ID negative
							2. converting the type of Surface ID to EInvalidSurface
						MapSurface with new Surface ID
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), MapSurface()
@SYMTestExpectedResults Check that the return value of MapSurface() is KErrArgument
*/
	case 35:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0034"));
		TestMapSurfaceInvalidParamsL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0035
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This tests SurfaceInfo by passing invalid SurfaceID
						Map the surface
						Change the type of the SurfaceID 
						Call SurfaceInfo, pass the changed SurfaceID as a parameter
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), SurfaceInfo()
@SYMTestExpectedResults Check that the return value of SurfaceInfo() is KErrArgument.
*/
	case 36:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0035"));
		TestSurfaceInfoChangedTypeL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0036
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This tests the Closing surface method with invalid parameters
						Create a Surface 
						Change Surface Id by
							1. making the Surface ID negative
							2. converting the type of the Surface ID to EInvalidSurface
						Call CloseSurface with the new Surface ID
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), CloseSurface()
@SYMTestExpectedResults Check that CloseSurface() returns KErrArgument
*/
	case 37:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0036"));
		TestCloseSurfaceInvalidParamsL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0037
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test  that SynchronizeCache() reacts properly under false conditions
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), SynchronizeCache(), CloseSurface()
@SYMTestExpectedResults SynchronizeCache() returns KErrArgument under false conditions
*/
	case 38:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0037"));
		TestSynchronizeCacheInvalidParamsL();
		break;		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0038
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	This tests closing a surface with the same ID twice
						Create a Surface
						Close the surface
						Close the surface again
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), CloseSurface()
@SYMTestExpectedResults Check that running CloseSurface() twice returns KErrArgument.
*/
	case 39:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0038"));
		TestCloseSurfaceSameIDTwiceL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0039
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Testing CreateSurface in out of memory conditions (for the debug mode only)
						Open the surface manager
						Call CreateSurface(), inducing a kernal alloc failure on each possible kernal alloc
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface()
@SYMTestExpectedResults 	Check that CreateSurface() returns KErrNoMemory.
*/
	case 40:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
	#ifdef _DEBUG
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0039"));
		TestCreateSurfaceOomL();
	#endif
    	break; 	
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0040
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Testing that GetSurfaceHint()reacts properly under false conditions
						Create the surface with valid creation attributes in a new shared chunk.
						Find THintPair, Close the surface
						Set false conditions and then call GetSurfaceHint()
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), GetSurfaceHint()
@SYMTestExpectedResults 	Check that GetSurfaceHint()returns KErrArgument under false conditions.
*/ 
	case 41:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0040"));
		TestGetSurfaceHintInFalseConditionsL();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0041
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Testing that SetSurfaceHint()reacts properly under false conditions
						Create the surface with valid creation attributes in a new shared chunk.
						Set THintPair, Close the surface
						Set false conditions and then call SetSurfaceHint()
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), SetSurfaceHint()
@SYMTestExpectedResults 	Check that SetSurfaceHint()returns KErrArgument under false conditions.
*/ 
	case 42:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0041"));
		TestSetSurfaceHintInFalseConditionsL();
		break;
		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0042
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Testing that AddSurfaceHint()reacts properly under false conditions
						Create the surface with valid creation attributes in a new shared chunk.
						Set THintPair, Close the surface
						Set false conditions and then call AddSurfaceHint()
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), AddSurfaceHint()
@SYMTestExpectedResults 	AddSurfaceHint () returns KErrArgument if the surface ID is invalid or not open in this process,
 							KErrAlreadyExists if duplicate hint key used, KErrOverflow if no space to add new pair.
*/ 
	case 43:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0042"));
		TestAddSurfaceHintInFalseConditionsL();
		break;
		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0043
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test  AddSurfaceHint, SetSurfaceHint and SynchronizeCache performance
						Calculate the average time:
							- for setting surface hints at different location
							- for adding hints at different location
							- for synchronizing cache 

@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface(), AddSurfaceHint()and SetSurfaceHint
@SYMTestExpectedResults 	Performance is reasonable
*/		
	case 44:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0043"));
		TestPerformanceOfSurfaceHintAndCacheL();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0044
@SYMPREQ				PREQ1007
@SYMREQ					REQ8222,REQ8223
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test  CreateSurface() and CloseSurface() performance
						Calculate the average time:
							- for creating a surface in a new chunk
							- for closing a surface
							- for creating a surface in an existing chunk
							
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface() and CloseSurface()
@SYMTestExpectedResults 	Performance is reasonable
*/				
	case 45:
        ((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0044"));
		TestPerformanceOfSurfaceCreatingAndClosingL();
		break;

/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0045
*/
	case 46:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0045"));
		TestZerosInHintsArrayL();
		break;
		
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0082
@SYMPREQ				DEF130006
@SYMREQ					
@SYMTestPriority		High 
@SYMTestCaseDesc	 	Test that Creating Surface with chunk that has "holes" works. 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface() with different sets of pages in the chunk. 
@SYMTestExpectedResults 	Test should "pass" - this test has both positive and negative subtests. 
*/		
	case 47:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0082"));
		TestCreateSurfaceChunkWithHolesL();
		break;
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0101
@SYMPREQ				cr1649
@SYMREQ					
@SYMTestPriority		Low 
@SYMTestCaseDesc	 	Test that Creating Surface with too many hints imply an error. 
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface() with too many hints. 
@SYMTestExpectedResults 	Negative test for CreateSurface. 
*/		
	case 48:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0101"));
		TestCreateSurfaceWithTooManyHintsL();
		break;		
	
/**
@SYMTestCaseID			GRAPHICS-SURFACEMANAGER-0102
@SYMPREQ				cr1649
@SYMREQ					
@SYMTestPriority		Low 
@SYMTestCaseDesc	 	Test that Creating Surface that is not mappable can then not be mapped.  
@SYMTestStatus			Implemented
@SYMTestActions			Call CreateSurface() with attribute iMappable = EFalse, 
                        then call MapSurface() and check that it is not successful. 
                        same with GetBufferOffset
@SYMTestExpectedResults 	Negative test for MapSurface and GetBufferOffset. 
*/
	case 49:
		((CTSurfaceManagerStep*)iStep)->SetTestStepID(_L("GRAPHICS-SURFACEMANAGER-0102"));
		TestCreateSurfaceUnMappableL();
		break;
		
	default:		
        ((CTSurfaceManagerStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTSurfaceManagerStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTSurfaceManagerStep*)iStep)->RecordTestResultL();
	TInt procHandles2  =0;
	TInt threadHandles2=0;
	RThread().HandleCount(procHandles2,threadHandles2);
	if (threadHandles1 != threadHandles2)
		{
		User::Leave(KErrGeneral);  // Thread-owned handles not closed
		}

	}

void CTSurfaceManager::TestPerformanceOfSurfaceCreatingAndClosingL()
	{
	INFO_PRINTF1(_L("Testing Surface Manager Performance\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
    	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(100,100);
	attributes.iBuffers = 1;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;	
	attributes.iStride = 400;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 9;	// way of reserving space before the surface pixel data
	attributes.iAlignment = 1;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned	
	RSurfaceManager::THintPair hints[2];
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	attributes.iSurfaceHints = hints;
	attributes.iOffsetBetweenBuffers = 50009;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;	 // Cache attributes
	attributes.iMappable = ETrue;

	// Create a surface
	TSurfaceId surfaceId;
	TUint32 timeBefore = 0;
	TUint32 timeAfter = 0;
	TReal measure = 0;
	TReal measureTotal = 0;
	TInt index = 0;

	RArray<TSurfaceId> surfaceIDArray;

	User::LeaveIfError(HAL::Get(HALData::EFastCounterFrequency, iFreq));
	// Scenario 1: measure the average time of creating
	timeBefore = User::FastCounter();
	index = 0;
	TInt r = KErrNone;
	
	do
		{
		TEST(KErrNone == r);
		timeBefore = User::FastCounter();
		r = iSurfaceManager.CreateSurface(buf, surfaceId);
		timeAfter = User::FastCounter();
		measure= 1000000 * ((TReal)(timeAfter - timeBefore)) / ((TReal) iFreq);
//		RDebug::Print(_L("Creating a surface [LOG]: \t%d \t%f "), index, measure);
		measureTotal += measure;				
		surfaceIDArray.Insert(surfaceId,index++);
		} while (r == KErrNone && index < 500);
			
	TInt numberOfSurfaces = surfaceIDArray.Count();
	INFO_PRINTF2(_L("the number of surface is %d\r\n"),numberOfSurfaces);
	INFO_PRINTF2(_L("the average time for creating a surface is %f\r\n"),measureTotal/numberOfSurfaces);
	index = 0;	
	measureTotal = 0;
	while (index < numberOfSurfaces -1)
		{
		surfaceId = surfaceIDArray[index++];
		
		// to delete the first surface being created
		timeBefore = User::FastCounter();
		TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
		timeAfter = User::FastCounter();
			// 1000 for all the opeations, 1000000 for deletion only 
		measure = 1000000 * ((TReal)(timeAfter - timeBefore)) / ((TReal) iFreq);
		measureTotal += measure;
//		RDebug::Print(_L("Closing the surface [LOG]: \t%d \t%f "), index, measure);
		}

	INFO_PRINTF2(_L("the average time for Closing a surface is %f\r\n"),measureTotal/numberOfSurfaces);

	surfaceIDArray.Close();
	RArray<TSurfaceId> surfaceIDArray2;

	// For CreateSurface in the existing chunk
	TEST(KErrNone ==iSurfaceManager.CreateSurface(buf, surfaceId)) ;
	//Map the surface in the current processs
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
  	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

	attributesNew.iSize = TSize(480,16);
	attributesNew.iBuffers = 2;				// number of buffers in the surface
	attributesNew.iPixelFormat = EUidPixelFormatYUV_422Reversed;		
	attributesNew.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributesNew.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data

	RSurfaceManager::THintPair hint;
	attributesNew.iHintCount=1;
	hint.Set(TUid::Uid(0x124545), 50, EFalse);
	attributesNew.iSurfaceHints = &hint;
	attributesNew.iAlignment = RSurfaceManager::EPageAligned;
	attributesNew.iOffsetBetweenBuffers = 0;
	attributesNew.iMappable = ETrue;
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceIdNew;

	r = KErrNone;
	index = 0;
	measureTotal = 0;
	do
		{
		TEST(KErrNone == r);
		timeBefore = User::FastCounter();
		r = iSurfaceManager.CreateSurface(buff, surfaceIdNew, handle);
		timeAfter = User::FastCounter();
		measure= 1000000 * ((TReal)(timeAfter - timeBefore)) / ((TReal) iFreq);
//		RDebug::Print(_L("CreateSurface in the existing chunk [LOG]: \t%d \t%f "), index, measure);
		measureTotal += measure;				
		surfaceIDArray2.Insert(surfaceIdNew,index++);
		} while (r == KErrNone && index < 500);
	numberOfSurfaces = surfaceIDArray2.Count();

	INFO_PRINTF2(_L("the average time for Creating a surface in the existing chunk is %f\r\n"),measureTotal/numberOfSurfaces);

	index = 0;	
	while (index < numberOfSurfaces)
		{
		surfaceIdNew = surfaceIDArray2[index++];
		TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew));
		}

	handle.Close();	
	
	surfaceIDArray2.Close();
	iSurfaceManager.Close();
 	}

void CTSurfaceManager::TestPerformanceOfSurfaceHintAndCacheL()
	{
	INFO_PRINTF1(_L("Testing Surface Manager AddSurfaceHint, SetSurfaceHint, GetSurfaceHint and SynchronizeChace Performance\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	TInt maxHintsPerSurface;
	iSurfaceManager.GetSurfaceManagerAttrib(RSurfaceManager::EMaxNumberOfHints,maxHintsPerSurface);
	// For SetSurfaceHint extreme search(binary search takes longest to locate the search if it is at the end)
	//Small surfaces
	RArray<TInt32> uidArray;
	TInt i;
	for (TInt i=0;i<maxHintsPerSurface-1;++i)
		{
		uidArray.Append(0x237750+i);
		}
	uidArray.Append(0x237778);
	
	attributes.iSize = TSize(1,1);
	attributes.iBuffers = 1;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 1;
	attributes.iOffsetToFirstBuffer = 1;
	attributes.iAlignment = RSurfaceManager::EPageAligned;
	//Add 16 surface hints 
	ReAllocHintsArrayL(maxHintsPerSurface);
	attributes.iHintCount=maxHintsPerSurface;
	for (i = 0; i<attributes.iHintCount; ++i)
		{
		iHintsArray[i].Set(TUid::Uid(uidArray[i]), 25, ETrue);
		}
	attributes.iSurfaceHints=iHintsArray;
	
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;	 // Cache attributes
	attributes.iMappable = ETrue;
	
	// Create a surface
	TSurfaceId surfaceId;
	TInt	freq;
	User::LeaveIfError(HAL::Get(HALData::EFastCounterFrequency, freq));
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));	
	TUint32 timeBefore =0;
	TUint32 timeAfter= 0;

	TReal measure1=0;
	TReal measureTotal = 0;
	const TInt numberOfCalculation = 500;

	RSurfaceManager::THintPair hintPair;
	RSurfaceManager::THintPair hintPair2;
	// Measure the average time of setting surface hints at different location
	for (i = 0; i<maxHintsPerSurface; ++i)
		{
		measure1 = 0;
		for (TInt j = 0; j<numberOfCalculation; j++)
			{
			hintPair.iKey.iUid = uidArray[i]; 
			hintPair.iValue = 300;
			hintPair.iMutable = ETrue;
			timeBefore = User::FastCounter();
			TEST(KErrNone == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair));
			timeAfter = User::FastCounter();
			measure1 += 1000 * ((TReal)(timeAfter-timeBefore)) / ((TReal) freq);
			hintPair2.iKey.iUid = uidArray[i];
			TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
			TEST(hintPair2.iValue == 300);
			TEST(hintPair2.iMutable == 1);
			}
//		RDebug::Print(_L("[LOG]: \t%d \t%f "), i, measure1/numberOfCalculation);
		measureTotal +=measure1;
		}
	INFO_PRINTF2(_L("the average time for SetSurfaceHint is %f\r\n"),measureTotal/8);

	RDebug::Print(_L("SetSurfaceHint Test finished"));
	iSurfaceManager.CloseSurface(surfaceId);	
// For AddSurfaceHint	
	RSurfaceManager::TSurfaceCreationAttributesBuf bufNew;
	RSurfaceManager::TSurfaceCreationAttributes& attributesNew = bufNew();
	attributesNew.iSize = TSize(1,1);
	attributesNew.iBuffers = 1;
	attributesNew.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributesNew.iStride = 1;
	attributesNew.iOffsetToFirstBuffer = 1;
	attributesNew.iAlignment = RSurfaceManager::EPageAligned;
	attributesNew.iOffsetBetweenBuffers = 0;
	attributesNew.iContiguous = ETrue;
	attributesNew.iCacheAttrib = RSurfaceManager::ECached;	 // Cache attributes
	attributesNew.iMappable = ETrue;
	
	// Create a surface with no surface hints defined
	TSurfaceId surfaceIdNew;
	TInt err1 = iSurfaceManager.CreateSurface(bufNew, surfaceIdNew);
	TEST(KErrNone == err1);
	measureTotal = 0;
	// Measure time of adding surface hint of different location
	for (i = 0; i<maxHintsPerSurface; ++i)
		{
		hintPair.iKey.iUid = uidArray[i]; 
		hintPair.iValue = 300+i;
		hintPair.iMutable = ETrue;
		timeBefore = User::FastCounter();
		TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceIdNew, hintPair));
		timeAfter = User::FastCounter();
		measure1 = 1000 * ((TReal)(timeAfter-timeBefore)) / ((TReal) freq);
		measureTotal += measure1;
//		RDebug::Print(_L("AddSurfaceHint [LOG]: \t%d \t%f "), i, measure1);
		}
	INFO_PRINTF2(_L("the average time for AddSurfaceHint is %f\r\n"),measureTotal/8);

	// Check all 8 hint pairs have the right values
	measureTotal = 0;
	for (i = 0; i<maxHintsPerSurface; ++i)
		{
		hintPair2.iKey.iUid = uidArray[i]; 
		timeBefore = User::FastCounter();
		TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceIdNew, hintPair2));
		timeAfter = User::FastCounter();
		measure1 = 1000 * ((TReal)(timeAfter-timeBefore)) / ((TReal) freq);
		measureTotal += measure1;
//		RDebug::Print(_L("GetSurfaceHint [LOG]: \t%d \t%f "), i, measure1);
		TEST(hintPair2.iValue == 300+i);
		TEST(hintPair2.iMutable == 1);
		}
	INFO_PRINTF2(_L("the average time for GetSurfaceHint is %f\r\n"),measureTotal/8);

	iSurfaceManager.CloseSurface(surfaceIdNew);	
	

	// For SynchronizeCache
	TEST(KErrNone == iSurfaceManager.CreateSurface(bufNew, surfaceIdNew));
	measureTotal = 0;
	TInt index = 0;
	do
		{
		timeBefore = User::FastCounter();
		TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceIdNew,0, RSurfaceManager::ESyncBeforeNonCPURead));
		timeAfter = User::FastCounter();
		measure1= 1000000 * ((TReal)(timeAfter - timeBefore)) / ((TReal) freq);
//		RDebug::Print(_L("SynchronizeCache [LOG]: \t%d \t%f "), index, measure1);
		measureTotal += measure1;				
		} while ( index++ < numberOfCalculation);
	
	INFO_PRINTF2(_L("the average time for SynchronizeCache a surface is %f\r\n"),measureTotal/numberOfCalculation);

	iSurfaceManager.CloseSurface(surfaceIdNew);	

	uidArray.Close();
	iSurfaceManager.Close();

	}


void CTSurfaceManager::TestCreateSurfaceNewChunk1L()
	{
	INFO_PRINTF1(_L("Creating a Surface in a new shared chunk\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(100,100);
	attributes.iBuffers = 1;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;	
	attributes.iStride = 400;				// Number of bytes between start of one line and start of next
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	attributes.iSurfaceHints = hints;	
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;	 // Cache attributes
	attributes.iMappable = ETrue;
	
	// Test create surface doesn't return an error
	SurfaceOperation(buf);

	//cache attribute is not valid on emulator
#ifndef __WINS__
	TEST(iInfo.iCacheAttrib == attributes.iCacheAttrib);
#else
	INFO_PRINTF1(_L("Cache attribute test is not valid on emulator\n"));
#endif
	    	
	//contiguous attribute is not valid on emulator
#ifndef __WINS__
	TEST(iInfo.iContiguous == attributes.iContiguous);
#else
	INFO_PRINTF1(_L("Contiguous attribute test is not valid on emulator\n"));
#endif
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	//Test different contiguous attricutes
	attributes.iContiguous=EFalse;
	// Test create surface doesn't return an error
	SurfaceOperation(buf);

	//contiguous attribute is not valid on emulator
#ifndef __WINS__
	if(attributes.iContiguous)
	    TEST(iInfo.iContiguous == attributes.iContiguous);
	else
	    INFO_PRINTF1(_L("We didn't ask for contiguous memory so we don't care how it's going to be (can be contiguous or not)\n"));
#else
	INFO_PRINTF1(_L("Contiguous attribute test is not valid on emulator\n"));
#endif
	  
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	// Test different cache attributes
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;	 // Cache attributes
	// Test create surface doesn't return an error
	SurfaceOperation(buf);
		
	//cache attribute is not valid on emulator
#ifndef __WINS__
	TEST(iInfo.iCacheAttrib == attributes.iCacheAttrib);
#else
	INFO_PRINTF1(_L("Cache attribute test is not valid on emulator\n"));
#endif
		
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	//Set different alignment for CPU cached and noncached, test create surface doesn't return an error 
	ChangeAttributes(&attributes, (TInt)RSurfaceManager::EPageAligned, 4096, 53248, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
		
	ChangeAttributes(&attributes, (TInt)RSurfaceManager::EPageAligned, 4096, 53248, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 1, 32, 50016, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 1, 10, 50010, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 2, 32, 50016, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 2, 10, 50010, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 4, 32, 50016, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 4, 12, 50012, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 8, 32, 50016, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 8, 16, 50016, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 16, 32, 50016, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
		
	ChangeAttributes(&attributes, 16, 16, 50016, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 32, 32, 50016, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
			
	ChangeAttributes(&attributes, 32, 32, 50016, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
	
	ChangeAttributes(&attributes, 64, 64, 50048, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
				
	ChangeAttributes(&attributes, 64, 64, 50048, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckAttributesL(attributes);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  

   	// Close the surface manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::ChangeAttributes(RSurfaceManager::TSurfaceCreationAttributes* aAttributes, TInt aAlignment, TInt aOffsetToFirstBuffer, TInt aOffsetBetweenBuffers, RSurfaceManager::TCacheAttribute aCacheAttrib)
	{
	aAttributes->iAlignment = aAlignment;
	aAttributes->iOffsetToFirstBuffer = aOffsetToFirstBuffer;
	aAttributes->iOffsetBetweenBuffers  = aOffsetBetweenBuffers; 
	aAttributes->iCacheAttrib = aCacheAttrib;
	}

void CTSurfaceManager::SurfaceOperation(RSurfaceManager::TSurfaceCreationAttributesBuf& aBuf)
	{
	TEST(KErrNone == iSurfaceManager.CreateSurface(aBuf, iSurfaceId)); 
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(iSurfaceId, iInfoBuf));
	iInfo = iInfoBuf();	
	}

void CTSurfaceManager::CheckAttributesL(RSurfaceManager::TSurfaceCreationAttributes& aAttributes)
	{	
	TEST(aAttributes.iSize==iInfo.iSize);
	TEST(aAttributes.iBuffers==iInfo.iBuffers);
	TEST(aAttributes.iPixelFormat==iInfo.iPixelFormat);
	TEST(aAttributes.iStride==iInfo.iStride);
    if(aAttributes.iContiguous)
        TEST(iInfo.iContiguous == aAttributes.iContiguous);
    else
        INFO_PRINTF1(_L("We didn't ask for contiguous memory so we don't care how it's going to be (can be contiguous or not)\n"));	
	TEST(aAttributes.iCacheAttrib==iInfo.iCacheAttrib);
	
	//Test the chunk size is big enough to hold the surface when we create it 
	//with the offset between buffers
	if (aAttributes.iOffsetBetweenBuffers!=0)
		{
		CheckSizeL(aAttributes.iOffsetToFirstBuffer, aAttributes.iOffsetBetweenBuffers);
		}
	}

void CTSurfaceManager::CheckSizeL(TInt aOffsetToFirstBuffer, TInt aOffsetBetweenBuffers)
	{
	TInt64 bufferSize = iInfo.iBuffers * aOffsetBetweenBuffers;
	TInt64 chunkSize = aOffsetToFirstBuffer + bufferSize;
	TESTL(KErrNone == iSurfaceManager.MapSurface(iSurfaceId,iHandle));
	TInt64 currChunkSize = iHandle.Size();
	TEST(currChunkSize >= chunkSize);	
	iHandle.Close();
	}

void CTSurfaceManager::TestCreateSurfaceNewChunk2L()
	{
	INFO_PRINTF1(_L("Creating a Surface in a new shared chunk\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(100,100);
	attributes.iBuffers = 1;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;	
	attributes.iStride = 400;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 9;	// way of reserving space before the surface pixel data
	attributes.iAlignment = 1;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	attributes.iSurfaceHints = hints;	
	
	attributes.iOffsetBetweenBuffers = 50009;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;	 // Cache attributes
	attributes.iMappable = ETrue;
	
	//Test create surface doesn't return an error 
	SurfaceOperation(buf);
	CheckSizeL(32, 50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId));  
		 
	// Test different cache attributes
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;	 // Cache attributes
	SurfaceOperation(buf);
	CheckSizeL(9, 50009);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 

	ChangeAttributes(&attributes, 2, 9, 50009, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckSizeL(32,50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 2, 9, 50009, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckSizeL(10,50010);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 4, 10, 50010, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckSizeL(32,50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 4, 10, 50010, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckSizeL(12,50012);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 8, 10, 50010, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckSizeL(32,50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 8, 10, 50010, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckSizeL(16,50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 16, 10, 50010, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckSizeL(32,50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 16, 10, 50010, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckSizeL(16,50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 32, 10, 50010, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckSizeL(32,50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 32, 10, 50010, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckSizeL(32,50016);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 64, 10, 50010, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckSizeL(64,50048);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, 64, 10, 50010, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckSizeL(64,50048);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	
	ChangeAttributes(&attributes, (TInt)RSurfaceManager::EPageAligned, 10, 50010, RSurfaceManager::ECached);
	SurfaceOperation(buf);
	CheckSizeL(4096,53248);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 
	
	ChangeAttributes(&attributes, (TInt)RSurfaceManager::EPageAligned, 10, 50010, RSurfaceManager::ENotCached);
	SurfaceOperation(buf);
	CheckSizeL(4096,53248);
	TEST(KErrNone == iSurfaceManager.CloseSurface(iSurfaceId)); 

   	// Close the surface manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::TestCreateSurfaceExisitingChunkL()
	{
	INFO_PRINTF1(_L("creating a surface in an exisiting shared chunk\r\n"));	
	// Call another device driver to get a valid shared chunk handle
	RSharedChunkLdd device;
	RChunk handle;
	TUint ChunkSize = 204800;	// bottom 8 bits reserved space for other chunk creation attributes info
	TUint ChunkAttribs = ChunkSize|EMultiple|EOwnsMemory|ECached;
	TUint ContiguousAttrib = 1;
	GetSharedChunkHandleL(device, handle, ChunkSize, ChunkAttribs, ContiguousAttrib);

	INFO_PRINTF1(_L("Creating a Surface in an exisitng shared chunk\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes 
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(100,100);
	attributes.iBuffers = 1;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;		
	attributes.iStride = 400;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 4096;	// way of reserving space before the surface pixel data
	attributes.iAlignment = 4;
	
	RSurfaceManager::THintPair hints[2];
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	attributes.iSurfaceHints = hints;
	
	attributes.iOffsetBetweenBuffers = 0;
	
	attributes.iMappable = ETrue;
	
	TSurfaceId surfaceId;
	// Test create surface doesn't return an error
	TInt err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrNone == err);
	
	RSurfaceManager::TInfoBuf infoBuf;
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
    if (err == KErrNone)
    	{
    	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
    	
    	//cache attribute is not valid on emulator
    #ifndef __WINS__
    	TEST(info.iCacheAttrib == device.GetCacheAttribute());
    #else
    	INFO_PRINTF1(_L("Cache attribute test is not valid on emulator\n"));
    #endif
    	
    //contiguous attribute is not valid on emulator
    #ifndef __WINS__
    	TEST(info.iContiguous == device.GetContiguousAttribute());
    #else
    	INFO_PRINTF1(_L("Contiguous attribute test is not valid on emulator\n"));
    #endif
    	
    	//Map the surface to get a new shared chunk handle
    	RChunk handle2;
    	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle2));
    	
    	//Check the two handles are different but refer to the same shared chunk
    	TEST(handle.Handle() != handle2.Handle());
    	TEST(handle.Base() == handle2.Base());
    	
    	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
    	
    	handle2.Close();	
    	}
    
    //Reset iOffsetBetweenBuffers is 40960
    attributes.iOffsetBetweenBuffers = 40960;
    attributes.iAlignment = RSurfaceManager::EPageAligned;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
        	
    TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));        	
    TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
    
    //Test different iOffsetBetweenBuffers attributes
    attributes.iOffsetBetweenBuffers = 53248;
    RChunk handle3;
    // Test create surface doesn't return an error
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
    TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle3));  	  
    TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
    
    //Test the chunk size is big enough to hold the surface
    TInt64 bufferSize= attributes.iBuffers * attributes.iOffsetBetweenBuffers;
    TInt64 chunkSize = attributes.iOffsetToFirstBuffer + bufferSize; 	
    TInt64 currChunkSize = handle3.Size();
    		
    TEST(currChunkSize >= chunkSize);
    TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));    
    
    //set iBuffer is equal to 2 and test iOffsetBetweenBuffers returns the right value, check the chunk size is big enough to hold the surface 
    attributes.iBuffers = 2;
    RChunk handle4;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
    TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle4));	  
    TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
    
    bufferSize = attributes.iBuffers * attributes.iOffsetBetweenBuffers;
    chunkSize = attributes.iOffsetToFirstBuffer + bufferSize;		
    currChunkSize = handle4.Size();
    		
    TInt offsetToFirstBuffer;
    TInt offsetBetweenBuffers;
   	err=iSurfaceManager.GetBufferOffset(surfaceId, 0, offsetToFirstBuffer);
   	err=iSurfaceManager.GetBufferOffset(surfaceId, 1, offsetBetweenBuffers);
   	offsetBetweenBuffers-=offsetToFirstBuffer;
    TEST(offsetBetweenBuffers == attributes.iOffsetBetweenBuffers);
    TEST(currChunkSize >= chunkSize);
    TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId)); 
	
    handle4.Close();
    handle3.Close();
    handle.Close();
	device.CloseChunkHandle(handle);
   	// Close the surface manager
    iSurfaceManager.Close();
    device.CloseChunk();
    device.Close();
	}


void CTSurfaceManager::TestCreateSurfaceChunkWithHolesL()
	{
	INFO_PRINTF1(_L("creating a surface in an exisiting shared chunk with HOLES\r\n"));	
	// Call another device driver to get a valid shared chunk handle
	RSharedChunkLdd device;
	RChunk handle;
	TUint ChunkSize = 204800;
	TUint ChunkAttribs = ChunkSize|EMultiple|EOwnsMemory|ECached;
	TUint ContiguousAttrib = 0;
	// We use the same stride everywhere to make life easier. 
	const TUint KStride = 64;
	TInt r;
	
	INFO_PRINTF1(_L("Load Logical Device\r\n"));
	r=User::LoadLogicalDevice(KSharedChunkLddName);
	TEST(r==KErrNone || r==KErrAlreadyExists);
	
	INFO_PRINTF1(_L("Open Device"));
	User::LeaveIfError(device.Open());
	
	//Create shared chunk and commit the memory
	INFO_PRINTF1(_L("Test chunk create"));
	
	TEST(KErrNone==device.CreateChunk(ChunkAttribs));

	// Find size of a page. 
	TInt pageSize;
	TEST(KErrNone == UserHal::PageSizeInBytes(pageSize));
	// Fill in one page's worth of memory.  
	TEST(KErrNone==device.CommitMemory(0 | ContiguousAttrib, pageSize));
	
	//Get the chunk handle
	INFO_PRINTF1(_L("Open user handle"));
	TEST(KErrNone==device.GetChunkHandle(handle));

	INFO_PRINTF1(_L("Creating a Surface in an exisitng shared chunk with holes\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes 
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(10,10);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;		
	attributes.iStride = KStride;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = pageSize;	// Add this to the first address -> fails, since the actual content is 
												// outside of the chunk. 
	attributes.iOffsetBetweenBuffers = 0;		// 0 => Let CreateSurface calculatet the size by itself. 
	attributes.iAlignment = 4;					// Need to have some alignment.
	attributes.iMappable = ETrue;
	
	TSurfaceId surfaceId;
	// This should FAIL.
	TInt err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrArgument == err);
	
	// Use a smaller "user area" so that it will fit. 
	attributes.iSize = TSize(10,10);
	attributes.iOffsetToFirstBuffer = 100;	// way of reserving space before the surface pixel data
	
	// This should PASS.
	err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrNone == err);
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	
	// Now add in another bit of memory 2*pagesize so that a second buffer fits in the page..
	TEST(KErrNone==device.CommitMemory(pageSize * 2 | ContiguousAttrib, pageSize));
	
	attributes.iSize = TSize(32,pageSize / KStride); 
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iOffsetBetweenBuffers = pageSize * 2;
	
	// This should PASS.
	err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrNone == err);
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	// Check that we do NOT get a contiguous surface back - it isn't contiguous when
	// there is a hole in it!
	TEST(!info.iContiguous);
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));

	attributes.iSize = TSize(32,(pageSize + pageSize / 2) / KStride);  // Use 1.5 pages worth. 
	
	// This should FAIL.
	err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrArgument == err);
	
	attributes.iSize = TSize(32,(pageSize / 2) / KStride); // Half a page of buffer. 
	attributes.iOffsetToFirstBuffer = pageSize / 2;	// Half a page of "user data" at first buffer. 
	
	// This should PASS.
	err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrNone == err);
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	info = infoBuf();
	// Check that we do NOT get a contiguous surface back - it isn't contiguous when
	// there is a hole in it!
	TEST(!info.iContiguous);
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));

	attributes.iSize = TSize(32,((pageSize / 2) / KStride)-1); // Half a page of buffer - minus a tiny bit.  
	attributes.iOffsetToFirstBuffer = KStride + pageSize / 2;	// Half a page of "user data" at first buffer, plus a bit. . 
	attributes.iAlignment = KStride;
	
	// This should PASS.
	err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrNone == err);
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	info = infoBuf();
	// Check that we do NOT get a contiguous surface back - it isn't contiguous when
	// there is a hole in it!
	TEST(!info.iContiguous);
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));

	// Half a page of "user data" at first buffer, plus 2 strides to go over the edge
	attributes.iOffsetToFirstBuffer = 2 * KStride + pageSize / 2;	 
	attributes.iAlignment = 4;
	
	// This should FAIL.
	err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrArgument == err);
	
	// Ask for HUGE buffer with no actual memory there. 
	attributes.iSize = TSize(32, pageSize * pageSize);   // A page of pages * stride is several gigabytes at 4K/page.  
	
	// This should FAIL
	err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrArgument == err);

	device.CloseChunkHandle(handle);
   	// Close the surface manager
    iSurfaceManager.Close();
    device.CloseChunk();
    device.Close();
	}

void CTSurfaceManager::GetSharedChunkHandleL(RSharedChunkLdd& aDevice, RChunk& aHandle, TUint aChunkSize, TUint aChunkAttribs, TUint aContiguousAttrib)
	{
	TInt r;

	INFO_PRINTF1(_L("Load Logical Device\r\n"));
	r=User::LoadLogicalDevice(KSharedChunkLddName);
	TEST(r==KErrNone || r==KErrAlreadyExists);
	
	INFO_PRINTF1(_L("Open Device"));
	User::LeaveIfError(aDevice.Open());
	
	//Create shared chunk and commit the memory
	INFO_PRINTF1(_L("Test chunk create"));
	
	TEST(KErrNone==aDevice.CreateChunk(aChunkAttribs));
	TEST(KErrNone==aDevice.CommitMemory(aContiguousAttrib, aChunkSize));
	
	//Get the chunk handle
	INFO_PRINTF1(_L("Open user handle"));
	TEST(KErrNone==aDevice.GetChunkHandle(aHandle));
	}

void CTSurfaceManager::TestCreateSurfaceExisitingChunk2L()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Creating a Surface with distinct creation attributes in an exisitng shared chunk which already contains a surface\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes 
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(100,100);
	attributes.iBuffers = 1;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;		
	attributes.iStride = 400;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = 2;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	ReAllocHintsArrayL(2);
	attributes.iHintCount=2;
	iHintsArray[0].Set(TUid::Uid(0x124578), 25, ETrue);
	iHintsArray[1].Set(TUid::Uid(0x237755), 50, ETrue);
	attributes.iSurfaceHints = iHintsArray;
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	//store all the attributes
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	
	//Map the surface in the current processs
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
  	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

	attributesNew.iSize = TSize(480,16);
	attributesNew.iBuffers = 2;				// number of buffers in the surface
	attributesNew.iPixelFormat = EUidPixelFormatYUV_422Reversed;		
	attributesNew.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributesNew.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data

	RSurfaceManager::THintPair hint;
	attributes.iHintCount=1;
	hint.Set(TUid::Uid(0x124545), 50, EFalse);
	attributes.iSurfaceHints = &hint;
	
	attributesNew.iAlignment = RSurfaceManager::EPageAligned;
	attributesNew.iOffsetBetweenBuffers = 0;
	attributesNew.iMappable = ETrue;
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceIdNew;
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buff, surfaceIdNew, handle));
	
	//Store all the attributes 
	RSurfaceManager::TInfoBuf infoBufNew;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceIdNew, infoBufNew));
	RSurfaceManager::TSurfaceInfoV01& infoNew = infoBufNew();
	
	//Map the new surface in the current process
	RChunk handleNew;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceIdNew,handleNew));
	
	//cache attribute is not valid on emulator
#ifndef __WINS__
	TEST(info.iCacheAttrib == infoNew.iCacheAttrib);
#else
	INFO_PRINTF1(_L("Cache attribute test is not valid on emulator\n"));
#endif
	
	//contiguous attribute is not valid on emulator
#ifndef __WINS__
	TEST(info.iContiguous == infoNew.iContiguous);
#else
	INFO_PRINTF1(_L("Contiguous attribute test is not valid on emulator\n"));
#endif
	
	//two surfaceIds are different
	TEST(surfaceId != surfaceIdNew);
	
	//check the offset between buffers is valid
    TInt offsetToFirstBufferNew;
    TInt offsetBetweenBuffersNew;   	
   	TInt err=iSurfaceManager.GetBufferOffset(surfaceIdNew, 0, offsetToFirstBufferNew);
   	err=iSurfaceManager.GetBufferOffset(surfaceIdNew, 1, offsetBetweenBuffersNew);
   	offsetBetweenBuffersNew-=offsetToFirstBufferNew;
   	TEST(offsetBetweenBuffersNew >= attributesNew.iSize.iHeight*attributesNew.iStride);    
	//check the two handles are different but refer to the same shared chunk 
	TEST(handle.Handle() != handleNew.Handle());
	TEST(handle.Base() == handleNew.Base());
	
	// Get the adress of this chunk of memory
	TUint8* surfaceAddNew = handleNew.Base();
	TUint8* bufferAddNew = surfaceAddNew + attributes.iOffsetToFirstBuffer;
	
	// Write to the second surface's buffer, and test the value is written
	*bufferAddNew = 20;
	TInt temp = *bufferAddNew;
	TEST(temp == 20);
	
	//Read the value from the address of the pixel data for this first surface
	TUint8* surfaceAdd = handle.Base();
	TUint8* bufferAdd = surfaceAdd + attributes.iOffsetToFirstBuffer;
	TEST(*bufferAdd == 20);
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	// As the surface is not created properly, closing returns KErrArgument
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew));  
	
	//Reset iOffsetBetweenBuffers is 20480
	attributesNew.iOffsetBetweenBuffers = 20480;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buff, surfaceIdNew, handle));
	    	
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceIdNew, infoBufNew));
	    	
	//Test that the attribute is returned correctly
  	err=iSurfaceManager.GetBufferOffset(surfaceIdNew, 0, offsetToFirstBufferNew);
   	err=iSurfaceManager.GetBufferOffset(surfaceIdNew, 1, offsetBetweenBuffersNew);
   	offsetBetweenBuffersNew-=offsetToFirstBufferNew;	
	
	TEST(offsetBetweenBuffersNew == attributesNew.iOffsetBetweenBuffers);
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew)); 
 
	handle.Close();
	handleNew.Close();
   	// Close the surface manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::TestCreateSurfaceExisitingChunk3L()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Creating a Surface with identical creation attributes in an exisitng shared chunk which already contains a surface\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes 
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = 4;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);
	attributes.iSurfaceHints = hints;
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	//store all the attributes
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	
	// Test create surface doesn't return an error
	TSurfaceId surfaceIdNew;
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdNew, handle));
  
	//Store all the attributes 
	RSurfaceManager::TInfoBuf infoBufNew;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceIdNew, infoBufNew));
	RSurfaceManager::TSurfaceInfoV01& infoNew = infoBufNew();
	
	//Map the new surface in the current process
	RChunk handleNew;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceIdNew,handleNew));
	
	//test two sets of attributes are identical for two surfaces
	TEST(info.iSize.iHeight == infoNew.iSize.iHeight);
	TEST(info.iSize.iWidth == infoNew.iSize.iWidth);
	TEST(info.iBuffers == infoNew.iBuffers);
	TEST(info.iPixelFormat == infoNew.iPixelFormat);
	TEST(info.iStride == infoNew.iStride);
	
	
	TInt offsetToFirstBuffer, offsetToFirstBufferNew;
	TInt offsetBetweenBuffers, offsetBetweenBuffersNew;
  	TInt err=iSurfaceManager.GetBufferOffset(surfaceId, 0, offsetToFirstBuffer);
   	err=iSurfaceManager.GetBufferOffset(surfaceId, 1, offsetBetweenBuffers);
   	offsetBetweenBuffers-=offsetToFirstBuffer;	
   	
  	err=iSurfaceManager.GetBufferOffset(surfaceIdNew, 0, offsetToFirstBufferNew);
   	err=iSurfaceManager.GetBufferOffset(surfaceIdNew, 1, offsetBetweenBuffersNew);
   	offsetBetweenBuffersNew-=offsetToFirstBufferNew;	
	
	
	TEST(offsetToFirstBuffer == offsetToFirstBufferNew);
	TEST(offsetBetweenBuffers == offsetBetweenBuffersNew);
	
	//cache attribute is not valid on emulator
#ifndef __WINS__
	TEST(info.iCacheAttrib == infoNew.iCacheAttrib);
#else
	INFO_PRINTF1(_L("Cache attribute test is not valid on emulator\n"));
#endif
	
	//contiguous attribute is not valid on emulator
#ifndef __WINS__
	TEST(info.iContiguous == infoNew.iContiguous);
#else
	INFO_PRINTF1(_L("Contiguous attribute test is not valid on emulator\n"));
#endif
	
	//check that two surfaceIds are different
	TEST(surfaceId != surfaceIdNew);
	
	//check the iOffsetBetweenBuffers return by SurfaceInfo() is valid
	TEST((offsetBetweenBuffers >= attributes.iSize.iHeight*attributes.iStride) && (offsetBetweenBuffers % attributes.iAlignment ==0));
	TEST((offsetBetweenBuffersNew >= attributes.iSize.iHeight*attributes.iStride) && (offsetBetweenBuffersNew % attributes.iAlignment ==0));
	
	//check the two handles are different but refer to the same shared chunk 
	TEST(handle.Handle() != handleNew.Handle());
	TEST(handle.Base() == handleNew.Base()); 
	
	// Get the adress of this chunk of memory
	TUint8* surfaceAddNew = handleNew.Base();
	TUint8* bufferAddNew = surfaceAddNew + attributes.iOffsetToFirstBuffer;
	
	// Write to the second surface's buffer, and test the value is written
	*bufferAddNew = 20;
	TInt temp = *bufferAddNew;
	TEST(temp == 20);
	
	//Read the value from the address of the pixel data for this first surface
	TUint8* surfaceAdd = handle.Base();
	TUint8* bufferAdd = surfaceAdd + attributes.iOffsetToFirstBuffer;
	TEST(*bufferAdd == 20);
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	// As the surface is not created properly, closing returns KErrArgument
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew));  
 
	handle.Close();
	handleNew.Close();
   	// Close the surface manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::TestCreateSurfaceExisitingChunk4L()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Creating a Surface with rotated orientation in an exisitng shared chunk which already contains a surface\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes 
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		
	attributes.iStride = 1200;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 10;	// way of reserving space before the surface pixel data
	attributes.iAlignment = 4;		// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned
	
	RSurfaceManager::THintPair hints[2];	     // two hint pairs specified
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);
	attributes.iSurfaceHints = hints;	
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	//store all the attributes
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	TInt offsetToFirstBuffer, offsetBetweenBuffers;
  	TInt err=iSurfaceManager.GetBufferOffset(surfaceId, 0, offsetToFirstBuffer);
   	err=iSurfaceManager.GetBufferOffset(surfaceId, 1, offsetBetweenBuffers);
   	offsetBetweenBuffers-=offsetToFirstBuffer;	
	
	//Map the surface in the current process to get the first chunk handle
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	
	
	// Test create surface doesn't return an error
	TSurfaceId surfaceIdNew;
	
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
  	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

	attributesNew.iSize = TSize(16,480);
	attributesNew.iBuffers = 2;				// number of buffers in the surface
	attributesNew.iPixelFormat =EUidPixelFormatYUV_422Reversed;		
	attributesNew.iStride = 34;				// Number of bytes between start of one line and start of next
	attributesNew.iOffsetToFirstBuffer = offsetToFirstBuffer;	// way of reserving space before the surface pixel data
	
	RSurfaceManager::THintPair hint;
	attributes.iHintCount=1;
	hint.Set(TUid::Uid(0x124578),20,ETrue);
	attributesNew.iSurfaceHints = &hint;
	
	attributesNew.iAlignment = 4;
	attributesNew.iOffsetBetweenBuffers = offsetBetweenBuffers;
	attributesNew.iMappable = ETrue;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buff, surfaceIdNew, handle));
 
	//Store all the attributes 
	RSurfaceManager::TInfoBuf infoBufNew;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceIdNew, infoBufNew));
	RSurfaceManager::TSurfaceInfoV01& infoNew = infoBufNew();
	TInt offsetToFirstBufferNew, offsetBetweenBuffersNew;
  	err=iSurfaceManager.GetBufferOffset(surfaceIdNew, 0, offsetToFirstBufferNew);
   	err=iSurfaceManager.GetBufferOffset(surfaceIdNew, 1, offsetBetweenBuffersNew);
   	offsetBetweenBuffersNew-=offsetToFirstBufferNew;	
	
	//Map the new surface in the current process
	RChunk handleNew;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceIdNew,handleNew));
	
	//cache attribute is not valid on emulator
#ifndef __WINS__
	TEST(info.iCacheAttrib == infoNew.iCacheAttrib);
#else
	INFO_PRINTF1(_L("Cache attribute test is not valid on emulator\n"));
#endif
	
	//contiguous attribute is not valid on emulator
#ifndef __WINS__
	TEST(info.iContiguous == infoNew.iContiguous);
#else
	INFO_PRINTF1(_L("Contiguous attribute test is not valid on emulator\n"));
#endif
	
	//check that two surfaceIds are different
	TEST(surfaceId != surfaceIdNew);
	
	TEST(info.iSize.iHeight == infoNew.iSize.iWidth);
	TEST(info.iSize.iWidth == infoNew.iSize.iHeight);
	TEST(offsetToFirstBuffer == offsetToFirstBufferNew);
	INFO_PRINTF2(_L("The offset to first Buffer is %d\n"),offsetToFirstBuffer);
	TEST(offsetBetweenBuffers == offsetBetweenBuffersNew);
	INFO_PRINTF2(_L("The offset between Buffers is %d \n"),offsetBetweenBuffers);
	
	TEST(info.iStride != infoNew.iStride);
	
	//check the iOffsetBetweenBuffers returned by SurfaceInfo() is valid
	TEST((offsetBetweenBuffers >= attributes.iSize.iHeight*attributes.iStride) && (offsetBetweenBuffers % attributes.iAlignment ==0));
	TEST((offsetBetweenBuffersNew >= attributesNew.iSize.iHeight*attributesNew.iStride) && (offsetBetweenBuffersNew % attributesNew.iAlignment ==0));
	
	//check the two handles are different but refer to the same shared chunk 
	TEST(handle.Handle() != handleNew.Handle());
	TEST(handle.Base() == handleNew.Base());
	
	// Get the adress of this chunk of memory
	TUint8* surfaceAddNew = handleNew.Base();
	TUint8* bufferAddNew = surfaceAddNew + offsetToFirstBufferNew;
	
	// Write to the second surface's buffer, and test the value is written
	*bufferAddNew = 20;
	TInt temp = *bufferAddNew;
	TEST(temp == 20);
	
	//Read the value from the address of the pixel data for this first surface
	TUint8* surfaceAdd = handle.Base();
	TUint8* bufferAdd = surfaceAdd + offsetToFirstBuffer;
	TEST(*bufferAdd == 20);
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	// As the surface is not created properly, closing returns KErrArgument
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew));  
 
	handle.Close();
	handleNew.Close();
   	// Close the surface manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::SetupAttributesL(RSurfaceManager::TSurfaceCreationAttributes& attributesParams)
	{
	attributesParams.iSize = TSize(100,100);
	attributesParams.iBuffers = 1;				// number of buffers in the surface
	attributesParams.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;		
	attributesParams.iStride = 400;				// Number of bytes between start of one line and start of next
	attributesParams.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributesParams.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned
	attributesParams.iContiguous=ETrue;
	attributesParams.iCacheAttrib = RSurfaceManager::ECached;	 // Cache attributes
	attributesParams.iOffsetBetweenBuffers = 0;
		
	//Set iSurfaceHints.iHints[0]
	ReAllocHintsArrayL(1);
	attributesParams.iSurfaceHints = iHintsArray;
	attributesParams.iSurfaceHints[0].Set(TUid::Uid(0x124578), 20, ETrue);
	attributesParams.iHintCount = 1;
	attributesParams.iMappable = ETrue;
	}
void CTSurfaceManager::TestCreateSurfaceAssociatedWithHintPairsL()
	{
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes 
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
	TInt maxHintsPerSurface;
	iSurfaceManager.GetSurfaceManagerAttrib(RSurfaceManager::EMaxNumberOfHints,maxHintsPerSurface);
	
	// Create a surface in a new shared chunk
	INFO_PRINTF2(_L("Creating a Surface associated with 0 to a maximum of %i hint pairs\r\n"),maxHintsPerSurface);
	
	ReAllocHintsArrayL(maxHintsPerSurface);
	iHintsArray[0].Set(TUid::Uid(0x124578),20,ETrue);
	TInt i;
	for (i=1;i<maxHintsPerSurface;++i)
		{
		iHintsArray[i].Set(TUid::Uid(0x124512+i),20,ETrue);
		}	
	attributes.iSurfaceHints = iHintsArray;
	
	
	for (attributes.iHintCount=1;attributes.iHintCount<=maxHintsPerSurface;++attributes.iHintCount)
		{		
		TSurfaceId surfaceId;
		// Test create surface doesn't return an error
		TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));		
		// Get the hint values and check them
		for (i=1;i<=attributes.iHintCount;++i)
			{
			RSurfaceManager::THintPair hint;
			hint.iKey=iHintsArray[i-1].iKey;
			TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hint));
			TEST(hint.iValue==iHintsArray[i-1].iValue);
			TEST(hint.iMutable==iHintsArray[i-1].iMutable);			
			}		
		TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
		}
	iSurfaceManager.Close();
	}

	void CTSurfaceManager::CheckAlignment(TSurfaceId& aId, TInt aAlign, RSurfaceManager::TCacheAttribute aCached)
		{
		RSurfaceManager::TInfoBuf infoBuf;
		RSurfaceManager::TSurfaceInfoV01& info = infoBuf(); 

		//cached surfaces are aligned to minimum of cache line which is 32
		if (aCached == RSurfaceManager::ECached && aAlign < 32)
			{
			// check the aligment is at least 32 for cached surfaces.
			aAlign = 32;
			}

		TInt mask = (aAlign == RSurfaceManager::EPageAligned) ? 0xFFF : aAlign - 1;

		TInt offsetToFirstBuffer, offsetBetweenBuffers;
	  	TInt err=iSurfaceManager.GetBufferOffset(aId, 0, offsetToFirstBuffer);
	   	err=iSurfaceManager.GetBufferOffset(aId, 1, offsetBetweenBuffers);
	   	offsetBetweenBuffers-=offsetToFirstBuffer;	
		
		TEST((offsetToFirstBuffer & mask) == 0);
		TEST((offsetBetweenBuffers & mask) == 0);

		}


	void CTSurfaceManager::TestCreateSurfaceAlignmentsMemoryTypeL() 
	{
	INFO_PRINTF1(_L("Creating a Surface with all types of alignment in cached and none cached memory\r\n"));
	// Open the surface manager
    User::LeaveIfError(iSurfaceManager.Open());
    
    // Setup attributes
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(61,47);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar;
	attributes.iStride = 61;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 1;	// way of reserving space before the surface pixel data
	attributes.iContiguous=EFalse;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	TInt x = 0;
	TSurfaceId sid;
   	for (x = 0; x < 7; x++)
		{
		attributes.iAlignment = 1 << x;
		
		attributes.iCacheAttrib = RSurfaceManager::ECached;
		TEST(KErrNone == iSurfaceManager.CreateSurface(buf, sid));
		CheckAlignment(sid, attributes.iAlignment, attributes.iCacheAttrib);
		TEST(KErrNone == iSurfaceManager.CloseSurface(sid));

		attributes.iCacheAttrib = RSurfaceManager::ENotCached;
		TEST(KErrNone == iSurfaceManager.CreateSurface(buf, sid));
		CheckAlignment(sid, attributes.iAlignment, attributes.iCacheAttrib);
		TEST(KErrNone == iSurfaceManager.CloseSurface(sid));
		}	

	attributes.iAlignment = RSurfaceManager::EPageAligned;

	attributes.iCacheAttrib = RSurfaceManager::ECached;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, sid));
	CheckAlignment(sid, attributes.iAlignment, attributes.iCacheAttrib);
	TEST(KErrNone == iSurfaceManager.CloseSurface(sid));

	attributes.iCacheAttrib = RSurfaceManager::ENotCached;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, sid));
	CheckAlignment(sid, attributes.iAlignment, attributes.iCacheAttrib);
	TEST(KErrNone == iSurfaceManager.CloseSurface(sid));
	
    // Close the surface manager
    iSurfaceManager.Close();	
	}




void CTSurfaceManager::TestOpenSurfaceL()
	{
	INFO_PRINTF1(_L("Test opening a surface\r\n"));
	// Open the surface manager
    User::LeaveIfError(iSurfaceManager.Open());
    
    // Setup attributes
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
	
	// Create surface
	TSurfaceId surfaceId;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    // Test OpenSurface returns KErrNone
	TEST(KErrNone == iSurfaceManager.OpenSurface(surfaceId));
	   	
   	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestClosingSurfaceDecrementsReferenceCountL()
	{
	INFO_PRINTF1(_L("CloseSurface (correctly) decrements reference count\r\n"));
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	SetupAttributesL(attributes);
	
	// Create a surface - sets reference count to 1
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    
	// Call SurfaceInfo - doesn't alter reference count
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
		
	// Check that CloseSurface returns KErrNone
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));

	// Check that OpenSurface fails (since ref count == 0)
	TEST(KErrArgument == iSurfaceManager.OpenSurface(surfaceId));
	
	// Close the manager
	iSurfaceManager.Close();
    }

void CTSurfaceManager::TestCreateSurfaceSetReferenceCountL()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("the new CreateSurface() API create the surface and set its reference count to one \r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes 
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();

	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8,16,32,64 byte aligned or EPageAligned

	RSurfaceManager::THintPair hints[2];
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	attributes.iSurfaceHints = hints;
		
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	//Map the new surface
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	
	
	// Test create surface doesn't return an error
	TSurfaceId surfaceIdNew;
	
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
  	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

	attributesNew.iSize = TSize(16,480);
	attributesNew.iBuffers = 2;				// number of buffers in the surface
	attributesNew.iPixelFormat =EUidPixelFormatYUV_422Reversed;		
	attributesNew.iStride = 34;				// Number of bytes between start of one line and start of next
	attributesNew.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	
	RSurfaceManager::THintPair hint;	     // one hint pairs specified
	attributes.iHintCount=1;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	attributes.iSurfaceHints = &hint;	
	
	attributesNew.iAlignment = RSurfaceManager::EPageAligned;
	attributesNew.iOffsetBetweenBuffers = 0;
	attributesNew.iMappable = ETrue;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buff, surfaceIdNew, handle));
  
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	
	// As the surface is not created properly, closing returns KErrArgument
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew));  
 
	TEST(KErrArgument == iSurfaceManager.CloseSurface(surfaceIdNew));
	
	handle.Close();
   	// Close the surface manager
    iSurfaceManager.Close();
	}


void CTSurfaceManager::TestWriteToTwoBuffersL()
	{
	INFO_PRINTF1(_L("Writing to two buffers\r\n"));
	// Open the surface manager
	
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Setup attributes, setting iBuffers to 2
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(12,80);    	// w > 0, h > 0
	attributes.iBuffers = 2; 				// > 0, <= 4
	attributes.iPixelFormat = EUidPixelFormatARGB_1555;	
	attributes.iStride = 25;				// > 0, > width * bpp
	attributes.iOffsetToFirstBuffer = 20;	// > 0, divisible by alignment
	attributes.iAlignment = 4;				// 1 || 2 || 4 || 8
	
	RSurfaceManager::THintPair hints[2];	     // one hint pairs specified
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 50, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	attributes.iSurfaceHints = hints;
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Create the surface
	TSurfaceId surfaceId;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	// Map the surface to a chunk of memory
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId, handle));
		
	// Get the adress of this chunk of memory
	TUint8* surfaceAdd = handle.Base();
	TUint8* bufferAdd = surfaceAdd + attributes.iOffsetToFirstBuffer;
	
	// Write to the first buffer, and test the value is written
	*bufferAdd = 20;
	TInt temp = *bufferAdd;
	TEST(temp == 20);
	
	// Write to the second buffer and test the value is written
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf(); 
	TInt offsetToFirstBuffer, offsetBetweenBuffers;
  	TInt err=iSurfaceManager.GetBufferOffset(surfaceId, 0, offsetToFirstBuffer);
   	err=iSurfaceManager.GetBufferOffset(surfaceId, 1, offsetBetweenBuffers);
   	offsetBetweenBuffers-=offsetToFirstBuffer;	
	bufferAdd += offsetBetweenBuffers;
	*bufferAdd = 220;
	*(bufferAdd + 1) = 14;
	temp = *bufferAdd;
	TEST(temp == 220);
	temp = *(bufferAdd + 1);
	TEST(temp == 14);
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	handle.Close();
	// Close the surface manager
    iSurfaceManager.Close();
    }

void CTSurfaceManager::TestQuerySurfaceInfoWithoutMappingL() 
	{
	INFO_PRINTF1(_L("Getting SurfaceInfo of a Surface without mapping it\r\n"));
	// Open the surface manager

	User::LeaveIfError(iSurfaceManager.Open());
	
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
	
	// Create a surface
	TSurfaceId surfaceId;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));

	
	// Call SurfaceInfo without mapping the surface
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf(); 
	
	//Check if the values are equal to the stored ones
	TEST(info.iSize == attributes.iSize);
	TEST(info.iBuffers == attributes.iBuffers);
	TEST(info.iPixelFormat == attributes.iPixelFormat);
	TEST(info.iStride == attributes.iStride);
	TInt offsetToFirstBuffer;
  	TInt err=iSurfaceManager.GetBufferOffset(surfaceId, 0, offsetToFirstBuffer);
	TEST(offsetToFirstBuffer == attributes.iOffsetToFirstBuffer);
	TEST(info.iContiguous == attributes.iContiguous);
	TEST(info.iCacheAttrib ==attributes.iCacheAttrib);
			
   	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
    // Close the surface manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::TestQuerySurfaceInfoAfterMappingL()
	{
	INFO_PRINTF1(_L("Getting SurfaceInfo of a Surface after mapping it\r\n"));
	// Open the surface manager
	
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
	
	// Create a surface
	TSurfaceId surfaceId;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	// Map the surface 
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId, handle));
	
	// Call SurfaceInfo to get the attributes of the Surface
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	
	// Test that the attributes are returned correctly
	TEST(info.iSize == attributes.iSize);
	TEST(info.iBuffers == attributes.iBuffers);
	TEST(info.iPixelFormat == attributes.iPixelFormat);
	TEST(info.iStride == attributes.iStride);
	TInt offsetToFirstBuffer;
  	TInt err=iSurfaceManager.GetBufferOffset(surfaceId, 0, offsetToFirstBuffer);
	TEST(offsetToFirstBuffer == attributes.iOffsetToFirstBuffer);
	TEST(info.iContiguous == attributes.iContiguous);
	TEST(info.iCacheAttrib ==attributes.iCacheAttrib);
	
	// Close the surface manager
	handle.Close();
 	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  

    iSurfaceManager.Close();
	}

void CTSurfaceManager::TestMapSurfaceL()
	{
	INFO_PRINTF1(_L("Basic test of the MapSurface() method\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
		
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);

	// Create a surface
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	// Map the surface 
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId, handle));
	handle.Close();
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));	
	iSurfaceManager.Close();
	}
	
void CTSurfaceManager::TestClosingSurfaceL()
	{
	INFO_PRINTF1(_L("Closing a surface which cant be open again\r\n"));
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
   	
	// Create a surface
	TSurfaceId surfaceId;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    // Close the surface
   	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
    
    // Try to open the surface using the surface id - should fail
    TEST(KErrArgument == iSurfaceManager.OpenSurface(surfaceId));
    
    // Close the manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::TestSharedChunkClosedL()
	{
	// Create a surface in a new shared chunk
	INFO_PRINTF1(_L("Checking the chunk gets deleted after the surface and handle are both closed\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	// Setup attributes 
	
   	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
		
	// Test create surface doesn't return an error
	TSurfaceId surfaceId;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId)); 
	
	// Get the adress of this chunk of memory
	TUint8* surfaceAdd = handle.Base();
	INFO_PRINTF2(_L("the base address of the chunk is : %x\r\n"),surfaceAdd);
	handle.Close();
	
   	// Close the surface manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::TestMultipleChannelsL()
	{ 
  	INFO_PRINTF1(_L("Testing multiple channels to surface manager LDD in one process and accessing them individually\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
    // Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
	
	// Create a surface
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));

	RSurfaceManager surfaceManagerTwo;
	User::LeaveIfError(surfaceManagerTwo.Open());
	
	TEST(KErrNone == surfaceManagerTwo.OpenSurface(surfaceId));
	iSurfaceManager.Close();
	
	// Call SurfaceInfo to get the attributes of the Surface
	RSurfaceManager::TInfoBuf infoBuf;
	// surfaceId is still accessible as surfaceManagerTwo is open
	TEST(KErrNone == surfaceManagerTwo.SurfaceInfo(surfaceId,infoBuf));
	
	TEST(KErrNone == surfaceManagerTwo.CloseSurface(surfaceId));
	TEST(KErrNone == surfaceManagerTwo.CloseSurface(surfaceId));
	TEST(KErrArgument == surfaceManagerTwo.SurfaceInfo(surfaceId,infoBuf));
	
	// Create a surface 
	TSurfaceId surfaceIdNew;
	TEST(KErrNone == surfaceManagerTwo.CreateSurface(buf, surfaceIdNew));
	TEST(KErrNone == surfaceManagerTwo.CloseSurface(surfaceIdNew));
	
	surfaceManagerTwo.Close();
	} 


void CTSurfaceManager::TestClosingChannelsL()
	{
 	INFO_PRINTF1(_L("Testing the behaviour of closing driver channels\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
    	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
	
	// Create a surface
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));

	RSurfaceManager surfaceManagerTwo;
	User::LeaveIfError(surfaceManagerTwo.Open());
	
	// Create a surface
	TSurfaceId surfaceIdNew;
	TEST(KErrNone == surfaceManagerTwo.CreateSurface(buf, surfaceIdNew));

	TEST(KErrNone == iSurfaceManager.OpenSurface(surfaceId));
	TEST(KErrNone == surfaceManagerTwo.OpenSurface(surfaceId));
	
	TEST(KErrNone == iSurfaceManager.OpenSurface(surfaceIdNew));
	TEST(KErrNone == surfaceManagerTwo.OpenSurface(surfaceIdNew));
	
	iSurfaceManager.Close();
	surfaceManagerTwo.Close();
	
	User::LeaveIfError(iSurfaceManager.Open());
	// Call SurfaceInfo to get the attributes of the Surface
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrArgument == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));

	RSurfaceManager::TInfoBuf infoBufNew;
	TEST(KErrArgument == iSurfaceManager.SurfaceInfo(surfaceIdNew, infoBuf));
	
	iSurfaceManager.Close();
	}


void CTSurfaceManager::TestSurfaceLimitL()
	{
 	INFO_PRINTF1(_L("Testing the number limit of surfaces\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(1,1);
	attributes.iBuffers = 1;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; 
	attributes.iStride = 1;
	attributes.iOffsetToFirstBuffer = 1;
	attributes.iAlignment = 1;

	RSurfaceManager::THintPair hints[2];
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, ETrue);
	attributes.iSurfaceHints = hints;
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Create a surface
	RArray<TSurfaceId> surfaceIdArray;
	TSurfaceId surfaceId;  
	TInt count=0;
	while (iSurfaceManager.CreateSurface(buf, surfaceId)==KErrNone && count < KCountLimit)
		{
		count++;
		if(count == KCountLimit)
			{
			INFO_PRINTF1(_L("Test hits the count, it is used to limit the number of surfaces used in this test"));
			}
		surfaceIdArray.Append(surfaceId);
		}
	INFO_PRINTF2(_L("the maximum number of surface created %d\r\n"),surfaceIdArray.Count());	
	TInt err = iSurfaceManager.CreateSurface(buf, surfaceId);
	INFO_PRINTF2(_L("the final error result is %d\r\n"),err);	

	for(TInt index = 0; index < surfaceIdArray.Count(); ++index)
		{
		TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdArray[index]));
		}	
	surfaceIdArray.Close();	
	iSurfaceManager.Close();
	}  
			
void CTSurfaceManager::TestSynchronizeCacheL()
	{
	INFO_PRINTF1(_L("Test the behaviour of SynchronizeCache()\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
    // Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesL(attributes);
	
	// Create a surface
	TSurfaceId surfaceId;
	// A surface made up of multiple pages
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));

	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncBeforeNonCPUWrite));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncAfterNonCPUWrite));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncBeforeNonCPURead));
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	handle.Close();
	
	//SynchronizeCache when the memory is not contiguous
	attributes.iContiguous = EFalse;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncBeforeNonCPUWrite));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncAfterNonCPUWrite));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncBeforeNonCPURead));
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	handle.Close();
	
	// A surface made up of 1 page
	attributes.iContiguous = ETrue;
	attributes.iSize = TSize(100,10);
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncBeforeNonCPUWrite));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncAfterNonCPUWrite));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncBeforeNonCPURead));
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	handle.Close();
	
	// Test SynchronizeCache to a non-cached surface
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncBeforeNonCPUWrite));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncAfterNonCPUWrite));
	TEST(KErrNone == iSurfaceManager.SynchronizeCache(surfaceId,0,RSurfaceManager::ESyncBeforeNonCPURead));
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	handle.Close();

	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestSharedChunkClosed2L()
	{
	INFO_PRINTF1(_L("Test that when two surfaces are created in the same shared chunk, closing one surface and chunk handle will not cause the chunk to be deleted.\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
    	
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed; 
	attributes.iStride = 1013;
	attributes.iOffsetToFirstBuffer = 1;
	attributes.iAlignment = RSurfaceManager::EPageAligned;
	
	RSurfaceManager::THintPair hints[2];	     // two hint pairs specified
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x124523), 50, ETrue);
	attributes.iSurfaceHints = hints;	
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Create a surface
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	
	// Get the adress of this chunk of memory
	TUint8* surfaceAdd = handle.Base();
	
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
  	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

	attributesNew.iSize = TSize(480,10);
	attributesNew.iBuffers = 2;				// number of buffers in the surface
	attributesNew.iPixelFormat = EUidPixelFormatYUV_422Reversed;		
	attributesNew.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributesNew.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	
	RSurfaceManager::THintPair hint;	     // one hint pairs specified 
	attributes.iHintCount=1;
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);
	attributes.iSurfaceHints = &hint;
	 	
	attributesNew.iAlignment = RSurfaceManager::EPageAligned;
	attributesNew.iOffsetBetweenBuffers = 0;
	attributesNew.iMappable = ETrue;
	
	// Test create surface doesn't return an error
	TSurfaceId surfaceIdNew;
	// Test create surface doesn't return an error
	// For the time being KErrArgument will be returned as the core codes are
	// not ready to check the passed in shared chunk handle.
	TEST(KErrNone == iSurfaceManager.CreateSurface(buff, surfaceIdNew, handle));
  

	// Close the first handle
	handle.Close();
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	
	RChunk handle2;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceIdNew,handle2));
	// Get the adress of this chunk of memory
	TUint8* surfaceAddNew = handle2.Base();
	
	TEST(surfaceAddNew == surfaceAdd);
	
	// Close the second handle
	handle2.Close();
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew));
	iSurfaceManager.Close();	
	}

void CTSurfaceManager::SetAttributesForHintTestL(RSurfaceManager::TSurfaceCreationAttributes& attributesParamsForHintTest)
	{
	attributesParamsForHintTest.iSize = TSize(16,480);
	attributesParamsForHintTest.iBuffers = 2;
	attributesParamsForHintTest.iPixelFormat = EUidPixelFormatYUV_422Reversed; 
	attributesParamsForHintTest.iStride = 50;
	attributesParamsForHintTest.iOffsetToFirstBuffer = 0;
	attributesParamsForHintTest.iAlignment = RSurfaceManager::EPageAligned;
	
	ReAllocHintsArrayL(2);
	attributesParamsForHintTest.iSurfaceHints = iHintsArray;
	attributesParamsForHintTest.iSurfaceHints[0].Set(TUid::Uid(0x124578), 20, ETrue);
	attributesParamsForHintTest.iSurfaceHints[1].Set(TUid::Uid(0x237755), 50, EFalse);
	attributesParamsForHintTest.iHintCount = 2;
	
	attributesParamsForHintTest.iContiguous = ETrue;
	attributesParamsForHintTest.iCacheAttrib = RSurfaceManager::ECached;
	attributesParamsForHintTest.iOffsetBetweenBuffers = 0;
	attributesParamsForHintTest.iMappable = ETrue;
	}
void CTSurfaceManager::TestGetSurfaceHintL()
	{
	INFO_PRINTF1(_L("Test  that GetSurfaceHint() behaves properly.\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
    // Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetAttributesForHintTestL(attributes);
	
	// Create a surface
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	RSurfaceManager::THintPair hintPair;
	hintPair.iKey.iUid = 0x124578;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair));
	TEST(hintPair.iValue == 20);
	TEST(hintPair.iMutable == 1);
	
	RSurfaceManager::THintPair hintPair2;
	hintPair2.iKey.iUid = 0x237755;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	
	TEST(hintPair2.iValue == 50);
	TEST(hintPair2.iMutable == EFalse);
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestSetSurfaceHintL()
	{
	INFO_PRINTF1(_L("Test  that SetSurfaceHint() behaves properly.\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
    	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetAttributesForHintTestL(attributes);
	
	// Create a surface
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	RSurfaceManager::THintPair hintPair;
	hintPair.iKey.iUid = 0x124578;
	hintPair.iValue = 300;
	hintPair.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair));
	
	RSurfaceManager::THintPair hintPair2;
	hintPair2.iKey.iUid = 0x124578;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 300);
	TEST(hintPair2.iMutable == 1);
	// Check the other value unchanged
	hintPair2.iKey.iUid = 0x237755;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 50);
	TEST(hintPair2.iMutable == 0);
	// Add 5 more hint pairs, then set one of them to another value, check all 8 values
	RSurfaceManager::THintPair hintPair1;
	hintPair1.iKey.iUid = 0x124612;
	hintPair1.iValue = 100;
	hintPair1.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair1));
	
	hintPair2.iKey.iUid = 0x124613;
	hintPair2.iValue = 200;
	hintPair2.iMutable = EFalse;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair2));
	
	RSurfaceManager::THintPair hintPair3;
	hintPair3.iKey.iUid = 0x124614;
	hintPair3.iValue = 300;
	hintPair3.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair3));
	
	RSurfaceManager::THintPair hintPair4;
	hintPair4.iKey.iUid = -0x124615;
	hintPair4.iValue = 400;
	hintPair4.iMutable = EFalse;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair4));
	
	RSurfaceManager::THintPair hintPair5;
	hintPair5.iKey.iUid = 0x124616;
	hintPair5.iValue = 500;
	hintPair5.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair5));
	// Set one of them to a new value
	hintPair.iKey.iUid = 0x124578;
	hintPair.iValue = 300;
	hintPair.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair));
	
	// Check all 8 pairs
	hintPair2.iKey.iUid = 0x124578;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 300);
	TEST(hintPair2.iMutable == 1);
	// Check the other value unchanged
	hintPair2.iKey.iUid = 0x237755;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 50);
	TEST(hintPair2.iMutable == 0);
	
	hintPair2.iKey.iUid = 0x124612;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 100);
	TEST(hintPair2.iMutable == 1);
	
	hintPair2.iKey.iUid = 0x124613;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 200);
	TEST(hintPair2.iMutable == 0);
	
	hintPair2.iKey.iUid = 0x124614;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 300);
	TEST(hintPair2.iMutable == 1);
		
	hintPair2.iKey.iUid = -0x124615;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 400);
	TEST(hintPair2.iMutable == 0);
		
	hintPair2.iKey.iUid = 0x124616;
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	TEST(hintPair2.iValue == 500);
	TEST(hintPair2.iMutable == 1);
		
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	iSurfaceManager.Close();
	}

void CTSurfaceManager::CheckHintPair(TSurfaceId& aSurfaceId, RSurfaceManager::THintPair& aHintPairToCheck, RSurfaceManager::THintPair& aHintPairCompareWith)
	{
	TEST(KErrNone == iSurfaceManager.GetSurfaceHint(aSurfaceId, aHintPairToCheck));
	TEST(aHintPairToCheck.iValue == aHintPairCompareWith.iValue);
	TEST(aHintPairToCheck.iMutable == aHintPairCompareWith.iMutable);
	}
void CTSurfaceManager::TestAddSurfaceHintL()
	{
   	INFO_PRINTF1(_L("Test that AddSurfaceHint behaves properly.\r\n"));
   	
   	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	
    // Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(16,480);
	attributes.iBuffers = 2;
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed; 
	attributes.iStride = 50;
	attributes.iOffsetToFirstBuffer = 0;
	attributes.iAlignment = RSurfaceManager::EPageAligned;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	// Create a surface
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	//Add HintPair1
	RSurfaceManager::THintPair hintPair1;
	hintPair1.Set(TUid::Uid(KSurfaceContent), EStillImage, ETrue);
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair1));
	
	RSurfaceManager::THintPair hintPairCheck1;
	hintPairCheck1.iKey.iUid = KSurfaceContent;
	CheckHintPair(surfaceId, hintPairCheck1, hintPair1);
	
	//Add HintPair2
	RSurfaceManager::THintPair hintPair2;
	hintPair2.iKey.iUid = KSurfaceProtection;
	hintPair2.iValue = EAllowAnalog | EAllowDigital;
	hintPair2.iMutable = EFalse;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair2));
		
	RSurfaceManager::THintPair hintPairCheck2;
	hintPairCheck2.iKey.iUid = KSurfaceProtection;
	CheckHintPair(surfaceId, hintPairCheck2, hintPair2);
	CheckHintPair(surfaceId, hintPairCheck1, hintPair1);

	//Add HintPair3
	RSurfaceManager::THintPair hintPair3;
	hintPair3.Set(TUid::Uid(KSurfaceCharacteristics), ENotPersistable, EFalse);
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair3));
	
	RSurfaceManager::THintPair hintPairCheck3;
	hintPairCheck3.iKey.iUid = KSurfaceCharacteristics;
	CheckHintPair(surfaceId, hintPairCheck3, hintPair3);
	CheckHintPair(surfaceId, hintPairCheck2, hintPair2);
	CheckHintPair(surfaceId, hintPairCheck1, hintPair1);
	
	//Add HintPair4
	RSurfaceManager::THintPair hintPair4;
	hintPair4.iKey.iUid = 0x125678;
	hintPair4.iValue = 150;
	hintPair4.iMutable = EFalse;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair4));
		
	RSurfaceManager::THintPair hintPairCheck4;
	hintPairCheck4.iKey.iUid = 0x125678;
	CheckHintPair(surfaceId, hintPairCheck4, hintPair4);
	CheckHintPair(surfaceId, hintPairCheck3, hintPair3);
	CheckHintPair(surfaceId, hintPairCheck2, hintPair2);
	CheckHintPair(surfaceId, hintPairCheck1, hintPair1);

	//Add HintPair5
	RSurfaceManager::THintPair hintPair5;
	hintPair5.iKey.iUid = 0x233506;
	hintPair5.iValue = 270;
	hintPair5.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair5));
	
	RSurfaceManager::THintPair hintPairCheck5;
	hintPairCheck5.iKey.iUid = 0x233506;
	CheckHintPair(surfaceId, hintPairCheck5, hintPair5);
	CheckHintPair(surfaceId, hintPairCheck4, hintPair4);
	CheckHintPair(surfaceId, hintPairCheck3, hintPair3);
	CheckHintPair(surfaceId, hintPairCheck2, hintPair2);
	CheckHintPair(surfaceId, hintPairCheck1, hintPair1);

	//Add HintPair6
	RSurfaceManager::THintPair hintPair6;
	hintPair6.iKey.iUid = -0x128899;
	hintPair6.iValue = 310;
	hintPair6.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair6));

	RSurfaceManager::THintPair hintPairCheck6;
	hintPairCheck6.iKey.iUid = -0x128899;
	CheckHintPair(surfaceId, hintPairCheck6, hintPair6);
	CheckHintPair(surfaceId, hintPairCheck5, hintPair5);
	CheckHintPair(surfaceId, hintPairCheck4, hintPair4);
	CheckHintPair(surfaceId, hintPairCheck3, hintPair3);
	CheckHintPair(surfaceId, hintPairCheck2, hintPair2);
	CheckHintPair(surfaceId, hintPairCheck1, hintPair1);

	//Add HintPair7
	RSurfaceManager::THintPair hintPair7;
	hintPair7.iKey.iUid = 0x225377;
	hintPair7.iValue = 70;
	hintPair7.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair7));
	
	RSurfaceManager::THintPair hintPairCheck7;
	hintPairCheck7.iKey.iUid = 0x225377;
	CheckHintPair(surfaceId, hintPairCheck7, hintPair7);
	CheckHintPair(surfaceId, hintPairCheck6, hintPair6);
	CheckHintPair(surfaceId, hintPairCheck5, hintPair5);
	CheckHintPair(surfaceId, hintPairCheck4, hintPair4);
	CheckHintPair(surfaceId, hintPairCheck3, hintPair3);
	CheckHintPair(surfaceId, hintPairCheck2, hintPair2);
	CheckHintPair(surfaceId, hintPairCheck1, hintPair1);
	
	//Add HintPair8
	RSurfaceManager::THintPair hintPair8;
	hintPair8.iKey.iUid = -0x257936;
	hintPair8.iValue = 560;
	hintPair8.iMutable = EFalse;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair8));
	
	RSurfaceManager::THintPair hintPairCheck8;
	hintPairCheck8.iKey.iUid = -0x257936;
	CheckHintPair(surfaceId, hintPairCheck8, hintPair8);
	CheckHintPair(surfaceId, hintPairCheck7, hintPair7);
	CheckHintPair(surfaceId, hintPairCheck6, hintPair6);
	CheckHintPair(surfaceId, hintPairCheck5, hintPair5);
	CheckHintPair(surfaceId, hintPairCheck4, hintPair4);
	CheckHintPair(surfaceId, hintPairCheck3, hintPair3);
	CheckHintPair(surfaceId, hintPairCheck2, hintPair2);
	CheckHintPair(surfaceId, hintPairCheck1, hintPair1);


	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	iSurfaceManager.Close();
	}

void CTSurfaceManager::SetupAttributesForNegativeTestL(RSurfaceManager::TSurfaceCreationAttributes& attributesForNegativeTest)
	{
	attributesForNegativeTest.iSize = TSize(480,16);
	attributesForNegativeTest.iBuffers = 2;				// number of buffers in the surface
	attributesForNegativeTest.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributesForNegativeTest.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributesForNegativeTest.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributesForNegativeTest.iAlignment = 32; 
	
	// two hint pairs specified
	ReAllocHintsArrayL(2);
	attributesForNegativeTest.iSurfaceHints = iHintsArray;
	attributesForNegativeTest.iHintCount=2;
	attributesForNegativeTest.iSurfaceHints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	attributesForNegativeTest.iSurfaceHints[1].Set(TUid::Uid(0x124523), 30, EFalse);
	
	attributesForNegativeTest.iContiguous = ETrue;	
	attributesForNegativeTest.iCacheAttrib = RSurfaceManager::ECached;
	attributesForNegativeTest.iOffsetBetweenBuffers = 0;
	attributesForNegativeTest.iMappable = ETrue;
	}

void CTSurfaceManager::TestCreateSurfaceInvalidParamsNewChunkL()
	{
   	INFO_PRINTF1(_L("Creating a surface with invalid parameters in a new shared chunk\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	// Setup valid attributes initially
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesForNegativeTestL(attributes);
	
	TSurfaceId surfaceId;
	// Test create surface doesn't return an error
	TInt err = iSurfaceManager.CreateSurface(buf, surfaceId);
	TEST(KErrNone == err);
	if (err == KErrNone)
		{		
		// No need to close the surface as it is not created properly.
		// Test with negative width
		attributes.iSize.iWidth = -2;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iSize.iWidth = 480; // reset width
		// Test with negative height
		attributes.iSize.iHeight = -56;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iSize.iHeight = 16; // reset
			
		// Test with invalid buffers
		attributes.iBuffers = 0; // no buffers
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iBuffers = -5; // negative buffers
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iBuffers = 2; // reset
			
		// Test with invalid stride
		attributes.iStride = -200; // negative
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iStride = 0; //smaller than required
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iStride = 1013; //reset
			
		//Test with invalid offset
		attributes.iOffsetToFirstBuffer = -14; // negative
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));

		attributes.iOffsetToFirstBuffer = 2147483647;	// invalid offset
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iOffsetToFirstBuffer = 0; // restore
			
		// Test with invalid alignment (valid parameters: 1:2:4:8)
		attributes.iAlignment = 0;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iAlignment = 3;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iAlignment = 5;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iAlignment = 12;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iAlignment = -2;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iAlignment = 32; // restore
			
		//Test with invalid iOffsetBetweenBuffers
		attributes.iOffsetBetweenBuffers = 50 ; //smaller than required
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));

		attributes.iOffsetBetweenBuffers = -5; // negative offset
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
		attributes.iOffsetBetweenBuffers = 0;
			
		//Test invalid surface hint
		TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
		RSurfaceManager::THintPair hintPair;
		hintPair.iKey.iUid = 0;
		hintPair.iValue = 30;
		hintPair.iMutable = ETrue;
		TEST(KErrArgument == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair));
		}
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestCreateSurfaceInvalidParamsExisitingChunkL()
	{
  	INFO_PRINTF1(_L("Creating a surface with invalid parameters in the existing shared chunk\r\n"));
  	
  	// Call another device driver to get a valid shared chunk handle
  	RSharedChunkLdd device;
	RChunk handle;
	TUint ChunkSize = 40960;	// bottom 8 bits reserved space for other chunk creation attributes info
	TUint ChunkAttribs = ChunkSize|EMultiple|EOwnsMemory;
	TUint ContiguousAttrib = 1;
	GetSharedChunkHandleL(device, handle, ChunkSize, ChunkAttribs, ContiguousAttrib);
  	
  	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	// Setup valid attributes initially
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesForNegativeTestL(attributes);

	TSurfaceId surfaceId;
	// Test create surface doesn't return an error
	TInt err = iSurfaceManager.CreateSurface(buf, surfaceId, handle);
	TEST(KErrNone == err);
	if (err == KErrNone)
		{
		// No need to close the surface as it is not created properly.
		// Test with negative width
		attributes.iSize.iWidth = -2;
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iSize.iWidth = 480; // reset width
		// 12.2: Test with negative height
		attributes.iSize.iHeight = -56;
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iSize.iHeight = 16; // reset
			
		//Test with invalid buffers
		attributes.iBuffers = 0; // no buffers
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iBuffers = -5; // negative buffers
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iBuffers = 2; // reset
		
		//Test with invalid stride
		attributes.iStride = -200; // negative
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
	   	attributes.iStride = 0; //smaller than required
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
	   	attributes.iStride = 1013; //reset
	   	
	   	//Test with invalid offset
	   	attributes.iOffsetToFirstBuffer = -14; //negative offset
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));	
	   	attributes.iOffsetToFirstBuffer = 5; //not divisable by iAlignment
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iOffsetToFirstBuffer = 2147483647; //invalid setting
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
	   	attributes.iOffsetToFirstBuffer = 0;
	   	
	   	//Test with invalid alignment (valid parameters: 1:2:4:8)
		attributes.iAlignment = 0;
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iAlignment = 3;
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iAlignment = 5;
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iAlignment = 12;
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iAlignment = -2;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iAlignment = 32; // restore
		
		attributes.iOffsetBetweenBuffers = attributes.iSize.iHeight*attributes.iStride -1 ; //smaller than required
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId,handle));
		attributes.iOffsetBetweenBuffers = 20010;  // not devisible by alignment
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iOffsetBetweenBuffers = -5; // negative offset
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
	   	attributes.iOffsetBetweenBuffers = 50000; // iOffsetBetweenBuffers is too big
	   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		attributes.iOffsetBetweenBuffers = 0;		
		
		//Test invalid surface hint
		TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		RSurfaceManager::THintPair hintPair;
		hintPair.iKey.iUid = 0;
		hintPair.iValue = 30;
		hintPair.iMutable = ETrue;
		TEST(KErrArgument == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair));
		
		//Reset the alignment is page aligned, if iOffsetToFirstBuffer or iOffsetBetweenBuffers is not page aligned, then the surface can't be created
		attributes.iAlignment = RSurfaceManager::EPageAligned;
		attributes.iOffsetToFirstBuffer = 4092;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		    
		attributes.iOffsetToFirstBuffer = 4096;
		attributes.iOffsetBetweenBuffers = 4092;
		TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId, handle));
		}
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	device.CloseChunkHandle(handle);
	device.CloseChunk();
	iSurfaceManager.Close();
	device.Close();
	}

void CTSurfaceManager::TestCreateSurfaceInvalidParamsExisitingChunk2L()
	{
  	INFO_PRINTF1(_L("Creating a surface with invalid parameters in the existing shared chunk which already contains a surface\r\n"));
	
  	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributesOld = buf();
	
	SetupAttributesForNegativeTestL(attributesOld);
	
	TSurfaceId surfaceIdOld;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdOld));
   	
   	//Map the surface in the current process
   	RChunk handle;
   	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceIdOld,handle));
   	
	// Setup valid attributes initially
	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buff();
	
	SetupAttributesForNegativeTestL(attributes);
	
	TSurfaceId surfaceId;
	// No need to close the surface as it is not created properly.
	// Test with negative width
	attributes.iSize.iWidth = -2;
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iSize.iWidth = 480; // reset width
	// 12.2: Test with negative height
	attributes.iSize.iHeight = -56;
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iSize.iHeight = 16; // reset
	
	//Test with invalid buffers
	attributes.iBuffers = 0; // no buffers
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iBuffers = -5; // negative buffers
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iBuffers = 1; // reset
	
	//Test with invalid stride
	attributes.iStride = -200; // negative
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iStride = 1040; // restore
	
	//Test with invalid offset
	attributes.iOffsetToFirstBuffer = -14; // negative
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iOffsetToFirstBuffer = 5; // not divisible by iAlignment
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iOffsetToFirstBuffer = 2147483647;	// invalid offset
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iOffsetToFirstBuffer = 100; // restore
	
	//Test with invalid alignment (valid parameters: 1:2:4:8)
	attributes.iAlignment = 0;
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iAlignment = 3;
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iAlignment = 5;
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iAlignment = 12;
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iAlignment = -2;
	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));	
	attributes.iAlignment = 32; // restore
	
	//Test with invalid iOffsetBetweenBuffers
	attributes.iOffsetBetweenBuffers = attributes.iSize.iHeight*attributes.iStride -1 ; //smaller than required
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iOffsetBetweenBuffers = attributes.iAlignment +1 ; // 1 (iAlignment  must not be set to 1) // not devisible by alignment
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iOffsetBetweenBuffers = -5; // negative offset
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	attributes.iOffsetBetweenBuffers = 0;

	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdOld));
	handle.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestDuplicatedHintKeyL()
	{
 	INFO_PRINTF1(_L("Test that Hint keys cant not be duplicated\r\n"));
	
  	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;	
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
		
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	attributes.iAlignment = RSurfaceManager::EPageAligned;			// alignment, 1,2,4,8 byte aligned
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	//Set duplicated Hint keys
	RSurfaceManager::THintPair hints[2];	     // two hint pairs specified
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x124578), 50, ETrue);
	attributes.iSurfaceHints = hints;
	
	TSurfaceId surfaceId;
   	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
	iSurfaceManager.Close();
	}
void CTSurfaceManager::TestInvalidContiguousPropertyL()
	{
	//This test is only valid on hardware
 	INFO_PRINTF1(_L("Test that iContiguous is indeed ignored when creating the second surface on the existing shared chunk which already contains a surface\r\n"));
  	
  	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributesOld = buf();
	
	SetupAttributesForNegativeTestL(attributesOld);
	
	TSurfaceId surfaceIdOld;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdOld));
   	RChunk handle;
   	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceIdOld,handle));
   	
	// Setup valid attributes initially
	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buff();
	
	SetupAttributesForNegativeTestL(attributes);
	//Reset iContiguous
	attributes.iContiguous = EFalse;
	
	TSurfaceId surfaceId;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf(); 
	
	//Check the iContiguous is identical to that of the first surface info structure
	TEST(info.iContiguous == 1);
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdOld));
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	handle.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestInvalidCachePropertyL()
	{
	//This test is only valid on hardware
	INFO_PRINTF1(_L("Test that iCacheAttrib is indeed ignored when creating the second surface on the existing shared chunk which already contains a surface\r\n"));
  	
  	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributesOld = buf();
	
	SetupAttributesForNegativeTestL(attributesOld);
	
	TSurfaceId surfaceIdOld;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdOld));
   	RChunk handle;
   	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceIdOld,handle));
   	
	// Setup valid attributes initially
	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buff();
	
	SetupAttributesForNegativeTestL(attributes);	
	attributes.iCacheAttrib = RSurfaceManager::ENotCached;
	
	TSurfaceId surfaceId;
   	TEST(KErrNone == iSurfaceManager.CreateSurface(buff, surfaceId,handle));
	RSurfaceManager::TInfoBuf infoBuf;
	TEST(KErrNone == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));
	RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
	
	//cache attribute is not valid on the emulator
	INFO_PRINTF2(_L("cache attribute is: %d"), info.iCacheAttrib);
#ifndef __WINS__
	TEST(info.iCacheAttrib == RSurfaceManager::ECached);
	INFO_PRINTF2(_L("cache attribute is: %d"), info.iCacheAttrib);
#endif	
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdOld));
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	handle.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestInvalidSharedChunkL()
	{
	INFO_PRINTF1(_L("Creating a Surface in an invalid chunk\r\n"));
  	
  	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
		
	attributes.iSize = TSize(480,16);
	attributes.iBuffers = 2;				// number of buffers in the surface
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributes.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributes.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data			
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iAlignment = 4;
	attributes.iMappable = ETrue;
	
	ReAllocHintsArrayL(2);
	attributes.iHintCount=2;
	iHintsArray[0].Set(TUid::Uid(0x124578), 25, ETrue);
	iHintsArray[1].Set(TUid::Uid(0x124523), 30, EFalse);
	attributes.iSurfaceHints = iHintsArray;  
	
	
	//Call another device driver to get another shared chunk handle
	RChunk handle;
	TSurfaceId surfaceId; 
	
	//Test when the chunk handle is NULL, the surface can't be created
	TEST(KErrBadHandle == iSurfaceManager.CreateSurface(buf, surfaceId,handle));
	handle.Close();
	
	//Test when the chunk is not shared chunk, the surface can't be created
	RChunk handle2;
	const TInt KCommitSize = 32768;
	const TInt KMaxSize = 40960;
	User::LeaveIfError(handle2.CreateLocal(KCommitSize, KMaxSize));
	TEST(KErrBadHandle == iSurfaceManager.CreateSurface(buf, surfaceId,handle2));
	handle2.Close();

	//Test when the chunk is created by the device driver but has the chunk type ESharedKernelSingle, the surface can't be created
	RSharedChunkLdd device;
	RChunk handle3;
	TUint ChunkSize = 40960;	// bottom 8 bits reserved space for other chunk creation attributes info
	TUint ChunkAttribs = ChunkSize|ESingle|EOwnsMemory;
	TUint ContiguousAttrib = 1;
	GetSharedChunkHandleL(device, handle3, ChunkSize, ChunkAttribs, ContiguousAttrib);	
	TEST(KErrBadHandle == iSurfaceManager.CreateSurface(buf, surfaceId,handle3));
	device.CloseChunkHandle(handle3);
	device.CloseChunk();
	
	//Test when the chunk is created by the device driver but the chunk size in bytes is too small for the new surfaces, then the surface can't be created
	RChunk handle4;
	//The chunk size is too small for the new surface
	ChunkSize = 4096;	// bottom 8 bits reserved space for other chunk creation attributes info
	ChunkAttribs = ChunkSize|EMultiple|EOwnsMemory;	
	TEST(KErrNone==device.CreateChunk(ChunkAttribs));
	TEST(KErrNone==device.CommitMemory(0, ChunkSize));
	TEST(KErrNone==device.GetChunkHandle(handle4));
  	
	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId,handle4));
	device.CloseChunkHandle(handle4);
	device.CloseChunk();
	
	//Test when the chunk is created by the device driver but the chunk memory is not commited, then the surface can't be created 
	RChunk handle5;
	ChunkSize = 40960;	// bottom 8 bits reserved space for other chunk creation attributes info
	ChunkAttribs = ChunkSize|EMultiple|EOwnsMemory;
	TEST(KErrNone==device.CreateChunk(ChunkAttribs));
	TEST(KErrNone==device.GetChunkHandle(handle5));
  	
	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId,handle5));
	
	device.CloseChunkHandle(handle5);
	device.CloseChunk();
	iSurfaceManager.Close();
	device.Close();
	}

void CTSurfaceManager::TestCreateVeryLargeSurfacesL()
	{
	INFO_PRINTF1(_L("Creating lots of surfaces\r\n"));
	// Open the surface manager

	User::LeaveIfError(iSurfaceManager.Open());
	// Setup valid attributes initially
    RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	TSurfaceId surfaceId;	
	attributes.iSize = TSize(5000,5000);    // w > 0, h > 0
	attributes.iBuffers = 5; 				// > 0
	attributes.iPixelFormat = EUidPixelFormatARGB_1555;	// 2bpp
	attributes.iStride = 100000;				// > 0, > width * bpp
	attributes.iOffsetToFirstBuffer = 1024;	// > 0, divisible by alignment
	attributes.iAlignment = RSurfaceManager::EPageAligned;				// 1 || 2 || 4 || 8
	attributes.iMappable = ETrue;
	
	RSurfaceManager::THintPair hints[2];	     // two hint pairs specified
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x124523), 30, EFalse);
	attributes.iSurfaceHints = hints;	
	
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	const TInt numAttempts = 10;
	TInt ii = 0;
	TInt ret = KErrNone;
	do
		{
		ret = iSurfaceManager.CreateSurface(buf, surfaceId);
		if(ret != KErrNone)
			break;
		}
	while(ii < numAttempts);
	
	// If we create 10 surfaces, we should get KErrNoMemory
	TEST(ii != numAttempts);
	TEST(KErrArgument == ret);
	
	attributes.iSize = TSize(5000, 25000);
	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	attributes.iSize = TSize(5000, 4294);
	attributes.iOffsetToFirstBuffer = 483613;
	attributes.iAlignment = 32;
	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestMapSurfaceInvalidParamsL()
	{
   	INFO_PRINTF1(_L("Testing the MapSurface method with invalid surfaceId\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesForNegativeTestL(attributes);	
	
	// Create a surface
	TSurfaceId surfaceId;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	
	// Set an invalid surfaceId 
	TSurfaceId invalidSurfaceId = surfaceId;
	// Change the surfaceId type to EInvalidSurface
	invalidSurfaceId.iInternal[3] = (surfaceId.iInternal[3] & 0x00FFFFFF) | ( TSurfaceId::EInvalidSurface <<24 ) ;
	 
	RChunk handle;
	// Map the surface 
	TEST(KErrArgument == iSurfaceManager.MapSurface(invalidSurfaceId, handle));
	handle.Close();	
   	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestSurfaceInfoChangedTypeL()
	{
   	INFO_PRINTF1(_L("Calling SurfaceInfo on surfaceId with a changed 'type'\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(361,341);    	// w > 0, h > 0
	attributes.iBuffers = 4; 				// > 0, <= 4
	attributes.iPixelFormat = EUidPixelFormatYUV_422Planar;	// 2bpp
	attributes.iStride = 1400;				// > 0,  width * bpp
	attributes.iOffsetToFirstBuffer = 184;	// > 0, divisible by alignment
	attributes.iAlignment = 4;				// 1 || 2 || 4 || 8
	attributes.iContiguous = EFalse;
	attributes.iMappable = ETrue;
	
	RSurfaceManager::THintPair hints[2];	     // two hint pairs specified
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 20, ETrue);
	hints[1].Set(TUid::Uid(0x237755), 50, EFalse);
	attributes.iSurfaceHints = hints;
	
	attributes.iOffsetBetweenBuffers = 0;
	
	// Create a surface
	TSurfaceId surfaceId;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    
	// Map the surface 
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId, handle));
	handle.Close();

	TSurfaceId surfaceIdBackup = surfaceId;
	// Call SurfaceInfo to get the attributes of the Surface
	RSurfaceManager::TInfoBuf infoBuf;
	// Want to change the type - replace it with twice the value of the old type
	TInt shiftedType = surfaceId.Type() << 25;
	// Mask out the top 8 bits and replace with the new type
	surfaceId.iInternal[3] = (surfaceId.iInternal[3] & 0x00FFFFFF) + shiftedType;

	TEST(KErrArgument == iSurfaceManager.SurfaceInfo(surfaceId, infoBuf));

	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdBackup));	
	// Close the surface manager
    iSurfaceManager.Close();
	}

void CTSurfaceManager::TestCloseSurfaceInvalidParamsL()
	{
   	INFO_PRINTF1(_L("Closing a surface with invalid parameters\r\n"));
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesForNegativeTestL(attributes);	

	// Create a surface - increments reference count by 1
	TSurfaceId surfaceId;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    // Set a negative surfaceId
    TSurfaceId invalidSurfaceId;
    invalidSurfaceId = surfaceId;
	// Change the surfaceId type to EInvalidSurface
	invalidSurfaceId.iInternal[3] = (surfaceId.iInternal[3] & 0x00FFFFFF) | ( TSurfaceId::EInvalidSurface <<24 ) ;
    TEST(KErrArgument == iSurfaceManager.CloseSurface(invalidSurfaceId));

   	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	iSurfaceManager.Close();
	}


void CTSurfaceManager::TestSynchronizeCacheInvalidParamsL()
	{
	INFO_PRINTF1(_L("Test that SynchronizeCache() reacts properly under false conditions\r\n"));
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	SetupAttributesForNegativeTestL(attributes);			//2 buffers in this surface
	
	// Create a surface - increments reference count by 1
	TSurfaceId surfaceId;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    
	//Close the surface
    TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));  
	
	TEST(KErrArgument == iSurfaceManager.SynchronizeCache(surfaceId,1,RSurfaceManager::ESyncBeforeNonCPURead));
    
	TSurfaceId surfaceIdNew;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdNew));
    // Check invalid operation
	TEST(KErrArgument == iSurfaceManager.SynchronizeCache(surfaceIdNew,1,(RSurfaceManager::TSyncOperation)4));
	// Check invalid buffer number
	TEST(KErrArgument == iSurfaceManager.SynchronizeCache(surfaceIdNew, attributes.iBuffers, RSurfaceManager::ESyncBeforeNonCPURead));
	// Check invalid buffer number
	TEST(KErrArgument == iSurfaceManager.SynchronizeCache(surfaceIdNew,KMaxTInt,RSurfaceManager::ESyncBeforeNonCPURead));
	// Check negative buffer number
	TEST(KErrArgument == iSurfaceManager.SynchronizeCache(surfaceIdNew,-1,RSurfaceManager::ESyncBeforeNonCPURead));

	// Change the surfaceId type to EInvalidSurface
	TSurfaceId surfaceIdBackup = surfaceIdNew;
	surfaceIdNew.iInternal[3] = (surfaceId.iInternal[3] & 0x00FFFFFF) | ( TSurfaceId::EInvalidSurface <<24 ) ;
	TEST(KErrArgument == iSurfaceManager.SynchronizeCache(surfaceIdNew,1,RSurfaceManager::ESyncBeforeNonCPURead));
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdBackup));  
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestCloseSurfaceSameIDTwiceL()
	{
  	INFO_PRINTF1(_L("Closing a surface with the same ID twice\r\n"));
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(150,412);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatARGB_1555; // 2bpp
	attributes.iStride = 301;
	attributes.iOffsetToFirstBuffer = 5;
	attributes.iAlignment = 1;
	attributes.iContiguous=EFalse;
	attributes.iMappable = ETrue;
	
	RSurfaceManager::THintPair hints[2];	     // two hint pairs specified
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x124523), 50, EFalse);
	attributes.iSurfaceHints = hints;	

	attributes.iOffsetBetweenBuffers = 0;

	// Create a surface - increments reference count by 1
	TSurfaceId surfaceId;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));		
    
    // Close the same surface twice
  	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	TEST(KErrArgument == iSurfaceManager.CloseSurface(surfaceId));
	
	iSurfaceManager.Close();
    }
	
void CTSurfaceManager::TestCreateSurfaceOomL()
	{
  	INFO_PRINTF1(_L("Testing CreateSurface in out of memory conditions\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(280,301);
	attributes.iBuffers = 3;
	attributes.iPixelFormat = EUidPixelFormatYUV_422SemiPlanar; // 2bpp
	attributes.iStride = 710;
	attributes.iOffsetToFirstBuffer = 4;
	attributes.iAlignment = 4;
	attributes.iContiguous=EFalse;
	attributes.iMappable = ETrue;
	
	RSurfaceManager::THintPair hints[2];	     // two hint pairs specified
	attributes.iHintCount=2;
	hints[0].Set(TUid::Uid(0x124578), 25, ETrue);
	hints[1].Set(TUid::Uid(0x124523), 50, EFalse);
	attributes.iSurfaceHints = hints;	

	attributes.iOffsetBetweenBuffers = 0;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	// Create a surface
	TSurfaceId surfaceId;
	
	// There are 6 kernal allocs, so test that KErrNoMemory is returned if we fail at each
	const TInt numKernalAllocs = 6;
	for(TInt ii = numKernalAllocs; ii > 0; --ii)
		{
		__KHEAP_SETFAIL(RHeap::EDeterministic, ii);
		TEST(KErrNoMemory == iSurfaceManager.CreateSurface(buf, surfaceId));
		}
	__KHEAP_RESET;
	
	// Test create surface doesn't return an error
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));

	//Map the surface in the current processs
	RChunk handle;
	TEST(KErrNone == iSurfaceManager.MapSurface(surfaceId,handle));
	
	// Cache, Contiguous and Alignment attributes are ignored for the already existing chunks
  	RSurfaceManager::TSurfaceCreationAttributesBuf buff;
	RSurfaceManager::TSurfaceCreationAttributes& attributesNew = buff();

	attributesNew.iSize = TSize(480,16);
	attributesNew.iBuffers = 2;				// number of buffers in the surface
	attributesNew.iPixelFormat = EUidPixelFormatYUV_422Reversed;		// 2bpp
	attributesNew.iStride = 1013;				// Number of bytes between start of one line and start of next
	attributesNew.iOffsetToFirstBuffer = 0;	// way of reserving space before the surface pixel data
	
	hints[0].Set(TUid::Uid(0x124545), 50, EFalse);	// two hint pairs specified

	attributesNew.iAlignment = 8;
	attributesNew.iOffsetBetweenBuffers = 0;
	attributesNew.iMappable = ETrue;

	// Test create surface doesn't return an error
	TSurfaceId surfaceIdNew;
	
	__KHEAP_SETFAIL(RHeap::EDeterministic, 1);
	TEST(KErrNoMemory == iSurfaceManager.CreateSurface(buff, surfaceIdNew, handle));
	__KHEAP_RESET;
	
	handle.Close();
	iSurfaceManager.Close();
	}

void CTSurfaceManager::SetAttributesForNegativeHintTestL(RSurfaceManager::TSurfaceCreationAttributes& attributesForNegativeHintTest)
	{
	attributesForNegativeHintTest.iSize = TSize(16,480);
	attributesForNegativeHintTest.iBuffers = 2;
	attributesForNegativeHintTest.iPixelFormat = EUidPixelFormatYUV_422Reversed; // 2bpp
	attributesForNegativeHintTest.iStride = 50;
	attributesForNegativeHintTest.iOffsetToFirstBuffer = 0;
	attributesForNegativeHintTest.iAlignment = RSurfaceManager::EPageAligned;
	ReAllocHintsArrayL(2);
	attributesForNegativeHintTest.iSurfaceHints = iHintsArray;
	attributesForNegativeHintTest.iHintCount = 2;
	attributesForNegativeHintTest.iSurfaceHints[0].Set(TUid::Uid(0x124578),20, ETrue);
	attributesForNegativeHintTest.iSurfaceHints[1].Set(TUid::Uid(0x237755),50, EFalse);
	attributesForNegativeHintTest.iContiguous = ETrue;
	attributesForNegativeHintTest.iCacheAttrib = RSurfaceManager::ECached;
	attributesForNegativeHintTest.iOffsetBetweenBuffers = 0;
	attributesForNegativeHintTest.iMappable = ETrue;
	}

void CTSurfaceManager::TestGetSurfaceHintInFalseConditionsL()
	{
	INFO_PRINTF1(_L("GetSurfaceHint()reacts properly under false conditions\r\n"));
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(16,480);
	attributes.iBuffers = 2;
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed; 
	attributes.iStride = 50;
	attributes.iOffsetToFirstBuffer = 0;
	attributes.iAlignment = RSurfaceManager::EPageAligned;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	// No hint pair
	RSurfaceManager::THintPair hintPair2;
	hintPair2.iKey.iUid = 0x123257;
	TEST(KErrArgument == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair2));
	
	
	SetAttributesForNegativeHintTestL(attributes);

    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    
    RSurfaceManager::THintPair hintPair;
    //Test GetSurfaceHint() returns KErrArgument if the hint pair has 0 UID
    hintPair.iKey.iUid = 0;
    TEST(KErrArgument == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair));
    
	hintPair.iKey.iUid = 0x124578;
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	TEST(KErrArgument == iSurfaceManager.GetSurfaceHint(surfaceId, hintPair));
	
	TSurfaceId surfaceIdNew;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdNew));
	
	//Convert the type of this surfaceId to EInvalidSurface
	TSurfaceId surfaceIdTemp;
	surfaceIdTemp = surfaceIdNew;
	// Change the surfaceId type to EInvalidSurface
	surfaceIdNew.iInternal[3] = (surfaceIdNew.iInternal[3] & 0x00FFFFFF) | ( TSurfaceId::EInvalidSurface <<24 ) ;
	TEST(KErrArgument == iSurfaceManager.GetSurfaceHint(surfaceIdNew, hintPair));
	
	//Change the surfaceId back to valid one and find a new THintPair with no keys defined
	hintPair2.iKey.iUid = 0x166778;
	surfaceIdNew = surfaceIdTemp;
	TEST(KErrArgument == iSurfaceManager.GetSurfaceHint(surfaceIdNew, hintPair2));
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew));
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestSetSurfaceHintInFalseConditionsL()
	{
	INFO_PRINTF1(_L("SetSurfaceHint()reacts properly under false conditions\r\n"));
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(16,480);
	attributes.iBuffers = 2;
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed; 
	attributes.iStride = 50;
	attributes.iOffsetToFirstBuffer = 0;
	attributes.iAlignment = RSurfaceManager::EPageAligned;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	// No hint pair, Setting a null UID hint pair still returns KErrArgument
	RSurfaceManager::THintPair hintPair2;
	hintPair2.iKey.iUid = 0;
	TEST(KErrArgument == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair2));
	
	
	SetAttributesForNegativeHintTestL(attributes);
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    
    //Set a THintPair with a key value set to 0x124578 and value set to 300
    RSurfaceManager::THintPair hintPair;
	hintPair.iKey.iUid = 0x124578;
	hintPair.iValue = 300;
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	
	TEST(KErrArgument == iSurfaceManager.SetSurfaceHint(surfaceId, hintPair));
	
	TSurfaceId surfaceIdNew;
    TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceIdNew));
	
	//Convert the type of this surfaceId to EInvalidSurface
	TSurfaceId surfaceIdTemp;
	surfaceIdTemp = surfaceIdNew;
	// Change the surfaceId type to EInvalidSurface
	surfaceIdNew.iInternal[3] = (surfaceIdNew.iInternal[3] & 0x00FFFFFF) | ( TSurfaceId::EInvalidSurface <<24 ) ;
	TEST(KErrArgument == iSurfaceManager.SetSurfaceHint(surfaceIdNew, hintPair));
	
	//Change the surfaceId back to valid one and try to access a hint pair which is not mutable
	hintPair2.iKey.iUid = 0x237755;
	hintPair2.iValue = 300;
	surfaceIdNew = surfaceIdTemp;
	TEST(KErrNone == iSurfaceManager.OpenSurface(surfaceIdNew));
	TEST(KErrAccessDenied == iSurfaceManager.SetSurfaceHint(surfaceIdNew, hintPair2));
	
	//find a new THintPair with no keys defined
	RSurfaceManager::THintPair hintPair3;
	hintPair3.iValue = 300;
	TEST(KErrArgument == iSurfaceManager.SetSurfaceHint(surfaceIdNew, hintPair3));
		
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceIdNew));
	iSurfaceManager.Close();
	}

void CTSurfaceManager::TestAddSurfaceHintInFalseConditionsL()
	{
	INFO_PRINTF1(_L("AddSurfaceHint()reacts properly under false conditions\r\n"));
	
	User::LeaveIfError(iSurfaceManager.Open());
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	attributes.iSize = TSize(16,480);
	attributes.iBuffers = 2;
	attributes.iPixelFormat = EUidPixelFormatYUV_422Reversed; 
	attributes.iStride = 50;
	attributes.iOffsetToFirstBuffer = 0;
	attributes.iAlignment = RSurfaceManager::EPageAligned;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iMappable = ETrue;
	
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	// No hint pair, adding a null UID hint pair still returns KErrArgument
	RSurfaceManager::THintPair hintPair2;
	hintPair2.iKey.iUid = 0;
	TEST(KErrArgument == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair2));
	
	
	SetAttributesForNegativeHintTestL(attributes);
	
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
    
    //Set a THintPair with a key value set to 0x124578 and value set to 300
    RSurfaceManager::THintPair hintPair;
	
	//Test AddSurfaceHint() returns KErrArgument if the hint pair has 0 UID
	hintPair.iKey.iUid = 0;
	TEST(KErrArgument == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair));
	hintPair.iKey.iUid = 0x124256;	//restore

	//Test AddSurfaceHint () returns KErrArgument if the surface ID is not open in this process
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	TEST(KErrArgument == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair));
	
	//Test AddSurfaceHint () returns KErrArgument if the surface ID is invalid
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	TSurfaceId surfaceIdTemp;
	surfaceIdTemp = surfaceId;
	surfaceId.iInternal[3] = (surfaceId.iInternal[3] & 0x00FFFFFF) | ( TSurfaceId::EInvalidSurface <<24 ) ;
	TEST(KErrArgument == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair));
	
	//Test AddSurfaceHint () returns KErrAlreadyExists if duplicate hint key used
	surfaceId = surfaceIdTemp;
	TEST(KErrNone == iSurfaceManager.OpenSurface(surfaceId));
	RSurfaceManager::THintPair hintPairDup;
	hintPairDup.iKey.iUid = 0x124578;
	hintPairDup.iValue = 300;
	TEST(KErrAlreadyExists == iSurfaceManager.AddSurfaceHint(surfaceId, hintPairDup));
	
	//AddSurfaceHint () returns KErrOverflow if no space to add new pair.	
	TInt k=1;
	
	TInt maxHintsPerSurface;
	iSurfaceManager.GetSurfaceManagerAttrib(RSurfaceManager::EMaxNumberOfHints,maxHintsPerSurface);

	for (k=1;k<maxHintsPerSurface-1;++k)
		{
		hintPair2.Set(TUid::Uid(0x124611+k),100*k,(k%2==0)?ETrue:EFalse);
		TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair2));
		}		
	
	hintPair2.Set(TUid::Uid(0x124611+k),100*k,(k%2==0)?ETrue:EFalse);
	TEST(KErrOverflow == iSurfaceManager.AddSurfaceHint(surfaceId, hintPair2));
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));
	iSurfaceManager.Close();
	}


void CTSurfaceManager::TestZerosInHintsArrayL()
{
  	INFO_PRINTF1(_L("Creating a surface with only last hint set, then add hint\r\n"));
	User::LeaveIfError(iSurfaceManager.Open());

	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(100,100);
	attributes.iBuffers = 2;
	attributes.iPixelFormat = EUidPixelFormatARGB_8888;
	attributes.iStride = 400;
	attributes.iOffsetToFirstBuffer = 10;
	attributes.iAlignment = RSurfaceManager::EPageAligned;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iMappable = ETrue;
	
	RSurfaceManager::THintPair hints[8];	     // one hint pairs specified
	attributes.iHintCount=2;
	memclr(hints, sizeof(hints));
	hints[7].Set(TUid::Uid(3), 25, ETrue);
	attributes.iSurfaceHints = hints;
 
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));

 	RSurfaceManager::THintPair hintPair;
	hintPair.iKey.iUid = 0x124570;
	hintPair.iValue = 50;
	hintPair.iMutable = ETrue;
	TEST(KErrNone == iSurfaceManager.AddSurfaceHint(surfaceId,hintPair));
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));	

	iSurfaceManager.Close();
    }



void CTSurfaceManager::TestCreateSurfaceWithTooManyHintsL()
	{  	
	User::LeaveIfError(iSurfaceManager.Open());	
	TInt maxHintsPerSurface;
	iSurfaceManager.GetSurfaceManagerAttrib(RSurfaceManager::EMaxNumberOfHints,maxHintsPerSurface);	
	INFO_PRINTF2(_L("Creating a surface with more than the Maximum of Hints (%d)\r\n"),maxHintsPerSurface);
	
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	
	attributes.iSize = TSize(100,100);
	attributes.iBuffers = 2;
	attributes.iPixelFormat = EUidPixelFormatARGB_8888;
	attributes.iStride = 400;
	attributes.iOffsetToFirstBuffer = 10;
	attributes.iAlignment = RSurfaceManager::EPageAligned;
	attributes.iOffsetBetweenBuffers = 0;
	attributes.iContiguous = ETrue;
	attributes.iCacheAttrib = RSurfaceManager::ECached;
	attributes.iMappable = ETrue;
	
	ReAllocHintsArrayL(maxHintsPerSurface+2);
	for (TInt k=0;k<maxHintsPerSurface+2;++k)
		{
		iHintsArray[k].Set(TUid::Uid(0x124611+k),100*k,(k%2==0)?ETrue:EFalse);		
		}	
	
	attributes.iSurfaceHints = iHintsArray;
	attributes.iHintCount=maxHintsPerSurface+2;
 
	TSurfaceId surfaceId;
	TEST(KErrArgument == iSurfaceManager.CreateSurface(buf, surfaceId));	
	iSurfaceManager.Close();		
	}


void CTSurfaceManager::TestCreateSurfaceUnMappableL()
	{
	INFO_PRINTF1(_L("Create an UNmappable surface and try to map it or get the offset of the first buffer\r\n"));
	// Open the surface manager
	User::LeaveIfError(iSurfaceManager.Open());
		
	// Store the attributes used to create the Surface
	RSurfaceManager::TSurfaceCreationAttributesBuf buf;
	RSurfaceManager::TSurfaceCreationAttributes& attributes = buf();
	SetupAttributesL(attributes);
	attributes.iMappable = EFalse;

	// Create a surface
	TSurfaceId surfaceId;
	TEST(KErrNone == iSurfaceManager.CreateSurface(buf, surfaceId));
	// Map the surface 
	RChunk handle;
	TEST(KErrNotSupported == iSurfaceManager.MapSurface(surfaceId, handle));
	handle.Close();
	
	TInt offsetToFirstBuffer;
	TEST(KErrNotSupported == iSurfaceManager.GetBufferOffset(surfaceId,0,offsetToFirstBuffer));
	
	TEST(KErrNone == iSurfaceManager.CloseSurface(surfaceId));	
	iSurfaceManager.Close();
	}


//--------------
__CONSTRUCT_STEP__(SurfaceManager)

void CTSurfaceManagerStep::TestSetupL()
	{
    }

void CTSurfaceManagerStep::TestClose()
	{
	}

void CTSurfaceManager::testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine)
    {
    
    testBooleanTrue(aCondition, aFile, aLine);
    if(!aCondition)
        User::Leave(TEST_ERROR_CODE);   // leave with standard error code
    }

