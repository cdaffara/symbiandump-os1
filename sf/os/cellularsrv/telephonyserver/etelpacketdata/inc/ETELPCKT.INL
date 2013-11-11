// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

inline TBool RPacketContext::TPacketFilterV3::IsPresent(TPacketFilterDataBitmapPos aDataMemberBitmapPos)
/**
	This method may be called by the client application. 
	Checks if the bitmap of a certain attribute is set or not.
	
	@param aDataMemberBitmapPos Bitmap of an attribute.

*/
	{
	return aDataMemberBitmapPos & iPresentDataBitmap;
	}
	
inline void RPacketContext::TPacketFilterV3::Reset(TPacketFilterDataBitmapPos aDataMemberBitmapPos)
/**
	This method may be called by the client application. 
	Resets the iPresentDataBitmap of an attribute accordign to its Bitmap value.
	
	@param aDataMemberBitmapPos Bitmap of an attribute.

*/
	{
	iPresentDataBitmap &= ~aDataMemberBitmapPos;
	}
	
inline void RPacketContext::TPacketFilterV3::Reset()
/**
	This method may be called by the client application. 
	Sets the iPresentDataBitmap to 0.

*/
	{
	iPresentDataBitmap = 0;
	}
	
inline TInt RPacketContext::TPacketFilterV3::ProtocolNumberOrNextHeader()
/**
	This method may be called by the client application. 
	Gets the iProtocolNumberOrNextHeader value which has been set by the 
	SetProtocolNumberOrNextHeader method.

*/
	{
	return iProtocolNumberOrNextHeader;
	}
		
inline TInt RPacketContext::TPacketFilterV3::SrcPortMin()
/**
	This method may be called by the client application. 
	Gets the iSrcPortMin value which has been set by the 
	SetSrcPortMin method.

*/
	{
	return iSrcPortMin;
	}
	
inline TInt RPacketContext::TPacketFilterV3::SrcPortMax()
/**
	This method may be called by the client application. 
	Gets the iSrcPortMax value which has been set by the 
	SetSrcPortMax method.

*/
	{
	return iSrcPortMax;
	}
		
inline TInt RPacketContext::TPacketFilterV3::DestPortMin()
/**
	This method may be called by the client application. 
	Gets the iDestPortMin value which has been set by the 
	SetDestPortMin method.

*/
	{
	return iDestPortMin;
	}
	
inline TInt RPacketContext::TPacketFilterV3::DestPortMax()
/**
	This method may be called by the client application. 
	Gets the iDestPortMax value which has been set by the 
	SetDestPortMax method.

*/
	{
	return iDestPortMax;
	}
		
inline TUint32 RPacketContext::TPacketFilterV3::SPI()
/**
	This method may be called by the client application. 
	Gets the iIPSecSPI value which has been set by the 
	SetSPI method.

*/
	{
	return iIPSecSPI;
	}
		
inline TUint16 RPacketContext::TPacketFilterV3::TOSorTrafficClass()
/**
	This method may be called by the client application. 
	Gets the iTOSorTrafficClass value which has been set by the 
	SetTOSorTrafficClass method.

*/
	{
	return iTOSorTrafficClass;
	}
	
inline TUint32 RPacketContext::TPacketFilterV3::FlowLabel()
/**
	This method may be called by the client application. 
	Gets the iFlowLabel value which has been set by the 
	SetFlowLabel method.

*/
	{
	return iFlowLabel;
	}
	
/** 
Default constructor.
*/
inline RPacketContext::TPcoId::TPcoId():iId(0)
	{}

/** 
Constructor that initialises Id by provoded value.

@param aId Id ofIE identifier.
*/	
inline RPacketContext::TPcoId::TPcoId(TUint16 aId):iId(aId)
	{}

/** 
Sets PCO Id.

@param aId PCO Id to set.
*/	
inline void RPacketContext::TPcoId::SetId(TUint16 aId)
	{
	iId=aId;
	}
	
/** 
Retrieves PCO Id.

@return PCO id.
*/
inline TUint16 RPacketContext::TPcoId::Id() const
	{
	return iId;
	}
	
/** 
Compares whether the PCO Id of other PCoId is the same.

@param PCO Id to compare.
*/		
inline TBool RPacketContext::TPcoId::IsEqual(const MTlvItemIdType& aOtherIdType) const
	{
	return iId==(static_cast<const TPcoId&>(aOtherIdType)).Id();
	}

/** 
Length of serialised data.

@return Length of id when serialized.
*/
inline TUint RPacketContext::TPcoId::SerializedLength()const
	{
	return sizeof(TUint16);
	}

/** 
Serialise to the descriptor provided

Converts internally stored little-endian data 		
to big-endian encoded data as specified in 3GPP TS 24.008, table 10.5.154.

@param aData On completion contains serialized PCO item identifier.
*/		
inline void RPacketContext::TPcoId::ExternalizeL(TDes8& aData) const
	{
	if(aData.Length()<2)
		User::Leave(KErrOverflow);
	aData[0]=(iId & 0xff00)>>8;
	aData[1]=iId & 0xff;	
	}

/** 
Internalize data from the buffer provided.

Converts internally stored little-endian data 		
to big-endian encoded data as specified in 3GPP TS 24.008, table 10.5.154.

@param aData Buffer containing PCO item Id to be internalized.
*/
inline void RPacketContext::TPcoId::InternalizeL(TDesC8& aData)
	{
	if(aData.Length()<2)
		User::Leave(KErrUnderflow);
	iId=aData[1];
	iId|=aData[0]<<8;	
	}
	
/** 
Constructor initialise length by provided value.

@param aLen Length of PCO item.
*/
inline RPacketContext::TPcoItemDataLength::TPcoItemDataLength(TUint8 aLen):iDataLength(aLen)
	{}

/** 
Default constructor.
*/		
inline RPacketContext::TPcoItemDataLength::TPcoItemDataLength():iDataLength(0)
	{}
	
/** 
Sets length of associated data.

@param aLength Length of PCO item.
*/	
inline void RPacketContext::TPcoItemDataLength::SetDataLength(TUint aLength)
	{
	iDataLength=(TUint8)aLength;
	}

/** 
Length when serialised.

@return Seriliazed length.
 */	
inline TUint RPacketContext::TPcoItemDataLength::SerializedLength() const
	{
	return sizeof(TUint8);
	}

/** 
Length of associated item data.

@return Length of PCO item.
*/		
inline TUint RPacketContext::TPcoItemDataLength::DataLength() const
	{
	return iDataLength;
	};

/** 
Serialize data member into provoded descriptor.

@param aData buffer into which length will be serialized.
*/	
inline void RPacketContext::TPcoItemDataLength::ExternalizeL(TDes8& aData) const
	{
	if(aData.Length()<1)
		User::Leave(KErrOverflow);
	aData[0]=iDataLength;
	}

/** 
Deserialize data from provided buffer

@param aData Buffer containing length to be internalized.
*/
inline void RPacketContext::TPcoItemDataLength::InternalizeL(TDesC8& aData)
	{	
	if(aData.Length()<1)
		User::Leave(KErrUnderflow);
	iDataLength=aData[0];
	}
	
