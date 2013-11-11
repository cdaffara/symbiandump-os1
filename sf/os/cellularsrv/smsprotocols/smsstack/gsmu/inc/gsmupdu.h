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
// Contains a class representing the generic interface to a GSM SMS PDU (CSmsPDU) as well as six concrete implementations representing the six types of CSmsPDU, namely CSmsDeliver, CSmsSubmit, CSmsDeliverReport, CSmsSubmitReport, CSmsStatusReport and CSmsCommand
// 
//

/**
 @file
*/

#ifndef __GSMUPDU_H__
#define __GSMUPDU_H__

#include <gsmuelem.h>
// forwarded class declarations

class TGsmSms;
class CSmsAddress;
struct TEncodeParams;

/**
 *  A generic interface to a GSM SMS PDU.
 *  
 *  The class can be used on its own to encode GSM SMS PDUs. It is also the base
 *  class for the six real implementations of GSM SMS PDUs. The derived classes
 *  are built up from the elements defined in gsmuelem.h according to the ETSI
 *  GSM 03.40. All the SMS elements have generic encode and decode functions so
 *  that each PDU type knows how to encode and decode itself into a TGsmSms.
 *  
 *  CSmsPDU and derived classes provide a human readable interface to a GSM SMS
 *  PDU, for example setting the destination address on a SUBMIT (CSmsSubmit).
 *  All the factory functions for CSmsPDUs are provided in the base class.
 *  
 *  @publishedAll
 *  @released 
 */
class CSmsPDU : public CBase
	{
public:
//note the enum values must be kept to ensure future binary compatibility with the TSAREntry store.
/** Flags for the 6 basic PDU types in GSM SMS messaging. */
	enum TSmsPDUType
		{
		ESmsDeliver       = 0, //< SMS-DELIVER, sent from SC to MS
		ESmsDeliverReport = 1, //< SMS-DELIVER-REPORT, sent from MS to SC
		ESmsSubmit        = 2, //< SMS-SUBMIT, sent from MS to SC
		ESmsSubmitReport  = 3, //< SMS-SUBMIT-REPORT, sent from SC to MS
		ESmsStatusReport  = 4, //< SMS-STATUS-REQUEST, sent from SC to MS
		ESmsCommand       = 5  //< SMS-COMMAND, sent from MS to SC
		};
public:
	IMPORT_C static CSmsPDU* NewL(RReadStream &aStream,CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	IMPORT_C static CSmsPDU* NewL(const TGsmSms& aGsmSms,CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TBool aIsRPError=EFalse,TBool aIsMobileTerminated=ETrue);
	IMPORT_C static CSmsPDU* NewL(TSmsPDUType aType,CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs,TBool aIsRPError=EFalse);
	
	inline TSmsPDUType Type() const;

	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	IMPORT_C void EncodeMessagePDUL(TGsmSms& aGsmSms) const;
	void EncodeMessagePDUL(TGsmSms& aGsmSms, const TEncodeParams* aEncodeParams) const;	

	IMPORT_C TPtrC ServiceCenterAddress() const;
	IMPORT_C void SetServiceCenterAddressL(const TDesC& aAddress);
	IMPORT_C void ParsedServiceCenterAddress(TGsmSmsTelNumber& aParsedAddress) const;
	IMPORT_C void SetParsedServiceCenterAddressL(const TGsmSmsTelNumber& aParsedAddress);

	IMPORT_C TPtrC ToFromAddress() const;
	IMPORT_C void SetToFromAddressL(const TDesC& aAddress);
	IMPORT_C void ParsedToFromAddress(TGsmSmsTelNumber& aParsedAddress) const;
	IMPORT_C void SetParsedToFromAddressL(const TGsmSmsTelNumber& aParsedAddress);

//  Functions on data coding scheme
//  The functions available are dependant on bits 7 to 4 in the data coding scheme.
//  Functions panic when they are not available.
//  This requires the caller to have some knowledge of ETSI GSM 03.38 in order to keep the data coding scheme in a consistent state.

	IMPORT_C TSmsDataCodingScheme::TSmsDCSBits7To4 Bits7To4() const;
	IMPORT_C void SetBits7To4(TSmsDataCodingScheme::TSmsDCSBits7To4 aBits7To4);

	IMPORT_C TSmsDataCodingScheme::TSmsAlphabet Alphabet() const;
	IMPORT_C void SetAlphabet(TSmsDataCodingScheme::TSmsAlphabet aAlphabet);
	IMPORT_C TBool Class(TSmsDataCodingScheme::TSmsClass& aClass) const;
	IMPORT_C void SetClass(TBool aClassDefined,TSmsDataCodingScheme::TSmsClass aClass);
	IMPORT_C TBool TextCompressed() const;
	IMPORT_C void SetTextCompressed(TBool aCompressed);

	IMPORT_C TSmsDataCodingScheme::TSmsIndicationState IndicationState() const;
	IMPORT_C void SetIndicationState(TSmsDataCodingScheme::TSmsIndicationState aState);
	IMPORT_C TSmsDataCodingScheme::TSmsIndicationType IndicationType() const;
	IMPORT_C void SetIndicationType(TSmsDataCodingScheme::TSmsIndicationType aType);

//  Functions on concatenation user data information element
	IMPORT_C TBool TextConcatenated(TBool* aIs16Bit=NULL) const;
	IMPORT_C void SetTextConcatenatedL(TBool aConcatenated,TBool aIs16Bit=EFalse);
	IMPORT_C TInt ConcatenatedMessageReference() const;
	IMPORT_C void SetConcatenatedMessageReference(TInt aReference);
	IMPORT_C TInt NumConcatenatedMessagePDUs() const;
	IMPORT_C void SetNumConcatenatedMessagePDUs(TInt aNum);
	IMPORT_C TInt ConcatenatedMessagePDUIndex() const;
	IMPORT_C void SetConcatenatedMessagePDUIndex(TInt aIndex);

//  Functions for application port addressing
	IMPORT_C TBool ApplicationPortAddressing(TInt& aDestination,TInt& aOriginator,TBool* aIs16Bit=NULL) const;
	IMPORT_C void SetApplicationPortAddressingL(TBool aAddressing,TInt aDestination,TInt aOriginator,TBool aIs16Bit=EFalse);

//  Functions on protocol identifier
	IMPORT_C TSmsProtocolIdentifier::TSmsPIDType PIDType() const;
	IMPORT_C void SetPIDType(TSmsProtocolIdentifier::TSmsPIDType aSmsPIDType);
	IMPORT_C TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator TelematicDeviceIndicator() const;
	IMPORT_C void SetTelematicDeviceIndicator(TSmsProtocolIdentifier::TSmsTelematicDeviceIndicator aIndicator);


	//-- concern 'ShortMessageType'
	IMPORT_C TSmsProtocolIdentifier::TSmsShortMessageType ShortMessageType() const;
	IMPORT_C void SetShortMessageType(TSmsProtocolIdentifier::TSmsShortMessageType aShortMessageType);


	IMPORT_C TSmsProtocolIdentifier::TSmsTelematicDeviceType TelematicDeviceType() const;
	IMPORT_C void SetTelematicDeviceType(TSmsProtocolIdentifier::TSmsTelematicDeviceType aDeviceType);

//  Functions on parameter indicator
	IMPORT_C TBool UserDataPresent() const;
	IMPORT_C void SetUserDataPresent(TBool aPresent);
	IMPORT_C TBool DataCodingSchemePresent() const;
	IMPORT_C void SetDataCodingSchemePresent(TBool aPresent);
	IMPORT_C TBool ProtocolIdentifierPresent() const;
	IMPORT_C void SetProtocolIdentifierPresent(TBool aPresent);

	IMPORT_C CSmsUserData& UserData();
	IMPORT_C const CSmsUserData& UserData() const;
	virtual void DecodeL(TGsmuLex8& aPdu)=0;
	void UpdateConcatenationDataL(TInt aRef, TInt aPduIndex, TInt aMaxPdu);
	void UpdateEmailHeaderDataL(TInt& aEmailOverallHeaderLength);
	
	//	SMSC Control Parameters
	TBool UpdateSMSCCtrlParameterL(const TUint8 aOctet);
	
	//	TPSRR Scheme
	TBool UpdateTPSRRL(TSmsFirstOctet aSmsReportRequest);
	
	//  National Language Encoding
	IMPORT_C TSmsEncoding NationalLanguageEncoding() const;
	IMPORT_C void SetNationalLanguageEncodingL(TSmsEncoding aEncoding);
		
	IMPORT_C CSmsPDU* DuplicateL() const;

protected:
	CSmsPDU(TSmsPDUType aSmsPDUType);

	virtual const TSmsDataCodingScheme* DataCodingScheme() const;  // Functions made const to avoid too many definitions
	virtual const TSmsProtocolIdentifier* ProtocolIdentifier() const;
	virtual const TSmsParameterIndicator* ParameterIndicator() const;
	virtual const CSmsUserData* UserDataPtr() const;
	virtual const CSmsAddress* ToFromAddressPtr() const;

	virtual void ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs)=0;
	virtual TUint8* EncodeL(TUint8* aPtr) const=0;
	virtual TUint8* EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const=0;	

	virtual void InternalizeMessagePDUL(RReadStream& aStream)=0;
	virtual void ExternalizeMessagePDUL(RWriteStream& aStream) const=0;

private:
	TBool DoTextConcatenated(TInt& aIndex,TBool* aIs16Bit) const;
	void DoSetTextConcatenatedL(TBool aIs16Bit);
	TBool DoApplicationPortAddressing(TInt& aIndex,TInt& aDestination,TInt& aOriginator,TBool* aIs16Bit) const;
	void DoSetApplicationPortAddressingL(TInt aDestination,TInt aOriginator,TBool aIs16Bit);

private:
	const TSmsPDUType iSmsPDUType;

protected:
	CSmsAddress* iServiceCenterAddress;
	};


/**
 *  SMS-DELIVER PDU - SC to MS
 *  @publishedAll
 *  @released
 */
class CSmsDeliver : public CSmsPDU
	{
public:
	 CSmsDeliver();
	~CSmsDeliver();

	IMPORT_C TBool MoreMessagesToSend() const;
	IMPORT_C void SetMoreMessagesToSend(TBool aMore);

	IMPORT_C TBool ReplyPath() const;
	IMPORT_C void SetReplyPath(TBool aReplyPath);

	IMPORT_C TBool StatusReportIndication() const;
	IMPORT_C void SetStatusReportIndication(TBool aIndication);

	IMPORT_C void ServiceCenterTimeStamp(TTime& aTime,TInt& aNumQuarterHours);
	IMPORT_C void SetServiceCenterTimeStamp(const TTime& aTime,TInt aNumQuarterHours=0);
	IMPORT_C const TSmsProtocolIdentifier* ProtocolIdentifier() const;

	IMPORT_C CSmsDeliver* DuplicateL() const;
	
protected:
	const TSmsDataCodingScheme* DataCodingScheme() const;
	const CSmsUserData* UserDataPtr() const;
	const CSmsAddress* ToFromAddressPtr() const;

	void ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	TUint8* EncodeL(TUint8* aPtr) const;
	TUint8* EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const;	
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeMessagePDUL(RReadStream& aStream);
	void ExternalizeMessagePDUL(RWriteStream& aStream) const;

private:
	TSmsFirstOctet iFirstOctet;
	CSmsAddress* iOriginalAddress;
	TSmsProtocolIdentifier iProtocolIdentifier;
	TSmsDataCodingScheme iDataCodingScheme;
	TSmsServiceCenterTimeStamp iServiceCenterTimeStamp;
	CSmsUserData* iUserData;
	};


/**
 *  SMS-SUBMIT PDU - MS to SC
 *  @publishedAll
 *  @released 
 */
class CSmsSubmit : public CSmsPDU
	{
public:
	CSmsSubmit();
	~CSmsSubmit();

	IMPORT_C TBool RejectDuplicates() const;
	IMPORT_C void SetRejectDuplicates(TBool aRejectDuplicates);

	IMPORT_C TSmsFirstOctet::TSmsValidityPeriodFormat ValidityPeriodFormat() const;
	IMPORT_C void SetValidityPeriodFormat(TSmsFirstOctet::TSmsValidityPeriodFormat aValidityPeriodFormat);

	IMPORT_C TBool ReplyPath() const;
	IMPORT_C void SetReplyPath(TBool aReplyPath);

	IMPORT_C TBool StatusReportRequest() const;
	IMPORT_C void SetStatusReportRequest(TBool aRequest);

	IMPORT_C TInt MessageReference() const;
	IMPORT_C void SetMessageReference(TInt aMessageReference);

	IMPORT_C const TTimeIntervalMinutes& ValidityPeriod() const;
	IMPORT_C void SetValidityPeriod(const TTimeIntervalMinutes& aTimeIntervalMinutes);

	IMPORT_C CSmsSubmit* DuplicateL() const;
	
protected:
	const TSmsDataCodingScheme* DataCodingScheme() const;
	const TSmsProtocolIdentifier* ProtocolIdentifier() const;
	const CSmsUserData* UserDataPtr() const;
	const CSmsAddress* ToFromAddressPtr() const;

	void ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	TUint8* EncodeL(TUint8* aPtr) const;
	TUint8* EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const;	
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeMessagePDUL(RReadStream& aStream);
	void ExternalizeMessagePDUL(RWriteStream& aStream) const;

private:
	TSmsFirstOctet iFirstOctet;
	TSmsOctet iMessageReference;
	CSmsAddress* iDestinationAddress;
	TSmsProtocolIdentifier iProtocolIdentifier;
	TSmsDataCodingScheme iDataCodingScheme;
	TSmsValidityPeriod iValidityPeriod;
	CSmsUserData* iUserData;
	};


/**
 *  SMS-DELIVER-REPORT PDU - MS to SC
 *  @publishedAll
 *  @released 
 */
class CSmsDeliverReport : public CSmsPDU
	{
public:
	CSmsDeliverReport(TBool aIsRPError=EFalse);
	~CSmsDeliverReport();

	inline TBool IsRPError() const;
	inline void SetIsRPError(TBool aIsRPError);

	IMPORT_C TInt FailureCause() const;
	IMPORT_C void SetFailureCause(TSmsFailureCause::TSmsFailureCauseError aFailureCause);

	IMPORT_C CSmsDeliverReport* DuplicateL() const;

protected:
	const TSmsDataCodingScheme* DataCodingScheme() const;
	const TSmsProtocolIdentifier* ProtocolIdentifier() const;
	const TSmsParameterIndicator* ParameterIndicator() const;
	const CSmsUserData* UserDataPtr() const;

	void ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	TUint8* EncodeL(TUint8* aPtr) const;
	TUint8* EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const;	
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeMessagePDUL(RReadStream& aStream);
	void ExternalizeMessagePDUL(RWriteStream& aStream) const;

private:
	TUint8 iIsRPError;

	TSmsFirstOctet iFirstOctet;
	TSmsFailureCause iFailureCause;
	TSmsParameterIndicator iParameterIndicator;
	TSmsProtocolIdentifier iProtocolIdentifier;
	TSmsDataCodingScheme iDataCodingScheme;
	CSmsUserData* iUserData;
	};


/**
 *  SMS-SUBMIT-REPORT PDU - SC to MS
 *  @publishedAll
 *   @released 
 */
class CSmsSubmitReport : public CSmsPDU
	{
public:
	CSmsSubmitReport(TBool aIsRPError=EFalse);
	~CSmsSubmitReport();

	inline TBool IsRPError() const;
	inline void SetIsRPError(TBool aIsRPError);

	IMPORT_C TInt FailureCause() const;
	IMPORT_C void SetFailureCause(TSmsFailureCause::TSmsFailureCauseError aFailureCause);

	IMPORT_C CSmsSubmitReport* DuplicateL() const;

protected:
	const TSmsDataCodingScheme* DataCodingScheme() const;
	const TSmsProtocolIdentifier* ProtocolIdentifier() const;
	const TSmsParameterIndicator* ParameterIndicator() const;
	const CSmsUserData* UserDataPtr() const;

	void ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	TUint8* EncodeL(TUint8* aPtr) const;
	TUint8* EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const;	
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeMessagePDUL(RReadStream& aStream);
	void ExternalizeMessagePDUL(RWriteStream& aStream) const;

private:
	TUint8 iIsRPError;

	TSmsFirstOctet iFirstOctet;
	TSmsFailureCause iFailureCause;
	TSmsParameterIndicator iParameterIndicator;
	TSmsServiceCenterTimeStamp iServiceCenterTimeStamp;
	TSmsProtocolIdentifier iProtocolIdentifier;
	TSmsDataCodingScheme iDataCodingScheme;
	CSmsUserData* iUserData;
	};


/**
 *  SMS-STATUS-REPORT PDU - SC to MS
 *  @publishedAll
 *  @released 
 */
class CSmsStatusReport : public CSmsPDU
	{
public:
	CSmsStatusReport();
	~CSmsStatusReport();

	inline TBool ParameterIndicatorPresent() const;
	inline void SetParameterIndicatorPresent(TBool aPresent);

	IMPORT_C TBool MoreMessagesToSend() const;
	IMPORT_C void SetMoreMessagesToSend(TBool aMore);

	IMPORT_C TSmsFirstOctet::TSmsStatusReportQualifier StatusReportQualifier() const;
	IMPORT_C void SetStatusReportQualifier(TSmsFirstOctet::TSmsStatusReportQualifier aQualifier);

	IMPORT_C TInt MessageReference() const;
	IMPORT_C void SetMessageReference(TInt aMessageReference);

	IMPORT_C void ServiceCenterTimeStamp(TTime& aTime,TInt& aNumQuarterHours);
	IMPORT_C void SetServiceCenterTimeStamp(const TTime& aTime,TInt& aNumQuarterHours);

	IMPORT_C void DischargeTime(TTime& aTime,TInt& aNumQuarterHours);
	IMPORT_C void SetDischargeTime(const TTime& aTime,TInt& aNumQuarterHours);

	IMPORT_C TSmsStatus::TSmsStatusValue Status() const;
	IMPORT_C void SetStatus(TSmsStatus::TSmsStatusValue aValue);

	IMPORT_C CSmsStatusReport* DuplicateL() const;

protected:
	const TSmsDataCodingScheme* DataCodingScheme() const;
	const TSmsProtocolIdentifier* ProtocolIdentifier() const;
	const TSmsParameterIndicator* ParameterIndicator() const;
	const CSmsUserData* UserDataPtr() const;
	const CSmsAddress* ToFromAddressPtr() const;

	void ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	TUint8* EncodeL(TUint8* aPtr) const;
	TUint8* EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const;	
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeMessagePDUL(RReadStream& aStream);
	void ExternalizeMessagePDUL(RWriteStream& aStream) const;

private:
	TUint8 iParameterIndicatorPresent;

	TSmsFirstOctet iFirstOctet;
	TSmsOctet iMessageReference;
	CSmsAddress* iRecipientAddress;
	TSmsServiceCenterTimeStamp iServiceCenterTimeStamp;
	TSmsServiceCenterTimeStamp iDischargeTime;
	TSmsStatus iStatus;
	TSmsParameterIndicator iParameterIndicator;
	TSmsProtocolIdentifier iProtocolIdentifier;
	TSmsDataCodingScheme iDataCodingScheme;
	CSmsUserData* iUserData;
	};


/**
 *  SMS-COMMAND PDU - MS to SC
 *  @publishedAll
 *  @released 
 */
class CSmsCommand : public CSmsPDU
	{
public:
	CSmsCommand();
	~CSmsCommand();

	IMPORT_C TBool StatusReportRequest() const;
	IMPORT_C void SetStatusReportRequest(TBool aRequest);

	IMPORT_C TInt MessageReference() const;
	IMPORT_C void SetMessageReference(TInt aMessageReference);

	IMPORT_C TInt CommandType() const;
	IMPORT_C void SetCommandType(TSmsCommandType::TSmsCommandTypeValue aCommandType);

	IMPORT_C TInt MessageNumber() const;
	IMPORT_C void SetMessageNumber(TInt aMessageNumber);

	IMPORT_C TInt NumInformationElements() const;
	IMPORT_C CSmsInformationElement& InformationElement(TInt aIndex) const;
	         CSmsInformationElement*& InformationElementPtr(TInt aIndex) const;
	IMPORT_C TBool InformationElementIndex(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier,
		TInt& aIndex) const;
	IMPORT_C void AddInformationElementL(CSmsInformationElement::TSmsInformationElementIdentifier aIdentifier, TDesC8& aData);
	IMPORT_C void RemoveInformationElement(TInt aIndex);

	IMPORT_C TInt MaxCommandDataLength() const;
	IMPORT_C TPtrC8 CommandData() const;
	IMPORT_C void SetCommandDataL(const TDesC8& aData);

	IMPORT_C CSmsCommand* DuplicateL() const;

protected:
	const TSmsProtocolIdentifier* ProtocolIdentifier() const;
	const CSmsAddress* ToFromAddressPtr() const;

	void ConstructL(CCnvCharacterSetConverter& aCharacterSetConverter,RFs& aFs);
	TUint8* EncodeL(TUint8* aPtr) const;
	TUint8* EncodeL(TUint8* aPtr, const TEncodeParams* aEncodeParams) const;	
	void DecodeL(TGsmuLex8& aPdu);
	void InternalizeMessagePDUL(RReadStream& aStream);
	void ExternalizeMessagePDUL(RWriteStream& aStream) const;
	void DoSetStatusReportRequest(TBool aRequest);

private:
	TSmsFirstOctet iFirstOctet;
	TSmsOctet iMessageReference;
	TSmsProtocolIdentifier iProtocolIdentifier;
	TSmsCommandType	iCommandType;
	TSmsOctet iMessageNumber;
	CSmsAddress* iDestinationAddress;
	CSmsCommandData* iCommandData;
	};

#include "gsmupdu.inl"

#endif // !defined __GSMUPDU_H__
