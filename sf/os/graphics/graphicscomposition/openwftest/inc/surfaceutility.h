// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef __SURFACE_UTILITY__
#define __SURFACE_UTILITY__

#include <e32base.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>
#include <graphics/symbianstream.h>
#include <WF/wfc.h>

//#define ENABLE_TEST_LOGGING
#if defined(ENABLE_TEST_LOGGING)
#define LOG(X)	RDebug::Printf X
#define LOGL(X)	RDebug::Print X
#else
#define LOG(X)
#define LOGL(X)
#endif

class TRgb;
class CFbsBitmap;

class CSurfaceUtility : public CBase
	{
public:
    CSurfaceUtility();
	static CSurfaceUtility* NewL();
	virtual ~CSurfaceUtility();
	TSize SurfaceSize(const TSurfaceId& aSurface);
	TSurfaceId CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride, TBool aContiguous, TInt aBuffers);
	TInt BytesPerPixelL(TUidPixelFormat aPixelFormat);
	void FillSurfaceL(TSurfaceId& aSurface, const TRgb& aColor);
	void FillSurfaceL(TSurfaceId& aSurface, TInt aBuffer, const TRgb& aColor);
	void DestroySurface(TSurfaceId& aSurface);
	TBool DestroyAll();
    TBool CheckRectColor(const TSurfaceId& aSurface, TRect& aRect, TInt aNumOfBuffer, const TRgb& aExpectedColor, TInt aTolerance);
    TBool SaveResultImageTGAL(const TSurfaceId& aOnscreenSurface, TInt aBufferNumber, TDesC& aDestination);
    TInt SubmitUpdate(const TSurfaceId& aSurface, TInt aBufferNumber, const TRegion* aRegion = NULL);
    void NotifyWhenDisplayed(TRequestStatus& aStatusDisplayed, TTimeStamp& aTimeStamp);
    void NotifyWhenDisplayedXTimes(TInt aCount, TRequestStatus& aStatusDisplayedX);
    void NotifyWhenAvailable(TRequestStatus& aStatusAvailable);	
	RSurfaceManager& Manager()	{return iManager;}
	RSurfaceUpdateSession& Session()	{return iSurfaceUpdateSession;}
	SymbianStreamType GetOnScreenStream(WFCDevice aDev, WFCContext aContext);
	TBool CreateImagePath(TDesC& aDir);

private:
	void ConstructL();
    TInt GetHeader(const TSurfaceId& aSurface, RSurfaceManager::TInfoBuf& aInfo);
    TUint8* GetBufferPointerL(const TSurfaceId& aSurface, TInt aNumOfBuffer, RChunk& aChunk);
    TRgb GetPixelL(RSurfaceManager::TInfoBuf& aInfo, TAny* aPixelData, TPoint aPosition);

private:
	RSurfaceManager iManager;
	RSurfaceUpdateSession iSurfaceUpdateSession;

	//This local object wraps the array operations in a heap switch
	//The reason for providing this wrapper is to allow threads to share the surface manager surface ID list.
	//In particular, the next test can attempt to release the previous test's surfaces if it paniced. 
	//Note that the presumption is that the owning thread will be paused, 
	//so there will be no thread interlock issues with it's heap manager.
	class RHeapSurfaceArray
		{
		public:
		typedef RArray<TSurfaceId>	Array;
		RHeapSurfaceArray(RHeapSurfaceArray* aUseExternalArray=NULL);

		//Emulation of RArray entrypoints. Add more stub methods if required...
		TSurfaceId& operator[](TUint aIndex);
		void Close();
		TInt Count() const;
		inline void Reset();
		void AppendL(const TSurfaceId &anEntry);
		TInt Find(const TSurfaceId &anEntry) const;
		void Remove(TInt anIndex);
		
		private:
		static void PopHeap(void* mainheap);
		
		Array iLocalArray;
		Array* iUseArray;
		RHeap &iExternalHeapRef;
		}	iSurfaces;
	};

#endif // __SURFACE_UTILITY__

