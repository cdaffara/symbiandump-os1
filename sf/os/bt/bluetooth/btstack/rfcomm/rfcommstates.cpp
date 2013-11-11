// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <bt_sock.h>
#include "rfcommstates.h"
#include "rfcomm.h"
#include "rfcommmuxer.h"
#include "rfcommflow.h"
#include "rfcommconsts.h"
#include "AsyncErrorKicker.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_RFCOMM);
#endif

// Together, these classes and the SAP implement the State pattern
// (GOF).  The states themselves are implemented using the Flyweight
// pattern.  Each state is a Flyweight object, and CRfcommStateFactory
// is manager of these objects.  As a result of being a flyweight, no
// state object may have state that can't be shared between all
// possible users of the state (ie no per-SAP state)


CRfcommStateFactory* CRfcommStateFactory::NewL()
	/**
	   Create all the state singletons.
	**/
	{
	CRfcommStateFactory* factory= new (ELeave) CRfcommStateFactory();
	CleanupStack::PushL(factory);
	// Create all the new states
	factory->ConstructL();
	CleanupStack::Pop();
	return factory;
	}

void CRfcommStateFactory::ConstructL()
	{
	iStates[EError]  = new (ELeave) TRfcommStateError(*this);
	iStates[EClosed] = new (ELeave) TRfcommStateClosed(*this);
	iStates[EWaitForMux]= new (ELeave) TRfcommStateWaitForMux(*this);
	iStates[EWaitForPNResp]= new (ELeave) TRfcommStateWaitForPNResp(*this);
	iStates[EWaitForUA]= new (ELeave) TRfcommStateWaitForUA(*this);
	iStates[EOpen]      = new (ELeave) TRfcommStateOpen(*this);
	iStates[EDisconnect]      = new (ELeave) TRfcommStateDisconnect(*this);
	iStates[EListening] = new (ELeave) TRfcommStateListening(*this);
	iStates[EWaitForSABM] = new (ELeave) TRfcommStateWaitForSABM(*this);
	iStates[EWaitForIncomingSecurityCheck] = new (ELeave) TRfcommStateIncomingSecurityCheck(*this);
	iStates[EWaitForOutgoingSecurityCheck] = new (ELeave) TRfcommStateOutgoingSecurityCheck(*this);
	iStates[EWaitForStart] = new (ELeave) TRfcommStateWaitForStart(*this);
	iStates[ECloseOnStart] = new (ELeave) TRfcommStateCloseOnStart(*this);
	iStates[EDisconnecting] = new (ELeave) TRfcommStateDisconnecting(*this);
	// etc...
	}

TInt CRfcommStateFactory::StateIndex(const TRfcommState* aState) const
	{
	TInt state;
	for (state = 0; state < ERfcommMaxState; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KUnknownState;
	}

/*

TRfcommState base class implementation

*/

TInt TRfcommState::SignalError(CRfcommSAP& aSAP, TInt aErr, MSocketNotify::TOperationBitmasks aType)
	/**
	   Signal an error, with appropriate types to tell socket what's
	   been affected.
	**/
	{
	if (aSAP.iSocket)
		{
		aSAP.iSocket->Error(aErr, aType);
		}

	if(aType & MSocketNotify::EErrorFatal)
		{
		ChangeState(aSAP, CRfcommStateFactory::EError);
		}
	return aErr;
	}

// State change utility function

void TRfcommState::ChangeState(CRfcommSAP& aSAP, CRfcommStateFactory::TRfcommStates aNewState) const
	{
	aSAP.iState->Exit(aSAP);
#ifdef __FLOG_ACTIVE
	TRfcommState* state= iFactory.GetState(aNewState);
	LOG3(_L("RFCOMM : Sap %08x : State %S -> %S"),
				  &aSAP, &aSAP.iState->iName, &state->iName);
#endif //__FLOG_ACTIVE
	aSAP.iState=iFactory.GetState(aNewState);
	aSAP.iState->Enter(aSAP);
	}

void TRfcommState::PanicInState(TRFCOMMPanic aPanic) const
	{
	Panic(aPanic, iFactory.StateIndex(this));
	}

#ifdef _DEBUG
void TRfcommState::DebugPanicInState(TRFCOMMPanic aPanic) const
#else
void TRfcommState::DebugPanicInState(TRFCOMMPanic /*aPanic*/) const
#endif
	{
	#ifdef _DEBUG
	PanicInState(aPanic);
	#endif
	}

TRfcommState::TRfcommState(CRfcommStateFactory& aFactory)
	: iFactory(aFactory)
	{
	}

/************************************************************************/
/*
   The default state operations.
*/

TRfcommStateDefault::TRfcommStateDefault(CRfcommStateFactory& aFactory)
	: TRfcommState(aFactory)
	{
	}

void TRfcommStateDefault::Enter(CRfcommSAP& /*aSAP*/)
	{
	// Do nothing
	}

void TRfcommStateDefault::Exit(CRfcommSAP& /*aSAP*/)
	{
	// Do nothing
	}
	
void TRfcommStateDefault::ActiveOpen(CRfcommSAP& /* aSAP */ )
	{
	PanicInState(ERfcommUnexpectedEvent);
	}

TInt TRfcommStateDefault::PassiveOpen(CRfcommSAP& /* aSAP */,
									  TUint /*aQueSize*/)
	{
	PanicInState(ERfcommUnexpectedEvent);
	return KErrGeneral;
	}


void TRfcommStateDefault::Shutdown(CRfcommSAP& aSAP)
	{
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateDefault::FastShutdown(CRfcommSAP& aSAP)
	{
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent, MSocketNotify::EErrorAllOperations);
	}


TInt TRfcommStateDefault::Send(CRfcommSAP& /*aSAP*/, const TDesC8& /*aData*/ )
	{
	PanicInState(ERfcommUnexpectedEvent);
	return 0;
	}

void TRfcommStateDefault::Read(CRfcommSAP& /*aSAP*/, TDesC8& /*aData*/)
	{
	PanicInState(ERfcommUnexpectedEvent);
	}

void TRfcommStateDefault::Ioctl(CRfcommSAP& aSAP, TUint /*aLevel*/,
								TUint /*aName*/, TDes8* /*aOption*/)
	{
	SignalError(aSAP, KErrNotSupported, MSocketNotify::EErrorIoctl);
	}

void TRfcommStateDefault::CancelIoctl(CRfcommSAP& aSAP, TUint /*aLevel*/, TUint /*aName*/)
	/**
	   A request to cancel an Ioctl.
	   
	   Since the user has already been completed, and ESOCK prevents
	   the completion occuring twice, there's very little we need to
	   do (in fact we could ignore this entirely).
	   
	   Instead we will simply reset out IoctlLevel and IoctlName,
	   since this will now mean that we will not complete the ioctl
	   when the complete signal comes in.  If the ioctl is going on
	   below us, we will ignore the ioctl complete from them as well.
	**/
 	{
	aSAP.iIoctlLevel=0;
	aSAP.iIoctlName=0;
 	}

void TRfcommStateDefault::Start(CRfcommSAP& /*aSAP*/)
	{
	PanicInState(ERfcommUnexpectedEvent);
	}

TInt TRfcommStateDefault::SetOption(CRfcommSAP &aSAP, TUint aLevel, 
								   TUint aName, const TDesC8& aOption)
	/**
	   Set Option from user - works the same for SAPs in all states.
	**/
	{
	if(aLevel == KSolBtRFCOMM)
		{
		switch(aName)
			{
		case KRFCOMMLocalPortParameter:
			// Just do a straight copy into the local port params
			aSAP.iLocalPortParams = *(TRfcommRemotePortParams*)aOption.Ptr();
			break;
		case KRFCOMMMaximumSupportedMTU:
    		{
			if ( aOption.Length() != sizeof(TUint16) )
				{
				return KErrArgument;
				}
			else
				{
				TPckgBuf<TUint16> maxMTU;
				maxMTU.Copy(aOption);
				aSAP.iUserDefinedMTU = maxMTU();
				}
			};
			break;

		case KRFCOMMFlowTypeCBFC:
			{
			if ( aOption.Length() != sizeof(TBool) )
				{
				return KErrArgument;
				}
			else
				{
				TPckgBuf<TBool> aCBFC;
				aCBFC.Copy(aOption);
				if(!aCBFC())
					{
					//Switch off CBFC facility by setting flow strategy to
					//non-CBFC and setting the flow negotiated flag to 'ETrue'.
					if(aSAP.iMux)
						{
						if(!aSAP.iMux->SetFlowType(CRfcommFlowStrategyFactory::EFlowNonCreditBased))
							{
							return KErrNotSupported;
							}
						}
					else
						{
						aSAP.DisallowCBFC(); //will cause muxer to use muxwise CBFC when created
						}
					}
				else
					{
					//Switch on CBFC facility by setting flow strategy to
					//CBFC and setting the flow negotiated flag to 'ETrue'.
					if(aSAP.iMux)
						{
						if(!aSAP.iMux->SetFlowType(CRfcommFlowStrategyFactory::EFlowCreditBased))
							{
							return KErrNotSupported;
							}
						}
					else
						{
						aSAP.AllowCBFC(); //will cause muxer to use muxwise CBFC when created
						}
					}
				}
			}
			break;
		case KRFCOMMErrOnMSC:
			{
			if ( aOption.Length() != sizeof(TUint8) )
				{
				return KErrArgument;
				}
			else
				{
				TPckgBuf<TUint8> aSignal;
				aSignal.Copy(aOption);
				aSAP.iDisconnectMSCSignal = aSignal();
				}
			}
			break;
		case KBTSecurityDeviceOverride:
			{
			// let the security object handle this
			return aSAP.SetDeviceOverride(aOption); // just pass the option
			}
		case KBTRegisterCodService:
			{
			TInt err = KErrNone;
	        TUint16 newServiceBits = *reinterpret_cast<const TUint16*>(aOption.Ptr());
			err = aSAP.SetCodServiceBits(newServiceBits);	// The service bits are saved and then registered when SAP becomes live
			return err;
			}

		case KRFCOMMForgiveCBFCOverflow:
    		{
			if ( aOption.Length() != sizeof(TBool) )
				{
				return KErrArgument;
				}
			else
				{
				aSAP.iForgiveCBFCOverflow = *reinterpret_cast<const TBool*>(aOption.Ptr());
				}
			};
			break;

		default:
			return KErrNotSupported;
			}
		return KErrNone;
		}

	// Not a RFCOMM setopt so error.  We don't support passdown (yet).
	return KErrNotSupported;	
	}
	
void TRfcommStateDefault::MuxUp(CRfcommSAP& aSAP)
	{
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent,
				MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateDefault::LinkDown(CRfcommSAP& aSAP)
	{
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent,
				MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateDefault::SABM(CRfcommSAP& /*aSAP*/, CRfcommMuxer& aMux, TUint8 aDLCI)
	/**
	   Send a DM, since we are not expecting this
	
	**/
	{
	aMux.SendDM(aDLCI);
	}

void TRfcommStateDefault::DISC(CRfcommSAP& aSAP)
	/**
	   Send a DM as we were not expecting this
	**/
	{
	aSAP.iMux->SendDM(aSAP.iDLCI);
	}

void TRfcommStateDefault::UA(CRfcommSAP& /*aSAP*/)
	/**
	   Do nothing - unexpected response
	**/
	{
	}

void TRfcommStateDefault::DM(CRfcommSAP& /*aSAP*/)
	/**
	   Do nothing - unexpected response
	**/
	{
	}
	
void TRfcommStateDefault::RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& /*aRPNTransaction*/, CRfcommMuxer& /*aMuxer*/, TUint8 /*aDLCI*/)
	/**
	   Send a DM as we didn't expect this
	**/
 	{
	aSAP.iMux->SendDM(aSAP.iDLCI);
 	}
 
void TRfcommStateDefault::RPNRsp(CRfcommSAP& /*aSAP*/, 
								const TRfcommRPNTransaction& 
								/*aRPNTransaction*/)
	/**
	   Do nothing - unwanted response
	**/
 	{
 	}


void TRfcommStateDefault::PN(CRfcommSAP& aSAP, TRfcommPortParams& /*aParams*/, CRfcommMuxer& /*aMux*/, TUint8 aDLCI)
	/**
	   Send a DM as we aren't handling this.
	**/
	{
	__ASSERT_DEBUG(aDLCI==aSAP.iDLCI, PanicInState(ERfcommPNMismatchedDLCI));
	aSAP.iMux->SendDM(aDLCI);
	}

void TRfcommStateDefault::PNResp(CRfcommSAP& /*aSAP*/, TRfcommPortParams& /*aParams*/)
	/**
	   Do nothing - unwanted response
	**/
	{
	}

void TRfcommStateDefault::MSC(CRfcommSAP& /*aSAP*/, TUint8 /*aSignals*/)
	/**
	   Do nothing - the response is autogenerated
	**/
	{
	}

void TRfcommStateDefault::RLS(CRfcommSAP& /*aSAP*/, TUint8 /*aStatus*/)
	/**
	   Do nothing - the response is autogenerated
	**/
 	{
 	}
	
void TRfcommStateDefault::NewData(CRfcommSAP& /*aSAP*/, const TDesC8& /*aData*/)
	/**
	   Do nothing - we're throwing this data away
	**/
	{
	}

void TRfcommStateDefault::NotifyNewDataCallback(CRfcommSAP& aSAP)
	{
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateDefault::CanSend(CRfcommSAP& aSAP)
	{
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateDefault::IoctlComplete(CRfcommSAP& /*aSAP*/, TInt /*aErr*/, TUint /*aLevel*/, TUint /*aName*/, TDesC8* /*aBuf*/)
	{
	// Do nothing, this isn't for us
	}

void TRfcommStateDefault::AccessRequestComplete(CRfcommSAP& /*aSAP*/, TInt /*aResult*/)
	{
	PanicInState(ERfcommUnexpectedSecurityRequesterCallback);
	}

void TRfcommStateDefault::Error(CRfcommSAP& aSAP, TInt aErr,
								CRfcommSAP::TErrorTypes /*aType*/)
	{
	SignalError(aSAP, aErr,
				MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateDefault::ParentClosed(CRfcommSAP& aSAP)
	/**
	Our parent SAP has been deleted - NULL our pointer to it.

	This function only really applies to cloned child SAPs, but we
	handle it here anyway.
	**/
	{
	aSAP.iParentSAP=NULL;
	}

TBool TRfcommStateDefault::HandleFrameResponseTimeout(CRfcommSAP& /*aSAP*/)
	{
	//	By default, we refuse to handle the frame timeout, indicating this by
	//	returning EFalse.
	return EFalse;
	}

//TRY_CBFC
TUint8 TRfcommStateDefault::FreeCredit(CRfcommSAP& /*aSAP*/)
	{
	// return the max initial credit
	return (TUint8)KInitialCredit;
	}

TInt TRfcommStateDefault::ProxyForRemoteCredit(const CRfcommSAP& /*aSAP*/) const
	{
	// return the max initial credit
	return (TInt)KInitialCredit;
	}

void TRfcommStateDefault::SetProxyForRemoteCredit(CRfcommSAP& aSAP, TInt aCredit)
	{
	aSAP.iProxyForRemoteCredit = aCredit;
	LOG1(_L("RFCOMM: Proxy for credits available on REMOTE (...NEW SETUP...) -> %d"), aSAP.iProxyForRemoteCredit);
	}

void TRfcommStateDefault::ProxyForRemoteCreditDecrement(CRfcommSAP& aSAP)
	{
	__ASSERT_DEBUG(aSAP.iProxyForRemoteCredit > 0,PanicInState(ERfcommCBFCProxyForRemoteCreditGoingNegative));

	aSAP.iProxyForRemoteCredit--;
	LOG(_L("RFCOMM: Proxies for CREDITS on REMOTE:-"));
	LOG2(_L("RFCOMM:     Available for use  (...DECREMENT...) %d -> %d: -1"), 
				  aSAP.iProxyForRemoteCredit+1,
				  aSAP.iProxyForRemoteCredit);
	#ifdef _DEBUG
	aSAP.iProxyForRemoteCreditsUsed++;
	LOG1(_L("RFCOMM:     Total USED so far during this session: %d"), 
				  aSAP.iProxyForRemoteCreditsUsed);
	#endif
	return;
	}

void TRfcommStateDefault::ProxyForRemoteCreditAddCredit(CRfcommSAP& aSAP, TUint8 aCredit)
	{
	aSAP.iProxyForRemoteCredit += aCredit;
	LOG(_L("RFCOMM: Proxies for CREDITS on REMOTE:-"));
	LOG3(_L("RFCOMM:     Available for use  (...ADD...) %d -> %d: +%d"), 
				  aSAP.iProxyForRemoteCredit-aCredit,
				  aSAP.iProxyForRemoteCredit,
				  aCredit);
	#ifdef _DEBUG
	aSAP.iProxyForRemoteCreditsSupplied += aCredit;
	LOG1(_L("RFCOMM:     Total SUPPLIED so far during this session: %d"), 
				  aSAP.iProxyForRemoteCreditsSupplied);
	#endif
	}


TInt TRfcommStateDefault::LocalCredit(const CRfcommSAP& /*aSAP*/) const
	{
	// return a zero credit - so cannot send
	return (TInt)0;
	}

void TRfcommStateDefault::SetInitialLocalCredit(CRfcommSAP& aSAP, TInt aCredit)
	{
	aSAP.iLocalCredit = aCredit;
	LOG1(_L("RFCOMM: LOCAL credits available (...NEW SETUP...) -> %d"), aSAP.iLocalCredit);
	}

void TRfcommStateDefault::LocalCreditDecrement(CRfcommSAP& aSAP)
	{
	__ASSERT_DEBUG(aSAP.iLocalCredit > 0,PanicInState(ERfcommCBFCLocalCreditGoingNegative));
	
	aSAP.iLocalCredit--;
	LOG(_L("RFCOMM: LOCAL CREDITS:-"));
	LOG2(_L("RFCOMM:     Available for use (...DECREMENT...) %d -> %d: -1"), 
				  aSAP.iLocalCredit+1,
				  aSAP.iLocalCredit);
	#ifdef _DEBUG
	aSAP.iLocalCreditsUsed++;
	LOG1(_L("RFCOMM:     Total USED so far during this session: %d"), 
				  aSAP.iLocalCreditsUsed);
	#endif
	return;
	}

void TRfcommStateDefault::LocalCreditAddCredit(CRfcommSAP& aSAP, TUint8 aCredit)
	{
	aSAP.iLocalCredit += aCredit;
	LOG(_L("RFCOMM: LOCAL CREDITS:-"));
	LOG3(_L("RFCOMM:     Available for use (...ADD...) %d -> %d: +%d"), 
				  aSAP.iLocalCredit-aCredit,
				  aSAP.iLocalCredit,
				  aCredit);
	#ifdef _DEBUG
	aSAP.iLocalCreditsSupplied += aCredit;
	LOG1(_L("RFCOMM:     Total SUPPLIED so far during this session: %d"), 
				  aSAP.iLocalCreditsSupplied);
	#endif
	}


/*************************************************************************/

//  TRfcommStateError

/*************************************************************************/

TRfcommStateError::TRfcommStateError(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	STATENAME("Error");
	}

void TRfcommStateError::ActiveOpen(CRfcommSAP& aSAP)
	{
	SignalError(aSAP, KErrGeneral, MSocketNotify::EErrorAllOperations);
	}

TInt TRfcommStateError::PassiveOpen(CRfcommSAP& aSAP, TUint /*aQueSize*/)
	{
	return SignalError(aSAP,KErrGeneral, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateError::Shutdown(CRfcommSAP& aSAP)
	{
	SignalError(aSAP,KErrGeneral, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateError::FastShutdown(CRfcommSAP& aSAP)
	{
	SignalError(aSAP,KErrGeneral, MSocketNotify::EErrorAllOperations);
	}

TInt TRfcommStateError::Send(CRfcommSAP& aSAP, const TDesC8&)
	{
	SignalError(aSAP,KErrGeneral, MSocketNotify::EErrorAllOperations);
	return 0;
	}

void TRfcommStateError::Read(CRfcommSAP& aSAP, TDesC8&)
	{
	SignalError(aSAP,KErrGeneral, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateError::Ioctl(CRfcommSAP& aSAP, TUint, TUint, TDes8*)
	{
	SignalError(aSAP,KErrGeneral, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateError::CancelIoctl(CRfcommSAP& /*aSAP*/, TUint, TUint)
	/**
	   Ignore the cancel.
	   
	   This is because cancelling in the error state may even be
	   valid, and it doesn't hurt to ignore cancels.
	**/
	{
	}

void TRfcommStateError::Start(CRfcommSAP& aSAP)
	{
	SignalError(aSAP,KErrGeneral, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateError::Enter(CRfcommSAP& aSAP)
	/**
	   Take this SAP off the mux, since once it's in error state we
	   don't want to get any more notifications from the mux
	**/
	{
	// Depending where we came from we may have applied either a
	// park override or an all LPM override.  We remove both these
	// overrides because it's safe to remove an override that 
	// hasn't been applied.
	// Note that although park is one of the LPMs the different
	// levels of override are managed separately, so an
	// UndoOverrideLPM does not remove an explicit park only 
	// override.
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EUndoOverridePark, aSAP.iRemoteDev);
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EUndoOverrideLPM, aSAP.iRemoteDev);

	aSAP.DeregisterCodService();	// See if there is a Service to remove for CodMan
	
	if(aSAP.iMux)
		{
		aSAP.iMux->DetachSAP(aSAP);
		}
	}
	  

/*************************************************************************/

// Implementation of TRfcommStateClosed
//
// This is the intial state for a SAP and represents it before Connect() has
// been called, and it is also the end state when closing down

TRfcommStateClosed::TRfcommStateClosed(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	STATENAME("Closed");
	}

void TRfcommStateClosed::Enter(CRfcommSAP& aSAP)
	/**
	   Entering the closed state.

	   We need to remove ourselves from the muxer (if we have one)
	**/
	{
	aSAP.iClosePending=EFalse;
	if(aSAP.iNotifyNewDataCallback->IsActive())
		{
		aSAP.iNotifyNewDataCallback->Cancel();
		}
	aSAP.iNewDataToNotify=0;
	aSAP.iDataBuffer.Reset();
	
	// Depending where we came from we may have applied either a
	// park override or an all LPM override.  We remove both these
	// overrides because it's safe to remove an override that 
	// hasn't been applied.
	// Note that although park is one of the LPMs the different
	// levels of override are managed separately, so an
	// UndoOverrideLPM does not remove an explicit park only 
	// override.
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EUndoOverridePark, aSAP.iRemoteDev);
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EUndoOverrideLPM, aSAP.iRemoteDev);

	if(aSAP.iMux)
		{
		aSAP.iMux->DetachSAP(aSAP);
		}
	aSAP.DeregisterCodService();	// See if there is a Service to remove for CodMan
	}

void TRfcommStateClosed::Error(CRfcommSAP& /*aSAP*/, TInt /*aErr*/, 
							   CRfcommSAP::TErrorTypes /*aType*/)
	/**
	   Panic, we'd never get here since state closed never has a mux associated.
	**/
	{
	DebugPanicInState(ERfcommUnexpectedEvent);
	}

void TRfcommStateClosed::ActiveOpen(CRfcommSAP& aSAP)
	/**
	   Called by Socket to cause a connection to be established.
	   
	   This causes a sequence of actions which ends with
	   ConnectComplete() being called on the socket notifier.  Since a
	   SAP in the closed state may not be fully initialised, all the
	   parameters must be checked before starting the connect.
	**/	   
	{
	LOG1(_L("RFCOMM: ActiveOpen while state closed, sap %08x"), &aSAP);
	if(!aSAP.ServerChannelValid() )
		{
		LOG(_L("SAP DLCI error"));
		SignalError(aSAP, KErrRfcommBadAddress, MSocketNotify::EErrorConnect);
		return;
		}

	aSAP.RegisterCodService();	// See if there is a Service set for CodMan
	
	// Override LPM while we do some sigalling to ensure it can complete in 
	// a timely fashion.
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EOverrideLPM, aSAP.iRemoteDev);

	// First get a Mux.
	ChangeState(aSAP, CRfcommStateFactory::EWaitForMux);
	}

TInt TRfcommStateClosed::PassiveOpen(CRfcommSAP& aSAP, 
									 TUint aQueSize)
	/**
	We've just asked to turn into a listening SAP.

	NB. We need to check with the protocol that no other listening SAPs
	are currently listening on our specified server channel.
	We should also return an error if the protocol has no listener
	**/
	{
	//	First, check that the protocol is listening

	if(aSAP.iProtocol.IncrementListeners() != KErrNone)	// get protocol to attempt to listen
		return KErrRfcommNotListening;

	//	Second, the check for other listening SAPs on our specified local address.

	TBTSockAddr listeningName;
	aSAP.LocalName(listeningName);
	CRfcommSAP* listener=aSAP.iProtocol.FindIdleSAP(listeningName);

	__ASSERT_DEBUG((listener==NULL), PanicInState(ERfcommSAPAllocatedBetweenBindAndListen));

	if(listener!=NULL)
		return KErrInUse;
	
	TBTSockAddr remoteAddr;
	remoteAddr.SetBTAddr(aSAP.iRemoteDev);
	remoteAddr.SetPort(aSAP.iServerChannel);
	if(aSAP.iProtocol.FindIdleSAP(remoteAddr))
		return KErrAlreadyExists;	//	RFCOMM already has a listener which would field this address...

	aSAP.iMaxClonesWaitingForStart=aQueSize;
	aSAP.RegisterCodService();	// See if there is a Service set for CodMan
	ChangeState(aSAP, CRfcommStateFactory::EListening);
	return KErrNone;
	}

void TRfcommStateClosed::Start(CRfcommSAP& /*aSAP*/)
	{
	//	Do nothing - this is quite a valid thing to happen, but the base class panics!
	}

void TRfcommStateClosed::Shutdown(CRfcommSAP& aSAP)
	/**
	   The ESOCK would like us to close down.  We can.
	**/
	{
	aSAP.iSocket->CanClose();
	}

TInt TRfcommStateClosed::SetOption(CRfcommSAP &aSAP, TUint aLevel, 
								   TUint aName, const TDesC8& aOption)
    /**
       Allow modem status to be modified whilst we are closed.
    **/
    {
	if(aLevel == KSolBtRFCOMM && aName == KRFCOMMLocalModemStatus)
        {
		if ( aOption.Length() != sizeof(TUint8) )
			{
			return KErrArgument;
			}
		else
			{
			TPckgBuf<TUint8> signals;
			signals.Copy(aOption);
			aSAP.iSignals = signals();
			return KErrNone;
			}
        }

    return TRfcommStateDefault::SetOption(aSAP, aLevel, aName, aOption);
    }

/*************************************************************************/

// Implementation of TRfcommStateConnecting

TRfcommStateConnecting::TRfcommStateConnecting(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	}


void TRfcommStateConnecting::Shutdown(CRfcommSAP&  aSAP)
/**
	We've been asked to do a controlled shutdown during connection.
	Record the fact that we want to send a disconnect command upon
	reception of the UA command.
**/	
	{
	aSAP.iClosePending=ETrue;
	}

void TRfcommStateConnecting::FastShutdown(CRfcommSAP&  aSAP)
/**
	We've been asked to shutdown with extreme prejudice.
	Move back to the Closed state - there is nothing we can do
	to make this clean
**/
	{
	ChangeState(aSAP, CRfcommStateFactory::EClosed);
	};


void TRfcommStateConnecting::DM(CRfcommSAP& aSAP)
/**
	We've received a DM (Disconnect Mode) response.
	If we've been asked to close down anyway, tell the SAP that we can be closed.
	If we haven't, signal an error to the socket.
**/
	{
	FailureWhileConnecting(aSAP, KErrCouldNotConnect);	
	}

void TRfcommStateConnecting::FailureWhileConnecting(CRfcommSAP& aSAP, TInt aErrorCode)
/**
	Send appropriate notification of failure back to SAP socket if failure occurs while connecting.
**/
	{
	if(!aSAP.iClosePending)
		{
		ChangeState(aSAP, CRfcommStateFactory::EClosed);
		SignalError(aSAP, aErrorCode, MSocketNotify::EErrorConnect);
		}
	else
		{
		aSAP.iSocket->CanClose();  // Will delete us
		}
	}

void TRfcommStateConnecting::LinkDown(CRfcommSAP& aSAP)
	/**
	   The link has gone down before our connection came up.

	   This can even (counter-intuitively) occur in WaitForMux because
	   there is a lag between the l2cap link coming up and the mux
	   channel itself coming up.  In the interval, the l2cap link can
	   drop again and thus we get a link down notification (the mux
	   will not attempt to reconnect).

	   Handle this by simply erroring the connection attempt.
	**/
	{
	// tell secman not to bother
	aSAP.CancelAccessRequest();

	FailureWhileConnecting(aSAP, KErrDisconnected);
	}


/*******************************************************************/
/*
  Implementation of WaitForMux state.
  When a mux has been found for this SAP, then MuxUp is called.
*/

TRfcommStateWaitForMux::TRfcommStateWaitForMux(CRfcommStateFactory& aFactory)
	: TRfcommStateConnecting(aFactory)
	{
	STATENAME("WaitForMux");
	}

void TRfcommStateWaitForMux::Enter(CRfcommSAP& aSAP)
	/**
	   Entry to the state.

	   Ask the protocol to find a mux for us. We can then
	   add our context to it.
	**/
	{
	CRfcommMuxer* mux=NULL;
	TBTSockAddr remoteAddr;
	aSAP.RemName(remoteAddr);
	TInt error=aSAP.iProtocol.GetMux(remoteAddr,mux);
	if(error)
		Error(aSAP, error, CRfcommSAP::EErrorGeneral);
	else
		{
		//	Calculate the DLCI that this SAP is trying to connect to
		aSAP.iDLCI=mux->MakeOutboundDLCI(TUint8(remoteAddr.Port()));
		//	Ensure that we don't already have a connection for the DLCI
		//	we're trying to connect to on this device...
		if(mux->FindSAP(aSAP.DLCI())!=NULL)
			{
			//	Problems! We've already got a SAP attached to this DLCI
			//	for this same device...we need to error this SAP.
			LOG1(_L("RFCOMM: Attempt to connect to connected SAP, DLCI %d"), aSAP.DLCI());
			Error(aSAP, KErrAlreadyExists, CRfcommSAP::EErrorGeneral);
			}
		else
			{
			// Transfer the sap to the mux
			if(aSAP.iMux)
				aSAP.iLink.Deque();
			aSAP.iMux=mux;

			//	The next line will call back into MuxUp, either synchronously 
			//	(when mux channel is already up) or asynchronously (when the
			//	mux channel needs to be opened).
			mux->AddSAP(aSAP);  
			}
		}
	}
	
void TRfcommStateWaitForMux::MuxUp(CRfcommSAP& aSAP)
	/**
	   A muxer has been found for this SAP.
	   On entry, the sap has had a muxer associated with it.
	   Change state to connecting, so it can do any config and the SABM.
	**/
	{
	ChangeState(aSAP, CRfcommStateFactory::EWaitForPNResp); // Enter the new state
	}

void TRfcommStateWaitForMux::Shutdown(CRfcommSAP& aSAP)
	/**
	   We've been asked to shutdown while connecting.

	   Since the mux isn't up yet, we must detach from the mux and
	   also signal that we can be deleted.
	**/
	{
	ChangeState(aSAP, CRfcommStateFactory::EClosed);// Enter this state, which detaches us
	aSAP.iSocket->CanClose();  // This deletes us
	}

void TRfcommStateWaitForMux::SABM(CRfcommSAP& aSAP, CRfcommMuxer& /*aMux*/, TUint8 /*aDLCI*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent, MSocketNotify::EErrorAllOperations);
	}
	
void TRfcommStateWaitForMux::UA(CRfcommSAP& /*aSAP*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	/*ignore and hope for the best*/
	}

void TRfcommStateWaitForMux::DISC(CRfcommSAP& aSAP)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateWaitForMux::DM(CRfcommSAP& /*aSAP*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	/*ignore and hope for the best*/
	}

void TRfcommStateWaitForMux::RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& /*aRPNTransaction*/, CRfcommMuxer& /*aMux*/, TUint8 /*aDLCI*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateWaitForMux::RPNRsp(CRfcommSAP& /*aSAP*/, const TRfcommRPNTransaction& /*aRPNTransaction*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	/*ignore and hope for the best*/
	}

// Parameter negotiation
void TRfcommStateWaitForMux::PN(CRfcommSAP& aSAP, TRfcommPortParams& /*aParams*/, CRfcommMuxer& /*aMux*/, TUint8 /*aDLCI*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	SignalError(aSAP, KErrRfcommSAPUnexpectedEvent, MSocketNotify::EErrorAllOperations);
	}
  
void TRfcommStateWaitForMux::PNResp(CRfcommSAP& /*aSAP*/, TRfcommPortParams& /*aParams*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	/*ignore and hope for the best*/
	}

void TRfcommStateWaitForMux::MSC(CRfcommSAP& /*aSAP*/, TUint8 /*aSignals*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	/*ignore and hope for the best*/
	}

void TRfcommStateWaitForMux::RLS(CRfcommSAP& /*aSAP*/, TUint8 /*aStatus*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	/*ignore and hope for the best*/
	}

void TRfcommStateWaitForMux::NewData(CRfcommSAP& /*aSAP*/, const TDesC8& /*aData*/)
	{
	DebugPanicInState(ERfcommFrameBeforeMuxUp);
	/*ignore and hope for the best*/
	}

/*****************************************************************

State Wait for PN Response

******************************************************************/

TRfcommStateWaitForPNResp::TRfcommStateWaitForPNResp(CRfcommStateFactory& aFactory)
	: TRfcommStateConnecting(aFactory)
	{
	STATENAME("WaitForPNResp");
	}

void TRfcommStateWaitForPNResp::Enter(CRfcommSAP& aSAP)
	/**
	The Mux is up - it's time to initiate Parameter Negotiations

	Send a PN packet with our preferred parameters.
	**/
	{
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	TRfcommPortParams params;
	params.iMaxFrameSize=aSAP.MaximumMTU();
	params.iInitialCredit = aSAP.iMux->FlowStrategy()->PNFinalOctet();

	aSAP.SetProxyForRemoteCredit(params.iInitialCredit); // set up our proxy
#ifdef _DEBUG
	aSAP.iProxyForRemoteCreditsSupplied = params.iInitialCredit;
#endif
	aSAP.iNegotiatedMTU=params.iMaxFrameSize;  // Remember the newly calculated MTU
	aSAP.iOptimalMTUForSending=params.iMaxFrameSize;  // Remember the newly calculated MTU

	LOG2(_L("RFCOMM: Sending PN (Calculated MTU %d from frame size %d)"), aSAP.iNegotiatedMTU, aSAP.iMux->GetMaxDataSize());	
	
	TInt error=aSAP.iMux->SendPN(aSAP, params);

	if(error!=KErrNone)
		{
		//	Error occurred while sending parameter negotiation command
		FailureWhileConnecting(aSAP, error);
		}
	}

void TRfcommStateWaitForPNResp::PNResp(CRfcommSAP& aSAP, TRfcommPortParams& aParams)
	/**
	We've received a PN response, which may or may not mean our
	datasize has been accepted

	If everything is OK (ie. the PN response accepts a MTU size
	between 1 and our original maximum MTU), send a SABM and begin
	waiting for a UA.  If things are not OK, we either signal an error
	or inform the socket that we CanClose (depending on whether we
	were trying to disconnect anyway).

	**/
	{
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	if(aParams.iCreditIndicator == (KCBFCResponseFlag >> 4))
		{
		aSAP.SetInitialLocalCredit(aParams.iInitialCredit);
		#ifdef _DEBUG
		aSAP.iLocalCreditsSupplied = aParams.iInitialCredit;
		#endif
		}
	TInt error=KErrNone;
	if(aParams.iMaxFrameSize<=aSAP.NegotiatedMTU() && aParams.iMaxFrameSize>0)
		{
		LOG1(_L("RFCOMM: Received acceptable PN Response with MTU %d"),aParams.iMaxFrameSize);
		
		// We have an MTU value so ask L2Cap to work out the optimal MTU value based on the one
		// received from the remote. Note that the Max Frame Size is the max data size for Rfcomm
		// so we need to add on the max Rfcomm header size to create the restriction on L2Cap
		// MTU.
		TPckgBuf<TInt> restrictedMtu(aParams.iMaxFrameSize + KMaxFrameOverhead);
		error = aSAP.GetOption(KSolBtL2CAP, KL2CAPOutboundMTUForBestPerformanceWithRestriction, restrictedMtu);

		// If we get an error continue but with a possible non-optimal MTU value. iMTU is the max
		// data size so remove the max Rfcomm header size as these will get added later.
 		aSAP.iOptimalMTUForSending = (error == KErrNone) ? restrictedMtu() - KMaxFrameOverhead : aParams.iMaxFrameSize;
 		aSAP.iNegotiatedMTU = aParams.iMaxFrameSize;
 		error = KErrNone;
 		
		}
	else
		{
		LOG1(_L("RFCOMM: Received unacceptable PN Response with MTU %d"),aParams.iMaxFrameSize);
		error=KErrRfcommParameterNegotiationFailure;
		};

	if(error!=KErrNone)
		{
		FailureWhileConnecting(aSAP,error);
		}
	else
		{
		ChangeState(aSAP, CRfcommStateFactory::EWaitForOutgoingSecurityCheck);
		}
	}

/*********************************************************************/
/*
	We've received a good PN configuration response and we're waiting for the connection to complete.

*/

TRfcommStateWaitForUA::TRfcommStateWaitForUA(CRfcommStateFactory& aFactory)
	: TRfcommStateConnecting(aFactory)
	{
	STATENAME("WaitForUA");
	}

void TRfcommStateWaitForUA::UA(CRfcommSAP& aSAP)
	/**
	   The SABM command has succeeded.

	   If there is no close pending we go to open, and
	   also construct the SAPs buffers.  If we need to close, we go to
	   the disconnect state.
	**/
	{
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	if(aSAP.iClosePending)
		{
		ChangeState(aSAP, CRfcommStateFactory::EDisconnect);
		}
	else
		{
		TRAPD(err, 
			  aSAP.iDataBuffer.SetLengthL(aSAP.iMux->FlowStrategy()->DataBufferMultiple()*
										  aSAP.iNegotiatedMTU));
		if(err != KErrNone)
			{
			// Failed to alloc, so fail this connect and error
			SignalError(aSAP, err, MSocketNotify::EErrorConnect);
			ChangeState(aSAP, CRfcommStateFactory::EClosed);
			return;
			}
		
		aSAP.iHighTideMark=KRfcommSAPBufferHighMultiple*(aSAP.iNegotiatedMTU);
		aSAP.iLowTideMark=KRfcommSAPBufferLowMultiple*(aSAP.iNegotiatedMTU);

		
		ChangeState(aSAP, CRfcommStateFactory::EOpen);
		}
	}

TBool TRfcommStateWaitForUA::HandleFrameResponseTimeout(CRfcommSAP& aSAP)
	/**
	We've waited too long for a UA to come back to us, so we start disconnecting.

	We also need to signal 
	**/
	{
	if(!aSAP.iClosePending)
		SignalError(aSAP, KErrTimedOut, MSocketNotify::EErrorConnect);
	
	ChangeState(aSAP, CRfcommStateFactory::EDisconnect);
	return ETrue;
	}

/******************************************************************/
/*
  The Listening state.

  Deals with incoming connection requests
*/

TRfcommStateListening::TRfcommStateListening(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	STATENAME("Listening");
	}

void TRfcommStateListening::Enter(CRfcommSAP& aSAP)
	/**
	The SAP is entering the listening state. The protocol needs to know.
	**/
	{
	aSAP.iProtocol.AddIdleSAP(aSAP);
	aSAP.iProtocol.RemoveBoundSAP(aSAP);
	}

void TRfcommStateListening::Exit(CRfcommSAP& aSAP)
	{
	aSAP.DeregisterCodService();	// See if there is a Service to remove for CodMan
	aSAP.StopListening();
	}

void TRfcommStateListening::SABM(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI)
	/**
	A SABM sent to a listening SAP means we need create a clone SAP
	**/
	{
	//	NB. The criteria of the following assert will vary for the different states
	//	that this function gets called in.
	__ASSERT_DEBUG(aDLCI!=0, PanicInState(ERfcommBadDLCIForClone));
	
	CRfcommSAP* newSAP=AttemptToClone(aSAP, aMux, aDLCI);
	if(newSAP) 
		newSAP->SABM(aMux, aDLCI);
	}

void TRfcommStateListening::PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, CRfcommMuxer& aMux, TUint8 aDLCI)
	/**
	A PN sent to a listening SAP means we need create a clone SAP
	**/
	{
	//	NB. The criteria of the following assert will vary for the different states
	//	that this function gets called in.
	__ASSERT_DEBUG(aDLCI!=0, PanicInState(ERfcommBadDLCIForClone));
	
	CRfcommSAP* newSAP=AttemptToClone(aSAP, aMux, aDLCI);
	if(newSAP)
		{
		newSAP->PN(aParams, aMux, aDLCI);
		}
	else
		{
		// Uh Oh! Couldn't get a SAP, don't worry too much - a DM has been sent
		LOG(_L("[rfcommstates.cpp]: Couldn't clone a SAP after receiving a PN\n"));
		}
	}

void TRfcommStateListening::RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, CRfcommMuxer& aMux, TUint8 aDLCI)
	/**
	A RPN sent to a listening SAP means we need create a clone SAP
	**/
	{
	__ASSERT_DEBUG(aDLCI!=0, PanicInState(ERfcommBadDLCIForClone));
	
	CRfcommSAP* newSAP=AttemptToClone(aSAP, aMux, aDLCI);
	if(newSAP)
		{
		newSAP->RPN(&aRPNTransaction, aMux, aDLCI);
		}
	else
		{
		// Uh Oh! Couldn't get a SAP, don't worry too much - a DM has been sent
		LOG(_L("[rfcommstates.cpp]: Couldn't clone a SAP after receiving a RPN\n"));
		}
	}

void TRfcommStateListening::Shutdown(CRfcommSAP& aSAP)
	{
	ChangeState(aSAP, CRfcommStateFactory::EClosed);	
	aSAP.iSocket->CanClose();
	}

void TRfcommStateListening::FastShutdown(CRfcommSAP& aSAP)
	{
	ChangeState(aSAP, CRfcommStateFactory::EClosed);
	}

CRfcommSAP* TRfcommStateListening::AttemptToClone(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI)
	/**
	Utility function private to TRfcommStateListening which attempts to create a cloned SAP

	@param aSAP The SAP to be cloned
	@param aMux The Muxer to attach it to
	@param aDLCI The DLCI of the cloned SAP
	**/
	{
	CRfcommSAP* newSAP=aSAP.CloneMe();
	
	if(!newSAP)
		{
		//	A new clone could not be created - send a DM, game over
		aMux.SendDM(aDLCI);
		}
	else
		{
		//	Set up details of the new identity of the clone

		//	Put it in WaitForSABM state
		newSAP->iState=iFactory.GetState(CRfcommStateFactory::EWaitForSABM);
		newSAP->iState->Enter(*newSAP);

		newSAP->iSignals=aSAP.iSignals;
		newSAP->iDLCI=aDLCI;
		newSAP->iServerChannel=aMux.MakeServerChannel(aDLCI);
		newSAP->iRemoteDev=aMux.RemoteBTAddr();
		//  Must come after registering the remote address with the new SAP because we will need 
		//  the remote address  to find a link in LinkMgrProtocol to override LPM on
		newSAP->iProtocol.ControlPlane().ModifyPhysicalLink(EOverrideLPM, newSAP->iRemoteDev);
		newSAP->iUserDefinedMTU=aSAP.iUserDefinedMTU;	//	We take our cues as
														//	regards max MTU from
														//	the listening SAP.
		
		newSAP->iMux=&aMux;
		//	NB. The listening SAP retains ownership of the SAP until it sends a
		//	ConnectComplete up for it.
		aMux.AddSAP(*newSAP);
		}

	return newSAP;
	}

TInt TRfcommStateListening::SetOption(CRfcommSAP &aSAP, TUint aLevel, 
								      TUint aName, const TDesC8& aOption)
    /**
       Allow modem status to be modified whilst we are listening.
    **/
    {
	if(aLevel == KSolBtRFCOMM && aName == KRFCOMMLocalModemStatus)
        {
		if ( aOption.Length() != sizeof(TUint8) )
			{
			return KErrArgument;
			}
		else
			{
			TPckgBuf<TUint8> signals;
			signals.Copy(aOption);
			aSAP.iSignals = signals();
			return KErrNone;
			}
        }

    return TRfcommStateDefault::SetOption(aSAP, aLevel, aName, aOption);
	}

/*****************************************************************/
/*
  The cloned base state
*/

TRfcommStateCloned::TRfcommStateCloned(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	}

void TRfcommStateCloned::LinkDown(CRfcommSAP& aSAP)
	{
	aSAP.iParentSAP->ChildConnectFailed(aSAP);
	}

void TRfcommStateCloned::Error(CRfcommSAP& aSAP, TInt /*aErr*/,
									CRfcommSAP::TErrorTypes /*aType*/)
	{
	aSAP.iParentSAP->ChildConnectFailed(aSAP);	//	Will delete us...
	}

void TRfcommStateCloned::DISC(CRfcommSAP& aSAP)
	/**
	   DISC, so the other end wants us to go away.
	   
	   Kill ourselves after acknowledging it.
	**/
	{
	aSAP.iMux->SendUA(aSAP.DLCI());		//	Send a UA which will outlive this SAP
	aSAP.iParentSAP->ChildConnectFailed(aSAP);
	}

void TRfcommStateCloned::DM(CRfcommSAP& aSAP)
	/**
	   We've got a DM in, which means that something's wrong.

	   We'll junk this connection, since we should not get a DM as we
	   haven't sent any commands.  The other end is obviously very
	   confused.
	**/
	{
	aSAP.iParentSAP->ChildConnectFailed(aSAP);
	}

/******************************************************************/
/*
  The Wait For SABM state.

  (The intial state of a cloned SAP)
*/

TRfcommStateWaitForSABM::TRfcommStateWaitForSABM(CRfcommStateFactory& aFactory)
	: TRfcommStateCloned(aFactory)
	{
	STATENAME("WaitForSABM");
	}

void TRfcommStateWaitForSABM::SABM(CRfcommSAP& aSAP, CRfcommMuxer& aMux, TUint8 aDLCI)
	/**
	We received a SABM - Tell our parent SAP that we're connected
	**/
	{
	if(aSAP.iNegotiatedMTU==0)	//	i.e. We've not entered into any negotiations...
		{
		aSAP.iNegotiatedMTU=KRfcommDefaultMTU;	//	...so set to default.
		aSAP.iOptimalMTUForSending=KRfcommDefaultMTU;
		}

	//	We can set up the Data Buffer since MTU negotiations are now over.
	//TRAPD(err, aSAP.iDataBuffer.SetLengthL(KRfcommSAPBufferMultiple*aSAP.iMTU));
		TRAPD(err, 
			  aSAP.iDataBuffer.SetLengthL(aMux.FlowStrategy()->DataBufferMultiple()*
										  aSAP.iNegotiatedMTU));
	if(err != KErrNone)
		{
		// Failed to alloc, so fail this connect and error
		aMux.SendDM(aDLCI);
		aSAP.iParentSAP->ChildConnectFailed(aSAP);	//	Will delete us...	
		return;
		}
	
	aSAP.iHighTideMark=KRfcommSAPBufferHighMultiple*(aSAP.iNegotiatedMTU);
	aSAP.iLowTideMark=KRfcommSAPBufferLowMultiple*(aSAP.iNegotiatedMTU);

	ChangeState(aSAP,CRfcommStateFactory::EWaitForIncomingSecurityCheck);
	}

#ifndef _DEBUG
void TRfcommStateWaitForSABM::PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, 
								 CRfcommMuxer& /*aMux*/, TUint8 /*aDLCI*/)
#else
void TRfcommStateWaitForSABM::PN(CRfcommSAP& aSAP, TRfcommPortParams& aParams, 
								 CRfcommMuxer& aMux, TUint8 /*aDLCI*/)
#endif
	{
	__ASSERT_DEBUG(aSAP.iMux==&aMux,PanicInState(ERfcommInvalidMuxInSAP));
	if(aSAP.iNegotiatedMTU==0)
		{
		//	We have no MTU set for the SAP at the moment. Try to determine
		//	the upper limit of what we can handle
		aSAP.iNegotiatedMTU=aSAP.MaximumMTU();
		aSAP.iOptimalMTUForSending= aSAP.NegotiatedMTU();
		}
		
	if(aParams.iMaxFrameSize>aSAP.iNegotiatedMTU || aParams.iMaxFrameSize<=0)
		{
		//	Either the remote device wants a larger MTU or has provided us
		//	with a nonsensical <=0 one.
		//	Either way, try to negotiate the MTU to our maximum
		aParams.iMaxFrameSize=aSAP.iNegotiatedMTU;
		}
	else
		{
		
		// We have an MTU value so ask L2Cap to work out the optimal MTU value based on the one
		// received from the remote. Note that the Max Frame Size is the max data size for Rfcomm
		// so we need to add on the max Rfcomm header size to create the restriction on L2Cap
		// MTU.
		TPckgBuf<TInt> restrictedMtu(aParams.iMaxFrameSize + KMaxFrameOverhead);
		TInt err = aSAP.GetOption(KSolBtL2CAP, KL2CAPOutboundMTUForBestPerformanceWithRestriction, restrictedMtu);

		
		// If we get an error continue but with a possible non-optimal MTU value. iMTU is the max
		// data size so remove the max Rfcomm header size as these will get added later.
 		aSAP.iOptimalMTUForSending = (err == KErrNone) ? restrictedMtu() - KMaxFrameOverhead : aParams.iMaxFrameSize;
 		aSAP.iNegotiatedMTU = aParams.iMaxFrameSize;
 		}	

	if(aParams.iCreditIndicator == (KCBFCCommandFlag >> 4))
		{
		// Remote device is using CBFC. Set up the credit counts
		aSAP.SetInitialLocalCredit(aParams.iInitialCredit); // Credits we've just been given
		
		 // now set to the credits we will give them
		aParams.iInitialCredit = aSAP.iMux->FlowStrategy()->PNFinalOctet();
		aSAP.SetProxyForRemoteCredit(aParams.iInitialCredit); // and record it
		}

	aSAP.iMux->SendPNResponse(aSAP, aParams);
	}

void TRfcommStateWaitForSABM::RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, 
								 CRfcommMuxer& /*aMux*/, TUint8 /*aDLCI*/)
	{
	// Write whatever's changed into our local port params. 
	aSAP.iLocalPortParams.UpdateFromRPNTransaction(aRPNTransaction);
	
	// Send L2CAP exactly what they sent us, except this is a response
	aSAP.iMux->SendRPN(aSAP, EFalse, KRPNResponseLength, aRPNTransaction);

	// Don't bother telling the user. If they're interested they'll just have
	// to keep doing getOpts whenever they feel like it!
	}

void TRfcommStateWaitForSABM::MuxUp(CRfcommSAP& /*aSAP*/)
	{
	//	Do nothing - included because the default class will panic.
	}


/******************************************************************/
/*
  The Incoming Security Check state.

  Handles making and responding to a security check on the security manager.

*/

TRfcommStateIncomingSecurityCheck::TRfcommStateIncomingSecurityCheck(CRfcommStateFactory& aFactory)
	: TRfcommStateCloned(aFactory)
	{
	STATENAME("IncomingSecurityCheck");
	}

void TRfcommStateIncomingSecurityCheck::Enter(CRfcommSAP& aSAP)
	{
	// INCOMING security
	// The listening SAP has the security settings, and possible device overrides
	aSAP.StartAccessRequest(aSAP.ListeningSAP());
	}

void TRfcommStateIncomingSecurityCheck::AccessRequestComplete(CRfcommSAP& aSAP, TInt aResult)
	{
	if(aResult==KErrNone)
		{
		LOG1(_L("RFCOMM: Access Request Successful (DLCI %d)"), aSAP.DLCI());
		ChangeState(aSAP, CRfcommStateFactory::EWaitForStart);
		}
	else
		{//	Something is not well - inform our parent.
		// This will result in us being deleted, so send DM for the DLCI
		LOG2(_L("RFCOMM: Access Request Failed (DLCI %d, Error %d)"), aSAP.DLCI(),aResult);
		aSAP.iMux->SendDM(aSAP.DLCI());
		aSAP.iParentSAP->ChildConnectFailed(aSAP);
		}
	}

void TRfcommStateIncomingSecurityCheck::Exit(CRfcommSAP& aSAP)
	{
	 aSAP.CancelAccessRequest(); 
	}

/******************************************************************/
/*
  The Outgoing Security Check state.

  Handles making and responding to a security check on the security manager.

*/

TRfcommStateOutgoingSecurityCheck::TRfcommStateOutgoingSecurityCheck(CRfcommStateFactory& aFactory)
	: TRfcommStateConnecting(aFactory)
	{
	STATENAME("OutgoingSecurityCheck");
	}

void TRfcommStateOutgoingSecurityCheck::Enter(CRfcommSAP& aSAP)
	{
	// Outgoing security
	// The listening SAP has the security settings, and possible device overrides
	aSAP.StartAccessRequest(aSAP);
	}

void TRfcommStateOutgoingSecurityCheck::AccessRequestComplete(CRfcommSAP& aSAP, TInt aResult)
	{
	if(aResult==EBTSecManAccessGranted)
		{
		LOG1(_L("RFCOMM: Access Request Successful (DLCI %d)"), aSAP.DLCI());
		aResult = aSAP.iMux->SendSABM(aSAP);
		}

	if (aResult < KErrNone || aResult == EBTSecManAccessDenied)
		{//	Something is not well - inform our parent.
		// This will result in us being deleted, so send DM for the DLCI
		LOG2(_L("RFCOMM: Outgoing security Failed (DLCI %d, Error %d)"), aSAP.DLCI(),aResult);
		FailureWhileConnecting(aSAP, KErrCouldNotConnect);
		}
	else
		{
		ChangeState(aSAP, CRfcommStateFactory::EWaitForUA);
		}
	}

void TRfcommStateOutgoingSecurityCheck::Exit(CRfcommSAP& aSAP)
	{
	aSAP.CancelAccessRequest(); 
	}


/******************************************************************/
/*
  The Wait For Start state.

*/

TRfcommStateWaitForStart::TRfcommStateWaitForStart(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	STATENAME("WaitForStart");
	}

void TRfcommStateWaitForStart::Enter(CRfcommSAP &aSAP)
	{
	//	NB. The following line effectively passes ownership of aSAP away from
	//	the parent SAP to ESOCK
	aSAP.iParentSAP->ChildConnected(aSAP);
	}

void TRfcommStateWaitForStart::Start(CRfcommSAP& aSAP)
	/**
	Start has been called on us. Move to the open state.
	**/
	{
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	if(aSAP.iParentSAP!=NULL)
		{
		//	We still have an extant parent listening SAP.
		//	Let them know that we have been started so that they can
		//	accept more cloned SAPs into the WaitForStart state.
		aSAP.iParentSAP->ChildStarted(aSAP);
		}
	aSAP.iMux->SendUA(aSAP);

	ChangeState(aSAP, CRfcommStateFactory::EOpen);
	}

void TRfcommStateWaitForStart::DISC(CRfcommSAP& aSAP)
	/**
	We are waiting for a Start, but the remote device has decided to disconnect us.

	Send a UA, and enter the CloseOnStart state.
	**/
	{
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	aSAP.iMux->SendUA(aSAP.DLCI());
	aSAP.iMux->DetachSAP(aSAP);		//	Because we're just waiting about for a Start
									//	from above - we're not interested in what is
									//	coming in from below (esp. LinkDown notification).
	ChangeState(aSAP,CRfcommStateFactory::ECloseOnStart);
	}

void TRfcommStateWaitForStart::Error(CRfcommSAP& aSAP, TInt /*aErr*/,
								   CRfcommSAP::TErrorTypes /*aType*/)
	{
	//	We can't really do anything at this point with an error,
	//	because ESOCK knows about us. Therefore, the best we can
	//	do is to move into CloseOnStart state.
	ChangeState(aSAP,CRfcommStateFactory::ECloseOnStart);
	}

	
void TRfcommStateWaitForStart::Exit(CRfcommSAP& aSAP)
	{
	aSAP.RegisterCodService();
	}

	
/******************************************************************/
/*
  The Close On Start state.

  For when a SAP triggers a ConnectComplete, but receives a remote DISC
  before it receives a local Start.
*/

TRfcommStateCloseOnStart::TRfcommStateCloseOnStart(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	STATENAME("CloseOnStart");
	}

void TRfcommStateCloseOnStart::Start(CRfcommSAP& aSAP)
	{
	//	NB. The Disconnect() invalidates the cloned SAP on ESOCKs
	//	accept queue - however, a client side Accept() will still be
	//	completed with KErrNone. The first read or write will be
	//	completed with KErrDisconnected.
	//
	aSAP.iSocket->Disconnect();

	if(aSAP.iParentSAP!=NULL)
		{
		//	We still have an extant parent listening SAP.
		//	Let them know that we have been started so that they can
		//	accept more cloned SAPs into the WaitForStart state.
		aSAP.iParentSAP->ChildStarted(aSAP);
		}

	ChangeState(aSAP, CRfcommStateFactory::EClosed);
	}

void TRfcommStateCloseOnStart::Error(CRfcommSAP& /*aSAP*/, TInt /*aErr*/,
									   CRfcommSAP::TErrorTypes /*aType*/)
	{
	//	Nothing we can really do with this error, since we're already
	//	going to close on start.
	}


/******************************************************************/
/*
  The open state.

  Here all the data xfer and flow control is handled.
*/

TRfcommStateOpen::TRfcommStateOpen(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	STATENAME("Open");
	}

void TRfcommStateOpen::Enter(CRfcommSAP& aSAP)
	/**
	   We've entered the open state, so tell the SAP.
	**/
	{
	// If this fails what happens?
	TInt err=aSAP.iMux->SendMSC(aSAP, aSAP.iSignals);
	if(err != KErrNone)
		{
		// Failed to alloc, so fail this connect and error
		SignalError(aSAP, err, MSocketNotify::EErrorConnect);
		ChangeState(aSAP, CRfcommStateFactory::EClosed);
		return;
		}
	
	// Depending where we came from we may have applied either a
	// park override or an all LPM override.  We remove both these
	// overrides because it's safe to remove an override that 
	// hasn't been applied.
	// Note that although park is one of the LPMs the different
	// levels of override are managed separately, so an
	// UndoOverrideLPM does not remove an explicit park only 
	// override.
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EUndoOverridePark, aSAP.iRemoteDev);
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EUndoOverrideLPM, aSAP.iRemoteDev);

	aSAP.iSocket->ConnectComplete();
	aSAP.CTS(EFalse);			//	So that we block should anyone try to write
								//	anything through this SAP before we get MSC
								//	from remote side.
	aSAP.iReceivedMSC=EFalse;	//	Until we receive an MSC, we consider any 
								//	user data we receive as being a bit naughty
								//	(other device is non 7.10 compliant)
	}

void TRfcommStateOpen::PerformDataAwareStateChange(CRfcommSAP& aSAP)
	{
	if(aSAP.iNewDataToNotify)
		{
		// We have outstanding data, so move to a half-closed state
		// where stuff can be read out but not sent
		ChangeState(aSAP, CRfcommStateFactory::EDisconnecting);
		}
	else
		{
		// Now call DisConnect().  Eventually this sap will be cleared up.
		ChangeState(aSAP, CRfcommStateFactory::EClosed);
		aSAP.iSocket->Disconnect();
		}
	}


void TRfcommStateOpen::Exit(CRfcommSAP& aSAP)
	/**
	   Exit the open channel
	**/
	{
	aSAP.DeregisterCodService();	// See if there is a Service to remove for CodMan
	}


void TRfcommStateOpen::Shutdown(CRfcommSAP& aSAP)
	/**
	   Shutdown the open channel gracefully
	**/
	{
	// Only override park for disconnect.  We need to be able
	// to send the signalling, but we don't care if it takes
	// ages.  No point starting a sniff bun-fight.
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EOverridePark, aSAP.iRemoteDev);
	aSAP.iClosePending=ETrue;
	ChangeState(aSAP, CRfcommStateFactory::EDisconnect);
	}

void TRfcommStateOpen::FastShutdown(CRfcommSAP& aSAP)
	/**
	   Terminate the session with extreme prejudice :-)

	   Send a disconnect, but don't bother to wait around since we'll
	   be deleted by our owner when this call returns.
	**/
	{
	//	Send a DISC using the DLCI based overload which ensures that
	//	the queued DISC frame does not get deleted when the SAP gets 
	//	removed from the muxer
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	
	// Only override park for disconnect.  We need to be able
	// to send the signalling, but we don't care if it takes
	// ages.  No point starting a sniff bun-fight.
	aSAP.iProtocol.ControlPlane().ModifyPhysicalLink(EOverridePark, aSAP.iRemoteDev);
	aSAP.iMux->SendDISC(aSAP.DLCI());  // Disassociated from the SAP
	ChangeState(aSAP, CRfcommStateFactory::EClosed);
	}

TInt TRfcommStateOpen::Send(CRfcommSAP& aSAP, const TDesC8& aData)
	/**
	   Attempt to send some data.
	   
	   We fragment the data and issue writes to the mux.  Keep going
	   until either the mux tells us to back off, or until all the
	   data has been sent.
	   
	   We also obey our flow control setting, so we only send if we
	   are clear to send
	   
	   Note that when we fail to send all the data, ESOCK will keep
	   calling us until we return zero before marking us as blocked.
	   Therefore we have to check if we're blocked before trying to
	   send anything.
	   
       @return Length of data actually sent
	**/
	{
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	
	TInt len=0; // Current amount we've sent.
	TInt fraglen;
	TInt sentlen=0;
	
	if(!aSAP.SendBlocked())
		{
		if(aSAP.CTS()) //FIXME???? make general to accommodate CBFC as well
			{
			while(len < aData.Length())
				{
				//NB1 0 if !CBFC
				//NB2 A successful 'Write' will update this value for next loop.
				TUint8 credit = aSAP.Mux()->FlowStrategy()->WriteCredit(aSAP); 
				fraglen=Min(aData.Length()-len, STATIC_CAST(TInt, aSAP.UsableMTU(credit)));
				if(aSAP.iMux->FlowStrategy()->AllowWrite(aSAP))
					sentlen=aSAP.iMux->Write(aSAP, credit, aData.Mid(len,fraglen));
				else
					sentlen=0;
				len+=sentlen;
				if(sentlen != fraglen)
					{
					// We failed to send all the data we wanted to, so
					// we're now blocked from sending, and must wait for a
					// notification that we can send again.
					aSAP.SendBlocked(aSAP.SendBlocked() | CRfcommSAP::EBlockedByMux);
					break;
					}
				}
			}
		else
			{
			// We're not clear to send to the other end, so back off ESOCK
			aSAP.SendBlocked(aSAP.SendBlocked() | CRfcommSAP::EBlockedByRemote);
			}
		}
	return len;
	}

void TRfcommStateOpen::Read(CRfcommSAP& aSAP, TDesC8& aData)
	/**
	   Read some data.
	   
	   This is filled from the circular buffer of incoming data.
	**/
	{
	__ASSERT_DEBUG(aSAP.DataBuffer().Count() >= aData.Length(),
				   PanicInState(ERfcommReadForTooMuchData));
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	LOG1(_L("RFCOMM: Read of %d bytes"), aData.Length());
	
	aSAP.DataBuffer().Remove(const_cast<TUint8*>(aData.Ptr()), aData.Length());
	//TRY_CBFC
	aSAP.iMux->FlowStrategy()->UpdateRxFlowControlState(aSAP);
	//FIXED_ME removed flow control code to flow strategy
	}

void TRfcommStateOpen::NewData(CRfcommSAP& aSAP, const TDesC8& aData)
	/**
	   New data has come in.

	   Store it in a buffer, and signal the socket to get it.
	**/
	{
	LOG1(_L("RFCOMM: New data length %d"), aData.Length());
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	__ASSERT_DEBUG(aData.Length() <= aSAP.iMux->GetMaxDataSize(), PanicInState(ERfcommDataPacketTooLong));

	if(!aSAP.iReceivedMSC)
		{
		//	We're dealing with a rather naughty non-7.10-compliant
		//	RFCOMM implementation which doesn't send an MSC before
		//	sending it's data. Note this fact and continue.
		LOG(_L("RFCOMM: Warning received data before MSC on new connection!"));
		}
	//TRY_CBFC
	if (!aSAP.iMux->FlowStrategy()->NewDataReviseCredits(aSAP, aData)) // ...the remote has overflowed us...
		{
		// remote is ignoring flow control decide whether to disconnect or drop packets (if using CBFC)
		if (aSAP.iMux->FlowStrategy()->FlowType() == CRfcommFlowStrategyFactory::EFlowCreditBased &&
			aSAP.iForgiveCBFCOverflow)
			{
			//drop packet - l2cap frees the underlying packet storage
			return;
			}
		else
			{				
			aSAP.iSocket->Disconnect();
			ChangeState(aSAP, CRfcommStateFactory::EClosed);
			return;
			}
		}
	
	TInt storelen=aSAP.DataBuffer().Add(aData.Ptr(), aData.Length());
	if(storelen != aData.Length())
		{
		__ASSERT_DEBUG(EFalse, PanicInState(ERfCommStateBufferFull));
		// We've not got room for some of the data
		LOG(_L("RFCOMM: Received data loss - disconnecting"));
		aSAP.iSocket->Disconnect();
		ChangeState(aSAP, CRfcommStateFactory::EClosed);
		return;
		}

	// Check to see if we need to quench the source
	LOG1(_L("RFCOMM: Data buffer level %d"), aSAP.DataBuffer().Count());

	aSAP.iMux->FlowStrategy()->NewData(aSAP);
	//FIXED-ME flow code block removed to strategy object (applies non CBFC flow only).

	if(storelen)
		{
		aSAP.iNewDataToNotify += storelen;
		aSAP.iNotifyNewDataCallback->CallBack();
		}
	}

void TRfcommStateOpen::NotifyNewDataCallback(CRfcommSAP& aSAP)
	{
	TInt newData = aSAP.iNewDataToNotify;
	if (newData)
		{
		aSAP.iNewDataToNotify = 0;
		LOG1(_L("RFCOMM: Actually telling Socket of newdata %d bytes"), newData);
		aSAP.iSocket->NewData(newData);
		}
	aSAP.iMux->FlowStrategy()->UpdateRxFlowControlState(aSAP);
	}

void TRfcommStateOpen::CanSend(CRfcommSAP& aSAP)
	/**
	   SAP is now clear to try to send data again (maybe) since these
	   can be sent any time.
	   
	   Notify the socket.
	**/
	{	

	__ASSERT_DEBUG(aSAP.SendBlocked() & CRfcommSAP::EBlockedByMux, PanicInState(ERfcommUnexpectedCanSend));
	
	aSAP.SendBlocked(aSAP.SendBlocked() & ~CRfcommSAP::EBlockedByMux);
	aSAP.iSocket->CanSend();
	}

void TRfcommStateOpen::MSC(CRfcommSAP& aSAP, TUint8 aSignals)
/**	
	Modem signals received from the remote end.
	The only bit we're really interested in is the FC bit but we store
	the rest anyway in case anyone above wants to read it (see GetOpt)
**/
	{
	aSAP.iRemoteModemStatus = aSignals;

	aSAP.iReceivedMSC=ETrue;

	//FC
	if(aSAP.iMux->FlowStrategy()->MSC(aSAP, aSignals))
		{
		aSAP.SendBlocked(aSAP.SendBlocked() & ~CRfcommSAP::EBlockedByRemote);
		aSAP.iSocket->CanSend();
		}
	//

	// Indication of config change
	IoctlComplete(aSAP, KErrNone, KSolBtRFCOMM, KRFCOMMConfigChangeIndicationIoctl, NULL);

	// If there has been a signal status to watch for set, and this is it, then we send a disconnect error to the client reader
	if(aSAP.iDisconnectMSCSignal && aSAP.iDisconnectMSCSignal == aSignals)
		{
		SignalError(aSAP, KErrDisconnected, MSocketNotify::EErrorRecv);
		}
	}

void TRfcommStateOpen::RLS(CRfcommSAP& aSAP, TUint8 aStatus)
//	Remote Line Status command received from remote end
//	Just set the SAP's local copy
	{
	aSAP.iRLSstatus = aStatus;
	// Indication of config change
	IoctlComplete(aSAP, KErrNone, KSolBtRFCOMM, KRFCOMMConfigChangeIndicationIoctl, NULL);
	}


void TRfcommStateOpen::DISC(CRfcommSAP& aSAP)
	/**
	   The remote end has sent a DISC.  We need to respond.

	   Q: What do we do about any data in our buffers?
	   A: Q the UA in order, so that all data in our buffers is
	   flushed before the UA is sent.
	**/
	{
	//	Use the DLCI based overload of SendUA, which ensures that the queued
	//	frame has no SAP set.
	//	This ensures that the frame does not get deleted when this SAP is detached
	//	from the muxer, when we enter the Closed state.
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	aSAP.iMux->SendUA(aSAP.DLCI());   // If this fails  there's not much we can do
	PerformDataAwareStateChange(aSAP);
	}


void TRfcommStateOpen::DM(CRfcommSAP& aSAP)
	/**
	   A DM, which means the other end has gone away.

	   Time to error everything.
	**/
	{
	SignalError(aSAP, KErrDisconnected, MSocketNotify::EErrorAllOperations);
	}

void TRfcommStateOpen::RPN(CRfcommSAP& aSAP, const TRfcommRPNTransaction& aRPNTransaction, CRfcommMuxer& /*aMuxer*/, TUint8 /*aDLCI*/)
//	The remote end has sent us an RPN negotiate command
//	Just accept whatever they suggested then tell them we've accepted
	{
	// Write whatever's changed into our local port params. 
	aSAP.iLocalPortParams.UpdateFromRPNTransaction(aRPNTransaction);
	
	// Send L2CAP exactly what they sent us, except this is a response
	aSAP.iMux->SendRPN(aSAP, EFalse, KRPNResponseLength, aRPNTransaction);

	// Indication of config change
	IoctlComplete(aSAP, KErrNone, KSolBtRFCOMM, KRFCOMMConfigChangeIndicationIoctl, NULL);
	}

void TRfcommStateOpen::RPNRsp(CRfcommSAP& aSAP, 
							  const TRfcommRPNTransaction& aRPNTransaction)
//	The remote end has sent us an RPN response
	{
	// Make sure this is the response we are waiting for
	switch(aSAP.iIoctlName)
		{
		case KRFCOMMRemotePortNegCmdIoctl:
			LOG1(_L("RFCOMM: Received Negotiation response for sap %08x"), &aSAP);		
			// Update the local port parameters with what was agreed by the remote end. 
			aSAP.iLocalPortParams.UpdateFromRPNTransaction(aRPNTransaction);
			break;
		case KRFCOMMRemotePortNegRequestIoctl:
			LOG1(_L("RFCOMM: Received Query response for sap %08x"), &aSAP);
			break;
		default:
			__DEBUGGER();
			LOG(_L("Unexpected RPN response\n"));
			return;
		}

	// Convert the RPN transaction structure into a descriptor
	TPckgC<TRfcommRPNTransaction> RPNTransaction(aRPNTransaction);
	
	// Send the package up to the user			
	IoctlComplete(aSAP, KErrNone, KSolBtRFCOMM, aSAP.iIoctlName, &RPNTransaction);
	}

void TRfcommStateOpen::PN(CRfcommSAP& aSAP, TRfcommPortParams& /*aParams*/, CRfcommMuxer& /*aMux*/, TUint8 /*aDLCI*/)
	/**
	We received a PN in the open state - no problem, but for simplicity
	we just respond with our current settings (i.e. MTU).
	NB. Importantly, the initial credit field should be set to 0...
	**/
	{
	TRfcommPortParams params;
	params.iMaxFrameSize=aSAP.NegotiatedMTU();
	params.iInitialCredit=0;	//	See F:1 5.5.3 Core spec 1.1
	aSAP.iMux->SendPNResponse(aSAP, params);
	}

void TRfcommStateOpen::LinkDown(CRfcommSAP& aSAP)
	/**
	   The link has gone down, which means our Mux will be deleting itself.

	   Go to closed state, which removes us from the mux
	**/
	{
	PerformDataAwareStateChange(aSAP);
	}


void TRfcommStateOpen::Ioctl(CRfcommSAP& aSAP,TUint aLevel,TUint aName,TDes8* aOption)
	/**
	   Perform an RFCOMM ioctl that applies to an RFCOMM connection.

		If it is not valid, an error occurs.
	**/
	{
	TInt ret = KErrNone;

	switch(aName)
		{
		case KRFCOMMModemStatusCmdIoctl:
			{
			// aOption stores modem status
			if(!aOption)
				{
				ret=KErrArgument;
				}
			else	
				{
				TUint8 flags = TUint8(*aOption->Ptr());
				// Make sure we send our real flow control status, 
				// not just whatever was in the data received from the user
				if(aSAP.CTR())
					{
					flags &= ~KModemSignalFC;		
					}
				else
					{
					flags |= KModemSignalFC;
					}

				aSAP.SetSignals(flags);
				ret = aSAP.iMux->SendMSC(aSAP, flags);
				}
			}
			// Complete Ioctl with what was sent to us
			IoctlComplete(aSAP, ret, aLevel, aName, aOption);
			break;

		case KRFCOMMRemoteLineStatusCmdIoctl:
			// aOption stores status of remote line
			ret = aOption ? aSAP.iMux->SendRLS(aSAP, TUint8(*aOption->Ptr())) : KErrArgument;
			// Complete Ioctl with what was sent to us
			IoctlComplete(aSAP, ret, aLevel, aName, aOption);
			break;

		case KRFCOMMRemotePortNegCmdIoctl:
			LOG1(_L("RFCOMM: Sending RPN negotiate for sap %08x"),
							&aSAP);
			// aOption is really an RPN transaction structure
			ret = aOption ? aSAP.iMux->SendRPN(aSAP, ETrue, KRPNCommandLength, 
				*(TRfcommRPNTransaction*)aOption->Ptr()) : KErrArgument;
			break;

		case KRFCOMMRemotePortNegRequestIoctl:
			LOG1(_L("RFCOMM: Sending RPN query for sap %08x"), 
							&aSAP);
			// aOption is really an RPN transaction structure
			ret = aOption ? aSAP.iMux->SendRPN(aSAP, ETrue, KRPNRequestLength, 
				*(TRfcommRPNTransaction*)aOption->Ptr()) : KErrArgument;
			break;

		case KRFCOMMFConIoctl:
			LOG2(_L("RFCOMM: Sending FCon command for device 0x%04x%08x"),
				TUint(TUint16((aSAP.iMux->RemoteBTAddr())[0])),
				TUint((aSAP.iMux->RemoteBTAddr())[2]));
			//FC
			ret=aSAP.iMux->SendFCon();
			//
			IoctlComplete(aSAP, ret, aLevel, aName, aOption);
			break;
			
		case KRFCOMMFCoffIoctl:
			LOG2(_L("RFCOMM: Sending FCoff command for device 0x%04x%08x"),
				TUint(TUint16((aSAP.iMux->RemoteBTAddr())[0])),
				TUint((aSAP.iMux->RemoteBTAddr())[2]));
			//FC
			ret=aSAP.iMux->SendFCoff();
			//
			IoctlComplete(aSAP, ret, aLevel, aName, aOption);
			break;

		case KRFCOMMConfigChangeIndicationIoctl:
			LOG(_L("RFCOMM: Registering KRFCOMMConfigChangeIndicationIoctl"));
			// Do nothing -- async completion on remote MSC, RPN, RLS
			break;

		default:
			ret = KErrNotSupported;
		}

	if(ret != KErrNone)
		{
		aSAP.iIoctlName=0;
		aSAP.iIoctlLevel=0;
		SignalError(aSAP, ret, MSocketNotify::EErrorIoctl);
		}
	}

void TRfcommStateOpen::IoctlComplete(CRfcommSAP& aSAP, TInt aErr, TUint aLevel,TUint aName, TDesC8* aBuf)
	/**
	   An ioctl has completed.

	   If it's for us, signal the socket
	**/
	{
	if (aSAP.iIoctlLevel==aLevel &&
		aSAP.iIoctlName ==aName)
		{
		if(aErr == KErrNone)
			{
			aSAP.iSocket->IoctlComplete(aBuf);
			}
		else
			{
			SignalError(aSAP, aErr, MSocketNotify::EErrorIoctl);
			}
		aSAP.iIoctlName=0;
		aSAP.iIoctlLevel=0;
		}
	}

TInt TRfcommStateOpen::SetOption(CRfcommSAP &aSAP, TUint aLevel, 
								   TUint aName, const TDesC8& aOption)
    /**
       Allow modem status to be modified whilst open.
    **/
    {
 	TInt ret;
    if(aLevel == KSolBtRFCOMM && aName == KRFCOMMLocalModemStatus)
		{
		if ( aOption.Length() != sizeof(TUint8) )
			{
			ret = KErrArgument;
			}
		else
			{
			TUint8 flags = TUint8(*aOption.Ptr());
			if(aSAP.CTR())
				{
				flags &= ~KModemSignalFC;		
				}
			else
				{
				flags |= KModemSignalFC;
				}

			ret = aSAP.iMux->SendMSC(aSAP, flags);
			if(ret==KErrNone)
				{
				// Oddly, ioctl KRFCOMMModemStatusCmdIoctl updates 
				// local signals regardless of an error sending them
				aSAP.SetSignals(flags);
				}
			}
	 	}
 	else
 		{
 		ret = TRfcommStateDefault::SetOption(aSAP, aLevel, aName, aOption);		
 		}
   	 	
	 	
    return ret;
    }

 
//TRY_CBFC
TUint8 TRfcommStateOpen::FreeCredit(CRfcommSAP& aSAP)
	{
	TUint8 creds = aSAP.FreeCreditCalculation();
	return creds;
	}

TInt TRfcommStateOpen::ProxyForRemoteCredit(const CRfcommSAP& aSAP) const
	{
	return aSAP.iProxyForRemoteCredit;
	}

TInt TRfcommStateOpen::LocalCredit(const CRfcommSAP& aSAP) const
	{
	return aSAP.iLocalCredit;
	}



/*********************************************************************/
/*
  Disconnect state
*/

TRfcommStateDisconnect::TRfcommStateDisconnect(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	STATENAME("Disconnect");
	}

void TRfcommStateDisconnect::Enter(CRfcommSAP& aSAP)
	/**
	   Send out a disconnect via the mux
	**/
	{
	__ASSERT_DEBUG(aSAP.iMux!=NULL,PanicInState(ERfcommNullMux));
	// Need to kill off any data callback since we don't want it
	aSAP.iNotifyNewDataCallback->Cancel();
	aSAP.iNewDataToNotify = 0;
	aSAP.iDataBuffer.Reset();
	aSAP.iMux->SendDISC(aSAP);
	}

void TRfcommStateDisconnect::DISC(CRfcommSAP& aSAP)
	/**
	We're attempting to DISC, and apparently so is our peer SAP.

	Send them a UA, and move to the closed state.
	**/
	{
	aSAP.iMux->SendUA(aSAP.DLCI());	//	Send to DLCI, or we may lose this when the SAP gets deleted.
	DisconnectComplete(aSAP);
	}

void TRfcommStateDisconnect::UA(CRfcommSAP& aSAP)
	/**
	   An answer to our DISC.

	   Disconnect from the Mux now, so it can go away if needs be.
	**/
	{
	DisconnectComplete(aSAP);
	}

void TRfcommStateDisconnect::DM(CRfcommSAP& aSAP)
	/**
	   Strange, the remote end has sent an error for our disconnect.
	   Oh well, better close anyway.
	**/
	{
	DisconnectComplete(aSAP);
	}	

void TRfcommStateDisconnect::DisconnectComplete(CRfcommSAP& aSAP)
	{
	//	Record whether there is a CanClose pending - entering the closed
	//	state will overwrite this data.
	TBool canCloseRequired=aSAP.iClosePending;
	
	ChangeState(aSAP, CRfcommStateFactory::EClosed);	// Must enter before CanClose, since
														// this may delete us
	if(canCloseRequired)
		aSAP.iSocket->CanClose();
	}

void TRfcommStateDisconnect::LinkDown(CRfcommSAP& aSAP)
	/**
	The link has gone down during our close.
	**/
	{
	ChangeState(aSAP, CRfcommStateFactory::EClosed);	  // Enter before we get deleted...
	aSAP.iSocket->CanClose();
	}

/***************************************************************

The disconnecting state

***************************************************************/


TRfcommStateDisconnecting::TRfcommStateDisconnecting(CRfcommStateFactory& aFactory)
	: TRfcommStateDefault(aFactory)
	{
	STATENAME("Disconnecting");
	}


void TRfcommStateDisconnecting::Enter(CRfcommSAP& aSAP)
	/**
	   Enter the disconnecting state.

	   This state is entered if we have data for the client, but the
	   other end has disconnected.  Thus we are in a half-disconnected
	   state, where the remote end thinks we're closed and the local
	   end thinks we're open. Since we have data to notify, we'll send
	   it to ESOCK immediately.

	   We disconnect from the mux as we have no further need for
	   incoming events.
	**/
	{
	__ASSERT_DEBUG(aSAP.iNewDataToNotify, PanicInState(ERfcommStateDisconnectingWithNoData));
	__ASSERT_DEBUG(aSAP.iNotifyNewDataCallback->IsActive(), PanicInState(ERfcommStateDisconnectingWithNoData));
//	aSAP.iNotifyNewDataCallback->Cancel(); // Cancel the outstanding callback
	aSAP.iMux->DetachSAP(aSAP);
	TInt newData = aSAP.iNewDataToNotify;
	if (newData)
		{
		aSAP.iNewDataToNotify = 0;
		aSAP.iSocket->NewData(newData);
		}
	}

TInt TRfcommStateDisconnecting::Send(CRfcommSAP& aSAP, const TDesC8& /*aData*/)
	/**
	   Attempt to send data, but we can't.

	   We will signal an error on the send.
	**/
	{
	aSAP.iErrorKicker->SetError(KErrDisconnected, CRfcommSAP::EErrorOperation);  
	aSAP.iErrorKicker->Call(); // FIXME: Note this currently errors everything 
	return 0;
	}

void TRfcommStateDisconnecting::Read(CRfcommSAP& aSAP, TDesC8& aData)
	/**
	   Read out some of the data from the buffer.

	   Note that there is no need to deal with flow control, as the
	   link is dead.  Also, there is no need to deal with notifying
	   the mux for more data, since we assume that only data that is
	   notified to us before the DISC is valid.
	**/
	{
	__ASSERT_DEBUG(aSAP.DataBuffer().Count() >= aData.Length(),
				   PanicInState(ERfcommReadForTooMuchData));
	LOG1(_L("RFCOMM: Read of (disconnecting) %d bytes"), aData.Length());
	
	aSAP.DataBuffer().Remove(const_cast<TUint8*>(aData.Ptr()), aData.Length());
	}

void TRfcommStateDisconnecting::NotifyNewDataCallback(CRfcommSAP& aSAP)
/**
	Method name is a bit dud in this state
	We notify the socket that it is Disconnected
	Provides better behaviour for the MSocketNotify - it doesn't end up
	with a Disconnect upcall whilst calling Read
**/
	{
	__ASSERT_DEBUG(aSAP.iNewDataToNotify == 0, PanicInState(ERfcommStateClosedWithData));

	LOG(_L("SAP has no more data - socket now Disconnected"));

	aSAP.iSocket->Disconnect();
	ChangeState(aSAP, CRfcommStateFactory::EClosed);
	}

void TRfcommStateDisconnecting::Ioctl(CRfcommSAP& aSAP,TUint /*aLevel*/,TUint /*aName*/,TDes8* /*aOption*/)
	/**
	   An ioctl has been requested while we are in state disconnected.

	   We fail all ioctls in this state.
	**/
	{
	__ASSERT_DEBUG(aSAP.iIoctlLevel == 0, PanicInState(ERfcommSAPTwoIoctls));
	__ASSERT_DEBUG(aSAP.iIoctlName == 0, PanicInState(ERfcommSAPTwoIoctls));

	SignalError(aSAP, KErrDisconnected, MSocketNotify::EErrorIoctl);
	}

void TRfcommStateDisconnecting::Shutdown(CRfcommSAP& aSAP)
	/**
	   We've been asked to close this side.

	   Since the other end has already gone away, there's no need to
	   send anything, we can go straight to closed.
	**/
	{
	ChangeState(aSAP, CRfcommStateFactory::EClosed);	// Must enter before CanClose, since
														// this may delete us
	aSAP.iSocket->CanClose();
	}

void TRfcommStateDisconnecting::FastShutdown(CRfcommSAP& aSAP)
	/**
	   We've been asked to close this side with extreme prejudice.

	   Since the other end has already gone away, there's no need to
	   send anything, we can go straight to closed.  Note we will be
	   deleted when this returns.
	**/
	{
	ChangeState(aSAP, CRfcommStateFactory::EClosed);
	}
