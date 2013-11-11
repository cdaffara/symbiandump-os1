// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include "rfcommutil.h"
#include "rfcommmuxer.h"
#include "rfcommflow.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_RFCOMM);
#endif

/*************************************************************************/


CRfcommFlowStrategyFactory::CRfcommFlowStrategyFactory()
	{
	LOG_FUNC
//	iFlowStrategies.Reset();  // Null all the pointers -- not necessary as array is in CBase class.
	}

CRfcommFlowStrategyFactory::~CRfcommFlowStrategyFactory()
	{
	LOG_FUNC
	// Destroy all the state objects
	iFlowStrategies.DeleteAll();
	}
	
CRfcommFlowStrategyFactory* CRfcommFlowStrategyFactory::NewL()
	{
	LOG_STATIC_FUNC
	CRfcommFlowStrategyFactory* factory  = new (ELeave) CRfcommFlowStrategyFactory();
	CleanupStack::PushL(factory);
	// Create all the new states
	factory->iFlowStrategies[EFlowInitial]  = new (ELeave) TRfcommFlowStrategyInitial(factory);
	factory->iFlowStrategies[EFlowCreditBased] = new (ELeave) TRfcommFlowStrategyCreditBased(factory);
	factory->iFlowStrategies[EFlowNonCreditBased] = new (ELeave) TRfcommFlowStrategyNonCreditBased(factory);
	// etc...
	CleanupStack::Pop();
	return factory;
	}

TRfcommFlowStrategy& CRfcommFlowStrategyFactory::GetFlowStrategy(const TFlowStrategies aFlowStrategy) const
	{
	LOG_FUNC
	__ASSERT_DEBUG(aFlowStrategy != EMaxFlowStrategy, Panic(ERfCommFlowStrategyOutOfBounds));
	return *iFlowStrategies[aFlowStrategy];
	}




/*************************************************************************/



TRfcommFlowStrategy::TRfcommFlowStrategy(CRfcommFlowStrategyFactory* aFactory)
:iFactory(aFactory)
	{
	LOG_FUNC
	//iFactory = aFactory;
	}


TRfcommFlowStrategy::~TRfcommFlowStrategy()
	{
	LOG_FUNC
	}





TRfcommFlowStrategyInitial::TRfcommFlowStrategyInitial(CRfcommFlowStrategyFactory* aFactory)
:TRfcommFlowStrategy(aFactory) 
	{
	LOG_FUNC
	}

TRfcommFlowStrategyInitial::~TRfcommFlowStrategyInitial()
	{
	LOG_FUNC
	}

TInt TRfcommFlowStrategyInitial::DataBufferMultiple()
	{
	LOG_FUNC
	return KRfcommSAPBufferMultiple;
	};

TUint16 TRfcommFlowStrategyInitial::UsableMTU(const CRfcommSAP& aSAP, TUint8 /*aCredit*/) const
	/**
	  Available frame space for data
	**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(aSAP.OptimalMTUForSending(), Panic(ERfcommBadCalculatedMTU));

	return aSAP.OptimalMTUForSending();
	}

TBool TRfcommFlowStrategyInitial::AllowWrite(CRfcommSAP& /*aSAP*/)
	/**
	  Ignore CBFC Credits
	**/
	{
	LOG_FUNC
	return ETrue;
	}

TBool TRfcommFlowStrategyInitial::AllowFCOnOff(TBool /*aCommand*/)
	/**
	  Always allow 'FC On/Off' commands (not be confused with FC bit in an MSC command!).
	**/
	{
	LOG_FUNC
	return ETrue;
	}

TInt TRfcommFlowStrategyInitial::PNConvergenceLayer(TBool aCommand)
	/**
	  We are negotiating so keep trying for CBFC.
	**/
	{
	LOG_FUNC
	return aCommand?KCBFCCommandFlag:KCBFCResponseFlag;
	}

TUint8 TRfcommFlowStrategyInitial::PNFinalOctet() // FIXME: Rename to InitialCredit()
	/**
	  We are negotiating so keep trying for CBFC.
	**/
	{
	LOG_FUNC
	return KInitialCredit;
	}

TUint8 TRfcommFlowStrategyInitial::WriteCredit(CRfcommSAP& /*aSAP*/)
	/**
	  Return number of CBFC credits to send in UIH frame.
	**/
	{
	LOG_FUNC
	return 0;
	}


void TRfcommFlowStrategyInitial::OutgoingMSCReviseSignals(TBool /*aCommand*/, TUint8& /*aSignals*/)
	/**
	  Only requires action for CBFC.
	**/
	{
	LOG_FUNC
	}

void TRfcommFlowStrategyInitial::ReviseDonatedCredits(CRfcommSAP& /*aSAP*/, TUint8 /*aCredit*/)
	/**
	  Only requires action for CBFC.
	**/
	{
	LOG_FUNC
	}

void TRfcommFlowStrategyInitial::ReviseTransmittedCredits(CRfcommSAP& /*aSAP*/)
	{
	LOG_FUNC
	/**
	  Only requires action for CBFC.
	**/
	}

CRfcommDataFrame* TRfcommFlowStrategyInitial::NewDataFrame(CRfcommMuxer& aMuxer, 
													 TUint8 aAddr, 
												     TInt aLen, 
#ifdef _DEBUG
												     TUint8 aCredit,
#else
												     TUint8 /*aCredit*/,
#endif

												     CRfcommSAP* aSAP)
	/**
	   Returns a new data (UIH) frame for the specified DLCI

	   The C/R bit for the UIH frame is always 1 for the initiator and
	   0 for the responder, which is the same as for commands.
	**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(!aCredit, Panic(ERfcommUnexpectedCBFCCredit));
	CRfcommDataFrame* frm=NULL;
	TRAPD(err, frm=CRfcommDataFrame::NewL(aLen, aMuxer, aSAP));
	if(!err)
		{
		frm->SetAddress(aAddr);
		frm->SetControl(KUIHCtrlField);
		}
	return frm;	//	NB should be NULL if CRfcommDataFrame was not created
	}

TBool TRfcommFlowStrategyInitial::CanProcessNewData(const TBool aAllowProcessNewData) const
	/**
	  If a SAP's buffer becomes too full, then that SAP will cause the muxer
	  to block any further intake of data. 'aSAPNoFreeSpaceMask' points to the
	  relevant muxer flag.
	**/
	{
	LOG_FUNC
	return aAllowProcessNewData;
	}

void TRfcommFlowStrategyInitial::DecodeLength(TBool /*aCBFC*/, 
										 	  TDesC8& /*aPacket*/, 
											  TInt& /*aCreditBuffer*/, 
											  TInt& /*aHeaderLengthBuffer*/)
	/**
	  Do nothing - not CBFC.
	**/
	{
	LOG_FUNC
	}

TBool TRfcommFlowStrategyInitial::ProcessDataFrameReviseCredits(CRfcommSAP& /*aSAP*/, 
													   TBool /*aPoll*/,
													   TUint8 /*aCredit*/)
	/**
	  Not CBFC - return false => do nothing.
	**/
	{
	LOG_FUNC
	return EFalse;
	}

TBool TRfcommFlowStrategyInitial::MSC(CRfcommSAP& aSAP, TUint8 aSignals)
	/**
	  Respond to flow control aspects of an MSC signal.
	**/
	{
	LOG_FUNC
	// Set CTS. We're clear to send if flow control is off so this 
	// is the inverse of the Flow Control bit
	aSAP.CTS(!(aSignals & KModemSignalFC));
	if(aSAP.CTS())
		{
		// Unblock ESOCK to try again (which might block again!)
		return ETrue;
		}
	return EFalse;
	}

void TRfcommFlowStrategyInitial::UpdateRxFlowControlState(CRfcommSAP& aSAP)
	/**
		Do non-CBFC flow control.
	**/
	{
	LOG_FUNC
	if(aSAP.DataBuffer().Count() + aSAP.Mux()->GetMaxDataSize() <=
	   aSAP.DataBuffer().Length())
		{
		// Let the mux know we can take more data
		//FC
		aSAP.Mux()->SetCanHandleData(aSAP, ETrue);
		}

	if(aSAP.DataBuffer().Count() <= aSAP.LowTideMark())
		{
		if(!aSAP.CTR())
			{
			// We should send a flow on since we've sent an off
			//FC
			TUint8 signals=aSAP.Signals();
			signals &= (~KModemSignalFC);
			TInt result=aSAP.Mux()->SendMSC(aSAP, signals);
			if(result == KErrNone)
				{
				// Update state if we sent it, otherwise don't
				aSAP.SetSignals(signals);
				aSAP.CTR(ETrue);
				}
			//
			}
		}
	}

TBool TRfcommFlowStrategyInitial::NewDataReviseCredits(CRfcommSAP& /*aSAP*/, const TDesC8& /*aData*/)
	/**
		Do nothing - SAPWise (CBFC) only.
	**/
	{
	LOG_FUNC
	return ETrue;
	}

void TRfcommFlowStrategyInitial::NewData(CRfcommSAP& aSAP)
	/**
		Deal with flow control when new data is coming in and CBFC is not on.
	**/
	{
	LOG_FUNC
	if(aSAP.DataBuffer().Count() + aSAP.Mux()->GetMaxDataSize() >=
	   aSAP.DataBuffer().Length())
		/**
			N.B. CircularBuffers: Length() is MaxLength() and Count() is Length()!
		**/
		{
		//Tell the mux we canna take any more, capt'n
		aSAP.Mux()->SetCanHandleData(aSAP, EFalse);
		}
		
	if(aSAP.DataBuffer().Count() >= aSAP.HighTideMark())
		{
		// Now try to send a flow off, if we haven't already
		if(aSAP.CTR())
			{
			//FC
			TUint8 signals=aSAP.Signals();
			signals |= KModemSignalFC;
			TInt result=aSAP.Mux()->SendMSC(aSAP, signals);
			if(result == KErrNone)
				{
				// we sent it, so update state
				aSAP.SetSignals(signals);
				aSAP.CTR(EFalse);
				}
			//
			}
		}
	return;
	}

CRfcommFlowStrategyFactory::TFlowStrategies TRfcommFlowStrategyInitial::FlowType()
	{
	LOG_FUNC
	return CRfcommFlowStrategyFactory::EFlowInitial;
	}


TRfcommFlowStrategyCreditBased::TRfcommFlowStrategyCreditBased(CRfcommFlowStrategyFactory* aFactory)
:TRfcommFlowStrategyInitial(aFactory) 
	{
	LOG_FUNC
	}

TRfcommFlowStrategyCreditBased::~TRfcommFlowStrategyCreditBased()
	{
	LOG_FUNC
	}


TInt TRfcommFlowStrategyCreditBased::DataBufferMultiple()
	{
	LOG_FUNC
	return KRfcommSAPCBFCBufferMultiple;
	};

TUint16 TRfcommFlowStrategyCreditBased::UsableMTU(const CRfcommSAP& aSAP, TUint8 aCredit) const
	/**
	  Available frame space for data
	**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(aSAP.OptimalMTUForSending(), Panic(ERfcommBadCalculatedMTU));

	//Allow for possible credit in header.
	return (STATIC_CAST(TUint16, (aSAP.OptimalMTUForSending() - (aCredit?1:0))));
	}

TBool TRfcommFlowStrategyCreditBased::AllowWrite(CRfcommSAP& aSAP)
	/**
	**/
	{
	LOG_FUNC
	return aSAP.LocalCredit();
	}

TBool TRfcommFlowStrategyCreditBased::AllowFCOnOff(TBool aCommand)
	/**
	  Allow responses only. Do not transmit an 'FC On/Off' command.
	  (not be confused with FC bit in an MSC command!)
	**/
	{
	LOG_FUNC
	return !aCommand; //N.B. Reverse logic - if command then return 'EFalse'
	}

TUint8 TRfcommFlowStrategyCreditBased::WriteCredit(CRfcommSAP& aSAP)
	/**
	  Return number of CBFC credits to send in UIH frame
	**/
	{
	LOG_FUNC
	return aSAP.FreeCredit();
	}

void TRfcommFlowStrategyCreditBased::OutgoingMSCReviseSignals(TBool aCommand, TUint8& aSignals)
	/**
	  There is a bit that when zero => CTS should be set to true. 
	  For CBFC this bit must always be zero if we are sending a command. 
	  Thus we are masking that part of an MSC command which tells the remote 
	  to switch its CTS off (and so stop sending). 
	  Required by SPEC.
	**/
	{
	LOG_FUNC
	if(aCommand)
		aSignals &= (~KModemSignalFC);
	}

void TRfcommFlowStrategyCreditBased::ReviseDonatedCredits(CRfcommSAP& aSAP, TUint8 aCredit)
	/**
	  Revise proxy credit (our knowledge of remote credit) upwards by 'aCredit'.
	**/
	{
	LOG_FUNC
	//credit change
	//Increase proxy credit by 'aCredit'.
	aSAP.ProxyForRemoteCreditAddCredit(aCredit);
	}

void TRfcommFlowStrategyCreditBased::ReviseTransmittedCredits(CRfcommSAP& aSAP)
	/**
	  Revise our own credits downward by one.
	  Revise proxy credit (our knowledge of remote credit) upwards by 'aCredit'.
	**/
	{
	LOG_FUNC
	//Decrement allowed transmission credits.
	aSAP.LocalCreditDecrement();
	}

CRfcommDataFrame* TRfcommFlowStrategyCreditBased::NewDataFrame(CRfcommMuxer& aMuxer, 
												   TUint8 aAddr, 
												   TInt aLen, 
												   TUint8 aCredit,
												   CRfcommSAP* aSAP)
	/**
	   Returns a new data (UIH) frame for the specified DLCI

	   The C/R bit for the UIH frame is always 1 for the initiator and
	   0 for the responder, which is the same as for commands.
	**/
	{
	LOG_FUNC
	CRfcommDataFrame* frm=NULL;
	TInt err = 0;
	if(aCredit)
		{
		TRAP(err, frm=CRfcommCreditDataFrame::NewL(aLen, aMuxer, aSAP));
		if(!err)
			{
			frm->SetAddress(aAddr);
			frm->SetControl(KUIHCBFCCtrlField); //FIXME - redone later
			((CRfcommCreditDataFrame*)frm)->SetCredit(aCredit);
			}
		}
	else
		{
		TRAP(err, frm=CRfcommDataFrame::NewL(aLen, aMuxer, aSAP));
		if(!err)
			{
			frm->SetAddress(aAddr);
			frm->SetControl(KUIHCtrlField); //FIXME - redone later ... Poll bit always 0 for UIH
			}
		}
	return frm;	//	NB should be NULL if CRfcommDataFrame was not created
	}

TBool TRfcommFlowStrategyCreditBased::CanProcessNewData(const TBool /*aAllowProcessNewData*/) const
	/**
	  CBFC is on (=>SAP wise flow control). This function is for MUX wide 
	  flow control, and so should ignore all flags and never block here.
	**/
	{
	LOG_FUNC
	return ETrue;
	}

void TRfcommFlowStrategyCreditBased::DecodeLength(TBool aCBFC, 
											      TDesC8& aPacket, 
											      TInt& aCreditBuffer, 
											      TInt& aHeaderLengthBuffer)
	/**
		Provides the credit buffer with the sent credit, and revises the
		header length buffer to take credit byte into account.
	**/
	{
	LOG_FUNC
	TInt cIndex = ((aPacket)[2] & 1)?3:4;
	if (aCBFC)
		{
		aCreditBuffer = aPacket[cIndex];
		aHeaderLengthBuffer++;	// data is 1 byte smaller
		LOG1(_L("RFCOMM: Incoming packet: value in credit field is %d"), aCreditBuffer);
		}
	else
		{
		LOG(_L("RFCOMM: No credit field in incoming packet"));
		}
	}

TBool TRfcommFlowStrategyCreditBased::ProcessDataFrameReviseCredits(CRfcommSAP& aSAP, 
													   TBool aPoll,
													   TUint8 aCredit)
	/**
	   Add 'aCredit' (discovered in incoming data frame)to our credit.
	   If as a result we have credits when non had previously existed, 
	   return true to unblock SAP.
	**/
	//credit change
	{
	LOG_FUNC
	LOG(_L("CRfcommFlowSAPWise::ProcessDataFrameReviseCredits"));
	if (aPoll)
		{
		TBool isUnblock = (!(aSAP.LocalCredit()) && aCredit)?ETrue:EFalse;
		aSAP.LocalCreditAddCredit(aCredit);
		if (isUnblock)
			{
			LOG(_L("RFCOMM: Transmit credit was zero."));
			if (aSAP.SendBlocked() & CRfcommSAP::EBlockedByMux)
				{
				return ETrue;
				}
			}
		}
	else
		{
		LOG(_L("RFCOMM: Data with no Tx Credit"));
		}
	return EFalse;
	}

TBool TRfcommFlowStrategyCreditBased::MSC(CRfcommSAP& aSAP, TUint8 /*aSignals*/)
	/**
	  "Clear To Send" must be set to true. Flow control is handled
	  only by credits. See "AllowWrite()".
	**/
	{
	LOG_FUNC
	aSAP.CTS(ETrue);
	return ETrue;
	}

void TRfcommFlowStrategyCreditBased::UpdateRxFlowControlState(CRfcommSAP& aSAP)
	/**
	  Donate credits if possible when data is read off SAPs ring buffer.
	  Note: Function was called Read() originally, but actually is independent
	  of anything to do with reading.
	**/
	{
	LOG_FUNC
	if(aSAP.ProxyForRemoteCredit() <= KRfcommCreditsLowWaterMark)
		{
		TUint8 spareCredits = aSAP.FreeCredit();
		if (spareCredits && spareCredits>=aSAP.ProxyForRemoteCredit())
			{// We've got a worth while no. of credits to forward. Do a donation.
			aSAP.Mux()->Donate(aSAP, spareCredits);	// send an empty frame with credits
			}
		}
	LOG2(_L("RFCOMM: After: RxCr %d, ReUsed (or Free) %d"), aSAP.ProxyForRemoteCredit(), aSAP.FreeCreditCalculation());
	}

TBool TRfcommFlowStrategyCreditBased::NewDataReviseCredits(CRfcommSAP& aSAP, const TDesC8& aData)
	/**
		Revise proxy credit downwards by one if a frame has arrived containing 
		new data.
	**/
	//credit change
	{
	LOG_FUNC
	if (aData.Length() > 0)
		{
		if (aSAP.ProxyForRemoteCredit() == 0)
			{
			// got to disconnect cos they're ignoring our credits
			LOG(_L("RFCOMM: Data received with 0 receive credit"));
			return EFalse;
			}
		else
			{
			// update remote transmit credit proxy
			aSAP.ProxyForRemoteCreditDecrement();
			}
		}
	return ETrue;
	}

void TRfcommFlowStrategyCreditBased::NewData(CRfcommSAP& aSAP)
	/**
		If other side have run out of credits, check that we really
		don't have any to send them. This should rarely happen, as
		NotifyNewData tries to forward credits before they run out.
	**/
	{
	LOG_FUNC
	if (aSAP.ProxyForRemoteCredit() == 0)
		{// they have run out! See if we have any credits
		TUint8 spareCredits = aSAP.FreeCredit();
		if(spareCredits)
			{// got some credits! Send them.
			aSAP.Mux()->Donate(aSAP, spareCredits);	// an empty frame with credits
			}
		}
	return;
	}

CRfcommFlowStrategyFactory::TFlowStrategies TRfcommFlowStrategyCreditBased::FlowType()
	{
	LOG_FUNC
	return CRfcommFlowStrategyFactory::EFlowCreditBased;
	}






TRfcommFlowStrategyNonCreditBased::TRfcommFlowStrategyNonCreditBased(CRfcommFlowStrategyFactory* aFactory)
:TRfcommFlowStrategyInitial(aFactory) 
	{
	LOG_FUNC
	}

TRfcommFlowStrategyNonCreditBased::~TRfcommFlowStrategyNonCreditBased()
	{
	LOG_FUNC
	}



TInt TRfcommFlowStrategyNonCreditBased::PNConvergenceLayer(TBool /*aCommand*/)
	/**
	  Always allow 'FC On/Off'.
	**/
	{
	LOG_FUNC
	return 0x00;
	}

TUint8 TRfcommFlowStrategyNonCreditBased::PNFinalOctet()
	/**
	  Return 0.
	**/
	{
	LOG_FUNC
	return 0x00;
	}


CRfcommFlowStrategyFactory::TFlowStrategies TRfcommFlowStrategyNonCreditBased::FlowType()
	{
	LOG_FUNC
	return CRfcommFlowStrategyFactory::EFlowNonCreditBased;
	}

