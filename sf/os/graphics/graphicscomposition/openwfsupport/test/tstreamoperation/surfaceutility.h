/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file
*/

#ifndef __SURFACE_UTILITY__
#define __SURFACE_UTILITY__

#include <e32base.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>

#define ENABLE_TEST_LOGGING
#define ALPHA_FIX_24BIT	0x7f //This forces a non-zero value into the X byte of 16MU color values

#if defined(ENABLE_TEST_LOGGING)
#define LOG(X)	RDebug::Printf X
#else
#define LOG(X)
#endif

class TRgb;
class CFbsBitmap;

class CSurfaceUtility : public CBase
	{
public:
	static CSurfaceUtility* NewL(CSurfaceUtility* aClone=NULL);
	virtual ~CSurfaceUtility();
	TSize SurfaceSize(const TSurfaceId& aSurface);
	TSurfaceId CreateSurfaceL(const TSize& aSize, TUidPixelFormat aPixelFormat, TInt aStride, TInt aBuffers = 1);
	TInt BytesPerPixelL(TUidPixelFormat aPixelFormat);
	void FillSurfaceL(TSurfaceId& aSurface, const TRgb& aColor);
	void FillSurfaceL(TSurfaceId& aSurface, TInt aBuffer, const TRgb& aColor);
	void DestroySurface(TSurfaceId& aSurface);
	TBool DestroyAll();
    TInt SubmitUpdate(TInt aScreenNumber, const TSurfaceId& aSurface, TInt aBufferNumber,TInt aNullRegion);
    TInt SubmitUpdate(TInt aScreenNumber, const TSurfaceId& aSurface, TInt aBufferNumber,const TRegion* aRegion = NULL);
    TInt SubmitUpdate(TInt aScreenNumber, const TSurfaceId& aSurface, const TRegion* aRegion, TInt aBufferNumber = 0);
    void NotifyWhenDisplayed(TRequestStatus& aStatusDisplayed, TTimeStamp& aTimeStamp);
    void NotifyWhenDisplayedXTimes(TInt aCount, TRequestStatus& aStatusDisplayedX);
    void NotifyWhenAvailable(TRequestStatus& aStatusAvailable);	
	//Native Stream
	void FillNativeStreamSurfaceL(TSurfaceId& aSurface, TUint8* aBufferPtr, const TRgb& aColor);
	TBool CompareSurfacesL(TSurfaceId& aSurface, TInt aBuffer, TSurfaceId& aStreamSurface, TUint8* aBufferPtr);
	RSurfaceManager& Manager()	{return iManager;}
	RSurfaceUpdateSession& Session()	{return iSurfaceUpdateSession;}
private:
	CSurfaceUtility(CSurfaceUtility* aClone);
	void ConstructL();

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
		static void SwitchHeapLC(RHeap* aNewHeap);
		
		Array iLocalArray;
		Array* iUseArray;
		RHeap &iExternalHeapRef;
		}	iSurfaces;
	};

#endif // __SURFACE_UTILITY__

