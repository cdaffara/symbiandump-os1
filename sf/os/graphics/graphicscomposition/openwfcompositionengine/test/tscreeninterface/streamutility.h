// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
//
// Description:
//

/**
 @file
*/

#ifndef __STREAM_UTILITY__
#define __STREAM_UTILITY__

#include <e32base.h>
#include <graphics/surface.h>
#include <graphics/surfacemanager.h>
#include <graphics/surfaceupdateclient.h>
#define ENABLE_TEST_LOGGING

#if defined(ENABLE_TEST_LOGGING)
#define LOG(X)	RDebug::Printf X
#else
#define LOG(X)
#endif

class TRgb;
class CFbsBitmap;

class CStreamUtility : public CBase
	{
public:
	CStreamUtility();
	static CStreamUtility* NewL();
	virtual ~CStreamUtility();
	TSize StreamSize(const SymbianStreamType aStream);
	SymbianStreamType CreateStreamL(const TSize& aSize, TUidPixelFormat aPixelFormat, 
                                    TInt aStride, TSurfaceId& aReturnSurface, TBool aContiguous, TInt aBuffers = 1);
	TInt BytesPerPixelL(const SymbianStreamType aStream);
	TInt BytesPerPixelL(TUidPixelFormat aPixelFormat);
	void FillStreamL(SymbianStreamType aStream, const TRgb& aColor);
	void DestroyStream(SymbianStreamType aStream);
	TBool DestroyAll();
	RSurfaceManager& Manager()	{return iManager;}
private:
	CStreamUtility(CStreamUtility* aClone);
	void ConstructL();

private:
	RSurfaceManager iManager;
    TInt    iScreenNum;

	//This local object wraps the array operations in a heap switch
	//The reason for providing this wrapper is to allow threads to share the surface manager stream ID list.
	//In particular, the next test can attempt to release the previous test's stream if it paniced. 
	//Note that the presumption is that the owning thread will be paused, 
	//so there will be no thread interlock issues with it's heap manager.
	class RHeapStreamArray
		{
		public:
		typedef RArray<SymbianStreamType>	Array;
		RHeapStreamArray(RHeapStreamArray* aUseExternalArray=NULL);

		//Emulation of RArray entrypoints. Add more stub methods if required...
		SymbianStreamType& operator[](TUint aIndex);
		void Close();
		TInt Count() const;
		inline void Reset();
		void AppendL(const SymbianStreamType &anEntry);
		TInt Find(const SymbianStreamType &anEntry) const;
		void Remove(TInt anIndex);
		
		private:
		static void PopHeap(void* mainheap);
		
		Array iLocalArray;
		Array* iUseArray;
		RHeap &iExternalHeapRef;
		}	iStreams;
	};

#endif // __STREAM_UTILITY__
