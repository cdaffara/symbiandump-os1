// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements Segmentation and Reassembly classes
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "gsmusarTraces.h"
#endif

#include "gsmusar.h"
#include "Gsmumain.h"

//
// CSmsBufferSegmenter - segments and reassembles (unpacked) data to and from CSmsBufferBase
//

/**
 *  This factory function creates a CsmsBufferSegmenter instance
 *  
 *  @param aAlphabetConverter Pre-configured
 *  @param aBuffer The message data
 *  @param aSegmentSize (Default = 0) The size of segment required. If the instance is to be used solely for determining the total length of the converted data with TotalConvertedLengthL() then the segment length need not be supplied, hence defaulting to zero.
 *  @return CSmsBufferSegmenter instance
 *  
 *  @capability None
 */
EXPORT_C CSmsBufferSegmenter* CSmsBufferSegmenter::NewLC(CSmsAlphabetConverter& aAlphabetConverter,const CSmsBufferBase& aBuffer,TInt aSegmentSize)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSBUFFERSEGMENTER_NEWLC_1, "CSmsBufferSegmenter::NewLC()");
	
	CSmsBufferSegmenter* segmenter=new(ELeave) CSmsBufferSegmenter(aAlphabetConverter,aBuffer,aSegmentSize);
	CleanupStack::PushL(segmenter);
	segmenter->ConstructL();
	return segmenter;
	} // CSmsBufferSegmenter::NewLC


CSmsBufferSegmenter::CSmsBufferSegmenter(CSmsAlphabetConverter& aAlphabetConverter,const CSmsBufferBase& aBuffer,TInt aSegmentSize)
	: iAlphabetConverter(aAlphabetConverter),iSmsBuffer(aBuffer),iSegmentSize(aSegmentSize),iConvertedBufferPtr(NULL,0)
	{
	// NOP
	} // CSmsBufferSegmenter::CSmsBufferSegmenter


/**
 *  2nd phase of constructor. Esures we have an initial buffer.
 */
void CSmsBufferSegmenter::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSBUFFERSEGMENTER_CONSTRUCTL_1, "CSmsBufferSegmenter::ConstructL()");

	iConvertedBuffer=HBufC8::NewMaxL(iSegmentSize);
	iConvertedBufferPtr.Set((TUint8*)iConvertedBuffer->Des().Ptr(),0,iConvertedBuffer->Length());
	} // CSmsBufferSegmenter::ConstructL


/**
 *  Destructor - Free resource
 *  
 *  @capability None
 */
EXPORT_C CSmsBufferSegmenter::~CSmsBufferSegmenter()
	{
	delete iConvertedBuffer;
	} // CSmsBufferSegmenter::ConstructL


/**
 *  Resets the converted buffer
 */
void CSmsBufferSegmenter::Reset()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSBUFFERSEGMENTER_RESET_1, "CSmsBufferSegmenter::Reset()");

	iConvertedBufferPtr.Zero();
	iElementsExtracted=0;
	} // CSmsBufferSegmenter::Reset


/**
 *  A client iteratively calls SegmentNextL() to retrieve each segment until ETrue is returned, indicating
 *  that the last segment has been reached. The output buffer, aSegmentBuffer, should be large enough to hold
 *  the number of elements specified by the segment size in the NewLC() constructor otherwise the call will
 *  Panic. The output buffer is automatically reset before it's filled.
 *  
 *  @param aSegmentBuffer The next segment
 *  @return True if the last segment has been reached
 *  
 *  @capability None
 */
EXPORT_C TBool CSmsBufferSegmenter::SegmentNextL(TDes8& aSegmentBuffer,
		                                         TInt& aUnconvertedChars, TInt& aDowngradedChars,
		                                         TSmsEncoding aEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSBUFFERSEGMENTER_SEGMENTNEXTL_1, "CSmsBufferSegmenter::SegmentNextL(): iSegmentSize=%d", iSegmentSize);

	TBool  ret = DoSegmentNextL(aSegmentBuffer, iSegmentSize,
			                    aUnconvertedChars, aDowngradedChars,
			                    aEncoding);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSBUFFERSEGMENTER_SEGMENTNEXTL_2, "CSmsBufferSegmenter::SegmentNextL() returns %d ", ret);

	return ret;
	} // CSmsBufferSegmenter::SegmentNextL


TBool CSmsBufferSegmenter::DoSegmentNextL(TDes8& aSegmentBuffer,TInt aSegmentSize,
                                          TInt& aUnconvertedChars, TInt& aDowngradedChars,
                                          TSmsEncoding aEncoding)
//
// Extracts a "native" segment from the SMS buffer, converts to the required
// character set and breaks off the next segment of required size.
// Returns true if this was the last segment
//
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSBUFFERSEGMENTER_DOSEGMENTNEXTL_1, "CSmsBufferSegmenter::DoSegmentNextL(): aSegmentSize=%d", aSegmentSize);

	__ASSERT_ALWAYS(aSegmentSize>0,Panic(KGsmuPanicIllegalSegmentSize));
	__ASSERT_ALWAYS(aSegmentBuffer.MaxLength()>=aSegmentSize,Panic(KGsmuPanicSegmentBufferTooSmall));

	// Extract from buffer until we have enough chars for a segment or we're at the end
	aSegmentBuffer.Zero();
	TBuf<CSmsBufferBase::EMaxBufLength> nativeChars;
	while ((iConvertedBufferPtr.Length()<aSegmentSize)&&(iElementsExtracted<iSmsBuffer.Length()))
		{
		TInt elementsToExtract=Min(static_cast<TInt>(CSmsBufferBase::EMaxBufLength),iSmsBuffer.Length()-iElementsExtracted);
		iSmsBuffer.Extract(nativeChars,iElementsExtracted,elementsToExtract);

		TInt  numberOfUnconvertibleCharacters;
		TInt  numberOfDowngradedCharacters;
		TPtrC8 smsCharsPtr=iAlphabetConverter.ConvertFromNativeL(nativeChars,
				                                                 aEncoding,
				                                                 numberOfUnconvertibleCharacters,
				                                                 numberOfDowngradedCharacters);
		aUnconvertedChars += numberOfUnconvertibleCharacters;
		aDowngradedChars  += numberOfDowngradedCharacters;

		CheckConvertedBufferAllocL(iConvertedBufferPtr.Length()+smsCharsPtr.Length());
		iConvertedBufferPtr.Append(smsCharsPtr);
		iElementsExtracted+=elementsToExtract;
		}
	// Determine number of converted elements to put in this segment
	TInt elementsInSegment=ElementsToReturnFromConvertedBufferL(aSegmentSize);
	// And copy...
	aSegmentBuffer.Copy(iConvertedBufferPtr.Ptr(),elementsInSegment);
	iConvertedBufferPtr.Delete(0,elementsInSegment);
	// If this is the last segment then ensure no unconverted characters remain
	return !MoreL();

	} // CSmsBufferSegmenter::DoSegmentNextL


TBool CSmsBufferSegmenter::MoreL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSBUFFERSEGMENTER_MOREL_1, "CSmsBufferSegmenter::MoreL()");

	if ((iElementsExtracted>=iSmsBuffer.Length())&&(iConvertedBufferPtr.Length()==0))
		{
		if (iAlphabetConverter.UnconvertedNativeCharacters().Length()>0)
			{
			User::Leave(KErrCorrupt);
			}

		return EFalse;
		}

	return ETrue;
	} // CSmsBufferSegmenter::MoreL


/**
 *  TotalConvertedLengthL() determines the total length in User Data Elements when the input buffer is converted.
 *  Depending on the conversion properties returned from the alphabet converter, a complete conversion may need
 *  to be performed, i.e. this call can be quite expensive.
 *  
 *  @return Total converted length
 *  
 *  @capability None
 */
EXPORT_C TInt CSmsBufferSegmenter::TotalConvertedLengthL(TSmsEncoding aEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSBUFFERSEGMENTER_TOTALCONVERTEDLENGTHL_1, "CSmsBufferSegmenter::TotalConvertedLengthL(): aEncoding=%d", aEncoding);
	
	// Check for shortcut
	CSmsAlphabetConverter::TSmsAlphabetConversionProperties conversionProperties;
	iAlphabetConverter.ConversionPropertiesL(conversionProperties);
	if (conversionProperties.iWidthConversion==CSmsAlphabetConverter::ESmsAlphabetWidthConversionFixed)
		return iSmsBuffer.Length()*conversionProperties.iUDElementsPerNativeCharacter;

	// No shortcut, have to do piecewise conversion
	Reset();

	//
	// Find the best encoding method to use...
	//
	TSmsEncoding  aEncodingToUse = FindBestAlternativeEncodingL(aEncoding, iSmsBuffer.Length());
	
	//
	// Segment and count the converted length...
	//
	TBool complete=EFalse;
	TInt totalConvertedLength=0;
	TInt unconvertedChars=0;
	TInt downgradedChars=0;
	TBuf8<CSmsBufferBase::EMaxBufLength> convertedChars;
	while (!complete)
		{
		complete=DoSegmentNextL(convertedChars,convertedChars.MaxLength(),
				                unconvertedChars, downgradedChars,
				                aEncodingToUse);
		totalConvertedLength+=convertedChars.Length();
		}
	Reset();
	return totalConvertedLength;
	} // CSmsBufferSegmenter::TotalConvertedLengthL


TSmsEncoding CSmsBufferSegmenter::FindBestAlternativeEncodingL(TSmsEncoding aSuggestedEncoding,
									  			       		   TInt aMaxBodyLength) const
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSBUFFERSEGMENTER_FINDBESTALTERNATIVEENCODINGL_1, "CSmsBufferSegmenter::FindBestAlternativeEncodingL(): aSuggestedEncoding=%d, aMaxBodyLength=%d",aSuggestedEncoding, aMaxBodyLength);

	TSmsEncoding  encodingToUse = ESmsEncodingNone;
	
	//
	// If this is not 7bit or the alternative encoding is not set then do
	// nothing...
	//
	if (aSuggestedEncoding != ESmsEncodingNone  &&
		iAlphabetConverter.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		//
		// Allocate a buffer and extract the text and ask the converter for
		// the best encoding...
		//
		HBufC*  buffer = HBufC::NewLC(aMaxBodyLength);
	    TPtr  bufferPtr = buffer->Des();
		TInt  elementsToExtract = Min(aMaxBodyLength, iSmsBuffer.Length() - iElementsExtracted);
		
		iSmsBuffer.Extract(bufferPtr, iElementsExtracted, elementsToExtract);
	
		encodingToUse = iAlphabetConverter.FindBestAlternativeEncodingL(bufferPtr,
										                				aSuggestedEncoding);
		
		CleanupStack::PopAndDestroy(buffer);
		}

	return encodingToUse;
	} // CSmsBufferSegmenter::FindBestAlternativeEncodingL


/**
 *  Ensures the segmentation buffer is of the specified length
 */
void CSmsBufferSegmenter::CheckConvertedBufferAllocL(TInt aMaxLength)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSBUFFERSEGMENTER_CHECKCONVERTEDBUFFERALLOCL_1, "CSmsBufferSegmenter::CheckConvertedBufferAllocL(): aMaxLength=%d",aMaxLength);

	if (iConvertedBuffer->Length()<aMaxLength)
		{
		iConvertedBuffer=iConvertedBuffer->ReAllocL(aMaxLength);
		iConvertedBuffer->Des().SetLength(aMaxLength);
		iConvertedBufferPtr.Set((TUint8*)iConvertedBuffer->Des().Ptr(),iConvertedBufferPtr.Length(),iConvertedBuffer->Length());
		}
	} // CSmsBufferSegmenter::CheckConvertedBufferAllocL


/**
 *  Determines the number of converted elements that should be returned in the
 *  segment - called from DoSegmentNextL
 */
TInt CSmsBufferSegmenter::ElementsToReturnFromConvertedBufferL(TInt aSegmentSize)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSBUFFERSEGMENTER_ELEMENTSTORETURNFROMCONVERTEDBUFFERL_1, "CSmsBufferSegmenter::CheckConvertedBufferAllocL(): aSegmentSize=%d",aSegmentSize);

	TInt elementCount=Min(aSegmentSize,iConvertedBufferPtr.Length());
	if (iAlphabetConverter.Alphabet()==TSmsDataCodingScheme::ESmsAlphabet7Bit)
		{
		// For 7-bit, don't break an extended character across a segment
		while ((elementCount>0)&&(iConvertedBufferPtr[elementCount-1]==KSms7BitAlphabetEscapeChar))
			--elementCount;
		if ((elementCount>0)&&(iConvertedBufferPtr[elementCount-1]==KSms7BitAlphabetEscapeChar))
			User::Leave(KErrCorrupt);
		}
	return elementCount;
	} // CSmsBufferSegmenter::ElementsToReturnFromConvertedBufferL


//------------------------------------------------------------------------------------------------------------


/**
 *  @capability None
 */
EXPORT_C CSmsEMSBufferSegmenter* CSmsEMSBufferSegmenter::NewLC(CSmsAlphabetConverter& aAlphabetConverter,const CSmsBufferBase& aBuffer, TInt aSegmentSize)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSEMSBUFFERSEGMENTER_NEWLC_1, "CSmsBufferSegmenter::NewLC(): aSegmentSize=%d", aSegmentSize);

	CSmsEMSBufferSegmenter* self = new (ELeave) CSmsEMSBufferSegmenter(aAlphabetConverter, aBuffer,  aSegmentSize);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	} // CSmsEMSBufferSegmenter::NewLC


CSmsEMSBufferSegmenter::CSmsEMSBufferSegmenter(CSmsAlphabetConverter& aAlphabetConverter,const CSmsBufferBase& aBuffer,TInt aSegmentSize) :
	CSmsBufferSegmenter(aAlphabetConverter, aBuffer, aSegmentSize)
	{
	// NOP
	} // CSmsEMSBufferSegmenter::CSmsEMSBufferSegmenter


/**
 *  Extracts Segment size out of the Sms Buffer and converts it into aSegmentBuffer
 *  
 *  @return aSegmentBuffer - Buffer to convert into
 *  @param aSegement - Size of the FOREIGN segment to extract
 *  @return ETrue if last segment
 *  
 *  @capability None
 */
EXPORT_C TBool CSmsEMSBufferSegmenter::SegmentNextL(TDes8& aSegmentBuffer, TInt aSegmentSize,
		                                            TInt& aUnconvertedChars, TInt& aDowngradedChars,
		                                            TSmsEncoding aEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSEMSBUFFERSEGMENTER_SEGMENTNEXTL_1, "CSmsEMSBufferSegmenter::SegmentNext(): aSegmentSize=%d", aSegmentSize);

	TBool ret=DoSegmentNextL(aSegmentBuffer, aSegmentSize, aUnconvertedChars, aDowngradedChars,
							 aEncoding);

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSEMSBUFFERSEGMENTER_SEGMENTNEXTL_2, "CSmsEMSBufferSegmenter::SegmentNext() returns %d ", ret);

	return ret;
	} // CSmsEMSBufferSegmenter::SegmentNextL


/**
 *  SegmentL encodes the amount of native chars into a SegmentBuffer
 *  WARNING: This method can not be used after a SegmentNextL
 *  
 *  @return aSegmentBuffer - Buffer to convert into.
 *  @param aNativeChars - Number of native chars to encode.
 *  @param aSegmentMax - The Ceiling the encode will not go past.
 *  @return The number of NATIVE characters added
 */
TInt CSmsEMSBufferSegmenter::SegmentL(TDes8& aSegmentBuffer, TInt aNativeChars, TInt aSegmentMax,
		                              TInt& aUnconvertedChars, TInt& aDowngradedChars,
		                              TSmsEncoding aEncoding)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSEMSBUFFERSEGMENTER_SEGMENTL_1, "CSmsEMSBufferSegmenter::SegmentL(): aNativeChars=%d, aSegmentMax=%d",aNativeChars, aSegmentMax);

	__ASSERT_ALWAYS(iConvertedBufferPtr.Length()==0, User::Leave(KGsmuPanicBufferNotReset));
	__ASSERT_ALWAYS(aNativeChars>0,User::Leave(KGsmuPanicIllegalSegmentSize));
	__ASSERT_ALWAYS(aSegmentMax>0,User::Leave(KGsmuPanicIllegalSegmentSize));
	__ASSERT_ALWAYS(aSegmentBuffer.MaxLength()>=aNativeChars,User::Leave(KGsmuPanicSegmentBufferTooSmall));

	// Extract native chars from the buffer and convert
	aSegmentBuffer.Zero();
	TBuf<CSmsBufferBase::EMaxBufLength> nativeChars;
	TInt nativeElemsToExtract=aNativeChars;
	nativeElemsToExtract=Min(nativeElemsToExtract,nativeChars.MaxLength());
	do
		{
		iSmsBuffer.Extract(nativeChars,iElementsExtracted, nativeElemsToExtract);

		TInt  numberOfUnconvertibleCharacters;
		TInt  numberOfDowngradedCharacters;
		TPtrC8 smsCharsPtr=iAlphabetConverter.ConvertFromNativeL(nativeChars,
				                                                 aEncoding,
				                                                 numberOfUnconvertibleCharacters,
				                                                 numberOfDowngradedCharacters);
		aUnconvertedChars += numberOfUnconvertibleCharacters;
		aDowngradedChars  += numberOfDowngradedCharacters;

		if (smsCharsPtr.Length()>aSegmentMax)
			--nativeElemsToExtract;
		else
			{
			iElementsExtracted += nativeElemsToExtract;
			aSegmentBuffer.Copy(smsCharsPtr.Ptr(),smsCharsPtr.Length());
			return nativeElemsToExtract;
			}
		} while (nativeElemsToExtract);

		return 0;
	} // CSmsEMSBufferSegmenter::SegmentL


//------------------------------------------------------------------------------------------------------------


/**
 *  
 *  Constructor initialise iAlphabetConverter & iSmsBuffer
 *  
 *  @capability None
 */
EXPORT_C TSmsBufferReassembler::TSmsBufferReassembler(CSmsAlphabetConverter& aAlphabetConverter,CSmsBufferBase& aBuffer)
	: iAlphabetConverter(aAlphabetConverter),iSmsBuffer(aBuffer)
	{
	// NOP
	} // TSmsBufferReassembler::TSmsBufferReassembler


/**
 *  Reassembly is performed by iteratively calling the ReassembleNextL() method until all segments have been
 *  passed in.  When the last segment is passed, the aIsLast flag should be set to ETrue to validate that no
 *  unconverted User Data Elements remain, otherwise the method will leave with KErrCorrupt.
 *  
 *  @param aSegmentBuffer A segmented buffer
 *  @param aIsLast Set to true if it's the last segment
 *  
 *  @capability None
 */
EXPORT_C void TSmsBufferReassembler::ReassembleNextL(const TDesC8& aSegmentBuffer,
													 TSmsEncoding aEncoding,TBool aIsLast)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TSMSBUFFERREASSEMBLER_REASSEMBLENEXTL_1, "TSmsBufferReassembler::ReassembleNextL(): aEncoding=%d aIsLast=%d",aEncoding, aIsLast);

	TPtrC nativeChars=iAlphabetConverter.ConvertToNativeL(aSegmentBuffer, aEncoding);
	iSmsBuffer.InsertL(iSmsBuffer.Length(),nativeChars);
	if ((aIsLast)&&(iAlphabetConverter.UnconvertedUDElements().Length()>0))
		{
		User::Leave(KErrCorrupt);
		}
	} // TSmsBufferReassembler::ReassembleNextL
