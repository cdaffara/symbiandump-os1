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
// Implements the code for the connected and group L2CAP saps
// 
//

#include <bluetooth/logger.h>

#include "l2sap.h"

#include "l2sapstates.h"
#include "l2cap.h"

#include "l2util.h"

#include "L2CapSDUQueue.h"
#include "L2CapDataController.h"
#include "l2capSAPSignalHandler.h"
#include "L2types.h"

#include "L2CapDebugControlInterface.h"
#include "l2capCommand.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

CL2CAPConnectionSAP* CL2CAPConnectionSAP::NewL(CL2CAPProtocol& aProt)
	{
	LOG_STATIC_FUNC
	// Create and return a new SAP
	CL2CAPConnectionSAP* sap= new (ELeave) CL2CAPConnectionSAP(aProt);
	CleanupStack::PushL(sap);
	sap->ConstructL();
	CleanupStack::Pop();
	return sap;
	}

void CL2CAPConnectionSAP::ConstructL()
	{
	LOG_FUNC
	CBluetoothSAP::ConstructL();
	TCallBack cb(NewDataAsyncCallBack, this);
	iNewDataAsyncCallBack = new (ELeave)CAsyncCallBack(cb, EActiveHighPriority);
	iL2CapSAPSignalHandler = CL2CapSAPSignalHandler::NewL(*this);
	LOG2(_L("CL2CAPConnectionSAP.iL2CapSAPSignalHandler = %X.%X"), (TAny*)this, (TAny*)iL2CapSAPSignalHandler)
	}

CL2CAPConnectionSAP::CL2CAPConnectionSAP(CL2CAPProtocol& aProt)
 : CBluetoothSAP(aProt.SecMan(), aProt.CodMan()),
   iProtocol(aProt),
   iShutdownReceived(ESAPShutdownNone),
   iSocketErrorCode(KErrNone),
   iSocketErrorAction(MSocketNotify::TOperationBitmasks(0))
	{
	LOG_FUNC
	iState=&aProt.StateFactory().GetState(CL2CAPSAPStateFactory::EClosed);

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ESAP,
	                             L2capDebugInfo::EAllocated));
	}



CL2CAPConnectionSAP::~CL2CAPConnectionSAP()
/**
   SAP destructor.
   Called when the socket deletes the SAP.
*/
	{
	LOG_FUNC

	__ASSERT_DEBUG(iClones.Count() == 0, Panic(EL2CAPClonesExistDuringSAPDestructor));

	// Ensure that everything is completely shutdown.
	iState->FastShutdown(*this);

	// Inform the signal handler that the SAP is being destructed.
	// The SH will normally destroy itself unless it has a message
	// that still needs to be sent.
	if(iL2CapSAPSignalHandler)
		{
		iL2CapSAPSignalHandler->SAPClosed();
		}

	__ASSERT_DEBUG(iL2CapDataQueue == NULL, Panic(EL2CAPSDUQueueStillExistsDuringSAPDestructor));
		
	delete iNewDataAsyncCallBack;
	iClones.Close();

	//We may be still attached to the listening SAP if the connection was not fully completed before shutdown
	//No checking needed because iListeningSAP will be NULL if this is the listening SAP or we are already 
	//detached from the listeningSAP 
	DetachFromListeningSAP();

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::ESAP,
	                             L2capDebugInfo::EDeleted));
	}


TBool CL2CAPConnectionSAP::CanAcceptConnection() const
	{
	LOG_FUNC
	return (TotalOutstandingCloneCount() < iMaxAcceptingQCount);
	}

TUint8 CL2CAPConnectionSAP::TotalOutstandingCloneCount() const
   	{
	LOG_FUNC
   	return static_cast<TUint8>(iClones.Count());
   	}

CL2CAPConnectionSAP* CL2CAPConnectionSAP::CloneListeningSAP(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	
	TInt rerr = KErrNone;
	CL2CAPConnectionSAP* clone = NULL;

	TRAP(rerr, clone = NewL(iProtocol));
	if(rerr == KErrNone)
		{
		if(!EnqueClone(clone))
			{
			delete clone;
			clone = NULL;
			}
		else
			{
			clone->SetState(iState->iFactory.GetState(CL2CAPSAPStateFactory::EPassiveLinkPending));
			clone->SetListeningSAP(*this);
			
			clone->SetCodServiceBits(iCodServiceBits);
			
			// Copy the details required for security.
			clone->iSecurity = iSecurity;
			clone->iRemoteDev = aAddr; 
			}
		}
	return clone;
	}


TBool CL2CAPConnectionSAP::EnqueClone(CL2CAPConnectionSAP* aSAP)
	{
	LOG_FUNC
	TInt rerr = iClones.Append(aSAP);
	return (rerr == KErrNone);
	}

TBool CL2CAPConnectionSAP::DequeClone(CL2CAPConnectionSAP* aSAP)
	{
	LOG_FUNC
	TBool rcode = EFalse;
	TInt ix = iClones.Find(aSAP);

	if(ix != KErrNotFound)
		{
		iClones.Remove(ix);
		rcode = ETrue;
		}
	return rcode;
	}

void CL2CAPConnectionSAP::DeleteAllClones()
/**
   This function is called by TL2CAPSAPStateListening::Exit() to clear
   the iClones queue and delete all the clones. However, any clones
   with their iAcceptingPending flag raised are owned by a socket,
   so they shouldn't be deleted. Instead their pointer to this SAP
   should be nulled, then they should be removed from the queue.
**/
	{
	LOG_FUNC
	
	for(TInt i=iClones.Count()-1;i>=0;i--)
		{
		if(iClones[i]->IsAcceptPending())
			{
			iClones[i]->DetachFromListeningSAP();
			}
		}
	iClones.ResetAndDestroy();
	}

void CL2CAPConnectionSAP::DeleteSAP()
	{
	LOG_FUNC
	delete this;
	}
	
void CL2CAPConnectionSAP::SocketConnectComplete()
/**
   A Clone has completed a connection, so we tell our socket.
   
   Send the socket a ConnectComplete, the clone remains in the listening SAPs clone 
   queue, but its iAcceptPending flag is raised. Logically ownership of the clone has
   been transferred from the listening SAP to the socket, however the listening SAP 
   keeps a pointer until the socket calls Start(). 
**/
	{
	LOG_FUNC

	// There was a mysterious problem with AVDTP signalling channel connections
	// which suggested that iL2CapDataQueue was 0 when BearerConnectionComplete was
	// being called. The problem has disappeared but added this just in case it comes
	// back again.
	__ASSERT_DEBUG(iL2CapDataQueue != NULL, Panic(EL2CAPSduQNotExisitantInConnectComplete));

	if(iListeningSAP)
		{
		SetState(iState->iFactory.GetState(CL2CAPSAPStateFactory::EAccepting));
		iAcceptPending = ETrue;
		iListeningSAP->Socket()->ConnectComplete(*this);
		}
	else
		{
		SetState(iState->iFactory.GetState(CL2CAPSAPStateFactory::EAwaitingInitialData));
		Socket()->ConnectComplete();
		}
	}

/*static*/ TInt CL2CAPConnectionSAP::NewDataAsyncCallBack(TAny* aSAP)
	{
	LOG_STATIC_FUNC
	CL2CAPConnectionSAP* sap = static_cast<CL2CAPConnectionSAP*>(aSAP);
	sap->iState->NewDataAsyncCallBack(*sap);
	return EFalse;
	}

void CL2CAPConnectionSAP::LocalName(TSockAddr& anAddr) const
/**
	Read the Local Name into aAddr.
	Note: On protocol start-up, the protocol might not have received the
	BTAddr back from the HW when this is called... :-(
	So the user should use the read local address ioctl to be sure of 
	getting the right result.

**/
	{
	LOG_FUNC

	// Copy iLocalPort into TSockAddr and return
	TL2CAPSockAddr& addr = TL2CAPSockAddr::Cast(anAddr);
	iL2CapSAPSignalHandler->GetLocalParameters(addr);
	}


TInt CL2CAPConnectionSAP::SetLocalName(TSockAddr& anAddr)
	{
	LOG_FUNC

	TL2CAPSockAddr l2capAddr = TL2CAPSockAddr::Cast(anAddr);
	TL2CAPPort psm = l2capAddr.Port();
	TInt err = KErrNone;

	if(psm == KL2CAPPassiveAutoBind)
		{
		// User wishes for us to choose free ServerChannel
		err = Protocol().MuxController().FindFreeUserPSM(psm);
		if(err == KErrNone)
			{
			l2capAddr.SetPort(psm);
			}
		}
	else
		{
		// User supplied explicit PSM.  Check that the value is odd.
		if(psm & 0x0001)
			{
			// Valid, so see if available
			if(Protocol().MuxController().FindIdleSignalHandler(psm))
				{//psm in use already
				err = KErrInUse;
				}
			}
		else
			{
			// PSM not valid (as per 1.0B spec)
			err = KErrArgument;
			}
		}


	if (err == KErrNone)
		{
		iL2CapSAPSignalHandler->SetLocalParameters(l2capAddr);	
		iState->Bound(*this);
		iSecurity = l2capAddr.BTSecurity();
		}

	return err;
	}

void CL2CAPConnectionSAP::RemName(TSockAddr& anAddr) const
	{
	LOG_FUNC
	//Return the remote name

	// Copy iRemoteDev and iRemotePort into TSockAddr and return
	TL2CAPSockAddr& addr = TL2CAPSockAddr::Cast(anAddr);
	iL2CapSAPSignalHandler->GetRemName(addr);
	}

TInt CL2CAPConnectionSAP::SetRemName(TSockAddr& anAddr)
	{
	LOG_FUNC
	
	// Copy this over
	TL2CAPSockAddr& addr = TL2CAPSockAddr::Cast(anAddr);
	iL2CapSAPSignalHandler->SetRemName(addr);
	
	// Set the security and remote address values in the base class.
	// This is required for security handling.
	iSecurity = addr.BTSecurity();	//outgoing security
	iRemoteDev = addr.BTAddr();
	
	LOG1(_L("L2CAP [l2sap.cpp]: Set remname to port %d, Device...."), addr.Port());
	LOGHEXDESC(addr.BTAddr().Des());
	return KErrNone;
	}

const TBTDevAddr& CL2CAPConnectionSAP::RemoteDev() const
	{
	LOG_FUNC
	return iRemoteDev;
	}

void CL2CAPConnectionSAP::SetRemoteDev(const TBTDevAddr& aAddr)
	{
	LOG_FUNC
	iRemoteDev = aAddr;
	LOGBTDEVADDR(iRemoteDev);
	}

TUint CL2CAPConnectionSAP::GetOptimalMTUSize(TUint aMTU, TUint aPduSize, TBool aBasicMode) const
	{
	LOG_FUNC

	// If the negotiated MTU minus any overhead will fit into the optimal PDU then that
	// is the optimal MTU. The overhead will differ for basic and non-basic mode, for basic mode
	// we have to consider the SDU overhead as there is no fragment overhead and for non-basic we 
	// consider the PDU overhead only (the additional SDU overhead is taken account of later if 
	// more than one PDU is required for the optimal MTU).
	TUint optimalMTU = aMTU;

	// Calculate the size of the MTU + any overhead assuming that the MTU is not segmented
	TUint singlePduSize = aBasicMode ? (aMTU + CL2CapSDU::GetSDUOverhead(aBasicMode)) : aMTU;

	// If the unsegmented MTU + overhead can fit into the optimal PDU size then no 
	// further calculation is required
	if(singlePduSize > aPduSize)
		{
		// The MTU will need to be segmented / fragmented (depending on L2CAP mode).
		// Calculate an MTU size that will be a factor of the PDU size.
		optimalMTU = aMTU - ((aMTU + CL2CapSDU::GetSDUOverhead(aBasicMode)) % aPduSize); 
		}

	return optimalMTU;
	}

TInt CL2CAPConnectionSAP::SAPSetOption(TUint aLevel, TUint aName, const TDesC8& aOption)
	{
	LOG_FUNC
	// Perform a setopt

	if (aLevel!=KSolBtL2CAP)
		{
		// pass down

		return iL2CapSAPSignalHandler->SetOption(aLevel, aName, aOption);
		}

	switch (aName)
		{
		case KL2CAPRTXTimer:
			{
			TInt val = *reinterpret_cast<const TInt*>(aOption.Ptr());
			if (aOption.Length() != sizeof(TInt) || val < HL2CapCommand::KMinRTXTimerDuration || val > HL2CapCommand::KMaxRTXTimerDuration)
				return KErrArgument;
			return iL2CapSAPSignalHandler->SetRTXTimerDuration(static_cast<TUint8>(val));
			}
		case KL2CAPERTXTimer:
			{
			TInt val = *reinterpret_cast<const TInt*>(aOption.Ptr());
			if (aOption.Length() != sizeof(TInt) || val < HL2CapCommand::KMinERTXTimerDuration || val > HL2CapCommand::KMaxERTXTimerDuration)
				return KErrArgument;
			return iL2CapSAPSignalHandler->SetERTXTimerDuration(static_cast<TUint8>(val));
			}
		case KL2CAPInboundMTU:
			{
			TUint16 val = *reinterpret_cast<const TUint16*>(aOption.Ptr());
			if (aOption.Length() != sizeof(TUint16) || val < KL2MinMTU)
				return KErrArgument;
			TL2CapConfig conf;
			conf.SetMaxReceiveUnitSize(val);
			return iL2CapSAPSignalHandler->UpdateChannelConfig(conf);
			}
#ifdef _DEBUG
		case KL2CAPDebugOptionMask:
			{// Set the debug options mask
			if (aOption.Length() != sizeof(TUint))
				return KErrArgument;
			Protocol().iDebugOptionMask = *reinterpret_cast<const TUint*>(aOption.Ptr());
			return KErrNone;
			}
#endif
		case KL2CAPNegotiatedOutboundMTU:
	    case KL2CAPOutboundMTUForBestPerformance: //equals KL2CAPGetOutboundMTU here for legacy purposes
	        {
	        TUint16 val = *reinterpret_cast<const TUint16*>(aOption.Ptr());
	        if (aOption.Length() != sizeof(TUint16) || val < KL2MinMTU)
	            return KErrArgument;
			TL2CapConfig conf;
			conf.SetMaxTransmitUnitSize(val);
			return iL2CapSAPSignalHandler->UpdateChannelConfig(conf);
	        }

		case KBTSecurityDeviceOverride:
			{
			return SetDeviceOverride(aOption); 
			}

		case KL2CAPUpdateChannelConfig:
			{
			const TL2CapConfig apiConf = *reinterpret_cast<const TL2CapConfig*>(aOption.Ptr());
            return UpdateChannelConfig(apiConf);
			}
			
		// Can't set these
		default:
			return KErrNotSupported;
		};
	}

TInt CL2CAPConnectionSAP::GetOption(TUint aLevel,TUint aName,TDes8& aOption) const
	{
	LOG_FUNC
	if (aLevel!=KSolBtL2CAP)
		{// it must be for the lower layers then
		
		return iL2CapSAPSignalHandler->GetOption(aLevel, aName, aOption);
		}

	TInt val = 0;
	TInt rValue = KErrNone;
	
	switch (aName)
		{
		case KL2CAPGetMaxOutboundMTU:
			if(aOption.Length() == sizeof(TInt))
				{
				val = KL2CapMaxMTUSize;
				aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
				}
			else
				{
				rValue = KErrArgument;
				}
			break;
			
		case KL2CAPGetMaxInboundMTU:
			if(aOption.Length() == sizeof(TInt))
				{
				val = KL2CapMaxMTUSize;
				aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
				}
			else
				{
				rValue = KErrArgument;
				}
			break;
			
		case KL2CAPNegotiatedOutboundMTU:
			if(aOption.Length() == sizeof(TInt))
				{
				if(iL2CapDataQueue)
					{
					val = iL2CapDataQueue->MaxOutgoingMTU();
					aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
					}
				else
					{
					rValue = KErrNotReady;
					}
				}
			else
				{
				rValue = KErrArgument;
				}
			break;	

		case KL2CAPOutboundMTUForBestPerformanceWithRestriction:
			// get the restriction value
			if((aOption.Length() == sizeof(TInt)) && iL2CapDataQueue && iL2CapSAPSignalHandler)
				{
				val = *reinterpret_cast<const TInt*>(aOption.Ptr());

				// Ensure that the restriction is less then the current MTU.
				if (val < iL2CapDataQueue->MaxOutgoingMTU())
					{
					// We now need to recalculate the optimal PDU size for the restricted MTU as
					// this is used in the calculation of the optimal MTU
					TPckgBuf<TInt> buf;	
					TInt err = iL2CapSAPSignalHandler->GetOption(KSolBtACL, ELMOutboundACLSize, buf);

					TInt optimalPduSize = HL2CapPDU::GetPDUOrFragmentSize(val, iL2CapDataQueue->MaximumPDUSize(), (err == KErrNone) ? buf() : 0, iL2CapDataQueue->IsBasicDataVersion());

					// update the data queue to use the new optimal PDU size from now on
					iL2CapDataQueue->SetOptimalPDUSize(optimalPduSize);
					}
				else
					{
					// can't increase the MTU at this stage so just use the existing MTU
					val = iL2CapDataQueue->MaxOutgoingMTU();
					}

				// work out the optimal MTU
				val = GetOptimalMTUSize(val, iL2CapDataQueue->OptimalPDUSize(), iL2CapDataQueue->IsBasicDataVersion());
				aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
				}			
			break;

		case KL2CAPOutboundMTUForBestPerformance: //equals KL2CAPGetOutboundMTU
			if(aOption.Length() == sizeof(TInt))
				{
				if(iL2CapDataQueue && iL2CapSAPSignalHandler)
					{
					// work out the optimal MTU
					val = GetOptimalMTUSize(iL2CapDataQueue->MaxOutgoingMTU(), iL2CapDataQueue->OptimalPDUSize(), iL2CapDataQueue->IsBasicDataVersion());
					aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
					}
				else
					{
					rValue = KErrNotReady;
					}
				}
			else
				{
				rValue = KErrArgument;
				}
			break;	
			
		case KL2CAPInboundMTU:
			if(aOption.Length() == sizeof(TInt))
				{
				if(iL2CapDataQueue)
					{
					val = iL2CapDataQueue->MaxIncomingMTU();
					aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
					}
				else
					{
					rValue = KErrNotReady;
					}
				}
			else
				{
				rValue = KErrArgument;
				}
			break;	

		case KL2CAPRTXTimer:
			if(aOption.Length() == sizeof(TInt))
				{
				val = iL2CapSAPSignalHandler->RTXTimerDuration();
				aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
				}
			else
				{
				rValue = KErrArgument;
				}
			break;

		case KL2CAPERTXTimer:
			if(aOption.Length() == sizeof(TInt))
				{
				val = iL2CapSAPSignalHandler->ERTXTimerDuration();
				aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
				}
			else
				{
				rValue = KErrArgument;
				}
			break;

		case KBTSecurityDeviceOverride:
			// aspect oriented programming would be nice.
			rValue = GetDeviceOverride(aOption); 
			break;
			
		case KL2CAPGetDebug1:
			val = static_cast<TInt>((static_cast<SBtTls*>(Dll::Tls()))->iPort);
			aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
			break;
			
		// returns the PSM for the accepted SAP (or listening SAP if clients forgot)
		case KL2CAPLocalPSM:
			{
			if (aOption.Length() == sizeof(TL2CAPPort))
				{
				const CL2CAPConnectionSAP* listeningSAP = ListeningSAP();
				if (!listeningSAP)
					{
					listeningSAP = this;
					}
				TL2CAPSockAddr locAddr;
				listeningSAP->LocalName(locAddr);
				TPckg<TL2CAPPort> pckg(locAddr.Port());
				aOption = pckg;
				}
			else
				{
				rValue = KErrArgument;
				}
			}
			break;

		// Returns the negotiated channel mode.
		// KErrNotReady if the channel hasn't undergone configuration yet. 
		case KL2CAPNegotiatedChannelMode:
			if(aOption.Length() == sizeof(TL2CapChannelMode))
				{
				TL2CapChannelMode mode;
				rValue = iL2CapSAPSignalHandler->GetNegotiatedChannelMode(mode);
				TPckgBuf<TL2CapChannelMode> pckg(mode);
				aOption = pckg;
				}
			else
				{
				rValue = KErrArgument;
				}
			break;

#ifdef _DEBUG

		case KL2CAPVersion1_2:
			// This debug option is used to check if the stack is a 1.2 version.
			// For pre 1.2 versions the stack will respond with KErrNotSupported.
			break;
						
		case KL2CAPHeapAlloc:
			if(aOption.Length() == sizeof(TInt))
				{
				val = User::Heap().Count();
				aOption = TPtrC8(reinterpret_cast<TUint8*>(&val), sizeof(TInt));
				}
			else
				{
				rValue = KErrArgument;
				}
			break;
			
		case KL2CAPDataPlaneConfig:
			{	
			if(iL2CapDataQueue)
				{
				TL2DataPlaneConfig conf;
				iL2CapDataQueue->GetDataPlaneConfig(conf);
				TL2DataPlaneConfigPkg pckg(conf);
				aOption = pckg;
				}
			else
				{
				rValue = KErrNotReady;
				}
			}
			break;
		
#endif
			
		default:
			rValue = KErrNotSupported;
			break;
		};
	return rValue;	
	}


void CL2CAPConnectionSAP::Ioctl(TUint aLevel,TUint aName,TDes8* aOption)
/** 
	Handle an Ioctl from above. 
	
	Some Ioctls are not for L2CAP but must be handled here anyway.
	Other non-L2CAP Ioctls should just be passed downward.

	If the Ioctl is for L2CAP and should be handled the same in all states, 
	then handle it here, otherwise, pass it on to the current state.

	@param aLevel		Level of Ioctl (should be KSolBtL2CAP here)
	@param aName		Name of Ioctl
	@param aOption		Ioctl data
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(iOutstandingIoctlName == 0, Panic(EL2SAPTwoIoctls));

	// Must set this first, in case any synchronous errors come up. 	
	iOutstandingIoctlName = aName;
	TInt rerr = KErrNotSupported;

	if(aLevel == KSolBtL2CAP)
		{
		switch(aName)
			{
			case KL2CAPEchoRequestIoctl:
				rerr = iL2CapSAPSignalHandler->SendEchoRequest(aOption);
				break;

			case KL2CAPIncomingMTUIoctl:
				{
				const TUint8* optionPtr = aOption->Ptr();
				TUint16 newSize = *((TUint16*)optionPtr);
				TL2CapConfig conf;
				conf.SetMaxReceiveUnitSize(newSize);
				rerr = iL2CapSAPSignalHandler->UpdateChannelConfig(conf);
			  	if(rerr == KErrNone)
					{
					// The operation has completed.   
					IoctlComplete(KErrNone, KSolBtL2CAP, KL2CAPIncomingMTUIoctl, aOption);
					rerr = KErrNone;
					}
				else
					{
					if(rerr == KErrL2CAPConfigPending)
						{
						// This indicates that configuration is pending.
						rerr = KErrNone;
						}
					}
				}
				break;
				
			case KL2CAPOutgoingMTUIoctl:
				{
				const TUint8* optionPtr = aOption->Ptr();
				TUint16 newSize = *((TUint16*)optionPtr);
				TL2CapConfig conf;
				conf.SetMaxTransmitUnitSize(newSize);
				rerr = iL2CapSAPSignalHandler->UpdateChannelConfig(conf);
			  	if(rerr == KErrNone)
					{
					// The operation has completed.   
					IoctlComplete(KErrNone, KSolBtL2CAP, KL2CAPOutgoingMTUIoctl, aOption);
					rerr = KErrNone;
					}
				else
					{
					if(rerr == KErrL2CAPConfigPending)
						{
						// This indicates that configuration is pending.
						rerr = KErrNone;
						}
					}
				}
				break;

			case KL2CAPUpdateChannelConfigIoctl:
				{
				const TL2CapConfig* conf = reinterpret_cast<const TL2CapConfig*>(aOption->Ptr());
				rerr = UpdateChannelConfig(*conf);
			  	if(rerr == KErrNone)
					{
					// The operation has completed.   
					IoctlComplete(KErrNone, KSolBtL2CAP, KL2CAPUpdateChannelConfigIoctl, aOption);
					rerr = KErrNone;
					}
				else
					{
					if(rerr == KErrL2CAPConfigPending)
						{
						// This indicates that configuration is pending.
						rerr = KErrNone;
						}
					}
				}
				break;

			case KL2CAPPretendIncomingSduQFull:
				{
#ifdef _DEBUG
				if (aOption && aOption->Length() == sizeof(TBool))
					{
					iL2CapDataQueue->PretendIncomingSduQFull(*reinterpret_cast<const TBool*>(aOption->Ptr()));
					IoctlComplete(KErrNone, KSolBtL2CAP, KL2CAPPretendIncomingSduQFull, aOption);
					rerr = KErrNone;
					}
				else
					{
					rerr = KErrArgument;
					}
#endif
				}
				// Return not supported in UREL.
				break;

			default:
				// Return not supported.
				break;
			}
		}

	if (rerr != KErrNone)
		{
		iOutstandingIoctlName = 0;
		iSocket->Error(rerr, MSocketNotify::EErrorIoctl);
		}
	}

#ifdef _DEBUG
void CL2CAPConnectionSAP::CancelIoctl(TUint aLevel, TUint aName)
#else
void CL2CAPConnectionSAP::CancelIoctl(TUint /*aLevel*/, TUint /*aName*/)
#endif
	{
	LOG_FUNC
	__ASSERT_DEBUG((aLevel == KSolBtL2CAP && aName == iOutstandingIoctlName), Panic(EL2SAPStrayIoctlCancel));
	iOutstandingIoctlName = 0;
	}

void CL2CAPConnectionSAP::IoctlComplete(TInt aErr, TUint aLevel, TUint aName, TDesC8* aBuf)
	{
	LOG_FUNC
	if(aLevel == KSolBtL2CAP && aName == iOutstandingIoctlName)
		{
		if(aErr == KErrNone)
			{
			if(iSocket)
				{
				iSocket->IoctlComplete(aBuf);
				}
			}
		else
			{
			iSocket->Error(aErr, MSocketNotify::EErrorIoctl);
			}
		iOutstandingIoctlName = 0;
		}
	}

void CL2CAPConnectionSAP::EchoResponseReceived(const TDesC8* aData)
	{
	LOG_FUNC
	TDesC8* data = const_cast<TDesC8*>(aData);
	IoctlComplete(KErrNone, KSolBtL2CAP, KL2CAPEchoRequestIoctl, data);
	}


void CL2CAPConnectionSAP::Start()
	{
	LOG_FUNC
	// Called when data's about to flow

	iAcceptPending = EFalse;
	iState->Start(*this);
	}


void CL2CAPConnectionSAP::ActiveOpen()
	{
	LOG_FUNC
	// Setup the link

	iState->ActiveOpen(*this);
	}

void CL2CAPConnectionSAP::ActiveOpen(const TDesC8& /*aConnectionData*/)
	{
	LOG_FUNC
	// This is an error
	Panic(EL2CAPSAPOpenWithConnectionDataNotSupported);
	}

TInt CL2CAPConnectionSAP::PassiveOpen(TUint aQueSize)
	{
	LOG_FUNC
	// A Listen request
	// Now a listening SAP has been opened, start the underlying protocol listening.
	return iState->PassiveOpen(*this, aQueSize);
	}

TInt CL2CAPConnectionSAP::PassiveOpen(TUint /*aQueSize*/, const TDesC8& /*aConnectionData*/)
	{
	LOG_FUNC
	// This operation is not currently supported.
	Panic(EL2CAPSAPOpenWithConnectionDataNotSupported);
	return KErrNotSupported;
	}

void CL2CAPConnectionSAP::Shutdown(TCloseType aOption)
	{
	LOG_FUNC
	LOG2(_L("L2CAP [l2sap.cpp]: CL2CAPConnectionSAP::Shutdown, option = %d, SAP %08x"), aOption, this);
	// Shutdown
	if(aOption != EImmediate)
		{
		// Normal shutdown, ESOCK will wait for the SAP to signal Can Close.
		iShutdownReceived = ESAPShutdownNormal;
		iState->Shutdown(*this);
		}
	else
		{
		iShutdownReceived = ESAPShutdownImmediate;
		iState->FastShutdown(*this);
		}
	}

void CL2CAPConnectionSAP::Shutdown(TCloseType /*option*/, 
								   const TDesC8& /*aDisconnectionData*/)
	{
	LOG_FUNC
	// this one's an error
	Panic(EL2CAPShutdownWithDisconnectionDataNotSupported);
	}

void CL2CAPConnectionSAP::AutoBind()
/**
	Auto bind from esock.
	Do nothing -- as we always allocate a local CID
	in ActiveConnect anyway.
**/
	{
	LOG_FUNC
	}


TInt CL2CAPConnectionSAP::Write(RMBufChain& aData, TUint aOptions, TSockAddr* /*anAddr*/)
	{
	LOG_FUNC
	// If the write completes without error return 1.  
	// If not the definition [of the return parameter
	// of this method] in CServProviderBase indicates that zero
	// should be returned.
	TInt err = iState->Send(*this, aData, aOptions);
	// If there is an error then simply returning 0 will just "flow off"
	// esock (i.e. the write won't be completed).  We are reliant on
	// some other event to either error the send or disconnect in order
	// to complete the request.
	return (err == KErrNone ? 1 : 0);
	}

TInt CL2CAPConnectionSAP::GetData(RMBufChain& aData, TUint /*aLength*/, TUint /*aOptions*/, TSockAddr* /*anAddr*/)
	{
	LOG_FUNC
	return iState->Read(*this, aData);
	}

TUint CL2CAPConnectionSAP::Write(const TDesC8& aDesc, TUint aOptions, TSockAddr* /*anAddr*/)
	{
	LOG_FUNC
	// Temporary Shim.  Should be remove when no longer used.
	RMBufChain buf;
	TInt err = KErrNone;
	
	// A zero return value indicates that no data was sent.
	TUint rValue = 0;
	
	TRAP(err, buf.CreateL(aDesc));
	if(err == KErrNone)
		{
		rValue = Write(buf, aOptions);
		}

	buf.Free();
	return rValue;
	}

void CL2CAPConnectionSAP::GetData(TDes8& aDesc, TUint aOptions, TSockAddr* anAddr)
	{
	LOG_FUNC
	// Temporary Shim.  Should be remove when no longer used.
	RMBufChain buf;
	GetData(buf, 0, aOptions, anAddr);
	buf.CopyOut(aDesc);
	buf.Free();
	}
	

// Methods from SAP Signal Handler
void CL2CAPConnectionSAP::LinkUp()
	{
	LOG_FUNC
	iState->LinkUp(*this);
	}

void CL2CAPConnectionSAP::ChannelOpened()
	{
	LOG_FUNC
	iState->ChannelOpened(*this);
	}

void CL2CAPConnectionSAP::ChannelConfigured(CL2CapChannelConfig& aConfig,
                                            CL2CAPMux& aMuxer,
                                            TL2CAPPort aLocalPort,
                                            TL2CAPPort aRemotePort)
	{
	LOG_FUNC
	iState->ChannelConfigured(*this, aConfig, aMuxer, aLocalPort, aRemotePort);
	}

void CL2CAPConnectionSAP::ReconfiguringChannel()
	{
	LOG_FUNC
	iState->ReconfiguringChannel(*this);
	}
	
void CL2CAPConnectionSAP::ChannelClosed()
	{
	LOG_FUNC
	iState->ChannelClosed(*this);
	}

void CL2CAPConnectionSAP::DataPlaneError(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction)
	{
	LOG_FUNC
	iState->DataPlaneError(*this, aErrorCode, aErrorAction);
	}

void CL2CAPConnectionSAP::SignalHandlerError(TInt aErrorCode, MSocketNotify::TOperationBitmasks aErrorAction)
	{
	LOG_FUNC
	iState->SignalHandlerError(*this, aErrorCode, aErrorAction);
	}
	

// Methods from Data Controller
void CL2CAPConnectionSAP::NewData()
	{
	LOG_FUNC
	iState->NewData(*this);
	}
	
void CL2CAPConnectionSAP::CanSend()
	{
	LOG_FUNC
	iState->CanSend(*this);
	}

void CL2CAPConnectionSAP::CloseOutgoingSDUQueue()
	{
	LOG_FUNC
	iState->CloseOutgoingSDUQueue(*this);
	}
	
void CL2CAPConnectionSAP::SDUQueueClosed()
	{
	LOG_FUNC
	// Delete the SDU queue.
	delete iL2CapDataQueue;
	iL2CapDataQueue = NULL;
	
	// Inform the state that the data plane is closed.
	iState->SDUQueueClosed(*this);	
	}	

void CL2CAPConnectionSAP::SetState(TL2CAPSAPState& aState)
	{
	LOG_FUNC
	iState->Exit(*this);
	iState = &aState;
	iState->Enter(*this);
	}

TInt CL2CAPConnectionSAP::CreateDataPlane(CL2CapChannelConfig& aConfig, 
                                          CL2CAPMux& aMuxer,
                                          TL2CAPPort aLocalPort,
                                          TL2CAPPort aRemotePort)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	const TL2CapFecNegotiator& fec = aConfig.FecNegotiator();
	TL2CapDataControllerConfig* dataConfig = new TL2CapDataControllerConfig(fec.DataControllerConfig());

	if(dataConfig)
		{
		// if UpdateChannelConfig was called before iL2CapDataQueue was created we update the priority now
		if (iChannelPriority != 0)
			{
			dataConfig->SetChannelPriority(iChannelPriority);
			}
		TRAP(rerr, iL2CapDataQueue = CL2CapSDUQueue::NewL(*this,
		                                                  aLocalPort,
		                                                  aRemotePort,
		                                                  aMuxer,
					                       				  fec.OutgoingMaximumPDUSize(),
		                                                  CL2CapSDUQueue::KDefaultOutboundQueueSize,
		                       			            	  dataConfig,
											   			  aConfig.OutgoingFlushTimeout().Negotiated().FlushTimeoutDuration(),
					                       				  aConfig.OutgoingMTU().Negotiated().MTU(),
					                       				  aConfig.IncomingMTU().Negotiated().MTU(),
					                       				  // Allow ourselves to drop SDUs we can't assemble if channel mode is not Reliable.
					                       				  !TRetransmissionAndFlowControlOption::IsModeReliable(fec.IncomingLinkMode())));

		// Set the optimal PDU size that the data controller should use.
		TPckgBuf<TInt> buf;	
		TInt err = iL2CapSAPSignalHandler->GetOption(KSolBtACL, ELMOutboundACLSize, buf);

		TInt optimalPduSize = HL2CapPDU::GetPDUOrFragmentSize(iL2CapDataQueue->MaxOutgoingMTU(), iL2CapDataQueue->MaximumPDUSize(), (err == KErrNone) ? buf() : 0, iL2CapDataQueue->IsBasicDataVersion());
		iL2CapDataQueue->SetOptimalPDUSize(optimalPduSize);
		}
	else
		{
		rerr = KErrNoMemory;
		}

	return rerr;
	}
	

void CL2CAPConnectionSAP::PauseDataPlane()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iL2CapDataQueue, Panic(EL2CAPPauseDataPlaneCalledWhenDataPlaneDoesNotExist));
	iL2CapDataQueue->SuspendSDUQueue();				
	}

void CL2CAPConnectionSAP::TryToCompleteConfigurationIoctl(TInt aError)
	{
	LOG_FUNC
	switch(iOutstandingIoctlName)
		{
		case KL2CAPIncomingMTUIoctl:
		case KL2CAPOutgoingMTUIoctl:
		case KL2CAPUpdateChannelConfigIoctl:
			if(aError == KErrNone)
				{
				IoctlComplete(KErrNone, KSolBtL2CAP, iOutstandingIoctlName, NULL);
				}
			else
				{
				iSocket->Error(aError, MSocketNotify::EErrorIoctl);
				}
			iOutstandingIoctlName = 0;
			break;
			
		default:
			break;
		};
	}
	
void CL2CAPConnectionSAP::AccessRequestComplete(TInt aResult)
	{
	LOG_FUNC
	iState->AccessRequestComplete(*this, aResult);
	}

TInt CL2CAPConnectionSAP::UpdateChannelConfig(const TL2CapConfig& aAPIConfig)
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(iL2CapSAPSignalHandler)
		{
		// Update the channel config.
		rerr = iL2CapSAPSignalHandler->UpdateChannelConfig(aAPIConfig);

		// Check if the a new priority has been specified.
		TBool specified;
		TUint8 newPriority = static_cast<TUint8>(aAPIConfig.ChannelPriority(specified));
		if(specified && iChannelPriority != newPriority)
			{
			iChannelPriority = newPriority;
			if(iL2CapDataQueue)
				{
				iL2CapDataQueue->UpdateChannelPriority(iChannelPriority);
				}
			}
		}
	else
		{
		rerr = KErrNotReady;
		}
	return rerr;
	}
	
void CL2CAPConnectionSAP::DetachFromListeningSAP()
	{
	LOG_FUNC
	if(iListeningSAP)
		{
		iListeningSAP->DequeClone(this);
		iListeningSAP = NULL;
		}
	}
