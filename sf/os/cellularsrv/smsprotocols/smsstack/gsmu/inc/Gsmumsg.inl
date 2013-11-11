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
//

/**
 @file
 
 Gets the SMS-XXX message type.
 
 @return SMS-XXX message type 
 
*/
inline CSmsPDU::TSmsPDUType CSmsMessage::Type() const
	{
	return SmsPDU().Type();
	}


/**
 *  Tests if the SMS message is complete.
 *  
 *  @return True if complete 
 */
inline TBool CSmsMessage::IsComplete() const
	{
	return iFlags&ESmsFlagIsComplete;
	}


/**
 *  Tests if the SMS message is Decoded.
 *  
 *  @return True if Decoded 
 */
inline TBool CSmsMessage::IsDecoded() const
	{
	return iFlags&ESmsFlagIsDecoded;
	}


/**
 *  Gets the physical location where the message represented by the object is stored.
 *  
 *  @return Physical location 
 */
inline CSmsMessage::TMobileSmsStorage CSmsMessage::Storage() const
	{
	return (TMobileSmsStorage) (iFlags&ESmsStorageMask);
	}


/**
 *  Sets the physical location where the message represented by the object is stored.
 *  
 *  @param aStorage Physical location 
 */
inline void CSmsMessage::SetStorage(TMobileSmsStorage aStorage)
	{
	iFlags=aStorage|(iFlags&(~ESmsStorageMask));
	}


/**
 *  Gets the SMS store status of the message.
 *  
 *  @return SMS store status 
 */
inline NMobileSmsStore::TMobileSmsStoreStatus CSmsMessage::Status() const
	{
	return iStatus;
	}


/**
 *  Sets the SMS store status of the message.
 *  
 *  @param aStatus SMS store status 
 */
inline void CSmsMessage::SetStatus(NMobileSmsStore::TMobileSmsStoreStatus aStatus)
	{
	iStatus=aStatus;
	}


/**
 *  Gets the Log Server Id.
 *  
 *  @return Log Server Id 
 */
inline TInt CSmsMessage::LogServerId() const
	{
	return iLogServerId;
	}


/**
 *  Sets the Log Server Id.
 *  
 *  @param aId Log Server Id 
 */
inline void CSmsMessage::SetLogServerId(TInt aId)
	{
	iLogServerId=aId;
	}


/**
 *  Gets the time of message creation.
 *  
 *  @return Time of message creation 
 */
inline const TTime& CSmsMessage::Time() const
	{
	return iTime;
	}


/**
 *  Sets the time of message creation.
 *  
 *  @param aTime Time of message creation 
 */
inline void CSmsMessage::SetTime(const TTime& aTime)
	{
	iTime=aTime;
	}


/**
 *  Gets the message PDU.
 *  
 *  @return PDU. 
 */
inline CSmsPDU& CSmsMessage::SmsPDU()
	{
	return *iSmsPDU;
	}


/**
 *  Gets the message PDU (const).
 *  
 *  @return PDU. 
 */
inline const CSmsPDU& CSmsMessage::SmsPDU() const
	{
	return *iSmsPDU;
	}


/**
 *  Gets the message Service Center Address.
 *  
 *  @return Service Center Address 
 */
inline TPtrC CSmsMessage::ServiceCenterAddress() const
	{
	return SmsPDU().ServiceCenterAddress();
	}


/**
 *  Sets the message Service Center Address.
 *  
 *  @param aAddress Service Center Address 
 */
inline void CSmsMessage::SetServiceCenterAddressL(const TDesC& aAddress)
	{
	SmsPDU().SetServiceCenterAddressL(aAddress);
	}


/**
 *  Gets the Service Center Address as an ETSI-formatted telephone number.
 *  
 *  @param aParsedAddress Service Center Address 
 */
inline void CSmsMessage::ParsedServiceCenterAddress(TGsmSmsTelNumber& aParsedAddress) const
	{
	SmsPDU().ParsedServiceCenterAddress(aParsedAddress);
	}


/**
 *  Sets the Service Center Address as an ETSI-formatted telephone number.
 *  
 *  @param aParsedAddress Service Center Address 
 */
inline void CSmsMessage::SetParsedServiceCenterAddressL(const TGsmSmsTelNumber& aParsedAddress)
	{
	SmsPDU().SetParsedServiceCenterAddressL(aParsedAddress);
	}


/**
 *  Gets unparsed To and From addresses.
 *  
 *  @return To and From addresses 
 */
inline TPtrC CSmsMessage::ToFromAddress() const
	{
	return SmsPDU().ToFromAddress();
	}


/**
 *  Sets unparsed To and From addresses.
 *  
 *  @param aAddress To and From addresses 
 */
inline void CSmsMessage::SetToFromAddressL(const TDesC& aAddress)
	{
	SmsPDU().SetToFromAddressL(aAddress);
	}


/**
 *  Gets To and From addresses in ETSI format.
 *  
 *  @param aParsedAddress To and From addresses 
 */
inline void CSmsMessage::ParsedToFromAddress(TGsmSmsTelNumber& aParsedAddress) const
	{
	SmsPDU().ParsedToFromAddress(aParsedAddress);
	}


/**
 *  Sets To and From addresses in ETSI format.
 *  
 *  @param aParsedAddress To and From addresses 
 */
inline void CSmsMessage::SetParsedToFromAddressL(const TGsmSmsTelNumber& aParsedAddress)
	{
	SmsPDU().SetParsedToFromAddressL(aParsedAddress);
	}


/**
 *  Gets the text portion of the message (non-const).
 *  
 *  @return Text portion of the message 
 */
inline CSmsBufferBase& CSmsMessage::Buffer()
	{
	return *iBuffer;
	}


/**
 *  @return The text portion of the CSmsMessage (const)
 */
inline const CSmsBufferBase& CSmsMessage::Buffer() const
	{
	return *iBuffer;
	}


/**
 *  @param aIsComplete Set to True for CSmsMessage complete
 */
inline void CSmsMessage::SetIsComplete(TBool aIsComplete)
	{
	iFlags=aIsComplete? iFlags|ESmsFlagIsComplete: iFlags&(~ESmsFlagIsComplete);
	}


/**
 *  @param aIsDecoded Set to True for CSmsMessage decoded
 */
inline void CSmsMessage::SetIsDecoded(TBool aIsDecoded)
	{
	iFlags=aIsDecoded? iFlags|ESmsFlagIsDecoded: iFlags&(~ESmsFlagIsDecoded);
	}


/**
 *  @return True if the User Data is Binary
 */
inline TBool CSmsMessage::BinaryData() const
	{
	return SmsPDU().UserData().IsBinaryData();
	}


inline void CSmsMessage::Set16BitConcatenation(TBool aIs16BitConcatenation)
	{
	iIs16BitConcatenation=aIs16BitConcatenation;
	}


inline TBool CSmsMessage::Is16BitConcatenation()const
	{
	return iIs16BitConcatenation;
	}
