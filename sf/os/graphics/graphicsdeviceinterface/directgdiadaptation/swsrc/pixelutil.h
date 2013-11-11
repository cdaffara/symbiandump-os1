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

#ifndef PIXELUTIL_H
#define PIXELUTIL_H

/**
@file
@internalComponent
*/

#include <e32cmn.h>
#include <gdi.h>
#include <pixelformats.h>

/**
Provides conversion between TDisplayMode/TUidPixelFormat and other pixel format related functionality.
*/
NONSHARABLE_STRUCT(PixelFormatUtil)
	{
	static TUidPixelFormat ConvertToPixelFormat(TDisplayMode aDisplayMode);
	static TBool HasAlpha(TUidPixelFormat aPixelFormat);
	static TInt BitsPerPixel(TUidPixelFormat aPixelFormat);
	static TDisplayMode ConvertToDisplayMode(TUidPixelFormat aPixelFormat);
	};

/**
Provides access to the contents of a pixel buffer. Functionality includes copying pixels into 
user buffer and perform format conversion or scaling up/down. Copying can be performed 
horizontally from left to right  or right to left, and vertically from top to bottom or bottom
to up.

Supported buffer pixel format:
-EUidPixelFormatRGB_565
-EUidPixelFormatXRGB_8888
-EUidPixelFormatARGB_8888
-EUidPixelFormatARGB_8888_PRE

Supported read pixel format:
-EUidPixelFormatRGB_565
-EUidPixelFormatXRGB_8888
-EUidPixelFormatARGB_8888
-EUidPixelFormatARGB_8888_PRE
*/
NONSHARABLE_CLASS(TPixelBufferReader)
	{
public:
	enum TReadDirection
		{
		EReadHorizontal,
		EReadHorizontalReverse,
		EReadVertical,
		EReadVerticalReverse
		};

	TPixelBufferReader(const TUint32* aPixelBuffer, const TSize& aSize, TInt aStride,TUidPixelFormat aFormat);

	void GetScanLine(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen,
			TUidPixelFormat aReadFormat, TReadDirection aReadDir) const;
	void GetScaledScanLine(TDes8& aReadBuf, const TPoint& aReadPos, TInt aClipDestPos,
			TInt aClipDestLen, TInt aDestLen, TInt aSrcLen, TUidPixelFormat aReadFormat,
			TReadDirection aReadDir) const;

	const TUint32* GetPixelAddr(const TPoint& aPos) const;

private:
	TPixelBufferReader(); //declare but not defined
	const TUint32* GetScanLineAddr(TInt aRow) const;

	void GetScanLineRGB_565(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen, TReadDirection aReadDir) const;
	void GetScanLineXRGB_8888(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen, TReadDirection aReadDir) const;
	void GetScanLineARGB_8888(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen, TReadDirection aReadDir) const;
	void GetScanLineARGB_8888_PRE(TDes8& aReadBuf, const TPoint& aReadPos, TInt aReadLen, TReadDirection aReadDir) const;

	void CopyFromRGB_565(TUint32* aDestPtr, const TUint16* aSrcPtr, TInt aLen, TInt aAdvance) const;
	TInt GetAdvance(TReadDirection aReadDir) const;

	void GetScaledScanLineH(TDes8& aReadBuf, const TPoint& aReadPos, TInt aClipDestPos,
			TInt aClipDestLen, TInt aDestLen, TInt aSrcLen, TUidPixelFormat aReadFormat,
			TReadDirection aReadDir) const;
	void GetScaledScanLineV(TDes8& aReadBuf, const TPoint& aReadPos, TInt aClipDestPos,
			TInt aClipDestLen, TInt aDestLen, TInt aSrcLen, TUidPixelFormat aReadFormat,
			TReadDirection aReadDir) const;

private:
	const TUint32* iBuffer;
	TSize iSize;
	TInt iStride;
	TUidPixelFormat iFormat;
	};

#endif /*PIXELUTIL_H*/
