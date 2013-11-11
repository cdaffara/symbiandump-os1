// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <lbs/lbsloccommon.h>
#include <lbs/lbslocclasstypes.h>

#ifndef EKA2

GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
/** DLL entry point.
@publishedAll
@released
@param aReason Ignored
@return Always KErrNone
*/
	{
	return KErrNone;
	}

#endif

//------------------------------------------------------------------------------
// TLbsClassTypeBase. 
//------------------------------------------------------------------------------
EXPORT_C TLbsLocClassTypeBase::TLbsLocClassTypeBase()
/** Default constructor for TLbsLocClassTypeBase

*/
:	iClassType(0),
	iClassSize(0)
	{}

EXPORT_C TUint TLbsLocClassTypeBase::ClassSize() const
/** Returns the size of the TPositionClassTypeBase-derived class
@return The size of the TPositionClassTypeBase-derived class
*/
	{
	return iClassSize;
	}


EXPORT_C TUint32 TLbsLocClassTypeBase::ClassType() const
/**  Returns the type of the TPositionClassTypeBase-derived class
@return The type of the TPositionClassTypeBase-derived class
 */
	{
	return iClassType;
	}

//------------------------------------------------------------------------------
// TLbsExternalRequestInfo. 
//------------------------------------------------------------------------------
/**
Constructor for TLbsExternalRequestInfo

*/
EXPORT_C TLbsExternalRequestInfo::TLbsExternalRequestInfo()
	: TLbsLocClassTypeBase(),
	iRequestType(ERequestTypeUnknown),
	iRequestSource(ERequestSourceLocal),
	iNetworkType(ENetworkTypeUnknown),
	iRequesterFormat(EFormatUnknown),
	iClientFormat(EFormatUnknown),
	iRequesterCoding(ECodingSchemeUnknown),
	iClientCoding(ECodingSchemeUnknown)
	{
	iClassType |= EExternalRequestInfoClass;
	iClassSize = sizeof(TLbsExternalRequestInfo);	
	}

/**  Get the requester Id as a raw 8-bit buffer.

@param aRequesterId Reference to a descriptor which Requester Id is copied into.
*/
EXPORT_C void TLbsExternalRequestInfo::GetRequesterId(TDes8& aRequesterId) const
	{
	aRequesterId = iRequesterId;
	}
	
/**  Get the client name as a raw 8-bit buffer.

@param aClientName Reference to a descriptor which Client Name is copied into.
*/
EXPORT_C void TLbsExternalRequestInfo::GetClientName(TDes8& aClientName) const
	{
	aClientName = iClientName;
	}
	
/**  Get the client external Id as a raw 8-bit buffer.

@param aClientExternalId Reference to a descriptor which Client External Id is copied into.
*/
EXPORT_C void TLbsExternalRequestInfo::GetClientExternalId(TDes8& aClientExternalId) const
	{
	aClientExternalId = iClientExternalId; 
	}
	
/**  Get the requester Id as a unicode string.
Should be used only if the coding scheme is unicode.

@see RequesterIdCodingScheme

@param aRequesterId Reference to a descriptor which Requester Id is copied into.
*/
EXPORT_C void TLbsExternalRequestInfo::GetRequesterId(TDes16& aRequesterId) const
	{
	aRequesterId.Copy(iRequesterId);
	}
	
/**  Get the client name as a unicode string.
Should be used only if the coding scheme is unicode.

@see ClientNameCodingScheme

@param aClientName Reference to a descriptor which Client Name is copied into.
*/
EXPORT_C void TLbsExternalRequestInfo::GetClientName(TDes16& aClientName) const
	{
	aClientName.Copy(iClientName);
	}

/**  Set the requester Id as a raw 8-bit buffer.
Note that it does not automatically change the coding scheme that
must be set separately.

@see SetRequesterIdCodingScheme
@see RequesterIdCodingScheme

@param aRequesterId Reference to a descriptor that contains the requester Id.
*/	
EXPORT_C void TLbsExternalRequestInfo::SetRequesterId(const TDesC8& aRequesterId)
	{
	iRequesterId = aRequesterId;
	}
	
/**  Set the client name as a raw 8-bit buffer.
Note that it does not automatically change the coding scheme that
must be set separately.

@see SetClientNameCodingScheme
@see ClientNameCodingScheme

@param aClientName Reference to a descriptor that contains the client name.
*/
EXPORT_C void TLbsExternalRequestInfo::SetClientName(const TDesC8& aClientName)
	{
	iClientName = aClientName;
	}
	
/**  Set the client external Id as a raw 8-bit buffer.

@param aClientExternalId Reference to a descriptor that contains the client external Id.
*/
EXPORT_C void TLbsExternalRequestInfo::SetClientExternalId(const TDesC8& aClientExternalId)
	{
	iClientExternalId = aClientExternalId;
	}

/**  Set the requester Id as a unicode string.
Note that it does not automatically change the coding scheme that
must be set separately.

@see SetRequesterIdCodingScheme
@see RequesterIdCodingScheme

@param aRequesterId Reference to a descriptor that contains the requester Id.
*/	
EXPORT_C void TLbsExternalRequestInfo::SetRequesterId(const TDesC16& aRequesterId)
	{
	iRequesterId.Copy(aRequesterId);
	}
	
/**  Set the client name as a unicode string.
Note that it does not automatically change the coding scheme that
must be set separately.

@see SetClientNameCodingScheme
@see ClientNameCodingScheme

@param aClientName Reference to a descriptor that contains the client name.
*/
EXPORT_C void TLbsExternalRequestInfo::SetClientName(const TDesC16& aClientName)
	{
	iClientName.Copy(aClientName);
	}

/**  Set the network type.

@param aNetworkType The network type.

*/
EXPORT_C void TLbsExternalRequestInfo::SetNetworkType(const TNetworkType aNetworkType)
	{
	iNetworkType = aNetworkType;
	}

/**  Get the network type.

@return The network type.

*/
EXPORT_C TLbsExternalRequestInfo::TNetworkType TLbsExternalRequestInfo::NetworkType() const
	{
	return iNetworkType;
	}

/**  Set the request type.

@param aRequestType The request type.

*/
EXPORT_C void TLbsExternalRequestInfo::SetRequestType(const TRequestType aRequestType)
	{
	iRequestType = aRequestType;
	}

/**  Get the request type.

@return The request type.

*/
EXPORT_C TLbsExternalRequestInfo::TRequestType TLbsExternalRequestInfo::RequestType() const
	{
	return iRequestType;
	}

/**  Set the request source. 
(e.g. local or network)

@param aRequestSource The request source.


*/
EXPORT_C void TLbsExternalRequestInfo::SetRequestSource(const TRequestSource aRequestSource)
	{
	iRequestSource = aRequestSource;
	}

/**  Get the request source. 
(e.g. local or network)

@return The request type.

*/
EXPORT_C TLbsExternalRequestInfo::TRequestSource TLbsExternalRequestInfo::RequestSource() const
	{
	return iRequestSource;
	}

/**  Set the requester id format indicator.

@param aFormat The requester id format indicator.


*/
EXPORT_C void TLbsExternalRequestInfo::SetRequesterIdFormat(const TFormatIndicator aFormat)
	{
	iRequesterFormat = aFormat;
	}

/**  Get the requester id format indicator.

@return The requester id format indicator.


*/
EXPORT_C TLbsExternalRequestInfo::TFormatIndicator TLbsExternalRequestInfo::RequesterIdFormat() const
	{
	return iRequesterFormat;
	}

/**  Set the client name format indicator.

@param aFormat The client name format indicator.

*/
EXPORT_C void TLbsExternalRequestInfo::SetClientNameFormat(const TFormatIndicator aFormat)
	{
	iClientFormat = aFormat;
	}

/**  Get the client name format indicator.

@return The client name format indicator.

*/
EXPORT_C TLbsExternalRequestInfo::TFormatIndicator TLbsExternalRequestInfo::ClientNameFormat() const
	{
	return iClientFormat;
	}
	
/**  Set the requester id coding scheme.

@param aCoding The requester id coding scheme.

*/
EXPORT_C void TLbsExternalRequestInfo::SetRequesterIdCodingScheme(const TCodingScheme aCoding)
	{
	iRequesterCoding = aCoding;
	}

/**  Get the requester id coding scheme.

@return The requester id coding scheme.

*/
EXPORT_C TLbsExternalRequestInfo::TCodingScheme TLbsExternalRequestInfo::RequesterIdCodingScheme() const
	{
	return iRequesterCoding;
	}

/**  Set the client name coding scheme.

@param aCoding The client name coding scheme.

*/
EXPORT_C void TLbsExternalRequestInfo::SetClientNameCodingScheme(const TCodingScheme aCoding)
	{
	iClientCoding = aCoding;
	}

/**  Get the client name coding scheme.

@return The client name coding scheme.

*/
EXPORT_C TLbsExternalRequestInfo::TCodingScheme TLbsExternalRequestInfo::ClientNameCodingScheme() const
	{
	return iClientCoding;
	}

//------------------------------------------------------------------------------
// TLbsExternalRequestInfo2. 
//------------------------------------------------------------------------------
/**
Constructor for TLbsExternalRequestInfo2.
*/
EXPORT_C TLbsExternalRequestInfo2::TLbsExternalRequestInfo2()
	: TLbsExternalRequestInfo()
	{
	iClassType |= EExternalRequestInfoClass2;
	iClassSize = sizeof(TLbsExternalRequestInfo2);
	TLbsExternalRequestInfo::SetClientNameCodingScheme(ECodingSchemeUCS2);
	TLbsExternalRequestInfo::SetRequesterIdCodingScheme(ECodingSchemeUCS2);
	}

/**  Get the requester Id as a unicode string.

@param aRequesterId Reference to a descriptor which Requester Id is copied into.
*/
EXPORT_C void TLbsExternalRequestInfo2::GetRequesterId(TDes16& aRequesterId) const
	{
	TLbsExternalRequestInfo::GetRequesterId(aRequesterId);
	aRequesterId.Append(iRequesterIdExt);
	}
	
/**  Get the client name as a unicode string.

@param aClientName Reference to a descriptor which Client Name is copied into.
*/
EXPORT_C void TLbsExternalRequestInfo2::GetClientName(TDes16& aClientName) const
	{
	TLbsExternalRequestInfo::GetClientName(aClientName);
	aClientName.Append(iClientNameExt);
	}

/**  Set the requester Id as a unicode string.
Applications that cannot handle the extended TLbsExternalRequestInfo2
class can access the initial KLbsMaxRequesterIdSize/2 characters of the
string through the TLbsExternalRequestInfo class.

@param aRequesterId Reference to a descriptor that contains the requester Id.
*/	
EXPORT_C void TLbsExternalRequestInfo2::SetRequesterId(const TDesC16& aRequesterId)
	{
	const TInt KLbsRequesterIdLength16 = (KLbsMaxRequesterIdSize / 2);
	TLbsExternalRequestInfo::SetRequesterId(aRequesterId.Left(KLbsRequesterIdLength16));
	if (aRequesterId.Length() > KLbsRequesterIdLength16)
		{
		iRequesterIdExt = aRequesterId.Mid(KLbsRequesterIdLength16);
		}
	else
		{
		iRequesterIdExt = KNullDesC16;
		}
	}
	
/**  Set the client name as a unicode string.
Applications that cannot handle the extended TLbsExternalRequestInfo2
class can access the initial KLbsMaxClientNameSize/2 characters of the
string through the TLbsExternalRequestInfo class.

@param aClientName Reference to a descriptor that contains the client name.
*/
EXPORT_C void TLbsExternalRequestInfo2::SetClientName(const TDesC16& aClientName)
	{
	const TInt KLbsClientNameLength16 = (KLbsMaxClientNameSize / 2);
	TLbsExternalRequestInfo::SetClientName(aClientName.Left(KLbsClientNameLength16));
	if (aClientName.Length() > KLbsClientNameLength16)
		{
		iClientNameExt = aClientName.Mid(KLbsClientNameLength16);
		}
	else
		{
		iClientNameExt = KNullDesC16;
		}
	}
