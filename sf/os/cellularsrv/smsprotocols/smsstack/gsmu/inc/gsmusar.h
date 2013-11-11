// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Segmentation and reassembly utility classes
// 
//

/**
 @file
*/

#ifndef __GSMUSAR_H__
#define __GSMUSAR_H__

#include <charconv.h>
#include <gsmubuf.h>
#include <gsmuelem.h>

/**
 *  @internalComponent
 */
_LIT8(KMOSES,"\x40\x4d\x64\xd3\x50\x00");
/**
 *  @internalComponent
 */
_LIT16(KNETWORK,"NETWORK");


// Constants
/**
 *  @internalComponent
 */
const TUint8 KSms7BitAlphabetEscapeChar=0x1b;


/**
 *  Segments unpacked data from a CsmsBufferBase
 *  @internalComponent
 */
class CSmsBufferSegmenter : public CBase
	{
friend class CSmsUserData;
public:
// Construction / destruction methods
	IMPORT_C static CSmsBufferSegmenter* NewLC(CSmsAlphabetConverter& aAlphabetConverter,const CSmsBufferBase& aBuffer,TInt aSegmentSize=0);
	IMPORT_C ~CSmsBufferSegmenter();

// Segmentation methods
	void Reset();
	IMPORT_C TBool SegmentNextL(TDes8& aSegmentBuffer, TInt& aUnconvertedChars,
			                    TInt& aDowngradedChars,
			                    TSmsEncoding aEncoding);
	IMPORT_C TInt TotalConvertedLengthL(TSmsEncoding aEncoding);
	TBool MoreL();

	TSmsEncoding FindBestAlternativeEncodingL(TSmsEncoding aSuggestedEncoding,
									          TInt aMaxBodyLength) const;

protected:
// Protected construction methods
	CSmsBufferSegmenter(CSmsAlphabetConverter& aAlphabetConverter,const CSmsBufferBase& aBuffer,TInt aSegmentSize);
	void ConstructL();

// Protected helper methods
	virtual TBool DoSegmentNextL(TDes8& aSegmentBuffer,TInt aSegmentSize,
			                     TInt& aUnconvertedChars, TInt& aDowngradedChars,
			                     TSmsEncoding aEncoding);
	void CheckConvertedBufferAllocL(TInt aMaxLength);
	TInt ElementsToReturnFromConvertedBufferL(TInt aSegmentSize);

protected:
// Private data
	CSmsAlphabetConverter& iAlphabetConverter;
	const CSmsBufferBase& iSmsBuffer;
	TInt iElementsExtracted;
	TInt iSegmentSize;
	HBufC8* iConvertedBuffer;
	TPtr8 iConvertedBufferPtr;
	};


/**
 *  @internalComponent
 */
class CSmsEMSBufferSegmenter : public CSmsBufferSegmenter
	{
public:
	IMPORT_C static CSmsEMSBufferSegmenter* NewLC(CSmsAlphabetConverter& aAlphabetConverter,const CSmsBufferBase& aBuffer, TInt aSegmentSize=0);

	IMPORT_C TBool SegmentNextL(TDes8& aSegmentBuffer,TInt aSegmentSize,
	                            TInt& aUnconvertedChars, TInt& aDowngradedChars,
	                            TSmsEncoding aEncoding);

	TInt SegmentL(TDes8& aSegmentBuffer, TInt aNativeChars, TInt aSegmentMax,
			      TInt& aUnconvertedChars, TInt& aDowngradedChars,
			      TSmsEncoding aEncoding);

protected:
	CSmsEMSBufferSegmenter(CSmsAlphabetConverter& aAlphabetConverter,const CSmsBufferBase& aBuffer,TInt aSegmentSize);

	};


/**
 *  TSmsBufferReassembler - reassembles (unpacked) data to CSmsBufferBase
 *  @internalComponent
 */
class TSmsBufferReassembler
	{
public:
// Construction / destruction methods
	IMPORT_C TSmsBufferReassembler(CSmsAlphabetConverter& aAlphabetConverter,CSmsBufferBase& aBuffer);

// Reassembly methods
	IMPORT_C void ReassembleNextL(const TDesC8& aSegmentBuffer,
			                      TSmsEncoding aEncoding, TBool aIsLast);

private:
	CSmsAlphabetConverter& iAlphabetConverter;
	CSmsBufferBase& iSmsBuffer;
	};

#endif // __GSMUSAR_H__
