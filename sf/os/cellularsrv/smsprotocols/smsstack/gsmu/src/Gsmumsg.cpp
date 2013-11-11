// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the implementation of the CSmsMessage class
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "GsmumsgTraces.h"
#endif

#include <gsmumsg.h>
#include "Gsmumain.h"
#include <gsmubuf.h>
#include <gsmuset.h>
#include <gsmusar.h>
#include "gsmumsgadditionalattributes.h"
#include <gsmuieoperations.h>
#include <gsmunonieoperations.h>

#include <logwraplimits.h>
#include <e32uid.h>
#include <etelmm.h>

#include <emsinformationelement.h>
#include <emsformatie.h>
#include <emsuserpromptie.h>
#include <emsobjectdistributionie.h>
#include "smsstackutils.h"

/**
 *  Allocates and creates a CSmsMessage instance from a TGsmSms.
 *  
 *  The type of SMS-XXXX message to construct is defined by the first byte of
 *  the TGsmSms TPDU header.
 *  
 *  @param aFs Reference handle to the file system
 *  @param aGsmSms Reference to TGsmSms
 *  @param aBuffer The Unicode text for the message. The object takes ownership
 *  of aBuffer.
 *  @param aIsRPError Set to true if the message contains Reply Path Error. Default
 *  is false.
 *  @param aIsMobileTerminated Set to True if the message is Mobile Terminated.
 *  Default is false.
 *  @return New CSmsMessage object
 *  @capability None
 */
EXPORT_C CSmsMessage* CSmsMessage::NewL(RFs& aFs, const TGsmSms& aGsmSms,CSmsBufferBase* aBuffer, TBool aIsRPError,TBool aIsMobileTerminated)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_NEWL_1, "CSmsMessage::NewL()");
	
	CleanupStack::PushL(aBuffer);
	CSmsMessage* smsmessage=new(ELeave) CSmsMessage(aFs, aBuffer);
	CleanupStack::Pop();
	CleanupStack::PushL(smsmessage);
	smsmessage->ConstructL(aGsmSms, aIsRPError,aIsMobileTerminated);
	CleanupStack::Pop();
	return smsmessage;
	} // CSmsMessage::NewL


/**
 *  Allocates and creates a CSmsMessage, specifying the SMS-XXX message type with
 *  a CSmsPDU::TSmsPduType.
 *  
 *  @param aFs Reference handle to the file system
 *  @param aType The PDU type
 *  @param aBuffer The Unicode text for the message. The object takes ownership
 *  of aBuffer.
 *  @param aIsRPError Set to true if the message contains Reply Path Error. Default
 *  is false.
 *  @return New CSmsMessage object
 *  @capability None
 */
EXPORT_C CSmsMessage* CSmsMessage::NewL(RFs& aFs, CSmsPDU::TSmsPDUType aType,CSmsBufferBase* aBuffer,TBool aIsRPError)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_NEWL1_1, "CSmsMessage::NewL()");

	CleanupStack::PushL(aBuffer);
	CSmsMessage* smsmessage=new(ELeave) CSmsMessage(aFs, aBuffer);
	CleanupStack::Pop();
	CleanupStack::PushL(smsmessage);
	smsmessage->ConstructL(aType,aIsRPError);
	CleanupStack::Pop();
	return smsmessage;
	} // CSmsMessage::NewL


/**
 *  Destructor, frees resources.
 *  @capability None
 */
EXPORT_C CSmsMessage::~CSmsMessage()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_DTOR_1, "CSmsMessage::~CSmsMessage()");

	delete iSmsPDU;
	delete iBuffer;
	delete iCharacterSetConverter;

	if (iInformationElementArray)
		{
		iInformationElementArray->ResetAndDestroy();
		delete iInformationElementArray;
		}

	delete iAdditionalInfo;
	} // CSmsMessage::NewL


/**
 *  Internalises all object data except for the CSmsBufferBase.
 *  
 *  This is used when the buffer is stored elsewhere.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
EXPORT_C void CSmsMessage::InternalizeWithoutBufferL(RReadStream& aStream)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_INTERNALIZEWITHOUTBUFFERL_1, "CSmsMessage::InternalizeWithoutBufferL()");

	InternalizeWithoutBufferAndVersionL(aStream);
	InternalizeVersionL(aStream);

	iAdditionalInfo->ResetAttributesL();

	if (iVersion > ESmsMessageV0)
        {
        iAdditionalInfo->InternalizeL(aStream, iVersion);
		}
	} // CSmsMessage::InternalizeWithoutBufferL


/**
 *  Externalises all object data except for the CSmsBufferBase.
 *  
 *  This is used when the buffer is stored elsewhere.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void CSmsMessage::ExternalizeWithoutBufferL(RWriteStream& aStream) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_EXTERNALIZEWITHOUTBUFFERL_1, "CSmsMessage::ExternalizeWithoutBufferL()");

	ExternalizeWithoutBufferAndVersionL(aStream);
	ExternalizeVersionL(aStream);

	if (iVersion > ESmsMessageV0)
		{
		iAdditionalInfo->ExternalizeL(aStream, iVersion);
		}
	} // CSmsMessage::ExternalizeWithoutBufferL


/**
 *  Internalises all object data.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
EXPORT_C void CSmsMessage::InternalizeL(RReadStream& aStream)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_INTERNALIZEL_1, "CSmsMessage::InternalizeL()");

	InternalizeWithoutBufferAndVersionL(aStream);
	InternalizeBufferL(aStream);
	InternalizeVersionL(aStream);

	iAdditionalInfo->ResetAttributesL();
	if (iVersion > ESmsMessageV0)
		{
		iAdditionalInfo->InternalizeL(aStream, iVersion);
		}
	} // CSmsMessage::InternalizeL


/**
 *  Externalises all object data.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void CSmsMessage::ExternalizeL(RWriteStream& aStream) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_EXTERNALIZEL_1, "CSmsMessage::ExternalizeL()");

	ExternalizeWithoutBufferAndVersionL(aStream);
	ExternalizeBufferL(aStream);
	ExternalizeVersionL(aStream);

	if (iVersion > ESmsMessageV0)
		{
		iAdditionalInfo->ExternalizeL(aStream, iVersion);
		}
	} // CSmsMessage::ExternalizeL


/**
 *  Tests if the message contains text.
 *  
 *  @return True if the message contains text
 *  @capability None
 */
EXPORT_C TBool CSmsMessage::TextPresent() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_TEXTPRESENT_1, "CSmsMessage::TextPresent()");

	CSmsPDU::TSmsPDUType pdutype=SmsPDU().Type();
	return (pdutype==CSmsPDU::ESmsSubmit) ||
	       (pdutype==CSmsPDU::ESmsDeliver) ||
	     (((pdutype==CSmsPDU::ESmsSubmitReport) || (pdutype==CSmsPDU::ESmsDeliverReport) ||
	      ((pdutype==CSmsPDU::ESmsStatusReport) && ((CSmsStatusReport&) SmsPDU()).ParameterIndicatorPresent())) &&
	      SmsPDU().UserDataPresent());
	} // CSmsMessage::TextPresent


/**
 *  Gets the number of PDU's required to encode the complete message.
 *  
 *  @leave KErrOverflow Leaves if the number of required PDUs exceeds the maximum
 *  or the message cannot be encoded.
 *  @return Number of PDU's
 *  @capability None
 */
EXPORT_C TInt CSmsMessage::NumMessagePDUsL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_NUMMESSAGEPDUSL_1, "CSmsMessage::NumMessagePDUsL()");

	TInt nummessagepdus=1;
	if (IsDecoded())
		{
		if (TextPresent())
			{
			nummessagepdus=NumMessageEmsPDUsL();
			TInt maxnummessagepdus=SmsPDU().TextConcatenated()? 0xFF: 1;
 			if (nummessagepdus>maxnummessagepdus)
				{
 				User::Leave(KErrOverflow);
				}
			}

		}
	else if (TextPresent() && SmsPDU().TextConcatenated())
		{
		nummessagepdus=SmsPDU().NumConcatenatedMessagePDUs();
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_NUMMESSAGEPDUSL_2, "CSmsMessage::NumMessagePDUsL() returns %d", nummessagepdus);

	return nummessagepdus;
	} // CSmsMessage::NumMessagePDUsL


/**
 *  Gets the maximum message length possible with the current settings.
 *  
 *  If Text is compressed, this returns the maximum number of bytes available
 *  with text compressed.
 *  
 *  @return Maximum message length
 *  @capability None
 */
EXPORT_C TInt CSmsMessage::MaxMessageLength() const
	{
	__ASSERT_DEBUG(TextPresent(),Panic(KGsmuPanicTextNotPresent));
	TInt maxmessagelength=SmsPDU().UserData().MaxBodyLengthInChars();
	if (SmsPDU().TextConcatenated())
		{
		maxmessagelength=maxmessagelength*0xFF;
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_MAXMESSAGELENGTH_1, "CSmsMessage::MaxMessageLength() returns %d", maxmessagelength);

	return maxmessagelength;
	} // CSmsMessage::MaxMessageLength


/**
 *  
 *  @return The converted buffer length
 *  @note Use with care - Expensive operation
 */
TInt CSmsMessage::ConvertedBufferLengthL(const CSmsBufferBase& aBuffer)
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF    
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CONVERTEDBUFFERLENGTHL_1, "CSmsMessage::ConvertedBufferLengthL()");
    
    TInt convertedBufferLength=0;
    CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(*iCharacterSetConverter,iFs,SmsPDU().Alphabet(),BinaryData());
    CSmsBufferSegmenter* segmenter=CSmsBufferSegmenter::NewLC(*converter,aBuffer);
    convertedBufferLength=segmenter->TotalConvertedLengthL(iAdditionalInfo->Alternative7bitEncoding());
    CleanupStack::PopAndDestroy(2, converter);
    
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CONVERTEDBUFFERLENGTHL_2, "CSmsMessage::ConvertedBufferLengthL() returns %d", convertedBufferLength);
    
    return convertedBufferLength;
    BULLSEYE_RESTORE
    }

/**
 *  Gets the message length.
 *  
 *  In the case where the message is compressed, this function compresses the
 *  text and returns the number of bytes of the compressed buffer.
 *  
 *  The function calls ConvertedBufferLengthL(), and is therefore an expensive
 *  operation.
 *  
 *  @return Message length
 *  @capability None
 */
EXPORT_C TInt CSmsMessage::MessageLengthL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_MESSAGELENGTHL_1, "CSmsMessage::MessageLengthL()");

	TInt messagelength=0;
	if (!SmsPDU().TextCompressed())
		{
		messagelength=iBuffer->Length();
		if(SmsPDU().Alphabet() == TSmsDataCodingScheme::ESmsAlphabetUCS2)messagelength*=2 ;
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	return messagelength;
	} // CSmsMessage::MessageLengthL


EXPORT_C void CSmsMessage::GetEncodingInfoL(TInt& aPdus, TInt& aUnconvertedChars,
		                                    TInt& aDowngradedChars, TInt& aFreeUDUnitsInLastPDU)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETENCODINGINFOL_1, "CSmsMessage::GetEncodingInfoL()");

	aPdus                 = 1;
	aUnconvertedChars     = 0;
	aDowngradedChars      = 0;
	aFreeUDUnitsInLastPDU = 0;

	if (TextPresent())
		{
		if (IsDecoded())
			{
			//
			// Clear the concatenated flag, EncodeBufferL() will add it if needed.
			//
			SmsPDU().SetTextConcatenatedL(EFalse, EFalse);

			//
			// Attempt to encode to a single PDU, and if that fails then attempt to
			// encode to a set of PDUs.
			//
			CArrayFixFlat<TGsmSms>*  tmpArray = new (ELeave) CArrayFixFlat<TGsmSms>(8);
			CleanupStack::PushL(tmpArray);
			
			//
			// Encode the message...
			//
			if (!EncodeIntoSinglePDUL(*tmpArray, aUnconvertedChars,
                                      aDowngradedChars, aFreeUDUnitsInLastPDU))
				{
				EncodeBufferL(*tmpArray, 0, *iBuffer, aUnconvertedChars,
						      aDowngradedChars, aFreeUDUnitsInLastPDU, EFalse);

				aPdus = iAdditionalInfo->SmsPDUArray().Count();
				if (aPdus > 255)
					{
	 				User::Leave(KErrOverflow);
					}
				}

			CleanupStack::PopAndDestroy(tmpArray);
			}
		else if (SmsPDU().TextConcatenated())
			{
			aPdus                 = SmsPDU().NumConcatenatedMessagePDUs();
			aUnconvertedChars     = 0;
			aDowngradedChars      = 0;
			aFreeUDUnitsInLastPDU = 0;
			}
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETENCODINGINFOL_2, "CSmsMessage::GetEncodingInfoL(): aPdus=%d", aPdus);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETENCODINGINFOL_3, "CSmsMessage::GetEncodingInfoL(): aUnconvertedChars=%d", aUnconvertedChars);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETENCODINGINFOL_4, "CSmsMessage::GetEncodingInfoL(): aDowngradedChars=%d", aDowngradedChars);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETENCODINGINFOL_5, "CSmsMessage::GetEncodingInfoL(): aFreeUDUnitsInLastPDU=%d", aFreeUDUnitsInLastPDU);
	} // CSmsMessage::GetEncodingInfoL


/**
 *  Gets the User Data Settings.
 *  
 *  @param aSettings User Data Settings
 *  @capability None
 */
EXPORT_C void CSmsMessage::UserDataSettings(TSmsUserDataSettings& aSettings) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_USERDATASETTINGS_1, "CSmsMessage::UserDataSettings()");

	__ASSERT_DEBUG(TextPresent(),Panic(KGsmuPanicTextNotPresent));
	aSettings.SetAlphabet(SmsPDU().Alphabet());
	aSettings.SetTextCompressed(SmsPDU().TextCompressed());
	TBool is16bit;
	TBool concatenated=SmsPDU().TextConcatenated(&is16bit);
	aSettings.SetTextConcatenated(concatenated,is16bit);
	} // CSmsMessage::UserDataSettings


/**
 *  Sets the User Data Settings.
 *  
 *  @param aSettings User Data Settings
 *  @capability None
 */
EXPORT_C void CSmsMessage::SetUserDataSettingsL(const TSmsUserDataSettings& aSettings)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_SETUSERDATASETTINGSL_1, "CSmsMessage::SetUserDataSettingsL()");

	__ASSERT_DEBUG(TextPresent(),Panic(KGsmuPanicTextNotPresent));
	SmsPDU().SetAlphabet(aSettings.Alphabet());
	SmsPDU().SetTextCompressed(aSettings.TextCompressed());
	TBool is16bit;
	TBool concatenated=aSettings.TextConcatenated(&is16bit);
	SmsPDU().SetTextConcatenatedL(concatenated,is16bit);
	} // CSmsMessage::SetUserDataSettingsL


/**
 *  Optimizes the user data settings.
 *  
 *  The alphabet flag causes an alphabet to be chosen which preserves information
 *  in the message and makes the number of PDUs as small as possible.
 *  
 *  The compression settings flag is not supported.
 *  
 *  The compression flag causes compression to be switched on if the resultant
 *  number of PDUs is smaller.
 *  
 *  If user explicitly defines alphabet as UCS2 or 8-bit coding, this setting is preserved;
 *  otherwise, if 7-bit (default) alphabet can not support current text of message UCS2
 *  alphabet (Unicode) is used.
 *  
 *  The two concatenation flags are mutually exclusive as they deal with 8-bit
 *  and 16-bit referenced concatenation. Both flags cause compression to be switched
 *  off and if the message length is greater than the maximum message length,
 *  concatenation is switched on.
 *  
 *  @param aOptions Combination of TSmsOptimizationFlags
 *  @capability None
 */
EXPORT_C void CSmsMessage::OptimizeSettingsL(TInt aOptions)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_OPTIMIZESETTINGSL_1, "CSmsMessage::OptimizeSettingsL()");

	__ASSERT_DEBUG(TextPresent(),Panic(KGsmuPanicTextNotPresent));
	__ASSERT_DEBUG(IsDecoded(),Panic(KGsmuPanicNotDecoded));

	if (aOptions&ESmsFlagOptimizeAlphabet && (SmsPDU().Alphabet()==TSmsDataCodingScheme::ESmsAlphabet7Bit))
		{
		TBool isSupported=ETrue;
		TInt numOfUnconvertibleChars;
		TInt indexOfFirstUnconvertibleChar;

		TInt size=iBuffer->Length();
		const TInt bufsize=128;
		TBuf<bufsize> buf;
		TInt extracted=0;
		TInt remaining=0;
		TInt toExtract=0;
		TInt pos=0;

		// Defect Fix. Previous for loop caused an access violation as extract size was set
		// to size of ibuffer not bufsize and caused a access violation when over 128. Implemented
		// fix copies the ibuffer in 128 chunks until its all done.

		while((pos<size) && isSupported)
			{
			remaining=(size - extracted);

			if(remaining>bufsize)
				toExtract=bufsize;
			else
				toExtract=remaining;

			iBuffer->Extract(buf,pos,toExtract);
			isSupported=IsSupportedL(buf,numOfUnconvertibleChars,indexOfFirstUnconvertibleChar);
			extracted=(extracted+toExtract);
			pos=extracted;
			}

		if(!isSupported)
			SmsPDU().SetAlphabet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
		}
	} // CSmsMessage::OptimizeSettingsL


/**
 *  This function returns the currently requested alternative encoding method.
 * 
 *  In the case of incoming SMS messages received on the device, it will often
 *  be TSmsEncodingNone unless the Information Elements imply that an
 *  alternative encoding has been used.
 * 
 *  @return The currently selected encoding method.
 * 
 *  @capability None
 */
EXPORT_C TSmsEncoding CSmsMessage::Alternative7bitEncoding() const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ALTERNATIVE7BITENCODING_1, "CSmsMessage::Alternative7bitEncoding()");

	return iAdditionalInfo->Alternative7bitEncoding();
	} // CSmsMessage::Alternative7bitEncoding


/**
 *  This function allows the client to specify an alternative encoding method
 *  incase the default GSM encoding cannot encode the message completely
 *  without data loss.
 * 
 *  @param aEncoding  Encoding method to select.
 * 
 *  @return A Symbian system wide error code.  This function will return
 *          KErrArgument if the encoding enum is invalid and will return
 *          KErrNotSupported if the required plug-ins are not present.
 * 
 *  @capability None
 */
EXPORT_C TInt CSmsMessage::SetAlternative7bitEncoding(TSmsEncoding aEncoding)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_SETALTERNATIVE7BITENCODING_1, "CSmsMessage::SetAlternative7bitEncoding(%d)", aEncoding);

	//
	// Get the encoders that would be used for this encoding method.
	// The function will also check that the required encoder is present.
	//
	TRAPD(err,
		{
		CSmsAlphabetConverter*  converter = CSmsAlphabetConverter::NewLC(*iCharacterSetConverter, iFs,
																	     TSmsDataCodingScheme::ESmsAlphabet7Bit,
																	     EFalse);
		converter->ConfirmAlternativeEncoderL(aEncoding);
		CleanupStack::PopAndDestroy(converter);
		});
	
	//
	// Set the variable if the encoders are available...
	//
	if (err == KErrNone)
		{
		iAdditionalInfo->SetAlternative7bitEncoding(aEncoding);
		}
	
	return err;
	} // CSmsMessage::SetAlternative7bitEncoding


/**
 *  Takes an encoding setting (received from a PDU) and merges it into the
 *  current setting (where possible).
 *  
 *  @param aEncoding  Encoding setting to merge.
 */
void CSmsMessage::MergeAlternative7bitEncoding(TSmsEncoding aEncoding) const
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_MERGEALTERNATIVE7BITENCODING_1, "CSmsMessage::MergeAlternative7bitEncoding(): aEncoding=%d (currently %d)",aEncoding, iAdditionalInfo->Alternative7bitEncoding());

	switch (iAdditionalInfo->Alternative7bitEncoding())
		{
		case ESmsEncodingNone:
			{
			// Anything is can be merge with this...
			iAdditionalInfo->SetAlternative7bitEncoding(aEncoding);
			}
			break;
		
		case ESmsEncodingTurkishSingleShift:
			{
			// Only Turkish locking shift can merge...
			if (aEncoding == ESmsEncodingTurkishLockingShift)
				{
				iAdditionalInfo->SetAlternative7bitEncoding(ESmsEncodingTurkishLockingAndSingleShift);
				}
			}
			break;
		
		case ESmsEncodingTurkishLockingShift:
			{
			// Only Turkish single shift can merge...
			if (aEncoding == ESmsEncodingTurkishSingleShift)
				{
				iAdditionalInfo->SetAlternative7bitEncoding(ESmsEncodingTurkishLockingAndSingleShift);
				}
			}
			break;
		
		case ESmsEncodingPortugueseSingleShift:
			{
			// Only Portuguese locking shift can merge...
			if (aEncoding == ESmsEncodingPortugueseLockingShift)
				{
				iAdditionalInfo->SetAlternative7bitEncoding(ESmsEncodingPortugueseLockingAndSingleShift);
				}
			}
			break;
		
		case ESmsEncodingPortugueseLockingShift:
			{
			// Only Portuguese single shift can merge...
			if (aEncoding == ESmsEncodingPortugueseSingleShift)
				{
				iAdditionalInfo->SetAlternative7bitEncoding(ESmsEncodingPortugueseLockingAndSingleShift);
				}
			}
			break;
		
		case ESmsEncodingTurkishLockingAndSingleShift:
		case ESmsEncodingSpanishSingleShift:
		case ESmsEncodingPortugueseLockingAndSingleShift:
		default:
			{
			// NOP - Cannot merge anything into these...
			}
		};

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_MERGEALTERNATIVE7BITENCODING_2, "CSmsMessage::MergeAlternative7bitEncoding(): New encoding=%d",iAdditionalInfo->Alternative7bitEncoding());
	} // CSmsMessage::MergeAlternative7bitEncoding


/**
 *  Tests if a buffer can be encoded without loss of information.
 *  
 *  @param aDes                                 The buffer to test for encoding
 *  @param aNumberOfUnconvertibleCharacters     On return, the number of 
 *                                              unconvertible characters
 *  @param aIndexOfFirstUnconvertibleCharacter  On return, the index of the
 *                                              first unconvertible character
 * 
 *  @return True if aDes can be encoded without loss of information
 * 
 *  @capability None
 */
EXPORT_C TBool CSmsMessage::IsSupportedL(const TDesC& aDes, TInt& aNumberOfUnconvertibleCharacters,
		                                 TInt& aIndexOfFirstUnconvertibleCharacter)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ISSUPPORTEDL_1, "[1] CSmsMessage::IsSupportedL()");

	__ASSERT_DEBUG(TextPresent(), Panic(KGsmuPanicTextNotPresent));

	aNumberOfUnconvertibleCharacters    = 0;
	aIndexOfFirstUnconvertibleCharacter = aDes.Length();

	if (SmsPDU().TextCompressed())
		{
		return EFalse;
		}

	return SmsPDU().UserData().IsSupportedL(aDes, aNumberOfUnconvertibleCharacters,
                                            aIndexOfFirstUnconvertibleCharacter);
	} // CSmsMessage::IsSupportedL


/**
 *  Tests if a buffer can be encoded without loss of information.
 *  
 *  @param aDes                                 The buffer to test for encoding
 *  @param aNumberOfUnconvertibleCharacters     On return, the number of 
 *                                              unconvertible characters
 *  @param aNumberOfDowngradedCharacters        On return, the number of 
 *                                              characters downgraded
 *  @param aNumberRequiringAlternativeEncoding  On return, the number of 
 *                                              characters needing alternative
 *                                              encoding support
 *  @param aIndexOfFirstUnconvertibleCharacter  On return, the index of the
 *                                              first unconvertible character
 * 
 *  @return True if aDes can be encoded without loss of information
 * 
 *  @capability None
 */
EXPORT_C TBool CSmsMessage::IsSupportedL(const TDesC& aDes, TInt& aNumberOfUnconvertibleCharacters,
		                                 TInt& aNumberOfDowngradedCharacters,
		                                 TInt& aNumberRequiringAlternativeEncoding,
		                                 TInt& aIndexOfFirstUnconvertibleCharacter) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ISSUPPORTEDL1_1, "[2] CSmsMessage::IsSupportedL()");

	__ASSERT_DEBUG(TextPresent(), Panic(KGsmuPanicTextNotPresent));

	aNumberOfUnconvertibleCharacters    = 0;
	aNumberOfDowngradedCharacters       = 0;
	aNumberRequiringAlternativeEncoding = 0;
	aIndexOfFirstUnconvertibleCharacter = aDes.Length();

	if (SmsPDU().TextCompressed())
		{
		return EFalse;
		}

	return SmsPDU().UserData().IsSupportedL(aDes, iAdditionalInfo->Alternative7bitEncoding(),
											aNumberOfUnconvertibleCharacters,
	                                        aNumberOfDowngradedCharacters,
                                            aNumberRequiringAlternativeEncoding,
                                            aIndexOfFirstUnconvertibleCharacter);
	} // CSmsMessage::IsSupportedL


/**
 *  Encodes message PDUs as an array of TGsmSms objects.
 *  
 *  Note, this function should only be called after EncodeIntoSinglePDUL() as EncodeBufferL() now
 *  automatically prepares PDUs for concatenation.
 *  
 *  @param aSmsArray (In) an empty array. On return, one or more encoded TGsmSms.
 *  @param aReference Sets a Concatenated Message Reference (default 0)
 *  @capability None
 */
EXPORT_C void CSmsMessage::EncodeMessagePDUsL(CArrayFix<TGsmSms>& aSmsArray, TInt aReference)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ENCODEMESSAGEPDUSL_1, "CSmsMessage::EncodeMessagePDUsL(): aReference=%d", aReference);

	__ASSERT_DEBUG((aSmsArray.Count()==0),Panic(KGsmuPanicSmsArrayNotEmpty));
	
	if (TextPresent())
		{
		if (SmsPDU().TextCompressed())
			{
			User::Leave(KErrNotSupported);
			}
		else
			{
			TInt  unconvertedChars, downgradedChars, freeUDUnitsInLastPDU;
				
			EncodeBufferL(aSmsArray, aReference, *iBuffer, unconvertedChars,
					      downgradedChars, freeUDUnitsInLastPDU);
			}
		}
	else
		{
        if(SmsPDU().Type()==CSmsPDU::ESmsCommand)
            {
            //Commands never contain text, so commands only encoded
            // in this branch.
            //
            // However  Commands don't support information
            // elements (per 23.040 6.5) and the CSmsIEOperation class
            // do not allow control information elements to be added
            // to Commands, hence the next line is commented out.
            // PrepareCommandMessageL();
            }

 		TGsmSms sms;
 		SmsPDU().EncodeMessagePDUL(sms);
 		aSmsArray.AppendL(sms);
		}
	} // CSmsMessage::EncodeMessagePDUsL


/**
 *  @internalComponent
 *  
 *  This method copies information elements from the collections contained in
 *  CSmsMessageAdditionalAttributes into the working PDU.
 *  
 *  Command Messages comprise only 1 PDU. Therefore all information
 *  elements must be present in the working PDU when encoding takes place.
 *  If there is not enough space in the PDU, the message encoding will fail.
 *  
 *  The 23.040 has not defined any information elements that are contained in
 *  Command PDUs. This method has been provided to maintain consistency with
 *  previous GSMU implementations.
 *  
 *  @leave KErrAlreadyExists or KErrAlreadyExists
 */
void CSmsMessage::PrepareCommandMessageL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_PREPARECOMMANDMESSAGEL_1, "CSmsMessage::PrepareCommandMessageL()");

    if (SmsPDU().Type()==CSmsPDU::ESmsCommand)
        {
        CSmsCommand& command = static_cast<CSmsCommand&>(SmsPDU());

        for (TUint8 category = 0; category < TSmsInformationElementCategories::ENumberOfCategories; category++)
            {
            switch(category)
                {
                case TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly:
                case TSmsInformationElementCategories::ECtrlSingleInstanceOnly:
                case TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed:
                    {
                    for (TUint j = 0; j < iAdditionalInfo->NumberOfControlInformationElements((CSmsMessageAdditionalAttributes::TCategory) category); j++)
                        {
                        CSmsInformationElement& informationElement = iAdditionalInfo->GetControlInformationElementL( ( (CSmsMessageAdditionalAttributes::TCategory) category), j);
                        TPtr8 data = informationElement.Data();
                        command.AddInformationElementL(informationElement.Identifier(), data);
                        }
                    break;
                    }
                default:
                OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_PREPARECOMMANDMESSAGEL_2, "CSmsMessage::PrepareCommandMessageL,default switch category = %d", category);
                    break;
                }
            }
        }
    } // CSmsMessage::PrepareCommandMessageL


/**
 *  Decodes message PDUs from an array of TGsmSms objects.
 *  
 *  The array contains a concatenated message, and must have the correct number
 *  of PDUs.
 *  
 *  @param aSmsArray Concatenated message
 *  @capability None
 */
EXPORT_C void CSmsMessage::DecodeMessagePDUsL(const CArrayFix<TGsmSms>& aSmsArray)
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_DECODEMESSAGEPDUSL_1, "CSmsMessage::DecodeMessagePDUsL(): PDUs=%d", aSmsArray.Count());

	TInt count=aSmsArray.Count();
	SetIsComplete(ETrue);
	CArrayPtrFlat<CSmsPDU>* smspduarray=new(ELeave) CArrayPtrFlat<CSmsPDU>(8);
	CleanupStack::PushL(smspduarray);
	// With CleanupResetAndDestroyPushL, only ResetAndDestroy method of smspduarray is invoked. 
	// smspduarray object is actually deleted with the first push. 
	// coverity[double_push]
	CleanupResetAndDestroyPushL(*smspduarray);  // Don't forget to destroy what the array elements point to.

	TInt i=0;
	TBool ismobileterminated=(Type()==CSmsPDU::ESmsDeliver) || (Type()==CSmsPDU::ESmsSubmitReport) || (Type()==CSmsPDU::ESmsStatusReport);
	for (; i<count; i++)
		{
		CSmsPDU* smspdu=CSmsPDU::NewL(aSmsArray[i],*iCharacterSetConverter,iFs, EFalse,ismobileterminated);
		CleanupStack::PushL(smspdu);
		TInt j=0;
		for (; (j<smspduarray->Count()) && (smspdu->ConcatenatedMessagePDUIndex()>(*smspduarray)[j]->ConcatenatedMessagePDUIndex()); j++)
			{
			}
		smspduarray->InsertL(j,smspdu);
		CleanupStack::Pop();
		}
	if (SmsPDU().TextCompressed())
		User::Leave(KErrNotSupported);
	else
		DecodeBufferL(*smspduarray,*iBuffer);

	CleanupStack::PopAndDestroy(2);  //  smspduarray elements (Reset and Destroy), smspduarray
	SetIsDecoded(ETrue);
	SmsPDU().UserData().SetBodyL(KNullDesC8);
	} // CSmsMessage::DecodeMessagePDUsL

/**
 *  Decodes partial complete message PDUs from an array of TGsmSms objects.
 *  
 *  The array contains a concatenated incomplete message, but the array of
 *  TGsmSms object must be in sequence.
 *  
 *	NOTE:
 *	It will not fully decode EMS and Control Information elements if it needs 
 *	to decode a partially complete class 0 SMS message. The assumption is that 
 *	it is necessary to display a partially complete message then only the raw 
 *	text will be displayed on the UI without any animation elements. The 
 *	rational for this design is that these information elements are normally 
 *	specified as being located at a specific position in a message. It will 
 *	complicate the design of client app if it has to re-calculate the position 
 *	of these elements when it receives a partially complete message under 
 *	Out Of Memory Conditions.
 
 *  @param aSmsArray Concatenated message
 *	@param aLastPartialCompleteMsg boolean value indicating this is the last
 *			incomplete message for a particular concatenated message.
 *  @capability None
 */
EXPORT_C void CSmsMessage::DecodePartialCompleteMessagePDUsL(const CArrayFix<TGsmSms>& aSmsArray, TBool aLastPartialCompleteMsg)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_DECODEPARTIALCOMPLETEMESSAGEPDUSL_1, "CSmsMessage::DecodePartialCompleteMessagePDUsL(): PDUs=%d", aSmsArray.Count());

	TInt count=aSmsArray.Count();
	SetIsComplete(EFalse);
	CArrayPtrFlat<CSmsPDU>* smspduarray=new(ELeave) CArrayPtrFlat<CSmsPDU>(8);
	CleanupStack::PushL(smspduarray);
	// With CleanupResetAndDestroyPushL, only ResetAndDestroy method of smspduarray is invoked. 
	// smspduarray object is actually deleted with the first push. 
	// coverity[double_push]
	CleanupResetAndDestroyPushL(*smspduarray);  // Don't forget to destroy what the array elements point to.

	TBool isMobileTerminated=(Type()==CSmsPDU::ESmsDeliver);
	/*
	The loop below goes through all the sms's received & check whether the received PDUs are of Class 0
	or not. The number of PDUs received must be less than the number of concatenated PDUs constitute this message.
	Then it order the PDUs in sequence for decoding purpose.
	*/
	for (TInt i=0; i<count; i++)
		{
		CSmsPDU* smspdu=CSmsPDU::NewL(aSmsArray[i],*iCharacterSetConverter,iFs, EFalse,isMobileTerminated);
		CleanupStack::PushL(smspdu);

		/*
		Check all the PDUs must be class 0 messages & also the number of received PDUs
		must be less than the number of concatenated PDUs constitute this message
		*/
		TSmsDataCodingScheme::TSmsClass  msgClass;
		if (smspdu->DataCodingSchemePresent()  &&  smspdu->Class(msgClass))
			{
			if (msgClass != TSmsDataCodingScheme::ESmsClass0)
				{
				User::Leave(KErrNotSupported);
				}
			__ASSERT_DEBUG((count < smspdu->NumConcatenatedMessagePDUs()),Panic(KGsmuPanicWrongNumberOfMessagePDUs));
			}
		else
			{
			User::Leave(KErrNotSupported);
			}
		//The below for statement finds the position in smspduarray where the PDU should be inserted.
		//The PDUs are put in sequence for decoding purpose.
		TInt j;
		for (j=0; (j<smspduarray->Count()) && (smspdu->ConcatenatedMessagePDUIndex()>(*smspduarray)[j]->ConcatenatedMessagePDUIndex()); j++)
			{
			}
		smspduarray->InsertL(j,smspdu);
		CleanupStack::Pop();
		}

	if (SmsPDU().TextCompressed())
		{
		User::Leave(KErrNotSupported);
		}
	else
		{
		DecodeOnlyTextL(*smspduarray,*iBuffer);
		}

	iVersion = CSmsMessage::ESmsIncompleteClass0MessageV;

	TInt startPDU = smspduarray->At(0)->ConcatenatedMessagePDUIndex();
	TInt endPDU = smspduarray->At(smspduarray->Count()-1)->ConcatenatedMessagePDUIndex();
	AddIncompleteMessageInfoL(startPDU, endPDU, aLastPartialCompleteMsg);

	CleanupStack::PopAndDestroy(2);  //  smspduarray elements (Reset and Destroy), smspduarray
	SetIsDecoded(ETrue);
	SmsPDU().UserData().SetBodyL(KNullDesC8);	
	}

CSmsMessage::CSmsMessage(RFs& aFs, CSmsBufferBase* aBuffer):
    iFlags(0),
    iStatus(NMobileSmsStore::EStoredMessageUnread),
    iLogServerId(KLogNullId),
    iBuffer(aBuffer),
    iFs(aFs),
	iSlotArray(8),
	iVersion(ESmsMessageV4),
	iAdditionalInfo(NULL)
    {
    iTime.UniversalTime();

    TBool result = SetUTCOffset(User::UTCOffset());
    __ASSERT_DEBUG(result, Panic(KGSMUPanicUserTimeZoneOffsetOutOfRange));
    } // NMobileSmsStore::EStoredMessageUnread

void CSmsMessage::ConstructL(const TGsmSms& aGsmSms, TBool aIsRPError,TBool aIsMobileTerminated)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CONSTRUCTL_1, "CSmsMessage::ConstructL()");

	iCharacterSetConverter=CCnvCharacterSetConverter::NewL();
	iInformationElementArray = new (ELeave) RPointerArray<CEmsInformationElement>(8);

	iAdditionalInfo = CSmsMessageAdditionalAttributes::NewL();
	CreateControlIEOperationsClassesL();
	CreateControlNonIEOperationsClassesL();

	iSmsPDU=CSmsPDU::NewL(aGsmSms,*iCharacterSetConverter,iFs, aIsRPError,aIsMobileTerminated);
	SetIsComplete(NumMessagePDUsL()==1);
	if (IsComplete() && TextPresent())
		{
		TSmsEncoding  encodingUsedInSegment = SmsPDU().NationalLanguageEncoding();
		
		CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(*iCharacterSetConverter,iFs,SmsPDU().Alphabet(),BinaryData());
		TPtrC nativeChars(converter->ConvertToNativeL(SmsPDU().UserData().Body(),
													  encodingUsedInSegment));
		MergeAlternative7bitEncoding(encodingUsedInSegment);
		
		if (SmsPDU().TextCompressed())
			User::Leave(KErrNotSupported);
		else
			iBuffer->InsertL(iBuffer->Length(),nativeChars);
		CleanupStack::PopAndDestroy(converter);
		CSmsPDU::TSmsPDUType pdutype= SmsPDU().Type();
		if(pdutype==CSmsPDU::ESmsCommand)
			{
			// This branch is never executed as TextPresent() never
			// returns true when pduType == command
            User::Leave(KErrArgument);
			}
		else if(SmsPDU().UserDataPresent())
			{
			TInt smscIndex(0);

			if(SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEISMSCControlParameters,smscIndex))
				{
				iAdditionalInfo->SetStatusReportSchemeL(EControlParametersScheme);
				}
			else
				{
				iAdditionalInfo->SetStatusReportSchemeL(EDefaultScheme);	
				}
				
            InstallControlInformationElementsL(SmsPDU().UserData(), 0);
			InstallEmsInformationElementsL(SmsPDU().UserData(),0);
			UpdateUserPromptAndODIElementsStartPosition();
			}
		}
	SetIsDecoded(IsComplete());

	if (TextPresent())
		SmsPDU().UserData().SetBodyL(KNullDesC8);
	} // CSmsMessage::ConstructL


void CSmsMessage::ConstructL(CSmsPDU::TSmsPDUType aType,TBool aIsRPError)
	{
    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CONSTRUCTL1_1, "CSmsMessage::ConstructL(): aType=%d, aIsRPError=%d", (TInt) aType,aIsRPError);

	iCharacterSetConverter=CCnvCharacterSetConverter::NewL();
	iInformationElementArray = new (ELeave) RPointerArray<CEmsInformationElement>(2);

	iAdditionalInfo = CSmsMessageAdditionalAttributes::NewL();
	CreateControlIEOperationsClassesL();
	CreateControlNonIEOperationsClassesL();

	iSmsPDU=CSmsPDU::NewL(aType,*iCharacterSetConverter,iFs,aIsRPError);
	SetIsComplete(ETrue);
	SetIsDecoded(ETrue);
	} // CSmsMessage::ConstructL


/**
 *  Counts the number of PDUs that this message will take. To do this the
 *  message will attempt to be encoded into 1 message, and if not into
 *  multiple messages. Due to the processing, the function should be used
 *  sparingly.
 * 
 *  @return  Number of PDUs required.
 */
TInt CSmsMessage::NumMessageEmsPDUsL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_NUMMESSAGEEMSPDUSL_1, "CSmsMessage::NumMessageEmsPDUsL()");

	//
	// Clear the concatenated flag, EncodeBufferL() will add it if needed.
	//
	SmsPDU().SetTextConcatenatedL(EFalse, EFalse);

	//
	// Attempt to encode to a single PDU, and if that fails then attempt to
	// encode to a set of PDUs.
	//
	CArrayFixFlat<TGsmSms>*  tmpArray = new (ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(tmpArray);
	TInt numMsgs = 1;
	
	if (!EncodeIntoSinglePDUL(*tmpArray))
		{
		TInt  unconvertedChars, downgradedChars, freeUDUnitsInLastPDU;
		
		EncodeBufferL(*tmpArray, 0, *iBuffer, unconvertedChars,
			          downgradedChars, freeUDUnitsInLastPDU, EFalse);

		numMsgs = iAdditionalInfo->SmsPDUArray().Count();
		}

	CleanupStack::PopAndDestroy(tmpArray);
	
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_NUMMESSAGEEMSPDUSL_2, "CSmsMessage::NumMessageEmsPDUsL() returns %d", numMsgs);

	return numMsgs;
	} // CSmsMessage::NumMessageEmsPDUsL


/**
 *  @internalComponent
 *  
 *  This method removes all non mandatory information elements from the
 *  working PDU.
 *  
 *  The working PDU is the PDU which is currently being encoded or decoded.
 *  
 *  @leave KErrAlreadyExists or KErrAlreadyExists
 */
void CSmsMessage::ResetWorkingPDUL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_RESETWORKINGPDUL_1, "CSmsMessage::ResetWorkingPDUL()");
	
	CSmsUserData& uData = SmsPDU().UserData();
	//remove non-mandatory EMS information elements
	for (TInt a=uData.NumInformationElements(); a>0; --a)
        {
        const CSmsInformationElement& ie = uData.InformationElement(a-1);

         TSmsInformationElementCategories::TInformationElementCategory category;

        if (TSmsInformationElementCategories::GetCategoryDefinition(ie.Identifier(), category)  &&
        	(category !=  TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUAndWithIdenticalValues)    &&
            (category !=  TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU) &&
            (category !=  TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUMultipleInstancesPerPDU))
            {
            uData.RemoveInformationElement(a-1);
            }
        }
    // reset user body
    uData.SetBodyL(KNullDesC8);
    } // CSmsMessage::ResetWorkingPDUL


void CSmsMessage::CorrectFormattingL(TUint aCharsAddedToCurrentPDU,
									 RPointerArray<CEmsInformationElement>& aCorrectedFormattingIEArray,
									 TUint aCharsAlreadyAdded)
	{
	OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CORRECTFORMATTINGL_1, "CSmsMessage::CorrectFormattingL(): aCharsAddedToCurrentPDU=%u, aCharsAlreadyAdded=%u",aCharsAddedToCurrentPDU, aCharsAlreadyAdded);

	CSmsUserData& uData = SmsPDU().UserData();
	for (TInt a= 0; a < uData.NumInformationElements(); a++)
		{
		CSmsInformationElement& ie = uData.InformationElement(a);
		if (CSmsInformationElement::ESmsEnhancedTextFormatting==ie.Identifier())
			{
			CEmsFormatIE& formatIE=static_cast<CEmsFormatIE&>(ie);

			TUint oldFormatLen=formatIE.FormatLength(); //
			if(aCharsAddedToCurrentPDU < formatIE.StartPosition() + oldFormatLen)
				{
				TUint newFormatLen=aCharsAddedToCurrentPDU - formatIE.StartPosition();
				formatIE.SetFormatLength(newFormatLen);

				// Now, re-encode the information element - this is because we have
				// changed a message in the UserData which is using EMS elements as
				// SMS IEs. Encoding is NOT automatic
				formatIE.EncodeInformationElementL();

				if((TInt)(aCharsAlreadyAdded+aCharsAddedToCurrentPDU) < iBuffer->Length())
					{
					CEmsFormatIE* newie=static_cast<CEmsFormatIE*>(formatIE.DuplicateL());
					CleanupStack::PushL(newie);
					newie->SetFormatLength(oldFormatLen - newFormatLen);
					newie->SetStartPosition(aCharsAlreadyAdded+aCharsAddedToCurrentPDU);
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CORRECTFORMATTINGL_2, "CSmsMessage::CorrectFormattingL Count=%d",aCorrectedFormattingIEArray.Count());
					User::LeaveIfError(aCorrectedFormattingIEArray.Append(newie));
					CleanupStack::Pop(newie);
					}
				}
			}
		}
	} // CSmsMessage::CorrectFormattingL


void CSmsMessage::CorrectFormattingInSinglePDUL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CORRECTFORMATTINGINSINGLEPDUL_1, "CSmsMessage::CorrectFormattingInSinglePDUL()");

	CSmsUserData& uData = SmsPDU().UserData();
	for (TInt a= 0; a < uData.NumInformationElements(); a++)
		{
		CSmsInformationElement& ie = uData.InformationElement(a);
		if (CSmsInformationElement::ESmsEnhancedTextFormatting==ie.Identifier())
			{
			CEmsFormatIE& formatIE=static_cast<CEmsFormatIE&>(ie);
			TUint oldFormatLen=formatIE.FormatLength(); //
			if(iBuffer->Length() < (TInt)(formatIE.StartPosition()+ oldFormatLen))
				{
				TUint newFormatLen=iBuffer->Length() - formatIE.StartPosition();
				formatIE.SetFormatLength(newFormatLen);

				// reencode
				formatIE.EncodeInformationElementL();
				}
			}
		}
	} // CSmsMessage::CorrectFormattingInSinglePDUL


/**
 *  Adds a copy of the current PDU to the PDU array used to record all the
 *  PDUs being encoded in a concatentated message.
 * 
 *  @param  aDoEncode  If true the PDU is added and updated. If false only a
 *                     NULL placeholder is stored. This allows the number of
 *                     PDUs to be quickly counted if the encoded PDUs are of
 *                     no further use.
 */
void CSmsMessage::AddCurrentPDUToPDUArrayL(TBool aDoEncode)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDCURRENTPDUTOPDUARRAYL_1, "CSmsMessage::AddCurrentPDUToPDUArrayL(): Adding PDU number %d",iAdditionalInfo->SmsPDUArray().Count() + 1);
	
	//
	// Maximum number of PDU is 255, so if we have that already then we cannot
	// continue.
	//
	TInt  numPDUs = iAdditionalInfo->SmsPDUArray().Count();

	if (numPDUs >= 255)
		{
		User::Leave(KErrOverflow);
		}
	
	//
	// We only do most of the work if we are actually encoding.
	//
	if (aDoEncode)
		{
		//
		// Update the Concatenated Message PDU numbers in the current PDU. This
		// means that at least the last PDU has the correct values (to update
		// all PDUs when a new PDU is added would be too slow, so we do that
		// only once at the end).
		//
		if (numPDUs > 0)
			{
			SmsPDU().SetConcatenatedMessagePDUIndex(numPDUs+1);
			SmsPDU().SetNumConcatenatedMessagePDUs(numPDUs+1);
			}
	
		//
		// Create copy of the current PDU and store it in the array...
		//
		CSmsPDU*  newPDU = SmsPDU().DuplicateL();
		CleanupStack::PushL(newPDU);
		iAdditionalInfo->SmsPDUArray().AppendL(newPDU);
		CleanupStack::Pop(newPDU);
		}
	else
		{
		//
		// Otherwise just append a NULL value to allow the data PDUs to be
		// counted...
		//
		iAdditionalInfo->SmsPDUArray().AppendL(NULL);
		}
	} // CSmsMessage::AddCurrentPDUToPDUArrayL


TBool CSmsMessage::AddIEToUserDataL(CEmsInformationElement* aIE, TInt aCharsAlreadyAdded,TUint& aCharsAddedToCurrentPDU,CSmsEMSBufferSegmenter& aSeg)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDIETOUSERDATAL_1, "CSmsMessage::AddIEToUserDataL()");

	TBool ieAdded=EFalse;
	if (SmsPDU().UserData().EmsInformationElementWillFitL(aIE,aSeg,aCharsAddedToCurrentPDU))
		{
		CEmsInformationElement* newIE=aIE->DuplicateL();
		newIE->SetStartPosition(newIE->StartPosition()-aCharsAlreadyAdded);
	    CleanupStack::PushL(newIE);
		SmsPDU().UserData().AddEmsInformationElementL(newIE);
		CleanupStack::Pop(newIE);
		ieAdded=ETrue;
		}
	return ieAdded;
	} // CSmsMessage::AddIEToUserDataL


/**
 *  Fills the current PDU with the number of specified native chars.
 *  
 *  @param aSeg Buffer containing the native chars
 *  @param aNumChars number of native characters to add.
 *  @param aEncoding  SMS Encoding if required.
 * 
 *  @return TInt Number of native characters added
 */
TInt CSmsMessage::FillPduL(CSmsEMSBufferSegmenter& aSeg, TInt aNumChars, TSmsEncoding aEncoding)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_FILLPDUL_1, "CSmsMessage::FillPduL()");

	TUint maxUDUnitsREmaining=SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();

	if (aNumChars==0 || maxUDUnitsREmaining==0)
		{
		return 0;
		}

	HBufC8* buf=HBufC8::NewMaxLC(2*aNumChars);
	TPtr8 ptr(buf->Des());
	TInt numberOfUnconvertibleCharacters(0); 
	TInt numberOfDowngradedCharacters(0);

	TInt nativeLength = aSeg.SegmentL(ptr, aNumChars, maxUDUnitsREmaining,
			                          numberOfUnconvertibleCharacters,
			                          numberOfDowngradedCharacters,
			                          aEncoding);

	SmsPDU().UserData().AppendBodyL(ptr);

	CleanupStack::PopAndDestroy(buf);
	return nativeLength;
	} // CSmsMessage::FillPduL


/**
 *  This method copies control information elements from the specified
 *  collection into the working PDU.
 *  
 *  @param aCategory        The category of control information element to be
 *                          added to the PDU.
 *  @param aMandatoryInPDU  Specifies whether the specified category of information
 *                          element must be added to this PDU.
 *  @param aDoEncode        Flag indicating if encoding is really wanted, or just the
 *                          calculation of the number of PDUs.
 *
 *  @leave KErrArgument     If the category is mandatory but one or more elements
 *                          cannot be added then the encoding will fail.
 *  @leave KErrOverFlow     If the information element is too big to be encoded into
 *                          a PDU that contains only the mandatory elements.
 */
void CSmsMessage::AddControlInformationElementsToMultiSegmentMessageL(TSmsInformationElementCategories::TInformationElementCategory aCategory,
			                                                          TBool aMandatoryInPDU, TBool aDoEncode)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDCONTROLINFORMATIONELEMENTSTOMULTISEGMENTMESSAGEL_1, "CSmsMessage::AddControlInformationElementsToMultiSegmentMessageL()");

    TUint numberOfInformationElements = iAdditionalInfo->NumberOfControlInformationElements(aCategory);

    for (TInt i = 0; i < numberOfInformationElements; i++)
        {
        CSmsInformationElement& informationElement = iAdditionalInfo->GetControlInformationElementL( aCategory, i);

        CSmsInformationElement* cloneInformationElement = CSmsInformationElement::NewL(informationElement.Identifier(),informationElement.Data());
        CleanupStack::PushL(cloneInformationElement);

        if (SmsPDU().UserData().ControlInformationElementWillFitL(cloneInformationElement))
            {
            SmsPDU().UserData().UpdateInformationElementArrayL(informationElement.Identifier(),informationElement.Data());
            }
        else if (aMandatoryInPDU)
                {
                // Error Scenario, cannot fit the mandatory PDU into the User Data
                User::Leave(KErrArgument);
                }
             else
                {
                // Close off PDU and transfer to output array
                AddCurrentPDUToPDUArrayL(aDoEncode);
                ResetWorkingPDUL();

                // Now test whether the information element will fit into a PDU at all.
                // It is possible to make Enhanced Voice Mail Information Elements
                // bigger than the available space in the PDU.
                TBool canFit = SmsPDU().UserData().ControlInformationElementWillFitL(cloneInformationElement);
                if (canFit == EFalse)
                    {
                    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDCONTROLINFORMATIONELEMENTSTOMULTISEGMENTMESSAGEL_2, "CSmsMessage::AddControlInformationElementsToMultiSegmentMessage, IE too bit to fit in any PDUL");
                    User::Leave(KErrArgument);
                    }
                i--;
                }

        CleanupStack::PopAndDestroy(cloneInformationElement);
        }
    } // CSmsMessage::AddControlInformationElementsToMultiSegmentMessageL


/**
 *  This method copies control information elements from the collections of
 *  control information elements contained in CSmsAdditionalAttributes into the
 *  working PDU.
 *  
 *  @param aDoEncode  Flag indicating if encoding is really wanted, or just the
 *                    calculation of the number of PDUs.
 *
 *  @leave KErrArgument  If the category is mandatory but one or more elements
 *                       cannot be added then the encoding will fail.
 */
void CSmsMessage::AddControlInformationElementsToMultiSegmentMessageL(TBool aDoEncode)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDCONTROLINFORMATIONELEMENTSTOMULTISEGMENTMESSAGEL1_1, "CSmsMessage::AddControlInformationElementsToMultiSegmentMessageL() 1");

    TBool mandatoryInEachPDU = ETrue;
    AddControlInformationElementsToMultiSegmentMessageL(TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly,
    													mandatoryInEachPDU, aDoEncode);
    mandatoryInEachPDU = EFalse;
    AddControlInformationElementsToMultiSegmentMessageL(TSmsInformationElementCategories::ECtrlSingleInstanceOnly,
    													mandatoryInEachPDU, aDoEncode);
    AddControlInformationElementsToMultiSegmentMessageL(TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed,
    													mandatoryInEachPDU, aDoEncode);
    } // CSmsMessage::AddControlInformationElementsToMultiSegmentMessageL


/**
 *  Attempts to add EMS IE to the current PDU.
 *  
 *  @param aSegmenter                  Reference to buffer segmenter.
 *  @param aCharsAddedToCurrentPDU     Returned number of characters added to current PDU.
 *  @param aDoEncode                   Flag indicating if encoding is really wanted,
 *                                     or just the calculation of the number of PDUs.
 *  @param aEncoding                   SMS 7bit encoding if appropriate.
 *  @param aCorrectedFormatingIEArray  Array of IEs that have to be placed in following
 *                                     PDUs (e.g. corrected).
 *  @param aCurrEMSIEno                Current IE number.
 *  @param aCharsAlreadyAdded          Returned number of characters added.
 *
 *  @return Whether EMS IE has beenn added, in singleMode returns true if there are no more than one PDU.
 */
TBool CSmsMessage::AddEMSInformationElementsToMultiSegmentMessageL(CSmsEMSBufferSegmenter& aSegmenter,
																   TUint& aCharsAddedToCurrentPDU,
																   TBool aDoEncode,
																   TSmsEncoding& aEncoding,
																   RPointerArray<CEmsInformationElement>& aCorrectedFormatingIEArray,
																   TUint& aCurrEMSIEno,
																   TUint& aCharsAlreadyAdded)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDEMSINFORMATIONELEMENTSTOMULTISEGMENTMESSAGEL_1, "CSmsMessage::AddEMSInformationElementsToMultiSegmentMessageL()");

	TUint startPosition=0;

	 // number of chars added to the current PDU
	TUint no=iInformationElementArray->Count();
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDEMSINFORMATIONELEMENTSTOMULTISEGMENTMESSAGEL_2, "CSmsMessage::AddEMSInformationElementsToMultiSegmentMessageL no of IE %d",no);
	CEmsInformationElement* ie = NULL;
	TUint msgLen=iBuffer->Length();
	TUint filledChars=0;

	while(aCurrEMSIEno < no || aCorrectedFormatingIEArray.Count() > 0)
	{
		ie = NULL;
		TBool correction=EFalse;
		if(aCurrEMSIEno < no)
			{
			ie = (*iInformationElementArray)[aCurrEMSIEno];
			startPosition=ie->StartPosition();
			}

		if(ie == NULL || ( startPosition != aCharsAlreadyAdded) )
		{
			if(aCorrectedFormatingIEArray.Count() > 0)
			{
				correction=ETrue;
				ie = aCorrectedFormatingIEArray[0];
				startPosition=ie->StartPosition();
			}
		}

		__ASSERT_ALWAYS(ie !=NULL, User::Leave(KErrCorrupt));

		if(startPosition <= msgLen)
			{
			__ASSERT_DEBUG(startPosition>=aCharsAlreadyAdded, User::Leave(KErrUnderflow));
			startPosition -= aCharsAlreadyAdded; // startPosition now relative to current PDU.

			// Add all chars upto startposition.
			filledChars=0;

			if(startPosition > aCharsAddedToCurrentPDU)
				{
				filledChars = FillPduL(aSegmenter, startPosition-aCharsAddedToCurrentPDU, aEncoding);
				aCharsAddedToCurrentPDU+=filledChars;
				}

			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDEMSINFORMATIONELEMENTSTOMULTISEGMENTMESSAGEL_3, "CSmsMessage::AddEMSInformationElementsToMultiSegmentMessageL: filled %d chars", filledChars);

			if (aCharsAddedToCurrentPDU==startPosition)
				{
				// Try adding the IE.
				if (AddIEToUserDataL(ie, aCharsAlreadyAdded,aCharsAddedToCurrentPDU,aSegmenter))
					{
					if(correction)
					{
						aCorrectedFormatingIEArray.Remove(0);
						// aCorrectedFormatingIEArray[0] has been removed. In next loop, ie will point another element. So there is no double free.
						// coverity[double_free]
						delete ie;
						correction=EFalse;
					}
					else aCurrEMSIEno++;
					}
				else
					{
					// Information Element will not fit send PDU
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDEMSINFORMATIONELEMENTSTOMULTISEGMENTMESSAGEL_4, "CSmsMessage::AddEMSInformationElementsToMultiSegmentMessageL: ie will not fit send Message");
					CorrectFormattingL(aCharsAddedToCurrentPDU,aCorrectedFormatingIEArray,aCharsAlreadyAdded);

					aCharsAlreadyAdded += aCharsAddedToCurrentPDU;
					aCharsAddedToCurrentPDU=0;

					AddCurrentPDUToPDUArrayL(aDoEncode);
					ResetWorkingPDUL();

					//
					// Find the encoding method for the next segment...
					//
					//aEncoding = ESmsEncodingNone;
					//TRAP_IGNORE(aEncoding = aSegmenter->FindBestAlternativeEncodingL(iAdditionalInfo->iAlternative7bitEncoding,
					//												                   maxBodyLength));
					SmsPDU().SetNationalLanguageEncodingL(aEncoding);
					}
				}
			else
				{
				// native chars upto start position will not fit send PDu.
				OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDEMSINFORMATIONELEMENTSTOMULTISEGMENTMESSAGEL_5, "CSmsMessage::AddEMSInformationElementsToMultiSegmentMessageL: PDU is filled with chars sending");

				CorrectFormattingL(aCharsAddedToCurrentPDU,aCorrectedFormatingIEArray,aCharsAlreadyAdded);

				aCharsAlreadyAdded += aCharsAddedToCurrentPDU;
				aCharsAddedToCurrentPDU=0;

				AddCurrentPDUToPDUArrayL(aDoEncode);
				ResetWorkingPDUL();

				//
				// Find the encoding method for the next segment...
				//
				//aEncoding = ESmsEncodingNone;
				//TRAP_IGNORE(aEncoding = aSegmenter->FindBestAlternativeEncodingL(iAdditionalInfo->iAlternative7bitEncoding,
				//												                   maxBodyLength));
				SmsPDU().SetNationalLanguageEncodingL(aEncoding);
				}
			}
		else
			{
			aCurrEMSIEno++;
			}
		}		// end of while loop for all IEs

	return ETrue;
	} // CSmsMessage::AddEMSInformationElementsToMultiSegmentMessageL


/**
 *  Attempts to add EMS IE to the current PDU
 *  
 *  @param aSegmenter
 *  @return TBool whether EMS IEs have been added to segment
 */
TBool CSmsMessage::AddEMSInformationElementsToSingleSegmentMessageL(CSmsEMSBufferSegmenter& aSegmenter,
																	TSmsEncoding aEncoding)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDEMSINFORMATIONELEMENTSTOSINGLESEGMENTMESSAGEL_1, "CSmsMessage::AddEMSInformationElementsToSingleSegmentMessageL()");

	TUint charsAddedToCurrentPDU=0;
	TUint numOfEmsIE=iInformationElementArray->Count();
	TUint currEmsIEindex=0;
	CEmsInformationElement* ie = NULL;
	TUint startPosition=0;
	TUint filledChars=0;

	while(currEmsIEindex < numOfEmsIE)
		{
		ie = (*iInformationElementArray)[currEmsIEindex];
		startPosition=ie->StartPosition();

		if (startPosition > charsAddedToCurrentPDU)
			{
			filledChars = FillPduL(aSegmenter, startPosition-charsAddedToCurrentPDU, aEncoding);
			charsAddedToCurrentPDU+=filledChars;
			}

		if (charsAddedToCurrentPDU != startPosition ||
			!AddIEToUserDataL(ie, 0,charsAddedToCurrentPDU,aSegmenter))
			{
			return EFalse;
			}

		++currEmsIEindex;
		}

	return ETrue;
	} // CSmsMessage::AddEMSInformationElementsToSingleSegmentMessageL


/**
 *  Encode the PDU into an TGsmSms array. The Text and the EMS objects are
 *  laid out with respect to start position locations and encoded into the
 *  correct TGsmSms object.
 *  
 *  @leave KErrUnderflow if the Ems objects are not in Start position order.
 *
 *  @param aSmsArray               Returned array of newly created TSms objects
 *                                 containing the encoded message.
 *  @param aReference              Unique reference number to be given to the
 *                                 TGsmSms objects.
 *  @param aBuffer                 Body Text buffer of the message.
 *  @param aUnconvertedChars       Exit param for the number of characters not converted.
 *  @param aDowngradedChars        Exit param for the number of characters downgraded.
 *  @param aFreeUDUnitsInLastPDU   Exit param for the number of characters free
 *                                 in the last PDU.
 *  @param aDoEncode               Flag indicating if encoding is really wanted,
 *                                 or just the calculation of the number of PDUs.
 */
void CSmsMessage::EncodeBufferL(CArrayFix<TGsmSms>& aSmsArray, TInt aReference,
								const CSmsBufferBase& aBuffer,
								TInt& aUnconvertedChars, TInt& aDowngradedChars,
						        TInt& aFreeUDUnitsInLastPDU, TBool aDoEncode)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_1, "CSmsMessage::EncodeBufferL()");

	aUnconvertedChars     = 0;
	aDowngradedChars      = 0;
	aFreeUDUnitsInLastPDU = 0;
	
	//
	// Reset the working array of PDUs...
	//
	iAdditionalInfo->SmsPDUArray().ResetAndDestroy();

	TUint currEMSIEno(0);
	
	TInt smscIndex(0);
	TBool smscPresent(EFalse);
	
	TInt emailOverallHeaderLength(0);
	TInt emailIndex(0);
	TBool emailPresent(EFalse);
	
	if(SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEISMSCControlParameters,smscIndex))
		{
		smscPresent=ETrue;
		}
	if(SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIRFC822EmailHeader,emailIndex))
		{
		emailPresent=ETrue;
		emailOverallHeaderLength=SmsPDU().UserData().InformationElement(emailIndex).Data()[0];
		}

	//
	// Create Array for corrected format elements. These will be elements that
	// have to be moved into subsequent PDUs due to lack of space in the
	// current PDU.
	//
	RPointerArray<CEmsInformationElement>  correctedFormatingIEArray(2);
	CleanupResetAndDestroyPushL(correctedFormatingIEArray);

	//
	// Reset the working PDU. This safes time be keeping fields we need and
	// droping any data from previous PDUs.
	//
	ResetWorkingPDUL();

	//
	// Automatically prepare for concatenation (any single PDU messages would
	// have been handled by EncodeIntoSinglePDUL() previously).
	//
	SmsPDU().SetTextConcatenatedL(ETrue,iIs16BitConcatenation);

	TInt maxBodyLength=SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();

	//
	// Add any elements that are required to be present, and ensure they
	// have correct data stored (for example if we are recycling the working PDU).
	//
	AddControlInformationElementsToMultiSegmentMessageL(aDoEncode);

	//
	// Create EMS Segmenter and Buffer Converter...
	//
	CSmsAlphabetConverter*  converter = CSmsAlphabetConverter::NewLC(*iCharacterSetConverter, iFs,SmsPDU().Alphabet(),
			                                                         BinaryData());
	CSmsEMSBufferSegmenter*  segmenter = CSmsEMSBufferSegmenter::NewLC(*converter, aBuffer, maxBodyLength);
	TBool  isUnicode = (SmsPDU().Alphabet() == TSmsDataCodingScheme::ESmsAlphabetUCS2);

	TBool informationToSend     = ETrue;
	TUint charsAdded2CurrentPDU = 0;
	TUint charsAlreadyAdded     = 0;

	//
	// Find the best alternative encoding for the current segment. This will be
	// set at the beginning of each PDU using the list of encoders.
	//
	TSmsEncoding  encodingToUse = ESmsEncodingNone;

	TRAP_IGNORE(encodingToUse = segmenter->FindBestAlternativeEncodingL(iAdditionalInfo->Alternative7bitEncoding(),
													                    aBuffer.Length()));
	SmsPDU().SetNationalLanguageEncodingL(encodingToUse);
	
	//
	// If there are information elements to add, add them to the first PDU (or
	// to the correct formatting array for later PDUs if more appropriate).
	//
	if(iInformationElementArray->Count() >0)
		{
		AddEMSInformationElementsToMultiSegmentMessageL(*segmenter, charsAdded2CurrentPDU,
														aDoEncode, encodingToUse,
														correctedFormatingIEArray,
														currEMSIEno, charsAlreadyAdded);
		}

	//
	// Allocate a temporary buffer to work with...
	//
	HBufC8* buf=HBufC8::NewMaxLC(maxBodyLength);
	TPtr8 ptr(buf->Des());

	//
	// While there is still data to encode see how much space is left in this
	// PDU and attempt to encode something into it...
	//
	while (segmenter->MoreL())
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_2, "CSmsMessage::EncodeBufferL - there is MoreL");
			
			//
			// Calculate the space left to use in this PDU...
			//
			TInt size = SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_3, "CSmsMessage::EncodeBufferL - remaining size in PDU is %d",size);

			//
			// While there is no space, correct the formatting (which may
			// introduce any un-placed EMS Elements into the corrected format
			// array) and then start a new PDU...
			//
			while (size==0)
				{
				//
				// Store any elements not yet placed into the array for later...
				//
				CorrectFormattingL(charsAdded2CurrentPDU, correctedFormatingIEArray, charsAlreadyAdded);

				//
				// Store this PDU, reset the working PDU and get ready to
				// continue with the next PDU...
				//
				aFreeUDUnitsInLastPDU = SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();
				AddCurrentPDUToPDUArrayL(aDoEncode);
				ResetWorkingPDUL();
				informationToSend=EFalse;
				charsAlreadyAdded+=charsAdded2CurrentPDU;
				charsAdded2CurrentPDU=0;

				//
				// Find the encoding method for the next segment...
				//
				//encodingToUse = ESmsEncodingNone;
				//TRAP_IGNORE(encodingToUse = segmenter->FindBestAlternativeEncodingL(iAdditionalInfo->iAlternative7bitEncoding,
				//												                    maxBodyLength));
				SmsPDU().SetNationalLanguageEncodingL(encodingToUse);

				//
				// Add any elements that can be placed now (from previous
				// PDUs and above)... 
				//
				OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_4, "CSmsMessage::EncodeBufferL: IE count  %d corrected  count %d",iInformationElementArray->Count(),correctedFormatingIEArray.Count() );
				if ((TUint)iInformationElementArray->Count() > currEMSIEno  ||
					correctedFormatingIEArray.Count() > 0)
					{
					AddEMSInformationElementsToMultiSegmentMessageL(*segmenter,charsAdded2CurrentPDU,
																	aDoEncode, encodingToUse,
																	correctedFormatingIEArray,
																	currEMSIEno,charsAlreadyAdded);
					}

				//
				// Calculate the space left remaining in this new PDU...
				//
				size = SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_5, "CSmsMessage::EncodeBufferL - remaining size in PDU is %d",size);
				}
			
			//
			// We have space in this PDU and no elements to place, so obtain
			// the next peice of text that can fit and append it.
			//
			segmenter->SegmentNextL(ptr, size, aUnconvertedChars, aDowngradedChars, encodingToUse);
			SmsPDU().UserData().AppendBodyL(ptr);

			TUint charsInSegment= isUnicode ? ptr.Length()/2 : ptr.Length();
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_6, "CSmsMessage::EncodeBufferL: segmenting added %d chars", charsInSegment);

			//
			// At this point the working PDU is either full (e.g. we filled the
			// remaining space with a chuck of text, or the is some space as we
			// stored the last part of the text.
			//
			// If there are any elements not yet stored, add them to the
			// formatting array...
			//
			charsAdded2CurrentPDU+=charsInSegment;
			CorrectFormattingL(charsAdded2CurrentPDU, correctedFormatingIEArray, charsAlreadyAdded);
			charsAlreadyAdded+=charsAdded2CurrentPDU;
			OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_7, "CSmsMessage::EncodeBufferL(): charsAlreadyAdded=%d", charsAlreadyAdded);
			
			//
			// Now store this PDU and reset the working PDU...
			//
			aFreeUDUnitsInLastPDU = SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();
			AddCurrentPDUToPDUArrayL(aDoEncode);
			ResetWorkingPDUL();
			informationToSend=EFalse;
			charsAdded2CurrentPDU=0;
			
			//
			// Find the encoding method for the next segment...
			//
			//encodingToUse = ESmsEncodingNone;
			//TRAP_IGNORE(encodingToUse = segmenter->FindBestAlternativeEncodingL(iAdditionalInfo->iAlternative7bitEncoding,
			//												                    maxBodyLength));
			SmsPDU().SetNationalLanguageEncodingL(encodingToUse);

			//
			// Add any elements that can be placed now given we have a new
			// empty PDU... 
			//
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_8, "CSmsMessage::EncodeBufferL: IE count  %d corrected  count %d",iInformationElementArray->Count(), correctedFormatingIEArray.Count() );
			if ((TUint)iInformationElementArray->Count() > currEMSIEno  ||
				correctedFormatingIEArray.Count() > 0)
				{
				AddEMSInformationElementsToMultiSegmentMessageL(*segmenter, charsAdded2CurrentPDU,
																aDoEncode, encodingToUse,
																correctedFormatingIEArray,
																currEMSIEno, charsAlreadyAdded);
				}
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_9, "CSmsMessage::EncodeBufferL end Moreloop");
			}
	CleanupStack::PopAndDestroy(buf);

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_10, "CSmsMessage::EncodeBufferL - last PDU");
	
	//
	// This is the last PDU. We need to check if there is a partial PDU left over
	// and add it if needed.
	//
	if (informationToSend)
		{
		aFreeUDUnitsInLastPDU = SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();
		AddCurrentPDUToPDUArrayL(aDoEncode);
		ResetWorkingPDUL();
		}

	//
	// At this point the segmenting and layouts of PDU is complete, so if we
	// find any more data something is wrong...
	//
	if (segmenter->MoreL()  ||  correctedFormatingIEArray.Count() > 0)
		{
		User::Leave(KErrCorrupt);
		}

	CleanupStack::PopAndDestroy(3, &correctedFormatingIEArray); // segmenter, converter & correctedFormatingIEArray

	//
	// By now the PDUs are segmented and correctly setup for encoding.
	// They have not been encoded yet, as this requires the PDU total to be known.
	// So we go through the array setting the final PDU number/total PDUs and
	// perform the encoding from CSmsPDU to the TGsmPdu object.
	//
	// Note: In the case were aDoEncode is EFalse, iSmsPDUArray will be full
	//       of NULL pointers as it is only the count that we need in that case.
	//
	TInt  numPDUs = iAdditionalInfo->SmsPDUArray().Count();

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEBUFFERL_11, "CSmsMessage::EncodeBufferL number of PDUs: %d", iAdditionalInfo->SmsPDUArray().Count());
	
	if (aDoEncode)
		{
		CSmsMessageAdditionalAttributes::CSmsStatusReportScheme&  scheme = iAdditionalInfo->GetStatusReportScheme();
		TGsmSms  gsmSms;
		
		for (TInt pdu = 0;  pdu < numPDUs;  pdu++)
			{
			if(scheme.Id() == EControlParametersScheme)
				{
				if(smscPresent)				
					{
					CSmsSMSCCtrlParameterOperations&  ieOp = (CSmsSMSCCtrlParameterOperations&)GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters);
					TUint8 octet(0);

					if (ieOp.GetStatusReport(pdu, octet) == KErrNone)
						{					
						iAdditionalInfo->SmsPDUArray()[pdu]->UpdateSMSCCtrlParameterL(octet);
						}
					}
				else
					{
					User::Leave(KErrNotFound);
					}
				}
			else if(scheme.Id() == ETPSRRScheme)
				{
				TSmsFirstOctet smsReportRequest;
				CSmsTPSRROperations& nonIEOp = (CSmsTPSRROperations&)GetOperationsForNonIEL(ESmsTPSRRParameter);
				smsReportRequest = nonIEOp.GetStatusReport(pdu);
								
				iAdditionalInfo->SmsPDUArray()[pdu]->UpdateTPSRRL(smsReportRequest);
				}

			if(emailPresent)				
				{
				iAdditionalInfo->SmsPDUArray()[pdu]->UpdateEmailHeaderDataL(emailOverallHeaderLength);
				}

			//
			// Set the concatenation Message Reference number and PDU numbers...
			//
			iAdditionalInfo->SmsPDUArray()[pdu]->UpdateConcatenationDataL(aReference, pdu+1, numPDUs);

			//
			// Encode this PDU...
			//
			
			TEncodeParams encodeParams;
			encodeParams.iTimeStamp = &Time();
			
			TTimeIntervalSeconds interval = UTCOffset();
			encodeParams.iTimeIntervalInSeconds = &interval;
						
			iAdditionalInfo->SmsPDUArray()[pdu]->EncodeMessagePDUL(gsmSms, &encodeParams);
	 		aSmsArray.AppendL(gsmSms);
			}

		//
		// Since the working PDU has been reset the data in it does not match
		// the data of the last PDU. Some tests in SMS Stack assume it does
		// and therefore to retain compatibility we decode the last PDU back
		// over the working PDU.
		//
		TGsmuLex8 lex(gsmSms.Pdu());
		SmsPDU().DecodeL(lex);
		}
	} // CSmsMessage::EncodeBufferL


/**
 *  Attempts to encode into the single PDU. This function is the private version
 *  to the public EncodeIntoSinglePDUL() function. It performs the work and also
 *  returns more information regarding the characters encoded.
 *  
 *  The Text and the EMS objects are laid out with respect to start position
 *  locations.
 *  
 *  @leave KErrUnderflow if the Ems objects are not in Start position order.
 *
 *  @param aSmsArray               Returned array of newly created TSms objects
 *                                 containing the encoded message.
 *  @param aUnconvertedChars       Exit param for the number of characters not converted.
 *  @param aDowngradedChars        Exit param for the number of characters downgraded.
 *  @param aFreeUDUnitsInLastPDU   Exit param for the number of characters free
 *                                 in the last PDU.
 */
TBool CSmsMessage::EncodeIntoSinglePDUL(CArrayFix<TGsmSms>& aSmsArray, TInt& aUnconvertedChars,
		                                TInt& aDowngradedChars, TInt& aFreeUDUnitsInLastPDU)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEINTOSINGLEPDUL_1, "CSmsMessage::EncodeIntoSinglePDUL()");

	__ASSERT_DEBUG((aSmsArray.Count()==0),Panic(KGsmuPanicSmsArrayNotEmpty));

	aUnconvertedChars     = 0;
	aDowngradedChars      = 0;
	aFreeUDUnitsInLastPDU = 0;

	iAdditionalInfo->SmsPDUArray().ResetAndDestroy();

	if (!TextPresent())
		{
        if(SmsPDU().Type()==CSmsPDU::ESmsCommand)
            {
            // Commands don't contain text, so commands
            // are only encoded in this branch.
            PrepareCommandMessageL();
            }

		TGsmSms sms;
		SmsPDU().EncodeMessagePDUL(sms);
		aSmsArray.AppendL(sms);
		return ETrue;
		}

	ResetWorkingPDUL();

    // find the length of all the control information elements
    TUint ieLength=0;
    for (TUint8 category = 0; category <  TSmsInformationElementCategories::ENumberOfCategories; category++)
        {
        switch (category)
            {
            case  TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly:
            case  TSmsInformationElementCategories::ECtrlSingleInstanceOnly:
            case  TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed:
                {
                for (TUint j = 0; j < iAdditionalInfo->NumberOfControlInformationElements((CSmsMessageAdditionalAttributes::TCategory) category); j++)
                    {
                    ieLength += iAdditionalInfo->GetControlInformationElementL( ( (CSmsMessageAdditionalAttributes::TCategory) category), j).Length();
                    }
                break;
                }
            default:
                break;
            }
        }
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEINTOSINGLEPDUL_2, "CSmsMessage::EncodeIntoSinglePDUL, ctrl elem len = %d", ieLength);

	CEmsInformationElement* emsIE =NULL;
	for (TInt num=0; num<iInformationElementArray->Count();num++)
		{
		emsIE = (*iInformationElementArray)[num];
        ieLength+=emsIE->Length();
		}

	TInt remainInBody=SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining(ieLength);

	TInt msgLength=MessageLengthL(); // in octets

	if( msgLength > remainInBody) return EFalse;

    OstTraceDefExt3(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ENCODEINTOSINGLEPDUL_3, "CSmsMessage::EncodeIntoSinglePDUL, ie len = %d, remainInBody = %d, msgLength = %d", ieLength, msgLength, remainInBody);
    //  add all control information elements into working PDU.
    //
    for (TUint8 category = 0; category <  TSmsInformationElementCategories::ENumberOfCategories; category++)
        {
        switch (category)
            {
            case  TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly:
            case  TSmsInformationElementCategories::ECtrlSingleInstanceOnly:
            case  TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed:
                {
                for (TUint j = 0; j < iAdditionalInfo->NumberOfControlInformationElements((CSmsMessageAdditionalAttributes::TCategory) category); j++)
                    {
                    CSmsInformationElement& informationElement = iAdditionalInfo->GetControlInformationElementL( ( (CSmsMessageAdditionalAttributes::TCategory) category), j);

                    CSmsInformationElement* cloneInformationElement = CSmsInformationElement::NewL(informationElement.Identifier(),informationElement.Data());
                    CleanupStack::PushL(cloneInformationElement);
                    TBool willFit = SmsPDU().UserData().ControlInformationElementWillFitL(cloneInformationElement);
                    CleanupStack::PopAndDestroy(cloneInformationElement);

                    if (willFit)
                        {
                        SmsPDU().UserData().UpdateInformationElementArrayL(informationElement.Identifier(),informationElement.Data());
                        }
                    else
                        {
                        ResetWorkingPDUL();
                        return EFalse;
                        }
                    }
                break;
                }
            default:
                break;
            }
        }
		
	EncodingTPSRRFromSchemesIntoSinglePDUL();

	TInt maxBodyLength=SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();
	CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(*iCharacterSetConverter,iFs,SmsPDU().Alphabet(),BinaryData());
	CSmsEMSBufferSegmenter* segmenter=CSmsEMSBufferSegmenter::NewLC(*converter,*iBuffer, maxBodyLength);

	//
	// Find the best alternative encoding for the current segment. This will be
	// set at the beginning of each PDU using the list of encoders.
	//
	TSmsEncoding  encodingToUse = ESmsEncodingNone;

	TRAP_IGNORE(encodingToUse= segmenter->FindBestAlternativeEncodingL(iAdditionalInfo->Alternative7bitEncoding(),
													                   maxBodyLength));
	SmsPDU().SetNationalLanguageEncodingL(encodingToUse);
	
	if(!AddEMSInformationElementsToSingleSegmentMessageL(*segmenter, encodingToUse))
		{
		aSmsArray.Reset();
		CleanupStack::PopAndDestroy(2, converter);
		return EFalse;
		}

	if(segmenter->MoreL())
			{
			TInt size = SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();
			// Defensive code to prevent SegmentNextL being called with size of zero
			if (size == 0)
				{
				CleanupStack::PopAndDestroy(2, converter);
				aSmsArray.Reset();
				return EFalse;
				}
			HBufC8* buf=HBufC8::NewMaxLC(maxBodyLength);
			TPtr8 ptr(buf->Des());
			segmenter->SegmentNextL(ptr, size, aUnconvertedChars, aDowngradedChars, encodingToUse);
			SmsPDU().UserData().AppendBodyL(ptr);
			CleanupStack::PopAndDestroy(buf);
			}
	CorrectFormattingInSinglePDUL();

	if (segmenter->MoreL())
		{
		CleanupStack::PopAndDestroy(2, converter);
		aSmsArray.Reset();
		return EFalse;
		}
	CleanupStack::PopAndDestroy(2, converter);

	aFreeUDUnitsInLastPDU = SmsPDU().UserData().MaxPackedUDUnitsInBodyRemaining();

	//
	// Encode the PDU...
	//
	TGsmSms  gsmSms;
		
	TEncodeParams encodeParams;
	encodeParams.iTimeStamp = &Time();
		
	TTimeIntervalSeconds interval = UTCOffset();
	encodeParams.iTimeIntervalInSeconds = &interval;
		
	SmsPDU().EncodeMessagePDUL(gsmSms, &encodeParams);
	aSmsArray.AppendL(gsmSms);

	return ETrue;
	} // CSmsMessage::EncodeIntoSinglePDUL


/**
 *  Attempts to encode into the single PDU
 *  The Text and the EMS objects are layed out with respect to start position locations and encoded into the correct Tsms object.
 *  Ensure this function is called before EncodeMessagePDUsL to properly process single PDU messages
 *  
 *  @return KErrUnderflow if the Ems objects are not in Start position order.
 *  @param aSmsArray returned array of newly created TSms objects containing the encoded message
 *  
 *  @capability None
 */
EXPORT_C TBool CSmsMessage::EncodeIntoSinglePDUL(CArrayFix<TGsmSms>& aSmsArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ENCODEINTOSINGLEPDUL1_1, "CSmsMessage::EncodeIntoSinglePDUL()");
	
	TInt  unconvertedChars, downgradedChars, freeUDUnitsInLastPDU;
	
	return EncodeIntoSinglePDUL(aSmsArray, unconvertedChars,
                                downgradedChars, freeUDUnitsInLastPDU);
	} // CSmsMessage::EncodeIntoSinglePDUL


void CSmsMessage::EncodingTPSRRFromSchemesIntoSinglePDUL()
	{
	TInt smscIndex(0);
	TBool smscPresent(EFalse);
	
	CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = iAdditionalInfo->GetStatusReportScheme();	
	if(scheme.Id() == EControlParametersScheme)
		{
		if(SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEISMSCControlParameters,smscIndex))
			{
			smscPresent=ETrue;
			}
		if(smscPresent)				
			{
			TUint8 octet(0);

			CSmsSMSCCtrlParameterOperations& ieOp = (CSmsSMSCCtrlParameterOperations&)GetOperationsForIEL(CSmsInformationElement::ESmsIEISMSCControlParameters);
			
			if (ieOp.GetStatusReport(0, octet) == KErrNone)
				{
				if(SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEISMSCControlParameters,smscIndex))
					{
					SmsPDU().UserData().InformationElement(smscIndex).Data()[0] = octet;
					if (SmsPDU().Type() == CSmsPDU::ESmsSubmit)
						{
						((CSmsSubmit&)SmsPDU()).SetStatusReportRequest(ETrue);	
						}
					else if (SmsPDU().Type() == CSmsPDU::ESmsDeliver)
						{
						((CSmsDeliver&)SmsPDU()).SetStatusReportIndication(ETrue);
						}
					}
				else
					{
					User::Leave(KErrCorrupt);	
					}	
				}
			}
		else
			{
			User::Leave(KErrNotFound);
			}	
		}
	else if(scheme.Id() == ETPSRRScheme)
		{
		TSmsFirstOctet smsReportRequest;
		CSmsTPSRROperations& nonIEOp = (CSmsTPSRROperations&)GetOperationsForNonIEL(ESmsTPSRRParameter);
		smsReportRequest = nonIEOp.GetStatusReport(0);
		
		if(smsReportRequest>=0)
			{
			if (SmsPDU().Type() == CSmsPDU::ESmsSubmit)
				{
				((CSmsSubmit&)SmsPDU()).SetStatusReportRequest(smsReportRequest);	
				}
			else if (SmsPDU().Type() == CSmsPDU::ESmsDeliver)
				{
				((CSmsDeliver&)SmsPDU()).SetStatusReportIndication(smsReportRequest);
				}	
			}
		}	
	} // CSmsMessage::EncodingTPSRRFromSchemesIntoSinglePDUL


/**
 *  Decode CSmsMessage
 *  
 *  @param aUserData Object containing the EMS objects.
 *  @param aCharsAlreadyAdded The current, running, start position to be applied to the objects taken out
 *  of the UserData.
 */
void CSmsMessage::DecodeBufferL(CArrayPtr<CSmsPDU>& aSmsPDUArray,
								CSmsBufferBase& aBuffer)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_DECODEBUFFERL_1, "CSmsMessage::DecodeBufferL()");

	iInformationElementArray->ResetAndDestroy();

	TSmsStatusReportScheme schemeId = FindSchemeL(aSmsPDUArray);
	
	TInt emailLen(0);
	for (TInt i=0; i<aSmsPDUArray.Count(); i++)
		{
		CSmsUserData& usrData = (aSmsPDUArray)[i]->UserData();
		InstallControlInformationElementsL(usrData, i);
		InstallEmsInformationElementsL(usrData, aBuffer.Length());
        // Possible refactoring opportunity:
        // This would be a good place to add a method which provides processing for control information
        // elements which contain values specific to the pdu (as opposed to being constant in every PDU.
        InstallEmailHeaderInformationElementL(usrData,emailLen);
        if(schemeId == ETPSRRScheme)
        	{
        	InstallTPSRRInformationL(aSmsPDUArray, i);
        	}
        
		CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(*iCharacterSetConverter,iFs,aSmsPDUArray[i]->Alphabet(),BinaryData());
		TSmsBufferReassembler reassembler(*converter,aBuffer);
        TSmsEncoding  encodingUsedInSegment = aSmsPDUArray[i]->NationalLanguageEncoding();
		
        reassembler.ReassembleNextL(usrData.Body(),
									encodingUsedInSegment,
									i==(aSmsPDUArray.Count()-1));
        
		CleanupStack::PopAndDestroy(converter);		
		MergeAlternative7bitEncoding(encodingUsedInSegment);
		}

    // Possible refactoring opportunity:
    // This would be a good place to add a method which provides processing for control information
    // elements which contain values specific to the pdu (as opposed to being constant in every PDU.
    if(emailLen>0)
		{
		if(emailLen>255)
			User::Leave(KErrTooBig);
		TBuf8<1> data;
		data.SetLength(1);
		data[0]=static_cast<TUint8>(emailLen);
		TInt emailIndex;
		if(SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIRFC822EmailHeader,emailIndex))
			SmsPDU().UserData().InformationElement(emailIndex).Data()[0]=static_cast<TInt8>(emailLen);
		else
			SmsPDU().UserData().AddInformationElementL(CSmsInformationElement::ESmsIEIRFC822EmailHeader,data);
		}
	UpdateUserPromptAndODIElementsStartPosition();
	} // CSmsMessage::DecodeBufferL

/**
 *  It decodes only raw text data.
 *  
 *  @param aSmsPDUArray Object containing PDUs.
 *  @param aBuffer where the decoded data will be stored.
 *
 */
void CSmsMessage::DecodeOnlyTextL(CArrayPtr<CSmsPDU>& aSmsPDUArray,
								CSmsBufferBase& aBuffer)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_DECODEONLYTEXTL_1, "CSmsMessage::DecodeOnlyTextL()");

	for (TInt i=0; i<aSmsPDUArray.Count(); i++)
		{
        CSmsAlphabetConverter* converter=CSmsAlphabetConverter::NewLC(*iCharacterSetConverter,iFs,aSmsPDUArray[i]->Alphabet(),BinaryData());
        TSmsBufferReassembler reassembler(*converter,aBuffer);
		TSmsEncoding  encodingUsedInSegment = aSmsPDUArray[i]->NationalLanguageEncoding();
        
		reassembler.ReassembleNextL(aSmsPDUArray[i]->UserData().Body(),
									encodingUsedInSegment,
									i==(aSmsPDUArray.Count()-1));
		
		CleanupStack::PopAndDestroy(converter);
		MergeAlternative7bitEncoding(encodingUsedInSegment);
		}
	} // CSmsMessage::DecodeBufferL

/**
 *  It adds the incomplete message info in additional attribute field.
 *  
 *  @param aStartPDU starting PDU index of incomplete message.
 *  @param aEndPDU end PDU index of incomplete message.
 *  @param aLastPartialCompleteMsg boolean value indicating whether 
 				this message is the last incomplete message sent to client.
 */
void CSmsMessage::AddIncompleteMessageInfoL(TInt aStartPDU, TInt aEndPDU, TBool aLastPartialCompleteMsg)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDINCOMPLETEMESSAGEINFOL_1, "CSmsMessage::AddIncompleteMessageInfoL()");

	CIncompleteClass0MessageInfo& incompleteClass0MsgInfo = (CIncompleteClass0MessageInfo&) iAdditionalInfo->GetNonIEOperationL(ESmsIncompleteClass0MessageParameter);
	incompleteClass0MsgInfo.SetVersion(CIncompleteClass0MessageInfo::ESmsIncompleteClass0MessageV0);
	incompleteClass0MsgInfo.SetIncompleteMessageInfoL(aStartPDU, aEndPDU, aLastPartialCompleteMsg);
	}

/**
 *  This method copies control information elements from the user data (passed as an
 *  input argument) into either the CSmsMessage's user data (working pdu) OR one of the
 *  collection of control information elements (sorted by category) which is contained
 *  in CSmsMessage::iAdditionalInfo.
 *  
 *  @param aUserData
 *  The user data which is currently being processed, and whose elements may be copied
 *  into the appropriate collection belonging to the CSmsMessage.
 *  @leave
 *  KErrArgument if an information element's type is unknown
 *  
 *  @internalComponent
 */
void CSmsMessage::InstallControlInformationElementsL(CSmsUserData& aUserData, TInt aSegmentSequenceNum)
    {
	// Installs all the information elements within the subsequent PDUs.
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_1, "CSmsMessage::InstallControlInformationElements()");

    CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = iAdditionalInfo->GetStatusReportScheme();
    
	for (TInt z=0; z<aUserData.NumInformationElements(); z++)
		{
		const CSmsInformationElement& ie = aUserData.InformationElement(z);

         TSmsInformationElementCategories::TInformationElementCategory category;

        if (TSmsInformationElementCategories::GetCategoryDefinition(ie.Identifier(), category))
        	{
            switch (category)
                {
                case  TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUMultipleInstancesPerPDU: // e.g. Special SMS Message Indication
                    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_2, "CSmsMessage::InstallControlInformationElements \ECtrlMandatoryInEveryPDUMultipleInstancesPerPDU id = %d", ie.Identifier());

                    if (ie.Identifier()== CSmsInformationElement::ESmsIEISpecialSMSMessageIndication)
                        {
                        TBool found = EFalse;
                        CArrayFixFlat<TInt>* indices = new(ELeave) CArrayFixFlat<TInt>(4);
                        CleanupStack::PushL(indices);
                        SmsPDU().UserData().InformationElementIndicesL(ie.Identifier(), *indices);

                        TInt count = indices->Count();
		                for (TInt i=0; ((i<count) && (found==EFalse)); i++)
		                    {
		                    TUint index = indices->operator[](i);
		                    CSmsInformationElement& ieAlreadyInWorkingPDU = SmsPDU().UserData().InformationElement(index);

                            if (ieAlreadyInWorkingPDU.Data()[0] == ie.Data()[0])
                                {
                                ieAlreadyInWorkingPDU.Data()[1] = ie.Data()[1];
                                found = ETrue;
                                break;
                                }
		                    }
                        CleanupStack::PopAndDestroy(indices);

                        if (found == EFalse)
                            {
                            SmsPDU().UserData().UpdateInformationElementArrayL(ie.Identifier(),ie.Data());
                            }
                        }
                    else
                        {
                        // Unknown category.
                        OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_3, "CSmsMessage::InstallControlInformationElementsL category = %d, id = %d", category, ie.Identifier());
                        User::Leave(KErrArgument);
                        }
                    break;
                case  TSmsInformationElementCategories::ECtrlMandatoryIn1stPDUOnly:
                case  TSmsInformationElementCategories::ECtrlSingleInstanceOnly:
                    {
                    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_4, "CSmsMessage::InstallControlInformationElements ECtrlMandatoryIn1stPDUOnly ""ECtrlSingleInstanceOnly, id = %d", ie.Identifier());

                    TUint index = 0;
                    if (iAdditionalInfo->Find1stInstanceOfControlInformationElement(ie.Identifier(), index))
                        {
                        iAdditionalInfo->DeleteControlInformationElement(category, index); //  might want to pass the
                                                                                                //  id in as an additional check
                                                                                                //  this should be an exception case
                        CSmsInformationElement* newInformationElement = CSmsInformationElement::NewL(ie.Identifier(),ie.Data());

                    	CleanupStack::PushL(newInformationElement);
                    	iAdditionalInfo->AddControlInformationElementL(category, newInformationElement);
                        CleanupStack::Pop(newInformationElement);
                        }
                     else
                        {
                        CSmsInformationElement* cloneInformationElement = CSmsInformationElement::NewL(ie.Identifier(),ie.Data());
                        CleanupStack::PushL(cloneInformationElement);
                        iAdditionalInfo->AddControlInformationElementL(category, cloneInformationElement);
                        CleanupStack::Pop(cloneInformationElement);
                        }
                    break;
                    }
                case  TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUAndWithIdenticalValues:
                    {
                    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_5, "CSmsMessage::InstallControlInformationElements ECtrlMandatoryInEveryPDUAndWithIdenticalValues ""ECtrlSingleInstanceOnly, id = %d", ie.Identifier());
                    TInt index = 0;
                    if (SmsPDU().UserData().InformationElementIndex(ie.Identifier(),index))
                        {
                        CSmsInformationElement* newInformationElement = CSmsInformationElement::NewL(ie.Identifier(),ie.Data());

                    	CleanupStack::PushL(newInformationElement);
                        CSmsInformationElement*& indexedPDU = SmsPDU().UserData().InformationElementPtr(index);
                        CSmsInformationElement* oldInformationElement = indexedPDU;
                        SmsPDU().UserData().InformationElementPtr(index) = newInformationElement;
						// In next loop, oldInformationElement will point to the newly created information element. So double free is no case.
						// coverity[double_free]
                        delete oldInformationElement;
                        CleanupStack::Pop(newInformationElement);
                        }
                    else
                        {
                        SmsPDU().UserData().UpdateInformationElementArrayL(ie.Identifier(),ie.Data());
                        }
                    break;
                    }

                case  TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed:
                    {
                    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_6, "CSmsMessage::InstallControlInformationElements ECtrlMandatoryInEveryPDUAndWithIdenticalValues ""ECtrlSingleInstanceOnly, id = %d", ie.Identifier() );

                    CSmsInformationElement* cloneInformationElement = CSmsInformationElement::NewL( ie.Identifier(),ie.Data() );
                    CleanupStack::PushL(cloneInformationElement);
                    iAdditionalInfo->AddControlInformationElementL( TSmsInformationElementCategories::ECtrlMultipleInstancesAllowed, cloneInformationElement);
                    CleanupStack::Pop(cloneInformationElement);
                    break;
                    }
                case  TSmsInformationElementCategories::ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU:
                    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_7, "CSmsMessage::InstallControlInformationElements ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU ""ECtrlSingleInstanceOnly, id = %d", ie.Identifier() );
                    
                    if (ie.Identifier() == CSmsInformationElement::ESmsIEISMSCControlParameters)
                    	{
                       	if(scheme.Id() == EControlParametersScheme)
                    		{
                    		CSmsMessageAdditionalAttributes::CControlParametersScheme& ctrlParamScheme = (CSmsMessageAdditionalAttributes::CControlParametersScheme&)scheme;
                    		ctrlParamScheme.iDefaultStatusReport = 0x00;
                    		
	                       	const TUint8* octet(0);
	                    	octet = ie.Data().Ptr();
	                    	if(*octet)
                    			{
                    			CSmsMessageAdditionalAttributes::CControlParametersScheme::TSmsSMSCCtrlParameterStatus smscCtrlParameterStatus;
								smscCtrlParameterStatus.iSegmentSequenceNum = aSegmentSequenceNum;
								smscCtrlParameterStatus.iSelectiveStatus = *octet;
								(ctrlParamScheme.iControlParametersStatusReport).AppendL(smscCtrlParameterStatus);
                    			}
                    		ctrlParamScheme.iNumOfPDUs++;
	                    	}
	                    }
                
                    // e.g  Email Header / Concatenation Elements.
                    // Consider whether EMail Header should be ported here.
                    // or left as is.
                    break;
                case  TSmsInformationElementCategories::EEmsInformationElement:
                    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_8, "CSmsMessage::InstallControlInformationElements ECtrlMandatoryInEveryPDUButWithValueSpecificToPDU ""ECtrlSingleInstanceOnly, id = %d", ie.Identifier() );
                    // Will be handled in the method InstallEmsInformationElements, nothing to do here
                    break;
                default:
                    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLCONTROLINFORMATIONELEMENTSL_9, "CSmsMessage::InstallControlInformationElementsToMultiSegmentMessageL, default switch, category = %d, id= %d", category, ie.Identifier() );
                    break;
                }
            }
    	}
    } // CSmsMessage::InstallControlInformationElementsL


/**
 *  Extracts all the EMS information objects out of the given UserData into the CSmsMessage IEArray.
 *  
 *  @param aUserData Object containing the EMS objects.
 *  @param aCharsAlreadyAdded The current, running, start position to be applied to the objects taken out
 *  of the UserData.
 */
void  CSmsMessage::InstallEmsInformationElementsL(CSmsUserData& aUserData, TInt aCharsAlreadyAdded)
	{
	// Installs all the information elements within the subsequent PDUs.
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLEMSINFORMATIONELEMENTSL_1, "CSmsMessage::InstallEmsInformationElements()");

	CSmsInformationElement::TSmsInformationElementIdentifier id;
	CEmsInformationElement* newIE =NULL;

	for (TInt z=0; z<aUserData.NumInformationElements(); z++)
		{
		const CSmsInformationElement& ie = aUserData.InformationElement(z);

		// is the ie an EMS information Element;
		id = ie.Identifier();
		if (CEmsFactory::Supported(id))
			{
			newIE=NULL;
			TRAPD(ret,newIE = CEmsFactory::CreateReceivedEmsIEL(ie,aCharsAlreadyAdded));
			if(newIE != NULL)
				{
				if(ret == KErrNone)
					{
					ret=AddReceivedEmsInformationElement(newIE);
					if(ret==KErrNone)  // Remove the Information Element from the User Data.
						aUserData.RemoveInformationElement(z--);
					else
						// newIE is made NULL in first place and then a new IE is created. so double free is no case
						// coverity[double_free]
						delete newIE;
					}
				else
					delete newIE;
				}
			}
		}
	} // CSmsMessage::InstallEmsInformationElementsL


/**
 *  Extracts all the EMS information objects out of the given Command into the CSmsMessage IEArray.
 *  
 *  @param aCommand Object containing the EMS objects.
 *  @param aCharsAlreadyAdded The current, running, start position to be applied to the objects taken out
 *  of the UserData.
 */
void  CSmsMessage::InstallEmsInformationElementsL(CSmsCommand& aCommand, TInt aCharsAlreadyAdded)
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF    
    // Installs all the information elements within the subsequent PDUs.
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLEMSINFORMATIONELEMENTSL1_1, "CSmsMessage::InstallEmsInformationElements()");
    
    CSmsInformationElement::TSmsInformationElementIdentifier id;
    CEmsInformationElement* newIE=NULL;
    
    for (TInt z=0; z<aCommand.NumInformationElements(); z++)
        {
        const CSmsInformationElement& ie = aCommand.InformationElement(z);
        // is the ie an EMS information Element;
        id = ie.Identifier();
        if (CEmsFactory::Supported(id))
            {
            newIE=NULL;
            TRAPD(ret,newIE = CEmsFactory::CreateReceivedEmsIEL(ie,aCharsAlreadyAdded));
            if(newIE != NULL )
                {
                    if(ret == KErrNone)
                    {
                        ret=AddReceivedEmsInformationElement(newIE);
                        if(ret==KErrNone)  // Remove the Information Element from the User Data.
                            aCommand.RemoveInformationElement(z--);
                        else
                            // newIE is made NULL in first place and then a new IE is created. so double free is no case.
                            // coverity[double_free]
                            delete newIE;
                    }
                    else
                        delete newIE;
                }
            }
        }
    BULLSEYE_RESTORE
    }

TSmsStatusReportScheme CSmsMessage::FindSchemeL(const CArrayPtr<CSmsPDU>& aSmsPDUArray)
	{
	TInt smscIndex(0);
	
	if(SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEISMSCControlParameters,smscIndex))
		{
		iAdditionalInfo->SetStatusReportSchemeL(EControlParametersScheme);
		return EControlParametersScheme;
		}
		
	TSmsFirstOctet oldTpsrr;
	TSmsFirstOctet tpsrr;
	CSmsSubmit* smsSubmit;
	CSmsDeliver* smsDeliver;
	
	if (SmsPDU().Type() == CSmsPDU::ESmsSubmit)
		{
		smsSubmit = reinterpret_cast<CSmsSubmit*>((aSmsPDUArray)[0]);
		oldTpsrr = smsSubmit->StatusReportRequest();
		}
	else if (SmsPDU().Type() == CSmsPDU::ESmsDeliver)
		{
		smsDeliver = reinterpret_cast<CSmsDeliver*>((aSmsPDUArray)[0]);
		oldTpsrr = smsDeliver->StatusReportIndication();
		}
		
	for (TInt ii=1; ii<aSmsPDUArray.Count(); ii++)
		{
		if (SmsPDU().Type() == CSmsPDU::ESmsSubmit)
			{
			smsSubmit = reinterpret_cast<CSmsSubmit*>((aSmsPDUArray)[ii]);
			tpsrr = smsSubmit->StatusReportRequest();
			}
		else if (SmsPDU().Type() == CSmsPDU::ESmsDeliver)
			{
			smsDeliver = reinterpret_cast<CSmsDeliver*>((aSmsPDUArray)[ii]);
			tpsrr = smsDeliver->StatusReportIndication();
			}
		if(tpsrr != oldTpsrr)
			{
			iAdditionalInfo->SetStatusReportSchemeL(ETPSRRScheme);
			return ETPSRRScheme;
			}
		}
	
	iAdditionalInfo->SetStatusReportSchemeL(EDefaultScheme);

	return EDefaultScheme;
	} // CSmsMessage::FindSchemeL

	
void CSmsMessage::InstallTPSRRInformationL(const CArrayPtr<CSmsPDU>& aSmsPDUArray, TInt aSegmentSequenceNum)
	{
    CSmsMessageAdditionalAttributes::CSmsStatusReportScheme& scheme = iAdditionalInfo->GetStatusReportScheme();
    CSmsMessageAdditionalAttributes::CTPSRRScheme& tpsrrScheme = (CSmsMessageAdditionalAttributes::CTPSRRScheme&)scheme;
    
    tpsrrScheme.iDefaultStatusReport = TSmsFirstOctet::ESmsStatusReportNotRequested;
    tpsrrScheme.iNumOfPDUs++;
    
    TSmsFirstOctet tpsrr;
    
    if (SmsPDU().Type() == CSmsPDU::ESmsSubmit)
		{
		CSmsSubmit* smsSubmit = reinterpret_cast<CSmsSubmit*>((aSmsPDUArray)[aSegmentSequenceNum]);
		tpsrr = smsSubmit->StatusReportRequest();
		}
	else if (SmsPDU().Type() == CSmsPDU::ESmsDeliver)
		{
		CSmsDeliver* smsDeliver = reinterpret_cast<CSmsDeliver*>((aSmsPDUArray)[aSegmentSequenceNum]);
		tpsrr = smsDeliver->StatusReportIndication();
		}
    
    if(tpsrr)
    	{
    	CSmsMessageAdditionalAttributes::CTPSRRScheme::TSmsTPSRRStatus tpsrrStatus;

    	tpsrrStatus.iSegmentSequenceNum = aSegmentSequenceNum;
		tpsrrStatus.iTPSRRStatus = TSmsFirstOctet::ESmsStatusReportRequested;
		tpsrrScheme.iTPSRRStatusReport.AppendL(tpsrrStatus);	
    	}
	} // CSmsMessage::InstallTPSRRInformationL


/**
 *  Adds EMS IE to a CSmsMessage
 *  
 *  @param aEmsIE EMS object to be added.
 *  
 *  @capability None
 */
EXPORT_C void CSmsMessage::AddEMSInformationElementL(const CEmsInformationElement& aEmsIE)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ADDEMSINFORMATIONELEMENTL_1, "CSmsMessage::AddEMSInformationElementL()");

	if(aEmsIE.StartPosition() > (TUint)iBuffer->Length())
		{
		User::Leave(KErrOverflow);
		}

	if(CSmsInformationElement::ESmsEnhancedTextFormatting == aEmsIE.Identifier() && aEmsIE.Length() ==0)
		{
		User::Leave(KErrUnderflow);
		}

	if(CSmsInformationElement::ESmsEnhancedUserPromptIndicator == aEmsIE.Identifier())
		{
		AddEmsUserPromptL(static_cast<const CEmsUserPrompt&>(aEmsIE));
		}
	else if(CSmsInformationElement::ESmsEnhancedODI == aEmsIE.Identifier())
		{
		AddEmsObjectDistributionL(static_cast<const CEmsObjectDistribution&>(aEmsIE));
		}
	else
		{
		TInt count=iInformationElementArray->Count();
		TInt currNo=0;
		const CEmsInformationElement* emsIE=NULL;

		while(currNo < count)
		{
			emsIE=(*iInformationElementArray)[currNo];
			if(emsIE->StartPosition() == aEmsIE.StartPosition() && (CSmsInformationElement::ESmsEnhancedUserPromptIndicator == emsIE->Identifier() || CSmsInformationElement::ESmsEnhancedODI == emsIE->Identifier()))
				User::Leave(KErrArgument);
			else if(emsIE->StartPosition() > aEmsIE.StartPosition())
				break;
			currNo++;
		}
		User::LeaveIfError(iInformationElementArray->Insert(aEmsIE.DuplicateL(),currNo));
		}
	} // CSmsMessage::AddEMSInformationElementL


TBool CSmsMessage::CanBeRemoved(const CEmsInformationElement& aIE, const TUint aIEIndex)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CANBEREMOVED_1, "CSmsMessage::CanBeRemoved()");

	TBool ret=ETrue;
	if(CSmsInformationElement::ESmsEnhancedODI == aIE.Identifier())
		return ret;

	TUint ieStartPosition=aIE.StartPosition();
	TUint count=iInformationElementArray->Count();
	CEmsInformationElement* ie=NULL;

	// is there a UserPrompt/ODI at the same position
	for (TUint i=0; i < count && ret; ++i)
		{
		ie = (*iInformationElementArray)[i];
		// Do not allow removal if start position matches a user prompt and it is not the user prompt itself being removed
		if (ie->StartPosition() == ieStartPosition && ie->Identifier() == CSmsInformationElement::ESmsEnhancedUserPromptIndicator && aIE.Identifier() != CSmsInformationElement::ESmsEnhancedUserPromptIndicator)
		    ret=EFalse;
		if (ie->Identifier() == CSmsInformationElement::ESmsEnhancedODI)
		    {
		    // don't allow removal of IE if position inside object count of ODI
		    if ((static_cast<CEmsObjectDistribution*>(ie)->ObjectCount()+i) >= aIEIndex || static_cast<CEmsObjectDistribution*>(ie)->ObjectCount() == 0)
		        ret=EFalse;
		    }
		}

	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CANBEREMOVED_2, "CSmsMessage::CanBeRemoved() returns %d", ret);

	return ret;
	} // CSmsMessage::CanBeRemoved


/**
 *  Adds EMS IE to a CSmsMessage
 *  
 *  @param aEmsIE EMS object to be added.
 */
void CSmsMessage::AddEmsUserPromptL(const CEmsUserPrompt& aUserPromptIE)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDEMSUSERPROMPTL_1, "CSmsMessage::AddEmsUserPromptL()");

		if(aUserPromptIE.ObjectCount() == 0 )User::Leave(KErrArgument);

		TUint count=iInformationElementArray->Count();
		TUint userPromptStartPosition=aUserPromptIE.StartPosition();

		TSmsId id=aUserPromptIE.Identifier();
		TBool allSameType=ETrue;
		TUint countOnTheSamePos=0;
		TUint firstIndex=0;

		// collect all IEs on the same startPosition as the new User Prompt
		CEmsInformationElement* ie=NULL;

		for (TUint i=0; i < count ; i++)
		{
			ie = (*iInformationElementArray)[i];
			if (ie->StartPosition() == userPromptStartPosition)
			{
				if(countOnTheSamePos==0)
				{
					firstIndex=i;
					id = ie->Identifier();
				}
				else
				{
					if(allSameType && id != ie->Identifier())allSameType=EFalse;
					else id = ie->Identifier();
				}
				++countOnTheSamePos;
			}
		}

		if(countOnTheSamePos != aUserPromptIE.ObjectCount())
			User::Leave(KErrArgument);


	if( countOnTheSamePos == 1)
	{
		switch(id)
		{
		case CEmsInformationElement::ESmsEnhancedPredefinedAnimation:
		case CEmsInformationElement::ESmsEnhancedLargeAnimation :
		case CEmsInformationElement::ESmsEnhancedSmallAnimation	:
		case CEmsInformationElement::ESmsEnhancedUserDefinedSound:
		case CEmsInformationElement::ESmsEnhancedLargePicture:
		case CEmsInformationElement::ESmsEnhancedSmallPicture:
		case CEmsInformationElement::ESmsEnhancedVariablePicture:
		User::LeaveIfError(iInformationElementArray->Insert(aUserPromptIE.DuplicateL(),firstIndex));
			break;
		default:
		User::Leave(KErrCorrupt);
			break;
		}
		return;
	}

	// stitching - more than one object
	if(allSameType  && (id == CEmsInformationElement::ESmsEnhancedUserDefinedSound || id == CEmsInformationElement::ESmsEnhancedLargePicture || id == CEmsInformationElement::ESmsEnhancedSmallPicture || id == CEmsInformationElement::ESmsEnhancedVariablePicture))
		{
		User::LeaveIfError(iInformationElementArray->Insert(aUserPromptIE.DuplicateL(),firstIndex));
		return;
		}

    //  not of the same type or not supported IE types
	User::Leave(KErrCorrupt);
	} // CSmsMessage::AddEmsUserPromptL


/**
 *  Adds EMS IE to a CSmsMessage
 *  
 *  @param aEmsIE EMS object to be added.
 */
void CSmsMessage::AddEmsObjectDistributionL(const CEmsObjectDistribution& aObjectDistributionIE)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDEMSOBJECTDISTRIBUTIONL_1, "CSmsMessage::AddEmsObjectDistributionL()");

    TUint count=iInformationElementArray->Count();
    TUint objectDistributionStartPosition=aObjectDistributionIE.StartPosition();

    TBool firstIndexFound=EFalse;
    TUint firstIndex=0;
    TUint countOnSameOrSubsequentPos=0;

    // collect any IEs on the same or greater startPosition as the new Object Distribution Indicator
    CEmsInformationElement* ie=NULL;

    for (TUint i=0; i < count; i++)
        {
        ie = (*iInformationElementArray)[i];
        if (ie->StartPosition() >= objectDistributionStartPosition)
            {
            // Only set index if at least one IE found at desired start position
            if (!firstIndexFound)
                {
                firstIndex=i;
                firstIndexFound=ETrue;
                }
            ++countOnSameOrSubsequentPos;
            }
        }
    // check number of IEs present for desired ObjectCount
    if(countOnSameOrSubsequentPos < aObjectDistributionIE.ObjectCount())
        User::Leave(KErrArgument);

    if (firstIndexFound)
        {
        User::LeaveIfError(iInformationElementArray->Insert(aObjectDistributionIE.DuplicateL(),firstIndex));
        return;
        }

    //  no valid IE for ODI to apply to
    User::Leave(KErrCorrupt);
    } // CSmsMessage::AddEmsObjectDistributionL


/**
 *  Removes first EMS IE that matches the criteria from the CSmsMessage
 *  
 *  @param aStartPosition of a EMS object to be removed.
 *  @param aEmsId type of a EMS object to be removed
 *  @return pointer EMS information element
 *  @capability
 *  @capability None
 */
EXPORT_C CEmsInformationElement* CSmsMessage::RemoveEMSInformationElementL(const TUint aStartPosition,const TSmsId aEmsId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_REMOVEEMSINFORMATIONELEMENTL_1, "CSmsMessage::RemoveEMSInformationElementL()");

	CEmsInformationElement* emsIE=NULL;
	CEmsInformationElement* ie=NULL;

	TUint count=iInformationElementArray->Count();

	for (TInt i=--count; i >=0 ; --i)
		{
		ie = (*iInformationElementArray)[i];
		if (ie->StartPosition() == aStartPosition && ie->Identifier() == aEmsId)
			{
				if(CanBeRemoved(*ie, i))
				{
					iInformationElementArray->Remove(i);
					emsIE=ie;
				}
				else
					emsIE=NULL;
				break;
			}

		}
	return emsIE;
	} // CSmsMessage::RemoveEMSInformationElementL


/**
 *  Removes all EMS IEs that matches the criteria from the CSmsMessage
 *  
 *  @param aStartPosition of a EMS object to be removed.
 *  @param aEmsId type of a EMS object to be removed
 *  @return pointer to array of EMS IE
 *  
 *  @capability None
 */
EXPORT_C RPointerArray<CEmsInformationElement>* CSmsMessage::RemoveEMSInformationElementsL(const TUint aStartPosition,const TSmsId aEmsId)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_REMOVEEMSINFORMATIONELEMENTSL_1, "CSmsMessage::RemoveEMSInformationElementsL()");

	CEmsInformationElement* ie=NULL;
	RPointerArray<CEmsInformationElement>* selectedIEs = NULL;
	TUint count=iInformationElementArray->Count();

	for (TInt i=--count; i >= 0 ; --i)
		{
		ie = (*iInformationElementArray)[i];
		if (ie->StartPosition() == aStartPosition && ie->Identifier() == aEmsId)
			{
				if(CanBeRemoved(*ie, i))
				{
					if(selectedIEs == NULL)selectedIEs=new (ELeave) RPointerArray<CEmsInformationElement>(8);
					iInformationElementArray->Remove(i);
					selectedIEs->Append(ie);
				}
			}

		}
	return selectedIEs;
	} // CSmsMessage::RemoveEMSInformationElementsL

/**
 *  Resets EMS IE from a CSmsMessage
 *  
 *  
 *  @capability None
 */
EXPORT_C void  CSmsMessage::ResetEMSL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_RESETEMSL_1, "CSmsMessage::ResetEMSL()");

	iInformationElementArray->ResetAndDestroy();
	} // CSmsMessage::ResetEMSL

/**
 *  Removes EMS IE from a CSmsMessage
 *  
 *  @return CArrayPtrFlat<const CEmsInformationElement>& of internal EMS array
 *  
 *  @capability None
 */
EXPORT_C const RPointerArray<const CEmsInformationElement>& CSmsMessage::GetEMSInformationElementsL()const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETEMSINFORMATIONELEMENTSL_1, "CSmsMessage::GetEMSInformationElementsL()");

	 return (const RPointerArray<const CEmsInformationElement>&)(*iInformationElementArray);
	} // CSmsMessage::GetEMSInformationElementsL


void CSmsMessage::UpdateUserPromptAndODIElementsStartPosition()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_UPDATEUSERPROMPTANDODIELEMENTSSTARTPOSITION_1, "CSmsMessage::UpdateUserPromptAndODIElementsStartPosition()");

		TUint num=iInformationElementArray->Count();
		TInt startPosition=-1;
		CEmsInformationElement* ie = NULL;


		// is the ie an EMS information Element;

		for(TInt i=num-1;i >=0; --i)
		{
			ie = (*iInformationElementArray)[i];
			if(ie->Identifier() == CSmsInformationElement::ESmsEnhancedUserPromptIndicator || ie->Identifier() == CSmsInformationElement::ESmsEnhancedODI)
			{
				if(startPosition >=0)
					ie->SetStartPosition(startPosition);
				else
					iInformationElementArray->Remove(i);
			}
			else
				startPosition=ie->StartPosition();

		}
	} // CSmsMessage::UpdateUserPromptAndODIElementsStartPosition


TInt CSmsMessage::AddReceivedEmsInformationElement(CEmsInformationElement* aEmsIE)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_ADDRECEIVEDEMSINFORMATIONELEMENT_1, "CSmsMessage::AddReceivedEmsInformationElement()");

	TInt ret=KErrNone;
	if(CSmsInformationElement::ESmsEnhancedUserPromptIndicator == aEmsIE->Identifier() || CSmsInformationElement::ESmsEnhancedODI == aEmsIE->Identifier())
		ret=iInformationElementArray->Append(aEmsIE);
	else
		{
			TInt count=iInformationElementArray->Count();
			TInt currNo=0;
			const CEmsInformationElement* emsIE=NULL;
			while(currNo < count)
			{
				emsIE=(*iInformationElementArray)[currNo];
				if(emsIE->StartPosition() > aEmsIE->StartPosition() && (CSmsInformationElement::ESmsEnhancedUserPromptIndicator != emsIE->Identifier() || CSmsInformationElement::ESmsEnhancedODI != emsIE->Identifier()))
					break;
				currNo++;
			}
			ret=iInformationElementArray->Insert(aEmsIE,currNo);
		}
	return ret;
	} // CSmsMessage::AddReceivedEmsInformationElement


/**
 *  Updates the slot information for the given message as described in the supplied buffer
 *  
 *  @param aDesc buffer, layout: byte 0 store id, byte 1..n PDU slot indexes
 *  @leave KErrArgument
 *  
 *  @capability None
 */
EXPORT_C void CSmsMessage::UpdateSlotsL(TDesC8& aDesc)
    {
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_UPDATESLOTSL_1, "CSmsMessage::UpdateSlotsL()");

    TGsmSmsSlotEntry newSlot;

    switch (aDesc[0])
        {
        case CSmsMessage::ESmsSIMStorage:
            {
            newSlot.iStore = KETelIccSmsStore;
            break;
            }
        case CSmsMessage::ESmsPhoneStorage:
            {
            newSlot.iStore = KETelMeSmsStore;
            break;
            }
        case CSmsMessage::ESmsCombinedStorage:
            {
            newSlot.iStore = KETelCombinedSmsStore;
            break;
            }
        default:
            User::Leave(KErrArgument);
        }

    for (TInt index = 1; index < aDesc.Size(); index++)
        {
        newSlot.iIndex = aDesc[index]; // point to PDU indexes
        iSlotArray.AppendL(newSlot);
        }
    } // CSmsMessage::UpdateSlotsL

/**
 *  Copy EMS elements to dest CSmsMessage
 *  Uses CSmsMessage extended EMS API
 *  Creates new CEmsInformationElement instance(s) and passes ownership to the dest CSmsMessage
 *  
 *  @return aToMessage  - Destination CSmsMessage
 *  
 *  @capability None
 */
EXPORT_C void CSmsMessage::CopyEmsElementsL(CSmsMessage& aToMessage) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_COPYEMSELEMENTSL_1, "CSmsMessage::CopyEmsElementsL()");

	// CSmsMessage extended EMS API method creates array of references to EMS elements in
	// the source message
	// Loop through the array copying each individual method using the CSmsMessage
	// and CEmsInformationElement API's
	CEmsInformationElement* dupl=NULL;
	aToMessage.ResetEMSL();
	for(TInt i = iInformationElementArray->Count()-1 ; i >=0; --i)
		{
		dupl = (*iInformationElementArray)[i]->DuplicateL();
		CleanupStack::PushL(dupl);
		aToMessage.AddEMSInformationElementL(*dupl);
		CleanupStack::PopAndDestroy(dupl);
		}
	} // CSmsMessage::CopyEmsElementsL

/**
 *  @capability None
 */
EXPORT_C void CSmsMessage::AddSlotL(const TGsmSmsSlotEntry& aSlot)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ADDSLOTL_1, "CSmsMessage::AddSlotL()");

		TInt count = iSlotArray.Count();
		TInt i(0);
		TBool found(EFalse);
		while(!found && i<count)
		{
			if(aSlot.iIndex == iSlotArray[i].iIndex)found=ETrue;
			else ++i;
		}
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ADDSLOTL_2, "CSmsMessage::AddSlotL current no in: %d, adds index %d", count,aSlot.iIndex );
		OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ADDSLOTL_3, "found %d at position %d",found,i);
		iSlotArray.AppendL(aSlot);
	} // CSmsMessage::AddSlotL

/**
 *  @capability None
 */
EXPORT_C TBool CSmsMessage::MatchSlots(const CArrayFixFlat<TGsmSmsSlotEntry>& aSlotArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_MATCHSLOTS_1, "CSmsMessage::MatchSlots()");

		TBool match = EFalse;
		TInt count = aSlotArray.Count();
		if(iSlotArray.Count() == count)
		{
			TInt i = 0;
			match = ETrue;
			while(match && i < count)
				{
				const TGsmSmsSlotEntry& newSlot=aSlotArray[i];
				const TGsmSmsSlotEntry& slot =iSlotArray[i];
				if ( ( slot.iIndex != newSlot.iIndex ) || ( slot.iStore != newSlot.iStore ) )
						match = EFalse;
				i++;
				}

		}
		return match;
	} // CSmsMessage::MatchSlots

/**
 *  c'tor
 *  @capability None
 */
EXPORT_C TGsmSmsSlotEntry::TGsmSmsSlotEntry()
	{
	} // TGsmSmsSlotEntry::TGsmSmsSlotEntry

/**
 *  Internalize
 *  
 *  @param aStream For internalizing the data
 */
void TGsmSmsSlotEntry::InternalizeL(RReadStream& aStream)
	{
	aStream >> iStore;
	iIndex=aStream.ReadInt32L();
	} // TGsmSmsSlotEntry::InternalizeL

/**
 *  Externalize
 *  
 *  @param aStream For Externalizing the data
 */
void TGsmSmsSlotEntry::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iStore;
	aStream.WriteInt32L(iIndex);
	} // TGsmSmsSlotEntry::ExternalizeL

 /**
  *  Extracts all RFC 822 Email header information objects out of the given UserData into the CSmsMessage's iSmsEmailHeaderLength.
  *  
  *  @param aUserData Object containing the EmailHeader information element.
  *  @param aCharsAlreadyAdded The current, running, start position to be applied to the objects taken out
  *  of the UserData.
  */
void  CSmsMessage::InstallEmailHeaderInformationElementL(CSmsUserData& aUserData,TInt& aHeaderLength)
 	{
 	// Installs all the information elements within the subsequent PDUs.
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLEMAILHEADERINFORMATIONELEMENTL_1, "CSmsMessage::InstallEmailHeaderInformationElementL()");

 	CSmsInformationElement::TSmsInformationElementIdentifier id;

 	for (TInt z=0; z<aUserData.NumInformationElements(); z++)
 		{
 		const CSmsInformationElement& ie = aUserData.InformationElement(z);
 		id = (CSmsInformationElement::TSmsInformationElementIdentifier)ie.Identifier();
 		if (id == CSmsInformationElement::ESmsIEIRFC822EmailHeader)
 			{
 				const TDesC8& des = ie.Data();
 				aHeaderLength+=des[0];
 			}
 		}
 	} // CSmsMessage::InstallEmailHeaderInformationElementL

/**
 *  Extracts all RFC 822 Email header information objects out of the given UserData into the CSmsMessage's iSmsEmailHeaderLength.
 *  
 *  @param aCommand Object containing the EmailHeader information element.
 *  @param aCharsAlreadyAdded The current, running, start position to be applied to the objects taken out
 *  of the UserData.
 */
void CSmsMessage::InstallEmailHeaderInformationElementL(CSmsCommand& aCommand,TInt& aHeaderLength)
    {
    // Ignore in code coverage - not used in SMS stack and not exported
    // but cannot be removed as impacts public header.
    BULLSEYE_OFF    
    // Installs all the information elements within the subsequent PDUs.
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_INSTALLEMAILHEADERINFORMATIONELEMENTL1_1, "CSmsMessage::InstallEmailHeaderInformationElementL()");
    
    CSmsInformationElement::TSmsInformationElementIdentifier id;
    
    for (TInt z=0; z<aCommand.NumInformationElements(); z++)
        {
        const CSmsInformationElement& ie = aCommand.InformationElement(z);
        id = (CSmsInformationElement::TSmsInformationElementIdentifier)ie.Identifier();
        if (id == CSmsInformationElement::ESmsIEIRFC822EmailHeader)
            {
            const TDesC8& des = ie.Data();
            aHeaderLength+=des[0];
            }
        }
    BULLSEYE_RESTORE
    }

// E-mail header methods

/**
 *  Adds a header to an email body
 *  
 *  @param aEMailBody The buffer to which the header gets appended
 *  @param aEmailHeader The buffer which holds RFC 822 e-mail header
 *  @publishedAll
 *  @released
 *  
 *  @capability None
 */
EXPORT_C void CSmsMessage::AddEmailHeaderL(const TDesC& aEmailHeader, const TDesC& aEmailBody)
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ADDEMAILHEADERL_1, "CSmsMessage::AddEmailHeaderL()");

 	if(IsEmailHeader())
 		User::Leave(KErrAlreadyExists);

 	TInt len =aEmailHeader.Length();
 	if(len <= 0)
 		User::Leave(KErrCorrupt);

	if(len >= 255 )
 		User::Leave(KErrTooBig);


	if(SmsPDU().UserData().NumInformationElements())
		{
		TBool is16bit = EFalse;
		TBool concatenationIEPresent= SmsPDU().TextConcatenated( &is16bit );
		if(concatenationIEPresent)
			{
			if(SmsPDU().UserData().NumInformationElements() >=2) // something more than concat IE
				User::Leave(KErrAccessDenied);
			}
		else
			User::Leave(KErrAccessDenied);
		}


	TBuf8<1> data;
	data.SetLength(1);
	data[0]=static_cast<TUint8>(len);
	SmsPDU().UserData().AddInformationElementL(CSmsInformationElement::ESmsIEIRFC822EmailHeader,data);

 	iBuffer->Reset();
 	iBuffer->InsertL(0,aEmailHeader);
 	iBuffer->InsertL(iBuffer->Length(),aEmailBody);
 	} // CSmsMessage::AddEmailHeaderL

 /**
  *  Checks whether the SmsMessage contains an e-mail.
  *  
  *  @return ETrue if CSmsMessage contains e-mail ( e-mail header information element), otherwise EFalse
  *  @publishedAll
  *  @released
  *  
  *  @capability None
  */
EXPORT_C TBool CSmsMessage::IsEmailHeader() const
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_ISEMAILHEADER_1, "CSmsMessage::IsEmailHeader()");

	TInt emailIndex;
	return SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIRFC822EmailHeader,emailIndex);
 	} // CSmsMessage::IsEmailHeader

 /**
  *  Extracts e-mail header and body from the SmsMessage
  *  
  *  @return aEMailHeader The buffer containing the extracted email header
  *  @return aEMailBody The buffer containing the extracted email body
  *  @return ETrue if extraction of e-mail header and body succeeds
  *  @publishedAll
  *  @released
  *  
  *  @capability None
  */
EXPORT_C TBool CSmsMessage::GetEmailHeaderL(HBufC** aEmailHeader,HBufC** aEmailBody)
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETEMAILHEADERL_1, "CSmsMessage::GetEmailHeaderL()");

 	if(IsEmailHeader())
 		{
 			TInt bufLen=iBuffer->Length();
 			TInt emailHeaderLen(0);
			TInt emailIndex(0);
			if(SmsPDU().UserData().InformationElementIndex(CSmsInformationElement::ESmsIEIRFC822EmailHeader,emailIndex))
				emailHeaderLen=SmsPDU().UserData().InformationElement(emailIndex).Data()[0];
			else
				User::Leave(KErrCorrupt);

 			TInt emailBodyLen=bufLen-emailHeaderLen;

 			if(emailBodyLen<0)
 				return EFalse;

 			*aEmailHeader=HBufC::NewL(emailHeaderLen);
 			CleanupStack::PushL(*aEmailHeader);

 			*aEmailBody=HBufC::NewL(emailBodyLen);
 			CleanupStack::PushL(*aEmailBody);

 			TPtr headerPtr((*aEmailHeader)->Des());
 			iBuffer->Extract(headerPtr,0,emailHeaderLen);

 			TPtr bodyPtr((*aEmailBody)->Des());
 			iBuffer->Extract(bodyPtr,emailHeaderLen,emailBodyLen);

 			CleanupStack::Pop(2);
 			return ETrue;
 		}
 	return EFalse;
 	} // CSmsMessage::GetEmailHeaderL


 /**
  *  Extracts the offset between universal time and local time in seconds.
  *  If the value is > 0, local time is ahead of universal time.
  *  If the value is < 0, local time is behind universal time.
  *  
  *  @return TTTimeIntervalSeconds The time zone offset in seconds.
  *  @publishedAll
  *  @capability None
  */
EXPORT_C TTimeIntervalSeconds CSmsMessage::UTCOffset() const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_UTCOFFSET_1, "CSmsMessage::UTCOffset()");

    TUint timeZoneOffset = ((iFlags & ESmsUTCOffsetSecondGranularityMask) >> ESecondBitOffset);

    if (iFlags & ESmsUTCOffsetSignBit)
        {
        timeZoneOffset = -timeZoneOffset;
        }

    return (TTimeIntervalSeconds(timeZoneOffset));
    } // CSmsMessage::UTCOffset


 /**
  *  Sets the offset between universal time and local time in seconds.
  *  If the value is > 0, local time is ahead of universal time.
  *  If the value is < 0, local time is behind universal time.
  *  The CSmsMessage Time Zone Offset has the range +/- 71100 seconds. This is same range as
  *  the Service Centre Time Stamp Time Zone Offset as specified in 23.040 V4.4.0 Sect 9.2.3.11.
  *  
  *  @return True if input value was set successfully (in range), False otherwise
  *  @publishedAll
  *  @capability None
  */
EXPORT_C TBool CSmsMessage::SetUTCOffset(const TTimeIntervalSeconds& aTimeOffset)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_SETUTCOFFSET_1, "CSmsMessage::SetUTCOffset()");

    TBool rc = ETrue;

    TInt timeOffset = aTimeOffset.Int();

    if ((timeOffset <=  EMaximumSeconds) &&
        (timeOffset >= -EMaximumSeconds))
        {
        // Clear the timezone offset bits
        iFlags &= (~(ESmsUTCOffsetSecondGranularityMask   |
                     ESmsUTCOffsetSignBit));

        if (timeOffset < 0)
            {
            iFlags |= ESmsUTCOffsetSignBit;
            timeOffset = -timeOffset;
            }

        iFlags |= (timeOffset << ESecondBitOffset);
        }
    else
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_SETUTCOFFSET_2, "CSmsMessage::SetUTCOffset offset [out of range] = %d",timeOffset);
        rc = EFalse;
        }

    return rc;
} // CSmsMessage::SetUTCOffset


/**
 *  Returns the message version number.
 *  @capability
 */
EXPORT_C TInt CSmsMessage::Version()
 	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_VERSION_1, "CSmsMessage::Version()");

 	return iVersion;
 	} // CSmsMessage::Version


/**
 *  @internalComponent
 *  Validates and sets the message version number.
 *  
 *  @param aVersion version number to set.
 *  @return KErrNone if aVersion is valid and successfully set, KErrArgument otherwise.
 *  @capability None
 */
EXPORT_C TInt CSmsMessage::SetVersion(TInt aVersion)
	{
 	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_SETVERSION_1, "CSmsMessage::SetVersion(), version = %d", aVersion);

	if((aVersion>=ESmsMessageV0) && (aVersion<=ESmsMessageV4))
		{
		iVersion=aVersion;
		return KErrNone;
		}

	return KErrArgument;
	} // CSmsMessage::SetVersion


/**
 *  @internalComponent
 *  Internalises all object data except for the CSmsBufferBase and the message version.
 *  
 *  This is used when the buffer is stored elsewhere.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
EXPORT_C void CSmsMessage::InternalizeWithoutBufferAndVersionL(RReadStream& aStream)
	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_INTERNALIZEWITHOUTBUFFERANDVERSIONL_1, "CSmsMessage::InternalizeWithoutBufferAndVersionL()");

	iFlags=aStream.ReadInt32L();
	iStatus=(NMobileSmsStore::TMobileSmsStoreStatus) aStream.ReadInt32L();
	iLogServerId=aStream.ReadInt32L();
	TInt64 time;
	aStream >> time;
	iTime=time;

	CSmsPDU* smspdu=CSmsPDU::NewL(aStream,*iCharacterSetConverter,iFs);
	delete iSmsPDU;
	iSmsPDU=smspdu;
	iIs16BitConcatenation=aStream.ReadInt32L();

	iInformationElementArray->ResetAndDestroy();
	CEmsFactory::InternalizeL(*iInformationElementArray, aStream);

	TInt count=aStream.ReadInt32L();
	iSlotArray.Reset();
	TInt i=0;
	TGsmSmsSlotEntry newSlot;
	for (; i<count; i++)
		{
		newSlot.InternalizeL(aStream);
		AddSlotL(newSlot);
		}
	} // CSmsMessage::InternalizeWithoutBufferAndVersionL


/**
 *  @internalComponent
 *  Externalises all object data except for the CSmsBufferBase and the message version.
 *  
 *  This is used when the buffer is stored elsewhere.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void CSmsMessage::ExternalizeWithoutBufferAndVersionL(RWriteStream& aStream) const
	{
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_EXTERNALIZEWITHOUTBUFFERANDVERSIONL_1, "CSmsMessage::ExternalizeWithoutBufferAndVersionL()");

	aStream.WriteInt32L(iFlags);
	aStream.WriteInt32L(iStatus);
	aStream.WriteInt32L(iLogServerId);
	aStream << Time().Int64();

	SmsPDU().ExternalizeL(aStream);
	aStream.WriteInt32L(iIs16BitConcatenation);

	CEmsFactory::ExternalizeL(*iInformationElementArray, aStream);
	TInt count=iSlotArray.Count();
	aStream.WriteInt32L(count);
	TInt i=0;
	for (; i<count; i++)
		{
		iSlotArray[i].ExternalizeL(aStream);
		}
	} // CSmsMessage::ExternalizeWithoutBufferAndVersionL


/**
 *  @internalComponent
 *  Internalises the CSmsBufferBase.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
EXPORT_C void CSmsMessage::InternalizeBufferL(RReadStream& aStream)
	{
	aStream >> *iBuffer;
	} // CSmsMessage::InternalizeBufferL


/**
 *  @internalComponent
 *  Externalises the CSmsBufferBase.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void CSmsMessage::ExternalizeBufferL(RWriteStream& aStream) const
	{
	aStream << *iBuffer;
	} // CSmsMessage::ExternalizeBufferL


/**
 *  @internalComponent
 *  Internalises the message version.
 *  
 *  @param aStream Stream to read from
 *  @capability None
 */
EXPORT_C void CSmsMessage::InternalizeVersionL(RReadStream& aStream)
	{
	TInt32 versionNumber = ESmsMessageV0;
	TRAPD(err,versionNumber=aStream.ReadInt32L());
	if(err==KErrEof)
		{
		versionNumber=ESmsMessageV0;
		}

	iVersion=static_cast<TSmsMessageVersion> (versionNumber);
	} // CSmsMessage::InternalizeVersionL


/**
 *  @internalComponent
 *  Externalises the message version.
 *  
 *  @param aStream Stream to write to
 *  @capability None
 */
EXPORT_C void CSmsMessage::ExternalizeVersionL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iVersion);
	} // CSmsMessage::ExternalizeVersionL


/**
 *  @publishedAll
 *  
 *  Returns a reference to a specialisation of the class CSmsIEOperation. Clients can use this sub-class
 *  to access the attributes of a specific type of information element encapsulated inside the CSmsMessage.
 *  The ownership to the CSmsIEOperation class belongs to the CSmsMessage. When the CSmsMessage is deleted,
 *  the CSmsIEOperation will also be deleted and the reference will become stale.
 *  
 *  @param aId
 *  Specifies the sub class of CSmsIEOperation required.
 *  @leave KErrNotSupported
 *  When there is not accessor class available for the specified type of information element.
 *  @capability None
 */
EXPORT_C CSmsIEOperation& CSmsMessage::GetOperationsForIEL(CSmsInformationElement::TSmsInformationElementIdentifier aId) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETOPERATIONSFORIEL_1, "CSmsMessage::GetOperationsForIEL()");

    if (iVersion < CSmsMessage::ESmsMessageV1)
        {
        OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETOPERATIONSFORIEL_2, "CSmsMessage::GetOperationsForIEL, Operation not supported, Msg Version %d", iVersion);
        User::Leave(KErrNotSupported);
        }

    return iAdditionalInfo->GetIEOperationL(aId);
    } // CSmsMessage::GetOperationsForIEL

EXPORT_C CSmsNonIEOperation& CSmsMessage::GetOperationsForNonIEL(TSmsNonIEIdentifier aId) const
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETOPERATIONSFORNONIEL_1, "CSmsMessage::GetOperationsForNonIEL");

	if (iVersion < CSmsMessage::ESmsMessageV2)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_GETOPERATIONSFORNONIEL_2, "GetOperationsForNonIEL not supported, Msg Version %d", iVersion);
		User::Leave(KErrNotSupported);
		}

	return iAdditionalInfo->GetNonIEOperationL(aId);
	} // CSmsMessage::GetOperationsForNonIEL


void CSmsMessage::CreateControlIEOperationsClassesL()
    {
 	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSMESSAGE_CREATECONTROLIEOPERATIONSCLASSESL_1, "CSmsMessage::CreateControlIEOperationsClassesL()");

    CSmsIEOperation* iEOperation = NULL;

    iEOperation = CSmsIEOperation::NewL(CSmsInformationElement::ESmsHyperLinkFormat, *this,	*iCharacterSetConverter, iFs);
    iAdditionalInfo->SetIEOperationL(iEOperation);

    iEOperation = CSmsIEOperation::NewL(CSmsInformationElement::ESmsReplyAddressFormat, *this,	*iCharacterSetConverter, iFs);
    iAdditionalInfo->SetIEOperationL(iEOperation);

    iEOperation = CSmsIEOperation::NewL(CSmsInformationElement::ESmsEnhanceVoiceMailInformation, *this,	*iCharacterSetConverter, iFs);
    iAdditionalInfo->SetIEOperationL(iEOperation);

    iEOperation = CSmsIEOperation::NewL(CSmsInformationElement::ESmsIEISpecialSMSMessageIndication, *this,	*iCharacterSetConverter, iFs);
    iAdditionalInfo->SetIEOperationL(iEOperation);
    
    iEOperation = CSmsIEOperation::NewL( CSmsInformationElement::ESmsIEISMSCControlParameters, *this,	*iCharacterSetConverter, iFs);
    iAdditionalInfo->SetIEOperationL(iEOperation);
    } // CSmsMessage::CreateControlIEOperationsClassesL

    
void CSmsMessage::CreateControlNonIEOperationsClassesL()
    {
    CSmsNonIEOperation* nonIEOperation = NULL;

    nonIEOperation = CSmsNonIEOperation::NewL(ESmsTPSRRParameter, *this);
    iAdditionalInfo->SetNonIEOperationL(nonIEOperation);

	nonIEOperation = CSmsNonIEOperation::NewL(ESmsIncompleteClass0MessageParameter, *this);
	iAdditionalInfo->SetNonIEOperationL(nonIEOperation);
	} // CSmsMessage::CreateControlNonIEOperationsClassesL


/**
 *  Gets the scheme of the status report.
 *  
 *  @return Staus Report Scheme 
 */
EXPORT_C TSmsStatusReportScheme CSmsMessage::Scheme() const
	{
	return iAdditionalInfo->GetStatusReportScheme().Id();
	}

/**
 *  @publishedAll
 *  
 *  Used by the SMS Stack to indicate that when the message was decoded, it was determined
 *  to be on the SIM. If the message's DCS byte is configured for automatic delete or the
 *  PID is set to Type 0, the message may subsequently have been deleted from the SIM during
 *  decoding in which case its storage status is unstored.
 *  
 *  @param aOnSim
 *  Used to indicate that the message was on the SIM when it was decoded.
 *  @capability None
 */
EXPORT_C void  CSmsMessage::SetDecodedOnSIM(TBool aOnSim)
    {
 	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_SETDECODEDONSIM_1, "CSmsMessage::SetDecodedOnSIM(): %d", aOnSim);

    if (aOnSim)
        {
        iFlags = iFlags | EDecodedOnSimBit;
        }
    else
        {
        iFlags = iFlags & ~EDecodedOnSimBit;
        }
    } // CSmsMessage::SetDecodedOnSIM


/**
 *  @publishedAll
 *  
 *  Used by the SMS Stack to indicate that when the message was decoded, it was determined
 *  to be on the SIM. If the message's DCS byte is configured for automatic delete or the
 *  PID is set to Type 0, the message may subsequently have been deleted from the SIM during
 *  decoding in which case its storage status is unstored.
 *  
 *  @return
 *  True if the message was on the SIM during decoding,
 *  False otherwise.
 *  @capability None
 */
EXPORT_C TBool CSmsMessage::DecodedOnSim()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_DECODEDONSIM_1, "CSmsMessage::DecodedOnSim()");

    if (iFlags & EDecodedOnSimBit)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    } // CSmsMessage::DecodedOnSim


/**
 *  @publishedAll
 *  
 *  Used by the SMS Stack to indicate that when the message was decoded, it was determined
 *  that it should be forwarded to the client. The message will not be forwarded to the
 *  client when the PID byte is set to type 0 (except by default for class 0 and class 2 messages).
 *  
 *  @param forward
 *  Used to indicate that the message needs to be forwarded to clients.
 *  @capability None
 */
EXPORT_C void CSmsMessage::SetForwardToClient(TBool aForward)
    {
 	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_SETFORWARDTOCLIENT_1, "CSmsMessage::SetForwardToClient(): %d", aForward);

    if (aForward)
        {
        iFlags = iFlags | EForwardToClientBit;
        }
    else
        {
        iFlags = iFlags & ~EForwardToClientBit;
        }
    } // CSmsMessage::SetForwardToClient


/**
 *  @publishedAll
 *  
 *  Used by the SMS Stack to indicate that when the message was decoded, it was determined
 *  that is should be forwarded to the client. The message will not be forwarded to the client
 *  when the PID byte is set to type 0 (except by default for class 0 and class 2 messages).
 *  
 *  @return
 *  True if the message is to be forwarded to clients
 *  False otherwise.
 *  @capability None
 */
EXPORT_C TBool CSmsMessage::ForwardToClient()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CSMSMESSAGE_FORWARDTOCLIENT_1, "CSmsMessage::ForwardToClient()");

    if (iFlags & EForwardToClientBit)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    } // CSmsMessage::ForwardToClient
