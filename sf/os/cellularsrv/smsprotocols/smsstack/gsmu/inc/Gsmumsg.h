// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the class CSmsMessage
// 
//


/**
 @file
 @publishedAll
*/

#ifndef __GSMUMSG_H__
#define __GSMUMSG_H__

#include <e32std.h>
#include <f32file.h>
#include <gsmuetel.h>
#include <charconv.h>
#include <gsmunmspacemobstore.h>
#include "gsmupdu.h"

#include <emsinformationelement.h>

/**
 *  @publishedAll
 */
const TUint IoctlWriteSMSBufferSize = 256;
const TUint KMaxMobileNameSize = 32;

class CSmsBufferBase;
class CSmsCompressedBuffer;
class TSmsUserDataSettings;
class CSmsEMSBufferSegmenter;
class CEmsUserPrompt;
class CEmsObjectDistribution;
class CSmsIEOperation;
class CSmsNonIEOperation;
class CSmsMessageAdditionalAttributes;

/**
 *  This class is used in CSmsMessage only.
 *  @publishedAll
 */
class TGsmSmsSlotEntry
	{
public:
	IMPORT_C TGsmSmsSlotEntry();

public:
    TInt iIndex;
    TBuf<KMaxMobileNameSize> iStore; 				// equivalent to RMobilePhone::TMobileName
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
    };

struct TEncodeParams
	{
	const TTime* iTimeStamp;
	const TTimeIntervalSeconds* iTimeIntervalInSeconds;		
	};

/**
 *  Represents a complete SMS message.
 *  
 *  This class can:
 *  
 *  Internalise and externalise its data
 *  
 *  Encode/decode itself to/from one or more SMS PDU's
 *  
 *  SMS Message Data is stored in the CSmsBufferBase iBuffer. SMS Message Settings
 *  are stored in the CSmsPDU iSmsPDU.
 *  @publishedAll
 *  @released 
 */
class CSmsMessage : public CBase
	{
public:
/** User data optimisation flags. */
	enum TSmsOptimizationFlags
		{
	/**
	 *  The alphabet flag causes an alphabet to be chosen which preserves information
	 *  	in the message and makes the number of PDUs as small as possible. 
	 */
		ESmsFlagOptimizeAlphabet=0x01,
	/**
	 *  8-bit referenced concatenation.
	 *  
	 *  This causes compression to be switched off and if the message length is greater
	 *  	than the maximum message length, concatenation is switched on. 
	 */
		ESmsFlagOptimizeConcatenation=0x08,
	/**
	 *  16-bit referenced concatenation.
	 *  
	 *  This causes compression to be switched off and if the message length is greater
	 *  	than the maximum message length, concatenation is switched on. 
	 */
		ESmsFlagOptimizeConcatenation16BitReference=0x10,
		};

/** Flags that indicate whether a message is stored. */
	enum TMobileSmsStorage
		{
	/** No storage. */
		ESmsNoStorage=0x00,
	/** SIM storage. */
		ESmsSIMStorage=0x04,
	/** Phone storage. */
		ESmsPhoneStorage=0x08,
	/** Combined storage. */
		ESmsCombinedStorage=0x0C
		};

	enum TTimeIntervalsRepresentedInSeconds
		{
		E15MinutesRepresentedInSeconds = 15 * 60,
		EMaximumSeconds                = 79 * E15MinutesRepresentedInSeconds
		                                      // Harmonised with maximum offset allowed for
                                              // Service Centre Time Stamp per 23.040 V4.4 Sect 9.2.3.11.
		};


public:
	IMPORT_C static CSmsMessage* NewL(RFs& aFs, const TGsmSms& aGsmSms,CSmsBufferBase* aBuffer, TBool aIsRPError=EFalse,TBool aIsMobileTerminated=ETrue);
	IMPORT_C static CSmsMessage* NewL(RFs& aFs, CSmsPDU::TSmsPDUType aType,CSmsBufferBase* aBuffer,TBool aIsRPError=EFalse);
	IMPORT_C ~CSmsMessage();

	inline CSmsPDU::TSmsPDUType Type() const;
	inline TBool IsComplete() const;
	inline TBool IsDecoded() const;
	inline TMobileSmsStorage Storage() const;
	inline void SetStorage(TMobileSmsStorage aStorage);
	inline NMobileSmsStore::TMobileSmsStoreStatus Status() const;
	inline void SetStatus(NMobileSmsStore::TMobileSmsStoreStatus aStatus);
	inline TInt LogServerId() const;
	inline void SetLogServerId(TInt aId);
	inline const TTime& Time() const;
	inline void SetTime(const TTime& aTime);
	IMPORT_C TTimeIntervalSeconds UTCOffset() const;
	IMPORT_C TBool SetUTCOffset(const TTimeIntervalSeconds& aTimeOffset);
    IMPORT_C void  SetDecodedOnSIM(TBool aOnSim);
    IMPORT_C TBool DecodedOnSim();
    IMPORT_C void  SetForwardToClient(TBool aForward);
    IMPORT_C TBool ForwardToClient();

	inline CSmsPDU& SmsPDU();
	inline const CSmsPDU& SmsPDU() const;

	inline TPtrC ServiceCenterAddress() const;
	inline void SetServiceCenterAddressL(const TDesC& aAddress);
	inline void ParsedServiceCenterAddress(TGsmSmsTelNumber& aParsedAddress) const;
	inline void SetParsedServiceCenterAddressL(const TGsmSmsTelNumber& aParsedAddress);

	inline TPtrC ToFromAddress() const;
	inline void SetToFromAddressL(const TDesC& aAddress);
	inline void ParsedToFromAddress(TGsmSmsTelNumber& aParsedAddress) const;
	inline void SetParsedToFromAddressL(const TGsmSmsTelNumber& aParsedAddress);

	IMPORT_C void InternalizeWithoutBufferL(RReadStream& aStream);
	IMPORT_C void ExternalizeWithoutBufferL(RWriteStream& aStream) const;

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	inline CSmsBufferBase& Buffer();
	inline const CSmsBufferBase& Buffer() const;

//  Functions related to encoding/decoding the text buffer
	IMPORT_C TBool TextPresent() const;
	IMPORT_C TInt NumMessagePDUsL();
	IMPORT_C TInt MaxMessageLength() const;
	IMPORT_C TInt MessageLengthL();
	IMPORT_C void GetEncodingInfoL(TInt& aPdus, TInt& aUnconvertedChars,
                                   TInt& aDowngradedChars, TInt& aFreeUDUnitsInLastPDU);

	IMPORT_C void UserDataSettings(TSmsUserDataSettings& aSettings) const;
	IMPORT_C void SetUserDataSettingsL(const TSmsUserDataSettings& aSettings);
	IMPORT_C void OptimizeSettingsL(TInt aOptions);

	IMPORT_C TSmsEncoding Alternative7bitEncoding() const;
	IMPORT_C TInt SetAlternative7bitEncoding(TSmsEncoding aEncoding);
	IMPORT_C TBool IsSupportedL(const TDesC& aDes,TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstUnconvertibleCharacter);
	IMPORT_C TBool IsSupportedL(const TDesC& aDes, TInt& aNumberOfUnconvertibleCharacters,
			                    TInt& aNumberOfDowngradedCharacters,
			                    TInt& aNumberRequiringAlternativeEncoding,
			                    TInt& aIndexOfFirstUnconvertibleCharacter) const;

	IMPORT_C void EncodeMessagePDUsL(CArrayFix<TGsmSms>& aSmsArray, TInt aReference=0);
	IMPORT_C void DecodeMessagePDUsL(const CArrayFix<TGsmSms>& aSmsArray);
	IMPORT_C const RPointerArray<const CEmsInformationElement>& GetEMSInformationElementsL()const;
	IMPORT_C void AddEMSInformationElementL(const CEmsInformationElement& aEmsIE);
	IMPORT_C RPointerArray<CEmsInformationElement>* RemoveEMSInformationElementsL(const TUint aStartPosition,const TSmsId aEmsId);
	IMPORT_C CEmsInformationElement* RemoveEMSInformationElementL(const TUint aStartPosition,const TSmsId aEmsId);
	IMPORT_C void ResetEMSL();
	IMPORT_C TBool EncodeIntoSinglePDUL(CArrayFix<TGsmSms>& aSmsArray);
	IMPORT_C void CopyEmsElementsL(CSmsMessage& aToMessage) const;
	IMPORT_C void AddSlotL(const TGsmSmsSlotEntry& aSlot);
	IMPORT_C TBool MatchSlots(const CArrayFixFlat<TGsmSmsSlotEntry>& aSlotArray);
// E-mail header
 	IMPORT_C void AddEmailHeaderL(const TDesC& aEmailHeader,const TDesC& aEmailBody);
 	IMPORT_C TBool IsEmailHeader() const;
 	IMPORT_C TBool GetEmailHeaderL(HBufC** aEmailHeader,HBufC** aEmailBody);
 	void InstallEmailHeaderInformationElementL(CSmsUserData& aUserData,TInt& aHeaderLength);
 	void InstallEmailHeaderInformationElementL(CSmsCommand& aCommand,TInt& aHeaderLength);
	inline void Set16BitConcatenation(TBool aIs16BitConcatenation);
	inline TBool Is16BitConcatenation()const;
	IMPORT_C void UpdateSlotsL(TDesC8& aBuf);
// Version information
	IMPORT_C TInt Version();
	// from this point downwards, all functions should be published internally only
	IMPORT_C TInt SetVersion(TInt aVersion);
 	IMPORT_C void InternalizeWithoutBufferAndVersionL(RReadStream& aStream);
	IMPORT_C void ExternalizeWithoutBufferAndVersionL(RWriteStream& aStream) const;
 	IMPORT_C void InternalizeBufferL(RReadStream& aStream);
	IMPORT_C void ExternalizeBufferL(RWriteStream& aStream) const;
 	IMPORT_C void InternalizeVersionL(RReadStream& aStream);
	IMPORT_C void ExternalizeVersionL(RWriteStream& aStream) const;

    void* AdditionalInfo() {return (void*)iAdditionalInfo;};
    IMPORT_C CSmsIEOperation& GetOperationsForIEL(CSmsInformationElement::TSmsInformationElementIdentifier) const;
    IMPORT_C CSmsNonIEOperation& GetOperationsForNonIEL(TSmsNonIEIdentifier) const;
    
  	IMPORT_C TSmsStatusReportScheme Scheme() const;
	IMPORT_C void DecodePartialCompleteMessagePDUsL(const CArrayFix<TGsmSms>& aSmsArray, TBool aLastPartialCompleteMsg);

private:

	CSmsMessage(RFs& aFs, CSmsBufferBase* aBuffer);
	void ConstructL(const TGsmSms& aGsmSms, TBool aIsRPError,TBool aIsMobileTerminated);
	void ConstructL(CSmsPDU::TSmsPDUType aType,TBool aIsRPError);

	void CreateControlIEOperationsClassesL();
	void CreateControlNonIEOperationsClassesL();

	inline void SetIsComplete(TBool aIsComplete);
	inline void SetIsDecoded(TBool aIsDecoded);


	inline TBool BinaryData() const;
	TInt ConvertedBufferLengthL(const CSmsBufferBase& aBuffer);
	void EncodeBufferL(CArrayFix<TGsmSms>& aSmsArray,TInt aReference,const CSmsBufferBase& aBuffer,
			           TInt& aUnconvertedChars, TInt& aDowngradedChars,
			           TInt& aFreeUDUnitsInLastPDU, TBool aDoEncode=ETrue);
	void DecodeBufferL(CArrayPtr<CSmsPDU>& aSmsPDUArray,CSmsBufferBase& aBuffer);
	TBool EncodeIntoSinglePDUL(CArrayFix<TGsmSms>& aSmsArray, TInt& aUnconvertedChars,
			                   TInt& aDowngradedChars, TInt& aFreeUDUnitsInLastPDU);
	void PrepareCommandMessageL(void);
	void InstallEmsInformationElementsL(CSmsUserData& aUserData, TInt aStartPosition);
	void InstallEmsInformationElementsL(CSmsCommand& aCommand, TInt aStartPosition);
	void InstallControlInformationElementsL(CSmsUserData& aUserData, TInt aSegmentSequenceNum);
	void InstallControlInformationElementsL(CSmsCommand& aCommand);
	TBool AddEMSInformationElementsToMultiSegmentMessageL(CSmsEMSBufferSegmenter& aSegmenter, TUint& aCharsAdded,
			                                              TBool aDoEncode, TSmsEncoding& aEncoding,
			                                              RPointerArray<CEmsInformationElement>& aCorrectedFormatingIEArray,
			                                              TUint& aCurEMSIEno, TUint& aBaseAddr);
	TBool AddEMSInformationElementsToSingleSegmentMessageL(CSmsEMSBufferSegmenter& aSegmenter,
			                                               TSmsEncoding aEncoding);
	void AddControlInformationElementsToMultiSegmentMessageL(TBool aDoEncode);
	void AddControlInformationElementsToMultiSegmentMessageL(TSmsInformationElementCategories::TInformationElementCategory aCategory, TBool aMandatoryInPDU, TBool aDoEncode);
	void AddCurrentPDUToPDUArrayL(TBool aDoEncode);
	TBool AddIEToUserDataL(CEmsInformationElement* aIe, TInt aCharsAlreadyAdded,TUint& aCharsAddedToCurrentPDU,CSmsEMSBufferSegmenter& aSeg);
	TInt FillPduL(CSmsEMSBufferSegmenter& aSeg, TInt aNumChars, TSmsEncoding aEncoding);
	TInt NumMessageEmsPDUsL();
	void CorrectFormattingL(TUint aCharsAddedToCurrentPDU,RPointerArray<CEmsInformationElement>& aCorrectedFormatingIEArray,TUint aBaseAddr);
	void CorrectFormattingInSinglePDUL();
	void ResetWorkingPDUL();
	TBool CanBeRemoved(const CEmsInformationElement& aIE, TUint aIEIndex);
	void AddEmsUserPromptL(const CEmsUserPrompt& aEmsIE);
	void AddEmsObjectDistributionL(const CEmsObjectDistribution& aObjectDistributionIE);
	void UpdateUserPromptAndODIElementsStartPosition();
	TInt AddReceivedEmsInformationElement(CEmsInformationElement* aIE);
	TSmsStatusReportScheme FindSchemeL(const CArrayPtr<CSmsPDU>& aSmsPDUArray);
	void InstallTPSRRInformationL(const CArrayPtr<CSmsPDU>& aSmsPDUArray, TInt aSegmentSequenceNum);
	void EncodingTPSRRFromSchemesIntoSinglePDUL();
	void DecodeOnlyTextL(CArrayPtr<CSmsPDU>& aSmsPDUArray,CSmsBufferBase& aBuffer);
	void AddIncompleteMessageInfoL(TInt aStartPDU, TInt aEndPDU, TBool aLastPartialCompleteMsg);
	void MergeAlternative7bitEncoding(TSmsEncoding aEncoding) const;

private:
	enum TSmsMessageFlags
		{
		ESmsFlagIsComplete=0x01,
		ESmsFlagIsDecoded=0x02,

		ESmsStorageMask=0x0C,
		ESmsUTCOffsetSecondGranularityMask  =0x1FFFF0,//Bits needed to represent 79 * 15 * 60 seconds
		ESmsUTCOffsetSignBit                =0x200000
		};

	enum TTimeBitMaskOffsets
		{
		ESecondBitOffset   =  4,
		ESignBitOffset     = 21
		};

    enum TDecodedOnSimBit
        {
        EDecodedOnSimBit = 0x80000000
        };

    enum TForwardToClientBit
        {
        EForwardToClientBit = 0x40000000
        };


private:

	TInt iFlags;
	NMobileSmsStore::TMobileSmsStoreStatus iStatus;
	TInt iLogServerId;
	TTime iTime;
	CSmsPDU* iSmsPDU;
	CSmsBufferBase* iBuffer;
	RFs& iFs;

	TBool iIs16BitConcatenation;
	RPointerArray<CEmsInformationElement>* iInformationElementArray;
	CCnvCharacterSetConverter* iCharacterSetConverter;

public:
	CArrayFixFlat<TGsmSmsSlotEntry> iSlotArray; //< Array for slots
/**
 *  SMS versions
 */
	enum TSmsMessageVersion
		{
	/** The default value. */
		ESmsMessageV0, 
	/** Release 6 SMS Support */	
		ESmsMessageV1, 
	/** Latest version */
		ESmsMessageV2,
	/**	Incomplete Class0 Information Support	*/
		ESmsMessageV3,
		ESmsIncompleteClass0MessageV = ESmsMessageV3,
	/** National language 7bit encoding support */
		ESmsMessageV4
		// for all other versions the corresponding enum values would be added i.e. ESmsMessageV5, etc. 
		};

protected:

	/**
	 *  iVersion is the version of CSmsMessage. This parameter will be
	 *  used to identify what iAdditionalInfo points to for that particular
	 *  version of CSmsMessage
	 */
	TInt iVersion;

	/**
	 *  iAdditionalInfo would point to any new added data structure.
	 *  iVersion would identify which data structure is valid for that
	 *   *  particular version of CSmsMessage 
	 */
	CSmsMessageAdditionalAttributes*  iAdditionalInfo;
	};

#include <gsmumsg.inl>

#endif // !defined __GSMUMSG_H__
