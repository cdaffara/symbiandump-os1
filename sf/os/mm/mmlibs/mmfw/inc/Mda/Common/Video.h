// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mda\Common\Video.h
// This version forms part of the backwards compatible ImageConversion routines
// 
//

#ifndef __MDA_COMMON_VIDEO_H__
#define __MDA_COMMON_VIDEO_H__

#include <gdi.h>
#include <mda/common/resource.h>
#include <mda/common/video.hrh>

// Video data type Uid

MDA_UID(KUidMdaMediaTypeVideo)
MDA_UID(KUidMdaVideoFrameSettings)
MDA_UID(KUidMdaVideoCurrentFrame)
MDA_UID(KUidMdaDstPortSourceRect)
MDA_UID(KUidMdaMbmClipFormat)
MDA_UID(KUidMdaMbmCodec)
MDA_UID(KUidMdaBmpClipFormat)
MDA_UID(KUidMdaBmpCodec)
MDA_UID(KUidMda1BppBmpCodec)
MDA_UID(KUidMda4BppBmpCodec)
MDA_UID(KUidMda8BppBmpCodec)
MDA_UID(KUidMda24BppBmpCodec)
MDA_UID(KUidMdaFbsBitmapDevice)
MDA_UID(KUidMdaFbsBitmapHandle)
MDA_UID(KUidMdaFbsBitmapMaskHandle)
MDA_UID(KUidMdaJfifClipFormat)
MDA_UID(KUidMdaJpgCodec)
MDA_UID(KUidMdaJpgQTable)
MDA_UID(KUidMdaJpgComment)
MDA_UID(KUidMdaWbmpClipFormat)
MDA_UID(KUidMdaWbmpCodec)
MDA_UID(KUidMdaOtaClipFormat)
MDA_UID(KUidMdaOtaCodec)
MDA_UID(KUidMdaPngClipFormat)
MDA_UID(KUidMdaPngCodec)
MDA_UID(KUidMdaWmfClipFormat)
MDA_UID(KUidMdaWmfApmClipFormat)
MDA_UID(KUidMdaWmfClpClipFormat)
MDA_UID(KUidMdaWmfCodec)
MDA_UID(KUidMdaTiffLittleEndianClipFormat)
MDA_UID(KUidMdaTiffBigEndianClipFormat)
MDA_UID(KUidMdaTiffCodec)
MDA_UID(KUidMdaIcoClipFormat)
MDA_UID(KUidMdaIcoCodec)
MDA_UID(KUidMdaGif87aClipFormat)
MDA_UID(KUidMdaGif89aClipFormat)
MDA_UID(KUidMdaGifCodec)


/**
@publishedAll
@released

Video buffer settings. May be combined.
*/
enum TMdaVideoBufferSettings
	{
	/** Can only handle buffers whilst playing
	*/
	EMdaVideoBufferAtPlay =		0x0000000,
	/** Can handle buffers when primed
	*/
	EMdaVideoBufferAtPrime =	0x0000001,
	/** Must have two buffers for uninterrupted play
	*/
	EMdaVideoBufferDouble =		0x0000002,
	/** Will supply buffers for efficiency
	*/
	EMdaVideoBufferSupply =		0x0000004
	};


/**
 * @publishedAll
 * @deprecated
 *
 * Bmp Clip Format
 */
class TMdaBmpClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaBmpClipFormat();
	};


/**
 * @publishedAll
 * @deprecated
 *
 * 1 Bpp Bmp Codec
 */
class TMda1BppBmpCodec : public TMdaPackage
	{
public:
	inline TMda1BppBmpCodec();
	};


/**
 * @publishedAll
 * @deprecated
 *
 * 4 Bpp Bmp Codec
 */
class TMda4BppBmpCodec : public TMdaPackage
	{
public:
	inline TMda4BppBmpCodec();
	};


/**
 * @publishedAll
 * @deprecated
 *
 * 8 Bpp Bmp Codec
 */
class TMda8BppBmpCodec : public TMdaPackage
	{
public:
	inline TMda8BppBmpCodec();
	};


/**
 * @publishedAll
 * @deprecated
 *
 * 24 Bpp Bmp Codec
 */
class TMda24BppBmpCodec : public TMdaPackage
	{
public:
	inline TMda24BppBmpCodec();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Jpeg settings
 */
class TMdaJpgSettings
	{
public:
	inline TMdaJpgSettings(); // Defaults to EColor420 and 50
public:
	enum TColorSampling
		{
		EMonochrome,
		EColor420,
		EColor422,
		EColor444
		};
public:
	TColorSampling iSampleScheme;
	TInt iQualityFactor; // 0 to 100 inclusive
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Jfif Clip Format
 */
class TMdaJfifClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaJfifClipFormat();
public:
	TMdaJpgSettings iSettings;
	};


/** @publishedAll */
const TInt KMdaJpgQTableEntries = 64;

class TMdaJpgQTable : public TMdaPackage
/**
 * @publishedAll
 * @deprecated
 *
 * Jpeg Quantization table - in direct format
 */
	{
public:
	inline TMdaJpgQTable();
public:
	enum TQTableIndex { ELumaTable = 0, EChromaTable = 1 };
public:
	TInt iTableIndex;
	TBuf8<KMdaJpgQTableEntries> iEntries;
	};


/** @deprecated */
const TInt KJpgCommentBufferSize = 256;
/**
 * @publishedAll  
 * @deprecated
 *
 * Jpeg comment
 */
class TMdaJpgComment : public TMdaPackage
	{
public:
	inline TMdaJpgComment();
public:
	TInt iCommentIndex; // Comment block to set/query
	TInt iCommentBufferIndex; // Index of KJpgCommentBufferSize block within comment to set/query
	TInt iTotalCommentLength; // Total size of comment being queried
	TBuf8<KJpgCommentBufferSize> iComment; // Comment to set/query
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Mbm Clip Format
 */
class TMdaMbmClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaMbmClipFormat();
public:
	TDisplayMode iDisplayMode;
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Wbmp Clip Format
 */
class TMdaWbmpClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaWbmpClipFormat();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Wbmp Codec
 */
class TMdaWbmpCodec : public TMdaPackage
	{
public:
	inline TMdaWbmpCodec();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Ota Clip Format
 */
class TMdaOtaClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaOtaClipFormat();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Ota Codec
 */
class TMdaOtaCodec : public TMdaPackage
	{
public:
	inline TMdaOtaCodec();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Png Clip Format
 */
class TMdaPngClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaPngClipFormat();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Png Codec
 */
class TMdaPngCodec : public TMdaPackage
	{
public:
	inline TMdaPngCodec();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Wmf Clip Format
 */
class TMdaWmfClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaWmfClipFormat();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Wmf Apm Clip Format
 */
class TMdaWmfApmClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaWmfApmClipFormat();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Wmf Clp Clip Format
 */
class TMdaWmfClpClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaWmfClpClipFormat();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Wmf Codec
 */
class TMdaWmfCodec : public TMdaPackage
	{
public:
	inline TMdaWmfCodec();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Gif87a Clip Format
 */
class TMdaGif87aClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaGif87aClipFormat();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Gif89a Clip Format (Animated)
 */
class TMdaGif89aClipFormat : public TMdaClipFormat
	{
public:
	inline TMdaGif89aClipFormat();
	};


/**
 * @publishedAll 
 * @deprecated
 *
 * Gif Codec
 */
class TMdaGifCodec : public TMdaPackage
	{
public:
	inline TMdaGifCodec();
	};

#include <mda/common/video.inl>

#endif

