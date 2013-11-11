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
// Implements the avdtp signalling messages
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avdtpSignallingMessages.h"
#include "avdtpSignallingChannel.h"
#include "avdtputil.h"
#include "avdtp.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVDTP);
#endif

using namespace SymbianBluetoothAV;

CAvdtpSignallingMessage::CAvdtpSignallingMessage()
	{
	LOG_FUNC
	Data().Init();
	}
	
CAvdtpInboundSignallingMessage::CAvdtpInboundSignallingMessage(CSignallingChannel& aSignallingChannel)
: iSignallingChannel(aSignallingChannel)
	{
	LOG_FUNC
	}

/*static*/ void CAvdtpOutboundSignallingMessage::PrependSignallingHeaderL(
									RMBufChain& aDest,
									const TAvdtpPacketType& aPacketType,
									const TAvdtpTransactionLabel& aTransLabel,
									const TAvdtpMessageType& aMessageType,
									TAvdtpMessage aSigId,
									TInt aNumberSignalPackets)
	{
	LOG_STATIC_FUNC
	RMBuf& header = *RMBuf::AllocL();
	aDest.Prepend(&header);
	
	TUint8	firstByte = 
		((aTransLabel<<KAvdtpTransactionLabelOffset) & KAvdtpTransactionLabelMask) |
		((aPacketType<<KAvdtpPacketTypeOffset)		 & KAvdtpPacketTypeMask)       |
		((aMessageType<<KAvdtpMessageTypeOffset)	 & KAvdtpMessageTypeMask);
	
	switch (aPacketType)
		{
		case ESingle:
			{
			header.SetLength(KAvdtpMinimumSignallingSinglePacketLength);
			TUint8 lastByte = ((aSigId) & KAvdtpSignalIdentifierMask);
			header.Put(firstByte,0);
			header.Put(lastByte,1);
			break;
			}
		case EStart:
			{
			header.SetLength(KAvdtpMinimumSignallingStartPacketLength);
			TUint8 lastByte = ((aSigId) & KAvdtpSignalIdentifierMask);
			header.Put(firstByte,0);
			header.Put(static_cast<TUint8>(aNumberSignalPackets),1);
			header.Put(lastByte,2);
			break;
			}
		case EEnd:
		case EContinue:
			header.SetLength(KAvdtpMinimumSignallingContinuePacketLength);
			header.Put(firstByte,0);
			break;			
		}	
	
	}


/*
Return True if more fragments in packet
*/
TBool CAvdtpOutboundSignallingMessage::GetNextOutboundFragmentL(RMBufChain& aDest, TInt aMaxSize)
	{
	LOG_FUNC
	TAvdtpPacketType pktType;
	
	if (!iFragInfo.iFragmented)
		{
		// no fragmentation already done - might be single or need fragmenting
		__ASSERT_DEBUG(Data().Length()<=(0xff*aMaxSize), Panic(EAvdtpBadlyFormattedOutboundSignallingMessage));
		
		if ((Data().Length()+
			KAvdtpMinimumSignallingSinglePacketLength) < aMaxSize)
			{
			//no fragmentation required
			PrependSignallingHeaderL(Data(),ESingle,TransactionLabel(),iMessageType,Signal());
			aDest.Assign(Data()); //transfer ownership of Data to aDest
			Deque();	// remove this from the outbound queue
			return EFalse; //no more frags - returned whole packet
			}
		else
			{
			// Start fragment(ing)
			//we can get MTU-3 bytes in the first pkt, MTU-1 in the rest
			//just pretend it's 2 bytes longer than it is and divide by
			//MTU - 1 
			iFragInfo.iTotalPackets = ((Data().Length()+2)/(aMaxSize-1));
			//and add one if it wasn't an exact fit
			if ((Data().Length()+2)%(aMaxSize-1))
				{
				iFragInfo.iTotalPackets++;
				}
			iFragInfo.iPacketNumber=0;
			iFragInfo.iFragmented=ETrue;
			
			pktType = EStart;
			}
		}
	else
		{
		// continue and end parts
		pktType = (iFragInfo.iPacketNumber<(iFragInfo.iTotalPackets-1)) ? EContinue : EEnd;
		}
		
	PrependSignallingHeaderL(Data(), pktType, TransactionLabel(), iMessageType,
							 Signal(), iFragInfo.iTotalPackets);

	// copy into "fragment" (whole packet if Single!) amount of message that will fit
	// the message here doesn't have the header, but we do know we need to fragment
	// (MTU-3) for Start, (MTU-1) for Continue,End
	TInt size = 0;
	
	switch (pktType)
		{
		case EStart:
		size = aMaxSize - KAvdtpMinimumSignallingStartPacketLength;
		break; 
		case EContinue:
		size = aMaxSize - KAvdtpMinimumSignallingContinuePacketLength;
		break;
		case EEnd:
		size = aMaxSize - KAvdtpMinimumSignallingEndPacketLength;
		break;
		case ESingle: // drop through
		default:
		__ASSERT_DEBUG(0, Panic(EAvdtpBadlyFormattedOutboundSignallingMessage));
		}

	Data().CopyL(aDest,0,size);
	// and trim off the copied data from Data()
	// esock workaround - panic if trim > size of chain
	Data().TrimStart(Min(size, Data().Length()));
	
	if (iFragInfo.iPacketNumber++==iFragInfo.iTotalPackets)
		{
		// all fragments done
		Deque();
		return EFalse; // no more fragments from this packet
		}
		
	return ETrue;
	}

/**
Check that packet header is ok for the packet build state we're in
@param aFragment the next incoming fragment to validate
@return KErrNone if ok, KErrCorrupt if not. KErrAbort if fragment is an Abort packet
*/
TInt CAvdtpInboundSignallingMessage::CheckPacketType(RMBufChain& aFragment, TAvdtpPacketType& aPacketType, TAvdtpMessageType& aMessageType, TAvdtpTransactionLabel& aTransactionLabel)
	{
	LOG_FUNC
	 //NB could be an Abort packet interleaved in building up the previous packet
	TInt ret = KErrCorrupt;

	if (aFragment.First())
		{
		TAvdtpPacketType packetType = static_cast<TAvdtpPacketType>((aFragment.First()->Ptr()[0] & KAvdtpPacketTypeMask) >> KAvdtpPacketTypeOffset);
		TAvdtpMessageType messageType = static_cast<TAvdtpMessageType>((aFragment.First()->Ptr()[0] & KAvdtpMessageTypeMask) >> KAvdtpMessageTypeOffset);
		TAvdtpTransactionLabel transactionLabel = static_cast<TAvdtpTransactionLabel>((aFragment.First()->Ptr()[0] & KAvdtpTransactionLabelMask) >> KAvdtpTransactionLabelOffset);
		
		aMessageType = messageType;
		aTransactionLabel = transactionLabel;
		aPacketType = packetType;

		/* check if an abort has come in
			The abort will be a single packet type
			The message type will be a command
			The indication (second byte) will EAvdtpAbort
		  if an abort has come in then return KErrAbort and
		  the abort will be correctly processed. The packetbuildstate will 
		  be reset to EAwaitingNew
		  */
		if ((ESingle == packetType) && (ECommand == messageType))
			{
			/* note that when checking for the signal identifier HERE
			   we haven't yet removed the first byte from the message
			 */
			if (aFragment.First()->Size() > 1)
				{
				TAvdtpMessage id = static_cast<TAvdtpMessage>(aFragment.First()->Ptr()[1] & KAvdtpSignalIdentifierMask);
				if (EAvdtpAbort == id)
					{
					return (KErrAbort);
					}
				}
			}
		
		switch (iPacketBuildState)
			{
			case EAwaitingNew:
				// if waiting for a new packet then packet type must be single or start
				if ((packetType==ESingle)||(packetType==EStart))
					{
					ret=KErrNone;
					}
				break;
			case EAwaitingContinue:
				// if waiting for continue then type must be continue, and the transaction label
				// of this fragment must match the packet we're building. Ditto for message type
				if ((packetType==EContinue) && 
				    (transactionLabel==iPacketBuildLabel) && 
				    (messageType==iMessageType))
					{
					ret=KErrNone;
					}
				break;
			case EAwaitingEnd:
				// if waiting for end then type must be end, and the transaction label
				// of this fragment must match the packet we're building. Ditto for message type			
				if ((packetType==EEnd) && 
				    (transactionLabel==iPacketBuildLabel) && 
				    (messageType==iMessageType))
				    {
				    ret=KErrNone;
				    }
				break;
			}
		}
	return ret;
	}

void CAvdtpSignallingMessage::SetType(TAvdtpMessageType aType,TAvdtpMessage aMessage)
	{
	LOG_FUNC
	iSignal = aMessage;
	iMessageType = aType;
	}
	

CAvdtpOutboundSignallingMessage::~CAvdtpOutboundSignallingMessage()
	{
	LOG_FUNC
	if (iMessageType==ECommand)
		{
		//OutBound Signalling message should never be EReserved
		__ASSERT_DEBUG(Signal()!= EReserved,Panic(EAvdtpInvalidReservedValueInOutboundSignallingMessage));
		iCommandLabel.Close();
		}
	Deque();
	}

void CAvdtpOutboundSignallingMessage::AddSEPInfoL(const TAvdtpSEPInfo& aSEPInfo)
	{
	LOG_FUNC
	// append SEP info to packet
	TBuf8<KAvdtpPacketSEPInfoSize> SEPInfoBuf;
	SEPInfoBuf.SetMax();

	SEPInfoBuf[0] = aSEPInfo.SEID().PacketValue();
	if (aSEPInfo.InUse())
		{
		SEPInfoBuf[0]|=(1<<KAvdtpInUseFlagOffset);
		}
	SEPInfoBuf[1] = aSEPInfo.MediaType()<<KAvdtpMediaTypeOffset;
	if (aSEPInfo.IsSink())
		{
		SEPInfoBuf[1]|=(1<<KAvdtpTSEPOffset);
		}
	AppendDataL(SEPInfoBuf);
	}
	
	
void CAvdtpOutboundSignallingMessage::AddSEPCapabilityL(const TAvdtpServiceCapability& aCapability)
	{
	LOG_FUNC
	// append SEP capability to packet, this temp buffer *cannot* be placed on cleanupstack (it may get reallocated)
	RBuf8 buf;
	buf.CreateL(KAvdtpServiceCapabilitiesHeaderLen);
	buf.CleanupClosePushL();
	User::LeaveIfError(aCapability.AsProtocol(buf));	
	AppendDataL(buf);
	buf.Close();
	CleanupStack::Pop();
	}

void CAvdtpOutboundSignallingMessage::Deque()
	{
	LOG_FUNC
	iLink.Deque();
	}
		
TInt CAvdtpOutboundSignallingMessage::AllocateTransactionLabel(TTransactionLabelManager& aManager)
	{
	LOG_FUNC
	return aManager.GetLabel(iCommandLabel);
	}

void CAvdtpOutboundSignallingMessage::SetTransactionLabel(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	iResponseLabel = aLabel;
	}

TInt CAvdtpOutboundSignallingMessage::NewData(TUint /*aCount*/)	
	{
	Panic(EAvdtpOutboundMessageNewDataCalled);
	return KErrNotSupported;
	}
	
/**
Upcall from signalling channel telling us of new inbound data
The data is synchronously retrieved, checked and added to the building packet
Once the packet is complete the packet begins parsing
*/	
/*virtual*/ TInt CAvdtpInboundSignallingMessage::NewData(TUint aCount)
	{
	LOG_FUNC
	RMBufChain fragment;
	TInt result = KErrNone;
	
	while (aCount--)
		{
		result = iSignallingChannel.GetData(fragment,NULL);
	
		if (result<KErrNone)
			{
			break;
			}
		
		TAvdtpPacketType packetType;
		TAvdtpMessageType messageType;
		TAvdtpTransactionLabel transactionLabel;
		// check the AVDTP Packet type part of the header	
		result = CheckPacketType(fragment, packetType, messageType, transactionLabel);

   		if (result == KErrAbort)
   			{
 			// if the packet build state is other than awaiting new then
 			// we have some existing data to throw out (the abort should discard)
 			if (iPacketBuildState != EAwaitingNew)
 				{
 				Data().Free(); 		//drop
 				}
 			// reset the packet build state so that we process the abort.
 			iPacketBuildState = EAwaitingNew;
   			// clear current message??
   			// at moment do nothing - mayneed to parse to see if it's the same SEID - yuk protocol
   			result = KErrNone; // will actually process the Abort message
   			}
		
		// if the packet type is ok then it can be added into the building up packet
		if (result == KErrNone)
			{
			iFragmentationInfo.iPacketNumber++;
			// see whether to start new packet
			switch (iPacketBuildState)
				{
				case EAwaitingNew:
					// can see what this message is going to be
					iMessageType = messageType;
					TRAP(result,Data() = MessageL(fragment,packetType));
					
					if (result==KErrNone)
						{
						iPacketBuildLabel = transactionLabel;
						if (packetType==ESingle)
							{
							// and can now process - no more to wait for!
							Process(iPacketBuildLabel);
							// reset the packet number counter
							iFragmentationInfo.iPacketNumber =0;
							}
						else
							{
							// a start, data remembered above, and hold off processing
							iFragmentationInfo.iFragmented = ETrue;
							iFragmentationInfo.iTotalPackets = NumberSignalPackets(fragment);
							iPacketBuildState = EAwaitingContinue;
							}
						}
					break;
					
				case EAwaitingContinue:
					{
					// check that the number of packets < NOSP
					result = iFragmentationInfo.iFragmented &&
								(iFragmentationInfo.iPacketNumber < iFragmentationInfo.iTotalPackets)
								? KErrNone : KErrNotReady;

					if (result==KErrNone)
						{
						RMBufChain message;
						TRAP(result,message = MessageL(fragment,packetType));
						
						if (result==KErrNone)
							{
							// add into forming packet
							Data().Append(message);

							// see if we expect End next
							if (iFragmentationInfo.iPacketNumber == iFragmentationInfo.iTotalPackets-1)
								{
								iPacketBuildState = EAwaitingEnd;
								}
							}
						
						}
					}
					break;
				case EAwaitingEnd:
					{
					// check that the number of packets == NOSP
					result = iFragmentationInfo.iFragmented &&
								(iFragmentationInfo.iPacketNumber == iFragmentationInfo.iTotalPackets)
								? KErrNone : KErrNotReady;
					
					if (result==KErrNone)
						{
						RMBufChain message;
						TRAP(result,message = MessageL(fragment,packetType));
						if (result==KErrNone)
							{
							// add into forming packet
							Data().Append(message);
							// and process cos no more to come
							Process(iPacketBuildLabel);
							iPacketBuildState = EAwaitingNew;
							}
						}
					}
					break;
				}
			}
		else
			{
			//drop - free'd below
			}
		
		// all done with the inbound fragment
		fragment.Free();	
		}
		
	return result;
	}


/**
Begin processing the complete packet
*/
void CAvdtpInboundSignallingMessage::Process(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	iSignal = SignalIdentifier(Data());
	
	// member function pointer
	// the handlers have no policy - they just parse and generate indications to SAP
   	void (*handler)(CAvdtpInboundSignallingMessage&, TAvdtpTransactionLabel);
	
	switch (Signal())
		{
		case EAvdtpDiscover:
			handler = HandleDiscoverL;
			break;
		case EAvdtpGetCapabilities:
			handler = HandleGetCapsL;
			break;
		case EAvdtpSetConfiguration:
			handler = HandleSetConfigL;
			break;
		case EAvdtpGetConfiguration:
			handler = HandleGetConfigL;
			break;
		case EAvdtpReconfigure:
			handler = HandleReconfigL;
			break;
		case EAvdtpOpen:
			handler = HandleOpenL;
			break;
		case EAvdtpStart:
			handler = HandleStartL;
			break;
		case EAvdtpRelease:
			handler = HandleReleaseL;
			break;
		case EAvdtpSuspend:
			handler = HandleSuspendL;
			break;
		case EAvdtpAbort:
			handler = HandleAbortL;
			break;
		case EAvdtpSecurityControl:
			handler = HandleSecurityControlL;
			break;
		case EReserved:
			Data().Free(); 		//drop
			iPacketBuildState = EAwaitingNew;
			return;
		default:
			{
			if(iMessageType==ECommand)
				{
				iSignallingChannel.SendReject(aLabel, Signal(), EAvdtpNotSupportedCommand/*ignored*/);
				}
			
			Data().Free(); 		//drop
			iPacketBuildState = EAwaitingNew;
			return;
			}
		}
	// trim off now-processed header stuff (that SigId)
	Data().TrimStart(1);

	// now invoke handler (via member function pointer)
	
	TRAPD(parseError, (*handler)(*this, aLabel));

	if (parseError != KErrNone && iMessageType==ECommand)
		{
		// kick the signalling channel to send appropriate error, but only for commands (responses are discarded)
		// further the handlers need to take care - if they are supposed to send extended error info
		// they cannot use this service
		__ASSERT_DEBUG(Signal()!=EAvdtpSetConfiguration && Signal()!=EAvdtpReconfigure, Panic(EAvdtpSignallingMessageHandlerLeft));
		
		switch(Signal())
			{
			case EAvdtpSetConfiguration:
				{
				iSignallingChannel.SendSetConfigurationReject(aLabel, AvdtpInternalUtils::SymbianErrorToAvdtpError(parseError), EServiceCategoryNull);
				break;
				}
			case EAvdtpReconfigure:
				{
				iSignallingChannel.SendReconfigureReject(aLabel, AvdtpInternalUtils::SymbianErrorToAvdtpError(parseError), EServiceCategoryNull);
				break;
				}
			default:
				{
				iSignallingChannel.SendReject(aLabel, Signal(), AvdtpInternalUtils::SymbianErrorToAvdtpError(parseError));
				}
			}
		}
	
	Data().Free(); //done parsing
	iPacketBuildState = EAwaitingNew;
	}

void CAvdtpInboundSignallingMessage::DoHandleDiscoverL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpDiscoverCommandMinimumLength, KAvdtpDiscoverCommandMaximumLength);
			iSignallingChannel.DiscoverIndication(aLabel);
			break;
			}
		case EResponseAccept:
			{
			TAvdtpInternalDiscoverConfirm cfm;

			TInt err = CheckPacketLength(KAvdtpDiscoverAcceptMinimumLength, KAvdtpDiscoverAcceptMaximumLength);
			if (Data().Length()%2!=0)
				{
				err = KErrCorrupt;
				}
			TInt numSEPs;
			if (err==KErrNone)
				{				
				const TInt KAvdtpDiscoverSEPLenOctets = 2;
				numSEPs = (Data().Length())/KAvdtpDiscoverSEPLenOctets;
				}
			else
				{
				numSEPs = 0;
				err = KErrNotFound;
				}
					
			if (numSEPs)
				{	
				err = AvdtpSignallingMessageDiscover::Response::ParseL(Data(),
																cfm.iDiscoveredSEPs,
																iSignallingChannel.iProtocol.RemoteSEPCache(),
																iSignallingChannel.RemoteAddress());
				}

			cfm.iResult = err;
			cfm.iNumSEPs = numSEPs;			
			iSignallingChannel.DiscoverConfirm(aLabel, err, &cfm);
			break;
			}
		case EResponseReject:
			{
			TInt err = CheckPacketLength(KAvdtpDiscoverRejectMinimumLength, KAvdtpDiscoverRejectMaximumLength);
			err = (err==KErrNone) ? SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(Data().First()->Ptr()[0]))
								: KErrCorrupt;
			// pass up NULL SEP data
			// NOTE:	It is safe to set the third parameter below to NULL
			// 			the ASSERT_DEBUG that might have been hit as a result won't be
			// 			because 'err' cannot equal 'KErrNone'.
			//			SymbianBluetoothAV::ConvertToSymbianError::AvdtpError
			//			calls
			// 			TInt SymbianBluetoothAV::ConvertToSymbianError::DoConvertError(TBluetoothAvDistributionError aAVError)
			// 			which subtracts a TUint8 from -18045, and thus has to return a value 
			//			between -18045 and -18045 - 255 = -18300
			iSignallingChannel.DiscoverConfirm(aLabel, err, NULL);
			break;
			}
		case EGeneralReject:
			{
			TInt err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.DiscoverConfirm(aLabel,err, NULL);
			break;
			}
		}
	}
	
void CAvdtpInboundSignallingMessage::DoHandleGetCapsL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	TInt err = KErrNone;
	
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpGetCapsCommandMinimumLength, KAvdtpGetCapsCommandMaximumLength);
			iSignallingChannel.GetCapsIndication(aLabel,LocalSEIDL());
			break;
			}
		case EResponseAccept:
			{
			err = CheckPacketLength(KAvdtpGetCapsAcceptMinimumLength, KAvdtpGetCapsAcceptMaximumLength);
			if (err==KErrNone)
				{
				// fish out the capabilities from mbufchain
				// as this is immutable HBufC8 is chosen over RBuf8
				HBufC8* capsBuf = NULL;
				capsBuf = HBufC8::NewMaxLC(Data().Length());
				TPtr8 des = capsBuf->Des();
				Data().CopyOut(des);

				// Remove unknown caps
				CCapabilityRemoveUnknownVisitor* remover = new (ELeave) CCapabilityRemoveUnknownVisitor;
				remover->Process(des);
				delete remover;
				
				// parse caps for broken ones
				CCapabilityValidateVisitor* validater = new (ELeave) CCapabilityValidateVisitor;
				validater->Process(des);

				if (validater->Result()!=KErrNone)
					{
					CleanupStack::PopAndDestroy(capsBuf);
					// drop (can't response to response)
					// pass up zero as capabilities "seen" with the result code (in symbian range)
					iSignallingChannel.GetCapsConfirm(aLabel, validater->Result(), 0);
					}
				else
					{
					// packet now only left with config payload
					// pass ownership of buf 
					CleanupStack::Pop(capsBuf);
					iSignallingChannel.GetCapsConfirm(aLabel, err, capsBuf);
					}

				delete validater;
				}
			else
 				{
 				iSignallingChannel.GetCapsConfirm(aLabel, err);
 				}				
			break;
			}
		case EResponseReject:
			{
			err = CheckPacketLength(KAvdtpGetCapsRejectMinimumLength, KAvdtpGetCapsRejectMaximumLength);
			err = (err==KErrNone) ? SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(Data().First()->Ptr()[0])) : 
									KErrCorrupt;
			// pass up zero as capabilities "seen"
			iSignallingChannel.GetCapsConfirm(aLabel, err, 0);
			break;
			}	
		case EGeneralReject:
			{
			err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.GetCapsConfirm(aLabel, err, 0);
			break;
			}
		}
	}
	
/*
Handle a fully reassembled SetConfig command or response
*/
void CAvdtpInboundSignallingMessage::DoHandleSetConfigL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	TInt err = KErrNone;
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpSetConfigCommandMinimumLength, KAvdtpSetConfigCommandMaximumLength);

			TSEID intSEID;
			TSEID acpSEID;
			acpSEID = LocalSEIDL();
			Data().TrimStart(1); // gulped ACP SEID
			intSEID = RemoteSEIDL();
			Data().TrimStart(1); // gulped INT SEID
			
			if (!Data().Length())
				{
				// empty configuration! not allowed?
				AvdtpInternalUtils::PacketErrorLeaveL(EAvdtpBadPayloadFormat);
				}
				
			RBuf8 caps;
			CleanupClosePushL(caps);
			caps.CreateMaxL(Data().Length());
			Data().CopyOut(caps);
			
			// parse caps for broken ones
			CCapabilityValidateVisitor* validater = new (ELeave) CCapabilityValidateVisitor;
			validater->Process(caps);
			
			CleanupStack::Pop(1); //caps

			if (validater->Result()!=KErrNone)
				{
				caps.Close();
				iSignallingChannel.SendSetConfigurationReject(aLabel,
													AvdtpInternalUtils::SymbianErrorToAvdtpError(validater->Result()),
													validater->InvalidCategory());
				}
			else
				{
				// packet now only left with config payload, pass RBuf ownership
				iSignallingChannel.SetConfigIndication(aLabel, acpSEID, intSEID, caps);
				}

			delete validater;			
			break;
			}
		case EResponseAccept:
			{
			CheckPacketLength(KAvdtpSetConfigAcceptMinimumLength, KAvdtpSetConfigAcceptMaximumLength);
			iSignallingChannel.SetConfigConfirm(aLabel, err, EServiceCategoryNull);
			break;
			}
		case EResponseReject:
			{
			err = CheckPacketLength(KAvdtpSetConfigRejectMinimumLength, KAvdtpSetConfigRejectMaximumLength);
			//Pull out the AVDTP error and convert to Symbian error
			TAvdtpServiceCategory cat = EServiceCategoryNull;
			if (err==KErrNone)
				{
				const TUint8* const rejData = Data().First()->Ptr();
				cat = static_cast<TAvdtpServiceCategory>(rejData[0]);
				err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(rejData[1]));
				}
			iSignallingChannel.SetConfigConfirm(aLabel, err, cat);
			break;
			}
		case EGeneralReject:
			{
			err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.SetConfigConfirm(aLabel, err, EServiceCategoryNull);
			break;
			}
		}
	}

void CAvdtpInboundSignallingMessage::DoHandleGetConfigL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	TInt err=KErrNone;
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpGetConfigCommandMinimumLength, KAvdtpGetConfigCommandMaximumLength);
			iSignallingChannel.GetConfigIndication(aLabel, LocalSEIDL());
			break;
			}
		case EResponseAccept:
			{
			err = CheckPacketLength(KAvdtpGetConfigAcceptMinimumLength, KAvdtpGetConfigAcceptMaximumLength);
			iSignallingChannel.GetConfigConfirm(aLabel, err);
			break;
			}
		case EResponseReject:
			{
			err = CheckPacketLength(KAvdtpGetConfigRejectMinimumLength, KAvdtpGetConfigRejectMaximumLength);
			err = (err==KErrNone) ? SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(Data().First()->Ptr()[0])) : 
									KErrCorrupt;
			iSignallingChannel.GetConfigConfirm(aLabel,err);
			break;
			}
		case EGeneralReject:
			{
			err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.GetConfigConfirm(aLabel,err);
			break;
			}
		default:
			break;
		}	
	}

void CAvdtpInboundSignallingMessage::DoHandleReconfigL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpReConfigCommandMinimumLength, KAvdtpReConfigCommandMaximumLength);

			TSEID intSEID;
			TSEID acpSEID;
			acpSEID = LocalSEIDL();
			Data().TrimStart(1); // gulped ACP SEID
			
			if (!Data().Length())
				{
				// empty configuration! not allowed?
				AvdtpInternalUtils::PacketErrorLeaveL(EAvdtpBadPayloadFormat);
				}
				
			RBuf8 caps;
			CleanupClosePushL(caps);
			caps.CreateMaxL(Data().Length());
			Data().CopyOut(caps);
			
			// parse caps for broken ones
			CCapabilityValidateVisitor* validater = new (ELeave) CCapabilityValidateVisitor;
			validater->Process(caps);
			
			CleanupStack::Pop(1); //caps
			
			if (validater->Result()!=KErrNone)
				{
				caps.Close();
				iSignallingChannel.SendReconfigureReject(aLabel,
													AvdtpInternalUtils::SymbianErrorToAvdtpError(validater->Result()),
													validater->InvalidCategory());
				}
			else
				{
				// packet now only left with config payload
				iSignallingChannel.ReconfigIndication(aLabel, acpSEID, caps);
				}

			delete validater;			
			break;
			}
		case EResponseAccept:
			{
			CheckPacketLength(KAvdtpReConfigAcceptMinimumLength, KAvdtpReConfigAcceptMaximumLength);
			iSignallingChannel.ReconfigConfirm(aLabel, KErrNone, EServiceCategoryNull);
			break;
			}
		case EResponseReject:
			{
			TInt err = CheckPacketLength(KAvdtpReConfigRejectMinimumLength, KAvdtpReConfigRejectMaximumLength);
			TAvdtpServiceCategory cat = EServiceCategoryNull;
			if (err==KErrNone)
				{
				//Pull out the AVDTP error and convert to Symbian error
				const TUint8* const rejData  = Data().First()->Ptr();
				cat = static_cast<TAvdtpServiceCategory>(rejData[0]);
				err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(rejData[1]));					
				}
			iSignallingChannel.ReconfigConfirm(aLabel, err, cat);
			break;
			}
		case EGeneralReject:
			{
			TInt err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.ReconfigConfirm(aLabel, err, EServiceCategoryNull);
			break;
			}
		}
	}

void CAvdtpInboundSignallingMessage::DoHandleOpenL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpOpenCommandMinimumLength, KAvdtpOpenCommandMaximumLength);
			iSignallingChannel.OpenIndication(aLabel, LocalSEIDL());
			break;
			}
		case EResponseAccept:
			{
			TInt err = CheckPacketLength(KAvdtpOpenAcceptMinimumLength, KAvdtpOpenAcceptMaximumLength);
			iSignallingChannel.OpenConfirm(aLabel, err);
			break;
			}
		case EResponseReject:
			{
			TInt err = CheckPacketLength(KAvdtpOpenRejectMinimumLength, KAvdtpOpenRejectMaximumLength);
			err = (err==KErrNone) ? SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(Data().First()->Ptr()[0])) : 
									KErrCorrupt;
			iSignallingChannel.OpenConfirm(aLabel, err);
			break;
			}
		case EGeneralReject:
			{
			TInt err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.OpenConfirm(aLabel, err);
			break;
			}
		default:
			break;
		}
	}

void CAvdtpInboundSignallingMessage::DoHandleStartL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	TInt err=KErrNone;
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpStartCommandMinimumLength, KAvdtpStartCommandMaximumLength);
			// get list of seids
			// copy out - the data at this point is just the SEID list
			TInt numSEIDs = Data().Length();
			if (!numSEIDs)
				{
				AvdtpInternalUtils::PacketErrorLeaveL(EAvdtpBadPayloadFormat);
				}
			else
				{
				// we issue the starts individually
				// as we cannot assume that the SEPs are within the same client process
				// so even if we could send multiple SEIDs to each client process there is
				// no synchronisation provided.
				for (TInt i=0; i<numSEIDs; i++)
					{
					iSignallingChannel.StartIndication(aLabel, LocalSEIDL());
					Data().TrimStart(1);
					}
				}
			break;
			}
		case EResponseAccept:
			{
			err = CheckPacketLength(KAvdtpStartAcceptMinimumLength, KAvdtpStartAcceptMaximumLength);
			iSignallingChannel.StartConfirm(aLabel, err);
			break;
			}
		case EResponseReject:
			{
			err = CheckPacketLength(KAvdtpStartRejectMinimumLength, KAvdtpStartRejectMaximumLength);
			err = (err==KErrNone) ? SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(Data().First()->Ptr()[1])) : 
									KErrCorrupt;
			// ignore the SEID - the channel will have stored the SEID (and the Accept doesnt have it!)
			iSignallingChannel.StartConfirm(aLabel, err);
			break;
			}
		case EGeneralReject:
			{
			err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.StartConfirm(aLabel, err);
			break;
			}
		default:
			break;
		} //end switch
	}
	
void CAvdtpInboundSignallingMessage::DoHandleReleaseL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpCloseCommandMinimumLength, KAvdtpCloseCommandMaximumLength);
			iSignallingChannel.ReleaseIndication(aLabel, LocalSEIDL());
			break;
			}
		case EResponseAccept:
			{
			TInt err = CheckPacketLength(KAvdtpCloseAcceptMinimumLength, KAvdtpCloseAcceptMaximumLength);
			iSignallingChannel.ReleaseConfirm(aLabel, err);
			break;
			}
		case EResponseReject:
			{
			TInt err = CheckPacketLength(KAvdtpCloseRejectMinimumLength, KAvdtpCloseRejectMaximumLength);
			err = (err==KErrNone) ? SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(Data().First()->Ptr()[0])) : 
									KErrCorrupt;
			iSignallingChannel.ReleaseConfirm(aLabel, err);
			break;
			}
		case EGeneralReject:
			{
			TInt err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.ReleaseConfirm(aLabel, err);
			break;
			}
		}
	}

void CAvdtpInboundSignallingMessage::DoHandleSuspendL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	TInt err = KErrNone;
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpSuspendCommandMinimumLength, KAvdtpSuspendCommandMaximumLength);
			// get list of seids
			// copy out - the data at this point is just the SEID list
			TInt numSEIDs = Data().Length();
			if (!numSEIDs)
				{
				AvdtpInternalUtils::PacketErrorLeaveL(EAvdtpBadPayloadFormat);
				}
			else
				{
				// we issue the suspends individually
				// as we cannot assume that the SEPs are within the same client process
				// so even if we could send multiple SEIDs to each client process there is
				// no synchronisation provided.
				for (TInt i=0; i<numSEIDs; i++)
					{
					iSignallingChannel.SuspendIndication(aLabel, LocalSEIDL());
					Data().TrimStart(1);
					}
				}
			break;
			}
		case EResponseAccept:
			{
			err = CheckPacketLength(KAvdtpSuspendAcceptMinimumLength, KAvdtpSuspendAcceptMaximumLength);
			iSignallingChannel.SuspendConfirm(aLabel, err);
			break;
			}
		case EResponseReject:
			{
			err = CheckPacketLength(KAvdtpSuspendRejectMinimumLength, KAvdtpSuspendRejectMaximumLength);
			err = (err==KErrNone) ? SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(Data().First()->Ptr()[1])) : 
									KErrCorrupt;
			// ignore the SEID as the Signalling Channel will have stored it
			iSignallingChannel.SuspendConfirm(aLabel,err);
			break;
			}
		case EGeneralReject:
			{
			err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.SuspendConfirm(aLabel,err);
			break;
			}
		default:
			break;
		}
	}

void CAvdtpInboundSignallingMessage::DoHandleAbortL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	switch (iMessageType)
		{
		case ECommand:
			{
			TInt err = KErrNone;

			err = CheckPacketLength(KAvdtpAbortCommandMinimumLength, KAvdtpAbortCommandMaximumLength);
			if (err==KErrNone)
				{
				iSignallingChannel.AbortIndication(aLabel, LocalSEIDL());
				}
			break;
			}
		case EResponseAccept:
			{
			iSignallingChannel.AbortConfirm(aLabel);
			break;
			}
		case EResponseReject:
		case EGeneralReject:	//General reject will be handled similar to Response Reject
			{
			//Don't expect this - spec a bit vague
			// just throw away
			break;
			}
		}
	}

void CAvdtpInboundSignallingMessage::DoHandleSecurityControlL(TAvdtpTransactionLabel aLabel)
	{
	LOG_FUNC
	TInt err;
	switch (iMessageType)
		{
		case ECommand:
			{
			CheckPacketLengthL(KAvdtpSecurityControlCommandMinimumLength, KAvdtpSecurityControlCommandMaximumLength);
			// fish out the capabilities from mbufchain
			// as this is immutable HBufC8 is chosen over RBuf8
			TSEID localSEID;
			localSEID = LocalSEIDL();
			Data().TrimStart(1); // gulp SEID
			HBufC8* securityData = HBufC8::NewMaxL(Data().Length());
			TPtr8 des = securityData->Des();
			Data().CopyOut(des);
			//HBuf ownserhsip is transferred
			iSignallingChannel.SecurityControlIndication(aLabel, localSEID, securityData);

			break;
			}
		case EResponseAccept:
			{
			err = CheckPacketLength(KAvdtpSecurityControlAcceptMinimumLength, KAvdtpSecurityControlAcceptMaximumLength);
			// fish out the capabilities from mbufchain
			HBufC8* securityData = HBufC8::NewMaxL(Data().Length());
			TPtr8 des = securityData->Des();
			Data().CopyOut(des);
			
			iSignallingChannel.SecurityControlConfirm(aLabel, err, *securityData);
			
			delete securityData;
			break;
			}
		case EResponseReject:
			{
			err = CheckPacketLength(KAvdtpSecurityControlRejectMinimumLength, KAvdtpSecurityControlRejectMaximumLength);
			err = (err==KErrNone) ? SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(Data().First()->Ptr()[0])) : 
									KErrCorrupt;
			iSignallingChannel.SecurityControlConfirm(aLabel, err, KNullDesC8);
			break;
			}
		case EGeneralReject:
			{
			err = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(static_cast<TAvdtpSignallingErrorCode>(EAvdtpNotSupportedCommand));
			iSignallingChannel.SecurityControlConfirm(aLabel, err, KNullDesC8);
			break;
			}
		}	
	}


/**
Splits the AVDTP message part from the packet
Returns a new mbufchain containing the message
The client has to free the input and output chains when convenient
*/
/*static*/ RMBufChain CAvdtpInboundSignallingMessage::MessageL(RMBufChain& aPacket, const TAvdtpPacketType aPacketType)
	{
	LOG_STATIC_FUNC
	TInt offset = 0;
	
	switch (aPacketType)
		{
		case ESingle:
		case EEnd:
		case EContinue:
			__ASSERT_DEBUG(aPacket.Length() > 0, Panic(EAvdtpGotBadDataFromL2CAP));
			//if not in DEBUG mode, prevent MBuf panic by leaving here if 
			//we have received a corrupt frame.
			User::LeaveIfError(aPacket.Length() > 0 ? KErrNone : KErrUnderflow);
			offset = 1;
			break;
		case EStart:
			__ASSERT_DEBUG(aPacket.Length() > 1, Panic(EAvdtpGotBadDataFromL2CAP));
			//if not in DEBUG mode, prevent MBuf panic by leaving here if 
			//we have received a corrupt frame.
			User::LeaveIfError(aPacket.Length() > 1 ? KErrNone : KErrUnderflow);
			offset = 2;
			break;
		}
	RMBufChain message;
	aPacket.SplitL(offset, message);
	return message;
	}

/**
Assumes NOSP is at offset 1
*/
/*static*/ TUint8 CAvdtpInboundSignallingMessage::NumberSignalPackets(const RMBufChain& aChain)
	{
	LOG_STATIC_FUNC
	return aChain.First()->Ptr()[1]; // The length was checked in MessageL, so we don't need to check it here.
	}

/**
Note for Start packets this assumes NOSP has been consumed, it takes the zeroth byte as the signal
Can only be used for Single and Start packets - no signal in Continue and End packets
*/
/*static*/ TAvdtpMessage CAvdtpInboundSignallingMessage::SignalIdentifier(const RMBufChain& aChain)
	{
	LOG_STATIC_FUNC
	TAvdtpMessage message;
	if (aChain.First() == NULL) 
		{
		message = EAvdtpNull;
		}
	else 
		{
		message = static_cast<TAvdtpMessage>(aChain.First()->Ptr()[0] & KAvdtpSignalIdentifierMask);
		}
	return message;
	}

/**
returns the (semantically local) SEID from the packet - leaves if bad seid
many commands received have SEID in the same place - this *assumes* that this is true
**/
TSEID CAvdtpInboundSignallingMessage::LocalSEIDL()
	{
	LOG_FUNC
	return TSEID::FromPacketL(Data().First()->Ptr()[0], ETrue); // The length of the packet is always checked by CheckPacketLengthL before this function is called
	}
	
/**
returns the (semantically remote) SEID from the packet - leaves if bad seid
many commands received have SEID in the same place - this *assumes* that this is true
**/
TSEID CAvdtpInboundSignallingMessage::RemoteSEIDL()
	{
	LOG_FUNC
	return TSEID::FromPacketL(Data().First()->Ptr()[0], EFalse); // The length of the packet is always checked by CheckPacketLengthL before this function is called
	}
	
/**
Returns what to do after packet sent:
e.g. setRTX, don't set RTX, or immediately scrub knowledge of sending
Whether or not the AVDTP Signalling RTX applies to this message
[calculated from the Message type based on GAVDTP 4.1.8]

@return the action to use after sending
*/
TPostSendAction CAvdtpOutboundSignallingMessage::PostSendAction() const
	{
	LOG_FUNC
	if (iMessageType == ECommand)
		{
		//OutBound Signalling message should never be EReserved
		__ASSERT_DEBUG(Signal()!=EReserved,Panic(EAvdtpInvalidReservedValueInOutboundSignallingMessage));
		switch(Signal())
			{
			case EAvdtpGetCapabilities:
			case EAvdtpDiscover:
			case EAvdtpSecurityControl:
				return EKeepDontSetRTX;
			default:
				return EKeepSetRTX;		
			}
		}
	else
		{
		return EDiscard;
		}	
	}
	
	
/**
Used for inbound command checking where the leave is trapped and a response sent
*/
void  CAvdtpInboundSignallingMessage::CheckPacketLengthL(TUint aMin, TUint aMax)
	{
	LOG_FUNC
	User::LeaveIfError(CheckPacketLength(aMin, aMax));
	}

/**
Used for inbound responses - doesn't leave as we don't send a response, but we do tell the client
that their command has completed with an AVDTP error
*/	
TInt CAvdtpInboundSignallingMessage::CheckPacketLength(TUint aMin, TUint aMax)
	{
	LOG_FUNC
	TInt ret = KErrNone;
	
	TInt pktLen = Data().Length();
	
	if  (pktLen>aMax || pktLen<aMin)
		{
		ret = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadLength);
		}
	return ret;
	}
	
/*static*/	
TInt AvdtpSignallingMessageDiscover::Response::ParseL(const RMBufChain& aMessageData,
												 TDes8& aClientBuffer,
												 CRemoteSEPCache& aSEPCache,
												 const TBTDevAddr& aAddr)
	{
	LOG_STATIC_FUNC
	aClientBuffer.Zero();
	
	TInt numSEPs = (aMessageData.Length())/2;
	
	__ASSERT_DEBUG(numSEPs, Panic(EAvdtpBadSEPCount)); // this should have been policed earlier
	
	// initial checks
	if (numSEPs>KMaxAvdtpNumSEPs)
		{
		return KErrCorrupt;
		}
		
	if (aClientBuffer.MaxLength()<(numSEPs*sizeof(TAvdtpSEPInfo)))
		{
		return KErrNoMemory;
		}
	
	// ok - parse
	TInt err = KErrNone;
	
	for (TInt i=0;i<=numSEPs-1;i++)
		{
		TBuf8<KAvdtpPacketSEPInfoSize> dataBuf;
		dataBuf.SetLength(KAvdtpPacketSEPInfoSize);
		aMessageData.CopyOut(dataBuf,(i*KAvdtpPacketSEPInfoSize));
		
		TAvdtpSEPInfo sepInfo;
		sepInfo.SetSEID(TSEID::FromPacketL(dataBuf[0], EFalse));
		sepInfo.SetInUse(dataBuf[0] & 0x02 ? ETrue : EFalse);
		sepInfo.SetMediaType(static_cast<SymbianBluetoothAV::TBluetoothMediaType>
										(dataBuf[1]>>4));
		sepInfo.SetIsSink((dataBuf[1] & 0x08) ? ETrue : EFalse);
		
		err = aSEPCache.AddSEP(aAddr, sepInfo);
		
		if (err==KErrNone)
			{
			TPckg<TAvdtpSEPInfo> desc(sepInfo);
			aClientBuffer.Append(desc);	
			}
		}
	
	return err;
	}


/*static*/ void AvdtpSignallingMessageSetConfiguration::Command::FormatL(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID,
																	TSEID aINTSEID,
																	const RBuf8& aConfiguration)
	{
	LOG_STATIC_FUNC
	aSignallingMessage.AppendDataL(aACPSEID.PacketValue());
	aSignallingMessage.AppendDataL(aINTSEID.PacketValue());
	aSignallingMessage.AppendDataL(aConfiguration);
	}

/*static*/ void AvdtpSignallingMessageReconfigure::Command::FormatL(CAvdtpOutboundSignallingMessage& aSignallingMessage,
																	TSEID aACPSEID,
																	const RBuf8& aConfiguration)
	{
	LOG_STATIC_FUNC
	aSignallingMessage.AppendDataL(aACPSEID.PacketValue());
	aSignallingMessage.AppendDataL(aConfiguration);
	}


/*static*/
void AvdtpSignallingMessage::Reject::FormatL(CAvdtpOutboundSignallingMessage& aSignallingMessage, 
					TBluetoothAvDistributionError aRejectionCode, const TDesC8* aRejectionData/*=NULL*/)
	{
	LOG_STATIC_FUNC
	if (aRejectionData)
		{
		aSignallingMessage.AppendDataL(*aRejectionData);
		}
	aSignallingMessage.AppendDataL(aRejectionCode);
	}


#ifdef _DEBUG
TBool CCapabilityFlogVisitor::Capability(TAvdtpServiceCategory /*aCat*/)
	{
//	FPrint(_L("Avdtp: Capability %d, payload %S"), aCat, &aCapPayload);
	return ETrue;
	}
#endif

	
	
TPtrC8 CCapabilityExtractorVisitor::GetCapability() const
	{
	return iPtr;
	}

	

CCapabilityExtractorVisitor::CCapabilityExtractorVisitor(TAvdtpServiceCategory aRequiredCapability)
: iRequiredCapability(aRequiredCapability)
	{
	}

TBool CCapabilityExtractorVisitor::Capability(TAvdtpServiceCategory aCat)
	{
	LOG_FUNC
	if (aCat == iRequiredCapability)
		{
		iPtr.Set(CapabilityPayload());
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}


TBool CCapabilityValidateVisitor::Capability(TAvdtpServiceCategory aCat)
	{
	LOG_FUNC
	// check lengths of payload of capability for known categories
	// unknown categories are marked invalid
	TBool valid = ETrue;
	
	switch (aCat)
		{
		case EServiceCategoryMediaTransport:
 		valid = IsLOSCValid(CapabilityPayload().Length(), KAvdtpCapabilityMediaTransportMinimumLOSC, KAvdtpCapabilityMediaTransportMaximumLOSC);
		iResult = valid ? KErrNone : SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadMediaTransportFormat);
 		break;
		case EServiceCategoryReporting:
		valid = IsLOSCValid(CapabilityPayload().Length(), KAvdtpCapabilityReportingMinimumLOSC, KAvdtpCapabilityReportingMaximumLOSC);
		// spec doesn't (yet?) have a specific error code for this! use bad-payload instead
		iResult = valid ? KErrNone : SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadPayloadFormat);
		break;
		case EServiceCategoryRecovery:
		valid = IsLOSCValid(CapabilityPayload().Length(), KAvdtpCapabilityRecoveryMinimumLOSC, KAvdtpCapabilityRecoveryMaximumLOSC);
		iResult = valid ? KErrNone : SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadRecoveryFormat);
		break;
		case EServiceCategoryContentProtection:
		valid = IsLOSCValid(CapabilityPayload().Length(), KAvdtpCapabilityContentProtectionMinimumLOSC, KAvdtpCapabilityContentProtectionMaximumLOSC);
		iResult = valid ? KErrNone : SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadCpFormat);
		break;
		case EServiceCategoryHeaderCompression:
		valid = IsLOSCValid(CapabilityPayload().Length(), KAvdtpCapabilityHeaderCompressionMinimumLOSC, KAvdtpCapabilityHeaderCompressionMaximumLOSC);
		iResult = valid ? KErrNone : SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadRohcFormat);
		break;
		case EServiceCategoryMultiplexing:
		valid = IsLOSCValid(CapabilityPayload().Length(), KAvdtpCapabilityMultiplexingMinimumLOSC, KAvdtpCapabilityMultiplexingMaximumLOSC);
		iResult = valid ? KErrNone : SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadMultiplexingFormat);
		break;
		case EServiceCategoryMediaCodec:
		valid = IsLOSCValid(CapabilityPayload().Length(), KAvdtpCapabilityMediaCodecMinimumLOSC, KAvdtpCapabilityMediaCodecMaximumLOSC);
		// daft spec doesn't have one for this! use bad-payload instead
		iResult = valid ? KErrNone : SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadPayloadFormat);
		break;
		
		default:
		// unknown capability
		iResult = SymbianBluetoothAV::ConvertToSymbianError::AvdtpError(EAvdtpBadServCategory);
		valid = EFalse;
		}

	if (!valid)		
		{
		// store the invalid details
		iInvalidCategory = aCat;
		}
		
	return valid; // at present this visitor causes termination at first bad category
	}

inline TInt CCapabilityValidateVisitor::Result() const
	{
	LOG_FUNC
	return iResult;
	}

inline TAvdtpServiceCategory CCapabilityValidateVisitor::InvalidCategory() const
	{
	LOG_FUNC
	return iInvalidCategory;
	}

TBool CCapabilityRemoveUnknownVisitor::Capability(TAvdtpServiceCategory aCat)
	{
	LOG_FUNC

	switch (aCat)
		{
		case EServiceCategoryMediaTransport:
 		case EServiceCategoryReporting:
		case EServiceCategoryRecovery:
		case EServiceCategoryContentProtection:
		case EServiceCategoryHeaderCompression:
		case EServiceCategoryMultiplexing:
		case EServiceCategoryMediaCodec:
			// Do nothing
			break;
		
		default:
		// unknown capability
			CapabilityDes().Zero();
		}
		
	return ETrue;
	}

inline TBool CCapabilityValidateVisitor::IsLOSCValid(TUint aLOSC, TUint aMin, TUint aMax)
	{
	LOG_FUNC
	return (aLOSC<=aMax && aLOSC>=aMin);
	}

// static forward functions to make more efficient than pointer-to-member-function
void CAvdtpInboundSignallingMessage::HandleDiscoverL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleDiscoverL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleGetCapsL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleGetCapsL(aLabel);	
	}
	
void CAvdtpInboundSignallingMessage::HandleSetConfigL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleSetConfigL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleGetConfigL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleGetConfigL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleReconfigL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleReconfigL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleOpenL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleOpenL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleStartL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleStartL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleReleaseL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleReleaseL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleSuspendL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleSuspendL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleAbortL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleAbortL(aLabel);
	}
	
void CAvdtpInboundSignallingMessage::HandleSecurityControlL(CAvdtpInboundSignallingMessage& aThat, TAvdtpTransactionLabel aLabel)
	{
	LOG_STATIC_FUNC
	aThat.DoHandleSecurityControlL(aLabel);
	}
	
