// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @prototype
*/

#ifndef EXAMPLERASTERIZER_H
#define EXAMPLERASTERIZER_H


#include <graphics/fbsrasterizer.h>
#include <graphics/fbsrasterizerclearcache.h>


class RMemReadStream;


const TUid KUidExampleExtendedBitmap = {0x10285A78};
const TInt KMaxRecentBmpCacheSize = 0x40000; // 256 KB


/** Example implementation of a rasterizer that is used to generate pixel
data for extended bitmaps of type KUidExampleExtendedBitmap.
 */
class CExampleRasterizer : public CFbsRasterizer, public MFbsRasterizerClearCache
	{
public:
	CExampleRasterizer();
	~CExampleRasterizer();

	// From CFbsRasterizer
	void BeginBitmap(TInt64 aBitmapId, const TBitmapDesc& aBitmapDesc, const TRegion* aRegionOfInterest);
	void EndBitmap(TInt64 aBitmapId);
	const TUint32* ScanLine(TInt64 aBitmapId, const TPoint& aPixel, TInt aLength);	
	TInt GetInterface(TUid aInterfaceId, TAny*& aInterface);
	
	// From MFbsRasterizerClearCache
	void ClearCache();

private:
	/** Class for holding information about an extended bitmap registered with this rasterizer.
	The buffer to store the pre-rendered pixels is allocated in the same heap cell, after the
	instance variables, by using the overload of operator new with an extra size parameter.
	 */
	class CRegisteredBitmap : public CBase
		{
	public:
		CRegisteredBitmap(TInt64 aBitmapId, const TBitmapDesc& aBitmapDesc, TInt aBufLength);
		~CRegisteredBitmap();
		TInt Draw(RMemReadStream& aReadStream, const TRegion* aRegionOfInterest);
		TUint8* Buffer();
		
	public:
		const TInt64 iBitmapId;
		const TSize iSizeInPixels;
		const TDisplayMode iDispMode;
		const TUid iDataType;
		const TInt iBufLength;
		TDblQueLink iLink;
		/** The number of times the bitmap with the ID aBitmapId is currently registered with this
		rasterizer or zero for recently used bitmaps.
		 */
		TInt iRefCount;
		/** Whether the whole bitmap has been rasterized into the buffer.
		 */
		TBool iWhole;
		};
	
private:
	CRegisteredBitmap* RegisteredBitmap(TInt64 aBitmapId);
	CRegisteredBitmap* RecentBitmap(TInt64 aBitmapId);
	static TInt IdleFunction(TAny* aPtr);

private:
	/** List of currently registered extended bitmaps, the key is the bitmap id.
	 */
	TDblQue<CRegisteredBitmap> iRegisteredBmps;
	/** List of recently used extended bitmaps, the key is the bitmap id.
	 */
	TDblQue<CRegisteredBitmap> iRecentBmps;
	/** Total size in bytes of the pre-rendered buffers for all the recently used extended bitmaps.
	 */
	TInt iTotalRecentBmpSize;
	/** An idle-time active object to clean up the recently used extended bitmaps.
	 */
	CIdle* iIdle;
	};

#endif // EXAMPLERASTERIZER_H
