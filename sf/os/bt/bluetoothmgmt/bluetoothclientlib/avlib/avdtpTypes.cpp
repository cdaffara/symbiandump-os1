// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetoothav.h>
#include "gavdpinterface.h"

enum TBTAVPanic
	{
	EBadArgument,
	EBTAVBadSEID,
	EBTAVUnusedOrdinal,
	};
	
void Panic(TBTAVPanic aPanic)
	{
	_LIT(KBTAVPanicName, "BluetoothAV Panic");
	User::Panic(KBTAVPanicName, aPanic);
	}

const TInt KAVDTPPortSessionOffset	=8;
const TInt KAVDTPPortSessionMask	=0x03;
const TInt KAVDTPPortSEIDOffset		=0;
const TInt KAVDTPPortSEIDMask		=0x3f;

// const TInt KMinMediaCodecLOSC = 2; // mediatype, codectype - see figure8-50 AVDTP
// const TInt KMinContentProtectionLOSC = 2; // protectiontype

static const TUint KAvdtpSockAddrLocalMask = static_cast<TUint>(1U<<31U);

/**
Default constructor using 0 as the parameter for setPort()
@publishedPartner
@released
*/
EXPORT_C TAvdtpSockAddr::TAvdtpSockAddr() : TBTSockAddr()
	{
	SetPort(0);
	}

/**
Copy constructor
@publishedPartner
@released
@param aAddr reference to the original socket address
*/
EXPORT_C TAvdtpSockAddr::TAvdtpSockAddr(const TSockAddr& aAddr)
	: TBTSockAddr(aAddr)
	{
	}

/**
Sets the AVDTP session type into the socket address
@publishedPartner
@released
@param aSession the session (eg EMedia)
*/
EXPORT_C void TAvdtpSockAddr::SetSession(TAvdtpTransportSessionType aSession)
	{
	TInt port = Port();
	// clear out current session part
	port &= ~(KAVDTPPortSessionMask << KAVDTPPortSessionOffset);
	// set new session part
	port |= (aSession & KAVDTPPortSessionMask) << KAVDTPPortSessionOffset;
	SetPort(port);
	}
	
/**
Getter for session from socket address
@publishedPartner
@released
@return the session type
*/
EXPORT_C TAvdtpTransportSessionType TAvdtpSockAddr::Session() const
	{
	return static_cast<TAvdtpTransportSessionType>
			((Port() >> KAVDTPPortSessionOffset) & KAVDTPPortSessionMask);
	}

/**
Sets the Stream endpoint identifier (SEID) into the socket address
@publishedPartner
@released
@param aSEID the SEID to set - can be remote or local
*/
EXPORT_C void TAvdtpSockAddr::SetSEID(TSEID aSEID)
	{
/*
	format of port is
	lowest octet: used for SEID (only l.s.6 bits meaningful at present)
	next octet  : used for session type (only l.s.2 bits at present)
	
*/
	TUint port = Port();
	// clear out current SEID part
	port &= ~(KAVDTPPortSEIDMask << KAVDTPPortSEIDOffset);

	// put local info at top bit
	port &= ~KAvdtpSockAddrLocalMask; 
	if (aSEID.IsLocal())
		{
		port |= KAvdtpSockAddrLocalMask; 
		}
		
	// set new SEID part
	port |= (aSEID.Value() & KAVDTPPortSEIDMask) << KAVDTPPortSEIDOffset;
	SetPort(port);
	}

/**
Getter for the stream endpoint identifier (SEID) from the socket address
@publishedPartner
@released
@return the SEID
*/	
EXPORT_C TSEID TAvdtpSockAddr::SEID() const
	{
	TUint port = Port();
	// see if LocalSeid
	TBool isLocal = port & KAvdtpSockAddrLocalMask;
	// clear out that bit
	port &=~KAvdtpSockAddrLocalMask;
	return TSEID((port & KAVDTPPortSEIDMask) >> KAVDTPPortSEIDOffset, isLocal);
	}

/**
Utility function to downcast a TSockAddr to a TAvdtpSockAddr
@publishedPartner
@released
@param aAddr reference to base class socket address
@return the downcast reference
*/
EXPORT_C TAvdtpSockAddr& TAvdtpSockAddr::Cast(const TSockAddr& aAddr)
	{
	return *const_cast<TAvdtpSockAddr*>(static_cast<const TAvdtpSockAddr*>(&aAddr));
	}
	
const TUint TSEID::KInvalidSEID = KMaxTUint8;


/**
Default constructor using KInvalidSEID as the stream endpoint identifier (SEID) value
@publishedPartner
@released
*/
EXPORT_C TSEID::TSEID() : iValue(KInvalidSEID)
	{
	}

/**
Constructor taking a stream endpoint identifier (SEID) value, and whether it refers to a local or remote endpoint
@publishedPartner
@released
@param aSEID, the SEID
@param aIsLocal ETrue if refers to a local endpoint, else EFalse
*/ 	
EXPORT_C TSEID::TSEID(TUint aSEID, TBool aIsLocal) : iValue(aSEID)
	{
	iValue|=aIsLocal ? KIsLocalBit : 0;
	}

/**
Constructor taking a direct stream endpoint identifier (SEID) value
@internalTechnology
@param aValue, the direct value
*/
EXPORT_C TSEID::TSEID(TUint aValue)
	{
	iValue = aValue;
	}

/**
Getter for the stream endpoint identifier (SEID) value, regardless of whether local or remote
@publishedPartner
@released
@return the SEID value
*/
EXPORT_C TUint TSEID::SEID() const
	{
	// clear local bit
	return (iValue&~KIsLocalBit);
	}

/**
Getter for INTERNAL stream endpoint identifier (SEID) value - for actual SEID value use SEID()
@internalTechnology
@see SEID
@return internal value
*/
EXPORT_C TUint TSEID::Value() const
	{
	return iValue;
	}

/**
Getter for SEID value in packet format
@internalTechnology
@return Packet value of SEID
*/
EXPORT_C TUint TSEID::PacketValue() const
	{
	__ASSERT_ALWAYS(IsValid(), Panic(EBTAVBadSEID));
	return (SEID() << KAvdtpPacketSEIDOffset); 
	}

/**
Checks whether the SEID is allowed by the AVDTP Specification
@publishedPartner
@released
@return ETrue if valid, else EFalse
*/
EXPORT_C TBool TSEID::IsValid() const
	{
	return (SEID()>=KMinimumRealSEID && SEID()<=KMaximumRealSEID);
	}

/**
Set SEID value directly
@internalTechnology
@param aValue value to set
*/ 	
EXPORT_C void TSEID::Set(TUint aValue)
	{
	// don't allow changing of local/remoteness
	iValue = aValue | (iValue & KIsLocalBit);
	__ASSERT_ALWAYS(IsValid(), Panic(EBTAVBadSEID));
	}

/**
Comparison operator
@publishedPartner
@released
@param right hand operand 
@return ETrue if aSEID equal to this SEID (the right hand operand), else EFalse
*/
EXPORT_C TBool TSEID::operator==(TSEID aSEID) const
	{
	return aSEID.Value()==Value();
	}
	
/**
Negative comparison operator
@publishedPartner
@released
@param right hand operand
@return EFalse if aSEID equal to this SEID (the right hand operand), else ETrue
*/
EXPORT_C TBool TSEID::operator!=(TSEID aSEID) const
	{
	return aSEID.Value()!=Value();
	}
	

/**
Not used
@internalTechnology
*/
/*static*/ EXPORT_C TSEID TSEID::_Spare(TUint8 /*aUnused*/)
	{
	Panic(EBTAVUnusedOrdinal);
	return TSEID(0);
	}

/**
Helper to create a SEID from a packet value. Leaves if invalid
@internalTechnology
@param aPacketValue the value in the packet format
@leave EAvdtpBadACPSEID (converts to -18062) if created SEID is invalid
@return new SEID object
*/
EXPORT_C /*static*/ TSEID TSEID::FromPacketL(TUint8 aPacketValue, TBool aIsLocal)
	{
	TSEID seid((aPacketValue>>KAvdtpPacketSEIDOffset) & KAvdtpPacketSEIDMask, aIsLocal);
	if (!seid.IsValid())
		{
		User::Leave(SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadACPSEID));
		}
	return seid;
	}
	
/**
Reset any internal state and values
@publishedPartner
@released
*/
EXPORT_C void TSEID::Reset()
	{
	iValue = KInvalidSEID;
	}
	
/**
Tests whether the SEID refers to a local endpoint
@publishedPartner
@released
@return ETrue if SEID refers to local endpoint, else EFalse
*/
EXPORT_C TBool TSEID::IsLocal() const
	{
	return (iValue & KIsLocalBit ? ETrue : EFalse);
	}

/**
Constructor of object returned during Discover phase
@publishedPartner
@released
@see AVDTP specification, AVDTP_DiscoverRsp
*/	
EXPORT_C TAvdtpSEPInfo::TAvdtpSEPInfo()
:iInUse(EFalse)
	{
	// SEID default constructed - others don't care
	}

/**
Getter for the SEID
@publishedPartner
@released
@return the SEID
*/
EXPORT_C TSEID TAvdtpSEPInfo::SEID() const
	{
	return iSEID;
	}
	
/**
Getter for whether the Stream End Point (SEP) is in use
@publishedPartner
@released
@return ETrue if the SEP is in use, else EFalse
*/
EXPORT_C TBool TAvdtpSEPInfo::InUse() const
	{
	return iInUse;
	}
	
/**
Getter for the SEP mediatype
@publishedPartner
@released
@return Media type of SEP
*/
EXPORT_C SymbianBluetoothAV::TBluetoothMediaType TAvdtpSEPInfo::MediaType() const
	{
	return iMediaType;
	}
	
/**
Getter for whether the SEP is sink or source
@publishedPartner
@released
@return ETrue if SEP is a sink, else EFalse;
*/
EXPORT_C TBool TAvdtpSEPInfo::IsSink() const
	{
	return iIsSink;
	}
	
/**
Setter for the SEID of the Stream End Point (SEP)
@internalTechnology
@param aSEID the SEID to set
*/
EXPORT_C void TAvdtpSEPInfo::SetSEID(TSEID aSEID)
	{
	iSEID = aSEID;
	}
	
/**
Setter for the SEP's inUse flag
@internalTechnology
@param aIsInUse ETrue if in use
*/
EXPORT_C void TAvdtpSEPInfo::SetInUse(TBool aIsInUse)
	{
	iInUse = aIsInUse;
	}
	
/**
Setter for the SEP mediatype
@publishedPartner
@released
@param aMediaType Mediatype of SEP
*/
EXPORT_C void TAvdtpSEPInfo::SetMediaType(SymbianBluetoothAV::TBluetoothMediaType aMediaType)
	{
	iMediaType = aMediaType;
	}
	
/**
Setter for whether the SEP is source or sink
@publishedPartner
@released
@param aIsSink ETrue if SEP is a sink, else ETrue;
*/
EXPORT_C void TAvdtpSEPInfo::SetIsSink(TBool aIsSink)
	{
	iIsSink = aIsSink;
	}	

/**
Constructor
@internalTechnology
*/
EXPORT_C CCapabilitySummaryVisitor::CCapabilitySummaryVisitor()
	{
	}
	
/**
Setter for the seen capabilities
@internalTechnology
@param aCat the Service Category
@return ETrue
*/
TBool CCapabilitySummaryVisitor::Capability(TAvdtpServiceCategory aCat)
	{
	// can ignore payload
	
	// just mask in category to "seen" record!
	iSeen.SetCapability(aCat);
	return ETrue; // always continue
	}
	
/**
Getter for the seen capabilities
@internalTechnology
@return the seen capabilities 
*/
EXPORT_C TAvdtpServiceCategories CCapabilitySummaryVisitor::CapabilitiesPresent() const
	{
	return iSeen;
	}

	
/**
Constructor
@internalTechnology
@param aRequiredCapability the required capability
*/
EXPORT_C CCapabilityPresentVisitor::CCapabilityPresentVisitor(TAvdtpServiceCategory aRequiredCapability)
: iRequiredCapability(aRequiredCapability)
	{
	}

/**
Checks if a capability is the required one
@internalTechnology
@param aCat the capability
@return ETrue if the capability is required else EFalse 
*/

TBool CCapabilityPresentVisitor::Capability(TAvdtpServiceCategory aCat)
	{
	iPresent = (aCat==iRequiredCapability);
	return !iPresent;
	}

/**
Constructor
@internalTechnology
@param aRequiredCategories the required capabilities
*/
EXPORT_C CCapabilityParseVisitor::CCapabilityParseVisitor(TAvdtpServiceCategories aRequiredCategories)
: iRequiredCategories(aRequiredCategories)
	{
	}
	
/**
Sets up a capability
@internalTechnology
@param aCat the capability category
@param aCapPayload the capability payload
@return ETrue if the visitor could continue with setting another cabability else EFalse 
*/
TBool CCapabilityParseVisitor::Capability(TAvdtpServiceCategory aCat)
	{
	TInt result = KErrNone;
	
	iCapabilities[aCat] = NULL;
	if (aCat & iRequiredCategories())
		{
		TRAP(result, iCapabilities[aCat] = TAvdtpServiceCapability::AllocFromPDUL(aCat, CapabilityPayload()));
		}
	
	// return true as we try to parse all of the required caps, unless failed, then try to continue
	// - the visitor loop can decide when it's got to the end of the data
	TBool keepGoing = EFalse;
	if ((result == KErrNone) || (result == KErrNotSupported)) 
		{
		keepGoing = ETrue;
		}
	
	return keepGoing;
	}
	
/**
Transfer ownership of capabilities array
@internalTechnology
@return the capabilities array
*/
EXPORT_C TCapabilitiesArray CCapabilityParseVisitor::GetCapabilities()
	{
	TCapabilitiesArray array = iCapabilities;
	iCapabilities.Reset(); // since we're transferring ownership
	return array;  
	}

/**
Destructor
@publishedPartner
@released
*/
CCapabilityParseVisitor::~CCapabilityParseVisitor()
	{
	// we may not have transferred ownership of capabilities, so release all if non-NULL
	iCapabilities.DeleteAll();
	}

EXPORT_C CCapabilityVisitor::CCapabilityVisitor(): iCapabilityPtr(NULL, 0)
	{
	}
/**
Checks for valid capabilities within a signalling message
@internalTechnology
@param aCapabilityPDUDomain the message
*/
EXPORT_C void CCapabilityVisitor::Process(TDes8& aCapabilityPDUDomain)
	{
	// walk through structure and allow visitor to operate on each cap
	TInt index =0;
	TInt capsLen = aCapabilityPDUDomain.Length()-KAvdtpServiceCapabilitiesHeaderLen;
	while(index <= capsLen)
		{
		TPtrC8 hdr;
		hdr.Set(aCapabilityPDUDomain.Mid(index, KAvdtpServiceCapabilitiesHeaderLen));
		
		TAvdtpServiceCategory cat = static_cast<TAvdtpServiceCategory>(hdr[0]);
		TInt losc = static_cast<TInt>(hdr[1]); // (losc is Length of Service Capability)
		
		// check for badly formed capabilities
		if (losc<=aCapabilityPDUDomain.Length()-index-KAvdtpServiceCapabilitiesHeaderLen)
			{
			iCapabilityPtr.Set(aCapabilityPDUDomain.MidTPtr(index, KAvdtpServiceCapabilitiesHeaderLen + losc));
			iCapabilityPayload.Set(aCapabilityPDUDomain.Mid(index+KAvdtpServiceCapabilitiesHeaderLen, losc));

			TBool repeat = Capability(cat);
			
			if (!repeat)
				{
				break;
				}
			else
				{
				/*
				We need to delete the end of iCapabilityPtr that the derived class has chomped
				
				We are currently processing capability n
				 _______________________________________________________
				|  Cap 0	|  Cap n	| Cap n chomped	|   Cap n+1		|
				|  - n-1	|			| data			|   - end		|
				|___________|___________|_______________|_______________|
					index > ^			^ < index + ptr.Length 
							 <--------->
							  ptr.Length
							 <------------------------->
								ptr.MaxLength
								
				We need to delete the chomped data, which has a start position of
				index + ptr.Length, and a length of ptr.MaxLength - ptr.Length
				*/
				
				TUint chompedLength = iCapabilityPtr.MaxLength() - iCapabilityPtr.Length();
				
				aCapabilityPDUDomain.Delete(index + iCapabilityPtr.Length(), chompedLength);
				index+=iCapabilityPtr.Length();
				capsLen -= chompedLength;
				}
			}
		else
			{
			// abandon remainder
			break;
			}
		}		
	}

EXPORT_C const TDesC8& CCapabilityVisitor::CapabilityPayload()
	{
	return iCapabilityPayload;
	}

EXPORT_C TDes8& CCapabilityVisitor::CapabilityDes()
	{
	return iCapabilityPtr;
	}

/**
Converts an enumeration value corresponding to an AVDTP Signalling error
into a system-wide error code.

The AVDTP Signalling error range: -18172 to -18045 (inclusive) 
@publishedPartner
@released
@param aAvdtpError The locally defined error value.
@return System-wide error code within the AVDTP Signalling error space; -18172 to -18045 (inclusive).
*/	
EXPORT_C /*static*/ TInt SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(SymbianBluetoothAV::TAvdtpSignallingErrorCode aAvdtpError)
	{
	return DoConvertError(aAvdtpError);
	}
	
/**
Converts an enumeration value corresponding to a GAVDP Signalling error
into a system-wide error code.

The GAVDP Signalling error range: -18236 to -18173 (inclusive) 
@publishedPartner
@released
@param aGavdpError The locally defined error value.
@return System-wide error code within the GAVDP Signalling error space; -18236 to -18173 (inclusive).
*/	
EXPORT_C /*static*/ TInt SymbianBluetoothAV::ConvertToSymbianError::GavdpError(TGavdpSignallingErrorCode aGavdpError)
	{
	return DoConvertError(aGavdpError);
	}
	
/**
Converts an enumeration value corresponding to an A2DP Signalling error
into a system-wide error code.  The A2DP Signalling error codes are mapped
into the concrete profile error code range which is shared with other profiles that sit on top of the GAVDP, such as VDP.  

Mappings into this range of values are done locally by each profile and will typically cover 
the same subsets of the error code space.  
These error codes are therefore	not globally unique and any value within this range 
requires the context of the profile it originated from in order for it to be fully defined.

The concrete profile signalling error range: -18300 to -18237 (inclusive) 
@publishedPartner
@released
@param aA2dpError The locally defined error value.
@return System-wide error code within the concrete profile error space;  -18300 to -18237 (inclusive).
*/	
EXPORT_C /*static*/ TInt SymbianBluetoothAV::ConvertToSymbianError::A2dpError(TA2dpSignallingErrorCode aA2dpError)
	{
	return DoConvertError(aA2dpError);
	}

/**
Converts an enumeration value corresponding to a VDP Signalling error
into a system-wide error code.  The VDP Signalling error codes are mapped
into the concrete profile error code range which is shared with other profiles that sit on top of the GAVDP, such as A2DP.  

Mappings into this range of values are done locally by each profile and will typically cover 
the same subsets of the error code space.  
These error codes are therefore	not globally unique and any value within this range 
requires the context of the profile it originated from in order for it to be fully defined.

The concrete profile signalling error range: -18300 to -18237 (inclusive) 
@publishedPartner
@released
@param aVdpError The locally defined error value.
@return System-wide error code within the concrete profile error space; -18300 to -18237 (inclusive) .
*/	
EXPORT_C /*static*/ TInt SymbianBluetoothAV::ConvertToSymbianError::VdpError(TVdpSignallingErrorCode aVdpError)
	{
	return DoConvertError(aVdpError);
	}

/**
Takes the local signalling error code (which is a relative offset from the Bluetooth Audio/Visual base signalling 
error code) of any A/V profile type and calculates the corresponding system-wide error code.  This function is called by 
a profile specific error conversion function.  For full documentation of what the error code corresponds to 
refer to the relevant profile's conversion function.  

A Bluetooth A/V Signalling error code will exist within the range -18045 to -18300 (inclusive).
@publishedPartner
@released
@param aError The value of the error enumeration object.
@return A standard Symbian system-wide error code; -18045 to -18300 (inclusive).

*/
/*static*/ TInt SymbianBluetoothAV::ConvertToSymbianError::DoConvertError(TBluetoothAvDistributionError aAVError)
	{
	// check not already in correct range
	return KErrAvdtpSignallingErrorBase-static_cast<TInt>(aAVError);
	}

