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
// Inline methods for operations on PDU elements
// 
//

/**
 @file
*/

#include <s32strm.h>
#include <badesca.h>


/**
 *  Leaves with KErrGsmuDecoding.
 */
inline void TGsmuLex8::LeaveL() const
	{
	User::Leave(KErrGsmuDecoding);
	}


/**
 *  Constructor, initialising the octet value with a TInt8.
 *  
 *  @param aValue Octet value 
 */
inline TSmsOctet::TSmsOctet(TUint8 aValue)
: iValue(aValue)
	{
	// NOP
	}


/**
 *  Constructor, initialising the octet value with a TInt.
 *  
 *  @param aValue Octet value 
 */
inline TSmsOctet::TSmsOctet(TInt aValue)
	:iValue(static_cast<TUint8>(aValue))
	{
	}


/**
 *  Sets the octet value to the next byte in TGsmuLex8 PDU.
 *  
 *  @param aPdu PDU to read value from 
 */
inline void TSmsOctet::DecodeL(TGsmuLex8& aPdu)
	{
	iValue=aPdu.GetL();
	}


/**
 *  @return The octet value
 */
inline TSmsOctet::operator TInt() const
	{
	return iValue;
	}


/**
 *  Converts an integer to semi-octet format and set the object's data to this
 *  value.
 *  
 *  @param aNum Value to convert to semi-octet 
 */
inline void TSmsOctet::FillSemiOctets(TInt aNum)
	{
	*this=(aNum/10)|((aNum%10)<<4);
	}


/**
 *  Converts a semi-octet value to an integer.
 *  
 *  @return Object's data as an integer 
 */
inline TInt TSmsOctet::SemiOctetsToNum() const
	{
	return ((iValue&0x0F)*10) + ((iValue&0xF0)>>4);
	}


/**
 *  Internalises the object.
 *  
 *  @param aStream Stream to read from 
 */
inline void TSmsOctet::InternalizeL(RReadStream& aStream)
	{
	aStream >> iValue;
	}


/**
 *  Externalises the object.
 *  
 *  @param aStream Stream to write to 
 */
inline void TSmsOctet::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iValue;
	}


/**
 *  Gets the error code.
 *  
 *  @return Error code 
 */
inline TInt TSmsFailureCause::Error() const
	{
	return iValue;
	}


/**
 *  Sets the error code.
 *  
 *  @param aError Error code 
 */
inline void TSmsFailureCause::SetError(TSmsFailureCauseError aError)
	{
	iValue=(TUint8) aError;
	}


/**
 *  Gets status value.
 *  
 *  @return Status value 
 */
TSmsStatus::TSmsStatusValue TSmsStatus::Status() const
	{
	return (TSmsStatusValue) iValue;
	}


/**
 *  Sets status value.
 *  
 *  @param aValue Status value 
 */
inline void TSmsStatus::SetStatus(TSmsStatusValue aValue)
	{
	iValue=(TUint8) aValue;
	}


/**
 *  Gets command type.
 *  
 *  @return Command type 
 */
inline TInt TSmsCommandType::CommandType() const
	{
	return iValue;
	}


/**
 *  Sets command type.
 *  
 *  @param aType Command type 
 */
inline void TSmsCommandType::SetCommandType(TSmsCommandTypeValue aType)
	{
	iValue=(TUint8) aType;
	}


/**
 *  Tests if the extension flag is set.
 *  
 *  @return True if data contains a PID extension 
 */
inline TBool TSmsParameterIndicator::Extension() const
	{
	return iValue&ESmsPIDExtension;
	}


/**
 *  Sets extension flag.
 *  
 *  @param aExtension Extension flag 
 */
inline void TSmsParameterIndicator::SetExtension(TBool aExtension)
	{
	iValue=(TUint8) (aExtension? iValue|ESmsPIDExtension: (iValue&(~ESmsPIDExtension)));
	}


/**
 *  Tests user data present flag.
 *  
 *  @return True if User Data is indicated 
 */
inline TBool TSmsParameterIndicator::UserDataPresent() const
	{
	return iValue&ESmsPIDUserDataPresent;
	}


/**
 *  Sets user data present flag.
 *  
 *  @param aPresent Set to True/False to indicate/clear User Data 
 */
inline void TSmsParameterIndicator::SetUserDataPresent(TBool aPresent)
	{
	iValue=(TUint8) (aPresent? iValue|ESmsPIDUserDataPresent: iValue&(~ESmsPIDUserDataPresent));
	}


/**
 *  Tests data coding scheme present flag.
 *  
 *  @return True to indicate Data Coding Scheme present 
 */
inline TBool TSmsParameterIndicator::DataCodingSchemePresent() const
	{
	return iValue&ESmsPIDDataCodingSchemePresent;
	}


/**
 *  Sets data coding scheme present flag.
 *  
 *  @param aPresent Set to True/False to indicate/clear Data Coding Scheme present 
 */
inline void TSmsParameterIndicator::SetDataCodingSchemePresent(TBool aPresent)
	{
	iValue=(TUint8) (aPresent? iValue|ESmsPIDDataCodingSchemePresent: iValue&(~ESmsPIDDataCodingSchemePresent));
	}


/**
 *  Tests protocol identifier present flag.
 *  
 *  @return True to indicate Protocol ID present 
 */
inline TBool TSmsParameterIndicator::ProtocolIdentifierPresent() const
	{
	return iValue&ESmsPIDProtocolIdentifierPresent;
	}


/**
 *  Sets protocol identifier present flag.
 *  
 *  @param aPresent PSet to True/False to indicate/clear Protocol ID present 
 */
inline void TSmsParameterIndicator::SetProtocolIdentifierPresent(TBool aPresent)
	{
	iValue=(TUint8) (aPresent? iValue|ESmsPIDProtocolIdentifierPresent: iValue&(~ESmsPIDProtocolIdentifierPresent));
	}


/**
 *  Gets the Protocol ID type.
 *  
 *  @return Protocol ID type 
 */
inline TSmsProtocolIdentifier::TSmsPIDType TSmsProtocolIdentifier::PIDType() const
	{
	return (TSmsPIDType) (iValue&ESmsPIDTypeMask);
	}


/**
 *  Sets the Protocol ID type.
 *  
 *  @param aSmsPIDType Protocol ID type 
 */
inline void TSmsProtocolIdentifier::SetPIDType(TSmsPIDType aSmsPIDType)
	{
	iValue=(TUint8) aSmsPIDType;
	}


/**
 *  Gets bits 7 - 4 of the Data Coding Scheme.
 *  
 *  @return Bits 7 - 4 of the Data Coding Scheme 
 */
inline TSmsDataCodingScheme::TSmsDCSBits7To4 TSmsDataCodingScheme::Bits7To4() const
	{
	return (TSmsDCSBits7To4) (iValue&ESmsDCSBits7To4Mask);
	}


/**
 *  Sets bits 7 - 4 of the Data Coding Scheme.
 *  
 *  @param aBits7To4 Bits 7 - 4 of the Data Coding Scheme 
 */
inline void TSmsDataCodingScheme::SetBits7To4(TSmsDCSBits7To4 aBits7To4)
	{
	iValue=(TUint8) (aBits7To4);
	}


/**
 *  Constructor, specifying an octet value.
 *  
 *  @param aValue Octet value 
 */
inline TGsmSmsTypeOfAddress::TGsmSmsTypeOfAddress(TInt aValue)
: TSmsOctet(aValue)
	{
	}


/**
 *  Constructor, specifying type of numbering and numbering plan identification.
 *  
 *  @param aTon Type of numbering
 *  @param aNPI Numbering plan identification 
 */
inline TGsmSmsTypeOfAddress::TGsmSmsTypeOfAddress(TGsmSmsTypeOfNumber aTon, TGsmSmsNumberingPlanIdentification aNPI)
: TSmsOctet(EGsmSmsFirstBitMask)
	{
	SetTON(aTon);
	SetNPI(aNPI);
	}


/**
 *  Gets the type of numbering.
 *  
 *  @return Type of numbering 
 */
inline TGsmSmsTypeOfNumber TGsmSmsTypeOfAddress::TON() const
	{
	return (TGsmSmsTypeOfNumber) (iValue & EGsmSmsTONMask);
	}


/**
 *  Sets the type of numbering.
 *  
 *  @param aTON Type of numbering 
 */
inline void TGsmSmsTypeOfAddress::SetTON(TGsmSmsTypeOfNumber aTON)
	{
	iValue = (TUint8) ((iValue & (~EGsmSmsTONMask)) | aTON);
	}


/**
 *  Gets the numbering plan identification.
 *  
 *  @return Numbering plan identification 
 */
inline TGsmSmsNumberingPlanIdentification TGsmSmsTypeOfAddress::NPI() const
	{
	return (TGsmSmsNumberingPlanIdentification) (iValue & EGsmSmsNPIMask);
	}


/**
 *  Set the Numbering Plan Identification 
 */
inline void TGsmSmsTypeOfAddress::SetNPI(TGsmSmsNumberingPlanIdentification aNumberingPlanIdentification)
	{
	iValue = (TUint8) ((iValue &(~EGsmSmsNPIMask)) | aNumberingPlanIdentification);
	}


/**
 *  Constructor
 */
inline TGsmSmsTelNumber::TGsmSmsTelNumber()
	{
	}


/**
 *  Gets the time zone offset from GMT in +/- quarter hours.
 *  The allowed range is +/-79 per 23.040 V4.4.0 9.2.3.11.
 *  
 *  @return Time zone offset 
 */
inline TInt TSmsServiceCenterTimeStamp::TimeOffset() const
	{
	return iTimeZoneNumQuarterHours;
	}


/**
 *  Gets the Service Center Time in Universal Time.
 *  
 *  @return The Service Center Time 
 */
inline const TTime& TSmsServiceCenterTimeStamp::Time() const
	{
	return iTime;
	}


/**
 *  Sets the Service Center Time Stamp in Universal Time.
 *  
 *  @param aTime The Service Center Time 
 */
inline void TSmsServiceCenterTimeStamp::SetTime(const TTime& aTime)
	{
	iTime=aTime;
	}


/**
 *  Gets the message Validity Period Format.
 *  
 *  @return The message Validity Period Format 
 */
inline TSmsFirstOctet::TSmsValidityPeriodFormat TSmsValidityPeriod::ValidityPeriodFormat() const
	{
	return (TSmsFirstOctet::TSmsValidityPeriodFormat) (iFirstOctet&TSmsFirstOctet::ESmsVPFMask);
	}


/**
 *  Sets the message Validity Period Format.
 *  
 *  @param aValidityPeriodFormat The message Validity Period Format 
 */
inline void TSmsValidityPeriod::SetValidityPeriodFormat(TSmsFirstOctet::TSmsValidityPeriodFormat aValidityPeriodFormat)
	{
	iFirstOctet=(iFirstOctet&(~TSmsFirstOctet::ESmsVPFMask)|aValidityPeriodFormat);
	}


/**
 *  Gets the message Validity Period in minutes.
 *  
 *  @return The message Validity Period in minutes 
 */
inline const TTimeIntervalMinutes& TSmsValidityPeriod::TimeIntervalMinutes() const
	{
	return iTimeIntervalMinutes;
	}


/**
 *  Sets the message Validity Period in minutes.
 *  
 *  @param aTimeIntervalMinutes The message Validity Period in minutes 
 */
inline void TSmsValidityPeriod::SetTimeIntervalMinutes(const TTimeIntervalMinutes& aTimeIntervalMinutes)
	{
	iTimeIntervalMinutes=aTimeIntervalMinutes;
	}


/**
 *  Resets the unconverted native character buffer.
 */
inline void CSmsAlphabetConverter::ResetUnconvertedNativeCharacters()
	{
	iUnconvertedNativeCharactersPtr.Zero();
	}


/**
 *  Resets the unconverted user data elements buffer.
 */
inline void CSmsAlphabetConverter::ResetUnconvertedUDElements()
	{
	iUnconvertedUDElementsPtr.Zero();
	}


/**
 *  Gets the unconverted native characters.
 *  
 *  @return Unconverted native characters 
 */
inline TPtrC CSmsAlphabetConverter::UnconvertedNativeCharacters()
	{
	return iUnconvertedNativeCharactersPtr;
	}


/**
 *  Gets the unconverted user data elements.
 *  
 *  @return Unconverted user data elements 
 */
inline TPtrC8 CSmsAlphabetConverter::UnconvertedUDElements()
	{
	return iUnconvertedUDElementsPtr;
	}


/**
 *  Gets the coding scheme alphabet.
 *  
 *  @return Coding scheme alphabet 
 */
inline TSmsDataCodingScheme::TSmsAlphabet CSmsAlphabetConverter::Alphabet() const
	{
	return iSmsAlphabet;
	}


/**
 *  @publishedAll
 *  @released
 */
inline CSmsInformationElement::CSmsInformationElement(TSmsInformationElementIdentifier aInformationElementIdentifier):iIdentifier(aInformationElementIdentifier)
	{
	}


/**
 *  Gets the number of information elements.
 *  
 *  @return Number of information elements 
 */
inline TInt CSmsUserData::NumInformationElements() const
	{
	return iInformationElementArray.Count();
	}


/**
 *  @return The number of Command Information Elements
 *  @publishedAll
 *  @released
 */
inline TInt CSmsCommandData::NumInformationElements() const
	{
	return iInformationElementArray.Count();
	}


/**
 *  @return The amount of space in the Command buffer
 *  @publishedAll
 *  @released
 */
inline TInt CSmsCommandData::MaxDataLength() const
	{
	return KSmsMaxDataSize-iBuffer->Des().Length();
	}
