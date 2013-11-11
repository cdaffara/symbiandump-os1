#include <e32debug.h>
// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology

*/


#include "rrlpassistancedata.h"
#include "rrlpassistancedataack.h"
#include "rrlpmeasureposrequest.h"
#include "rrlpmeasureposresponse.h"
#include "supldevloggermacros.h"
#include "suplrrlpstatemachine.h"
#include "suplpositioningprotocolfsm.inl"

// statics
const TInt CSuplRrlpFsm::KAssistanceDataChunkTimeout = 10; // Seconds 
const TInt CSuplRrlpFsm::KRequestTimeout = 60; // Seconds
const TInt CSuplRrlpFsm::KResponseDelayAddition = 1000; // Micro-Seconds
const TInt CSuplRrlpFsm::KResponseDelayTimeout = 60; // Seconds
const TInt CSuplRrlpFsm::KMeasureRequestWithAssistanceDataDelay = 10000; // Micro-Seconds


// Construction
EXPORT_C CSuplRrlpFsm* CSuplRrlpFsm::NewL(MSuplPositioningProtocolFsmObserver& aObserver, RLbsAssistanceDataBuilderSet& aDataBuilder)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::NewL() Begin\n");
	CSuplRrlpFsm* self = new(ELeave)CSuplRrlpFsm(aObserver,aDataBuilder);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	SUPLLOG(ELogP1, "CSuplRrlpFsm::NewL() End\n");
	return self;
	}
	
CSuplRrlpFsm::CSuplRrlpFsm(MSuplPositioningProtocolFsmObserver& aObserver, RLbsAssistanceDataBuilderSet& aDataBuilder) :
	CSuplPositioningProtocolFsm(aObserver), iCurrentState(EStateNull), iAssistanceData(aDataBuilder)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::CSuplRrlpFsm() Begin\n");
	SUPLLOG(ELogP1, "CSuplRrlpFsm::CSuplRrlpFsm() End\n");
	}

void CSuplRrlpFsm::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ConstructL() Begin\n");
	iAssistanceDataChunkTimer = CLbsCallbackTimer::NewL(*this);
	iRequestTimer = CLbsCallbackTimer::NewL(*this);
	iResponseDelayTimer = CLbsCallbackTimer::NewL(*this);
	iMeasureRequestWithAssitanceDataDelayTimer = CLbsCallbackTimer::NewL(*this);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ConstructL() End\n");
	}

CSuplRrlpFsm::~CSuplRrlpFsm()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::~CSuplRrlpFsm() Begin\n");
	delete iAssistanceDataChunkTimer;
	delete iRequestTimer;
	delete iResponseDelayTimer;
	delete iMeasureRequestWithAssitanceDataDelayTimer;
	SUPLLOG(ELogP1, "CSuplRrlpFsm::~CSuplRrlpFsm() End\n");
	}

// Base class functions
EXPORT_C void CSuplRrlpFsm::ProcessPositioningMessage(CSuplPosPayload* aMessage)
	{
	TInt message = aMessage->MessageType();
	SUPLLOG2(ELogP1, "CSuplRrlpFsm::ProcessPositioningMessage() Begin. Message: %d\n", message);
	
	// Set the reference number
	CRrlpMessageBase* base = static_cast<CRrlpMessageBase*>(aMessage);
	base->GetReference(iLastReferenceNumber);
	
	switch (message)
		{
		case CSuplPosPayload::ERrlpAssistanceData:
			{
			CRrlpAssistanceData* pAssData = static_cast<CRrlpAssistanceData*>(aMessage);

			HandleAssistanceDataMessage(pAssData);
			break;
			}
		case CSuplPosPayload::ERrlpMeasurePositionReq:
			{
			CRrlpMeasurePositionRequest* pReq = static_cast<CRrlpMeasurePositionRequest*>(aMessage);
			HandleMeasurementPositionRequest(pReq);
			break;
			}
		case CSuplPosPayload::ERrlpProtocolError:
			{
			CRrlpProtocolError* pErr = static_cast<CRrlpProtocolError*>(aMessage);
			HandleProtocolError(pErr);
			break;
			}
		default:
			{
			iObserver.PositioningProtocolError(KErrNotFound);
			}
		} // switch

	delete aMessage;
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ProcessPositioningMessage() End\n");
	}

EXPORT_C void CSuplRrlpFsm::CancelMachine(const TCancelSource& /*aCancelSource*/, TInt /*aReason*/)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::CancelMachine() Begin\n");
	iResponseDelayTimer->Cancel();
	iAssistanceDataChunkTimer->Cancel();
	iRequestTimer->Cancel();
	iMeasureRequestWithAssitanceDataDelayTimer->Cancel();
	
	TransistionTo(EStateNull);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::CancelMachine() End\n");
	}

EXPORT_C void CSuplRrlpFsm::AssistanceDataRequest(const TLbsAssistanceDataGroup& aData)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::AssistanceDataRequest() Begin\n");
	if (TransistionTo(EStateMoreAssistDataRequested))
		{
		iResponseDelayTimer->Cancel();
		CRrlpMeasurePositionResponse* pResp = NULL;
		TRAPD(err, pResp = CRrlpMeasurePositionResponse::NewL());
		if (err == KErrNone)
			{
			AddReference(*pResp);
			pResp->SetLocationError(ERrlpLocErrorGpsAssDataMissing, aData); 
			
			iObserver.PositioningPayloadToNetwork(pResp);
			pResp = NULL;
			} // if
		
		if (err != KErrNone)
			{
			TransistionToError(err);
			} // if
		} // if
	SUPLLOG(ELogP1, "CSuplRrlpFsm::AssistanceDataRequest() End\n");
	}

EXPORT_C void CSuplRrlpFsm::LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::LocationResp() Begin\n");
	// No call to TrnasistionTo as we to ignore if we are not in the correct state
	if (iCurrentState == EStateRequestHandled) 
		{
		SUPLLOG(ELogP1, "iCurrentState == EStateRequestHandled\n");
		if(aReason != KErrNone)	
			{
			SUPLLOG2(ELogP1, "Location error %d\n", aReason);
			TransistionToError(aReason);
			}
		else
			{
			iCurrentState = EStateMeasureRespSent;
			
			// Cancel the timeout timer
			iResponseDelayTimer->Cancel();
			
			CRrlpMeasurePositionResponse* pResp = NULL;
			TRAPD(err, pResp = CRrlpMeasurePositionResponse::NewL());
			if (err == KErrNone)
				{
				AddReference(*pResp);
	
				// Is aPosInfo measurements or position?
				TPositionInfoBase* pBase = const_cast<TPositionInfoBase*>(&aPosInfo);
				
				if ((aPosInfo.PositionClassType() & EPositionGpsMeasurementInfoClass))
					{
					err = pResp->SetMeasurementInformation(*pBase);
					}
				else
					{
					err = pResp->SetLocationInformation(*pBase);
					}
				
				if (err == KErrNone)
					{
					iObserver.PositioningSessionEnded();				
					iObserver.PositioningPayloadToNetwork(pResp);
					pResp = NULL;
					TransistionTo(EStateNull);				
					}
				else
					{
					delete pResp;
					}
				} // if
			if (err != KErrNone)
				{
				TransistionToError(err);
				} // if
			} // else
		} // if
	SUPLLOG(ELogP1, "CSuplRrlpFsm::LocationResp() End\n");
	}

EXPORT_C bool CSuplRrlpFsm::IsAssistDataRequestAllowed()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::IsAssistDataRequestAllowed() Begin\n");
	SUPLLOG(ELogP1, "CSuplRrlpFsm::IsAssistDataRequestAllowed() End\n");
	return ((iCurrentState == EStateNull) || (iCurrentState == EStateRequestHandled));
	}


// Handler functions
void CSuplRrlpFsm::HandleAssistanceDataMessage(CRrlpAssistanceData* aData)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::HandleAssistanceDataMessage() Begin\n");
	TLbsAsistanceDataGroup dataMask = 0;
	if (TransistionTo(EStateAssitDataChunkRecvd))
		{
		// Kick off the timer
		StartAssitanceDataChunkTimer();
		
		TInt err = KErrNone;
		
		if ((err == KErrNone) && (aData->AssistanceDataPresent())) // It is possible for an empty assistance data message
																   			// we just ignore it
			{
			err = aData->BuildAssistanceData(dataMask, iAssistanceData);
			} // if
		
		if (err == KErrNone)
			{
			iAssistanceDataMask |= dataMask;
			// Ack the assistance data
			CRrlpAssistanceDataAck* pAck = NULL;
			TRAP(err, pAck = CRrlpAssistanceDataAck::NewL());
			if (err == KErrNone)
				{
				if(!aData->MoreAssDataToBeSent())
					{
					// Inform observer that these could be the last
					// RRLP messages sent.
					iObserver.PositioningSessionEnded();
					}
				AddReference(*pAck);
				iObserver.PositioningPayloadToNetwork(pAck);
				pAck = NULL;
				}
			}
		
		if ((err == KErrNone) && (!aData->MoreAssDataToBeSent()))
			{
			TransistionTo(EStateAssitDataAcknowledged);
			
			// Pass the assistance data to LBS
			iObserver.ProcessAssistanceData(iAssistanceDataMask,KErrNone);
			iAssistanceDataMask = 0;
			
			// Start the request timer
			iAssistanceDataChunkTimer->Cancel();
			StartRequestTimer();
 
			} // if
		
		if (err != KErrNone)
			{
			TransistionToError(err);
			}
		} // if
	SUPLLOG(ELogP1, "CSuplRrlpFsm::HandleAssistanceDataMessage() End\n");
	}

void CSuplRrlpFsm::HandleMeasurementPositionRequest(CRrlpMeasurePositionRequest* aReq)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::HandleMeasurementPositionRequest() Begin\n");
	TLbsAsistanceDataGroup dataMask = 0;
	if (TransistionTo(EStateMeasureReqRecvd))
		{
		// Cancel any outstanding timers
		iAssistanceDataChunkTimer->Cancel(); // We could be expecting an assistance data chunk
		// Is there assistance data
		TInt delay = 0;
		TInt err = KErrNone;
		if (aReq->AssistanceDataPresent())
			{
			err = aReq->BuildAssistanceData(dataMask, iAssistanceData);
			iAssistanceDataMask |= dataMask;
			if (err == KErrNone)
				{
				// Pass the assistance data to LBS
				iObserver.ProcessAssistanceData(iAssistanceDataMask,KErrNone);
				iAssistanceDataMask = 0;
				delay = KMeasureRequestWithAssistanceDataDelay;
				} // if
			} // if
		
		if (err == KErrNone)
			{
			// Store quality and method for later request
			aReq->GetPositionInstruct(iLocReqQuality, iPosMethod);
			
			// Start a timer to give LBS a chance to handle the assistance data(this could be 0)
			StartMeasureRequestWithAssitanceDataTimer(delay);
			}
		
		if (err != KErrNone)
			{
			TransistionToError(err);
			}
		} // if
	SUPLLOG(ELogP1, "CSuplRrlpFsm::HandleMeasurementPositionRequest() End\n");
	}

void CSuplRrlpFsm::HandleProtocolError(CRrlpProtocolError* aError)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::HandleProtocolError() Begin\n");
	if (TransistionTo(EStateNull))
		{
		TRrlpErrorCode error;
		aError->GetProtocolError(error);
		iObserver.PositioningProtocolError(error);
		}
	SUPLLOG(ELogP1, "CSuplRrlpFsm::HandleProtocolError() End\n");
	}

// Timers
// MLbsCallbackTimerObserver methods
void CSuplRrlpFsm::OnTimerEventL(TInt aTimerId)
	{
	SUPLLOG2(ELogP1, "CSuplRrlpFsm::OnTimerEventL() Begin, aTimerId: %d\n", aTimerId);
	switch (aTimerId)
		{
		case EAssitanceDataChunk:
			ReceivedAssistanceDataChunkTimer();
			break;
		case ERequest:
			ReceivedRequestTimer();
			break;
		case EResponseDelay:
			ReceivedPosResultTimer();
			break;
		case EMeasureRequestWithAssistanceDataDelay:
			ReceivedMeasureRequestWithAssitanceDataTimer();
			break;
		default:
			ASSERT(EFalse);
		} // switch
	SUPLLOG(ELogP1, "CSuplRrlpFsm::OnTimerEventL() End\n");
	}

TInt CSuplRrlpFsm::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::OnTimerEventL() Begin\n");
	return aError;
	SUPLLOG(ELogP1, "CSuplRrlpFsm::OnTimerEventL() End\n");
	}

void CSuplRrlpFsm::StartAssitanceDataChunkTimer()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::StartAssitanceDataChunkTimer() Begin\n");
	// Cancel current one if running
	iAssistanceDataChunkTimer->Cancel();
	
	// Start again
	TTimeIntervalSeconds time(KAssistanceDataChunkTimeout);
	iAssistanceDataChunkTimer->EventAfter(time, EAssitanceDataChunk);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::StartAssitanceDataChunkTimer() End\n");
	}

void CSuplRrlpFsm::ReceivedAssistanceDataChunkTimer()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ReceivedAssistanceDataChunkTimer() Begin\n");
	// If we receive this then there has been a timeout in receiving the assistance data chunks
	// this is an error
	iObserver.PositioningProtocolError(KErrTimedOut);
	TransistionTo(EStateNull);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ReceivedAssistanceDataChunkTimer() End\n");
	}

void CSuplRrlpFsm::StartRequestTimer()   
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::StartRequestTimer() Begin\n");
	// Cancel this it its running
	iRequestTimer->Cancel();
	
	// Start timer
	TTimeIntervalSeconds time(KRequestTimeout);
	iRequestTimer->EventAfter(time, ERequest);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::StartRequestTimer() End\n");
	}

void CSuplRrlpFsm::ReceivedRequestTimer()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ReceivedRequestTimer() Begin\n");
	// If we receive this then there has been a timeout in receiving a measurement request
	// after receiving assistance data
	iObserver.PositioningProtocolError(KErrTimedOut);
	TransistionTo(EStateNull);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ReceivedRequestTimer() End\n");
	}

void CSuplRrlpFsm::StartPosResultTimer(TTimeIntervalMicroSeconds aTime) 
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::StartPosResultTimer() Begin\n");
	// Add on the extra time to allow it to filter through from LBS
	aTime = aTime.Int64() + KResponseDelayAddition;
	
	iResponseDelayTimer->EventAfter(aTime, EResponseDelay);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::StartPosResultTimer() End\n");
	}

void CSuplRrlpFsm::ReceivedPosResultTimer()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ReceivedPosResultTimer() Begin\n");
	// If we recieve this then LBS has timed out
	iObserver.PositioningProtocolError(KErrTimedOut);
	TransistionTo(EStateNull);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ReceivedPosResultTimer() End\n");
	}

void CSuplRrlpFsm::StartMeasureRequestWithAssitanceDataTimer(TInt aTimer)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::StartMeasureRequestWithAssitanceDataTimer() Begin\n");
	TTimeIntervalMicroSeconds time(aTimer);
	iAssistanceDataChunkTimer->EventAfter(time, EMeasureRequestWithAssistanceDataDelay);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::StartMeasureRequestWithAssitanceDataTimer() End\n");
	}
void CSuplRrlpFsm::ReceivedMeasureRequestWithAssitanceDataTimer()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ReceivedMeasureRequestWithAssitanceDataTimer() Begin\n");
	if (TransistionTo(EStateRequestHandled))
		{
		iObserver.ProcessPositioningRequest(iLocReqQuality, iPosMethod);
		
		// Expect results from LBS before the MaxFixTime elapses (allow an
		// additional second for the results to propagate through LBS) 
		StartPosResultTimer(iLocReqQuality.MaxFixTime().Int64() + (1000*KResponseDelayAddition));
		}
	SUPLLOG(ELogP1, "CSuplRrlpFsm::ReceivedMeasureRequestWithAssitanceDataTimer() End\n");
	}


// Others
TBool CSuplRrlpFsm::TransistionTo(TRrlpState aNewState)
	{
	SUPLLOG3(ELogP1, "CSuplRrlpFsm::TransistionTo() Begin, iCurrentState %d, aNewState %d\n", iCurrentState, aNewState);
	
	TBool ret = EFalse;
	
	if (aNewState != EStateNull)
		{
		switch (iCurrentState)
			{
			case EStateNull:
			case EStateAssitDataChunkRecvd:
				{
				ret = (aNewState == EStateAssitDataChunkRecvd) ||
				      (aNewState == EStateAssitDataAcknowledged) ||
				      (aNewState == EStateMeasureReqRecvd);
				break;
				}
			case EStateAssitDataAcknowledged:
				{
				ret = (aNewState == EStateMeasureReqRecvd);
				break;
				}
			case EStateMeasureReqRecvd:
				{
				ret = (aNewState == EStateRequestHandled);
				break;
				}
			case EStateRequestHandled:
				{
				ret = (aNewState == EStateMoreAssistDataRequested) ||
					  (aNewState == EStateMeasureRespSent);
				break;
				}
			case EStateMeasureRespSent:
				{
				ret = (aNewState == EStateNull);
				break;
				}
			case EStateMoreAssistDataRequested:
				{
				ret = (aNewState == EStateMeasureReqRecvd);
				break;	
				}
			}
		}
	else
		{
		ret = ETrue;
		}

	if (ret)
		{
		iCurrentState = aNewState;
		}
	else
		{
		TransistionToError(KErrGeneral);
		}
	
	SUPLLOG(ELogP1, "CSuplRrlpFsm::TransistionTo() End\n");
	return ret;
	}

void CSuplRrlpFsm::TransistionToError(TInt aError)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::TransistionToError() Begin\n");
	
	iObserver.PositioningProtocolError(aError);
	
	// Prevent delayed actions from happening
	iResponseDelayTimer->Cancel();
	iAssistanceDataChunkTimer->Cancel();
	iRequestTimer->Cancel();
	iMeasureRequestWithAssitanceDataDelayTimer->Cancel();
	
	// Go back to null state
	TransistionTo(EStateNull);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::TransistionToError() End\n");
	}

void CSuplRrlpFsm::AddReference(CRrlpMessageBase& aMessage)
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::AddReference() Begin\n");
	aMessage.SetReference(iLastReferenceNumber);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::AddReference() End\n");
	}


// Unused functions from base class
void CSuplRrlpFsm::RunL()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::RunL() Begin\n");
	ASSERT(EFalse);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::RunL() End\n");
	}
void CSuplRrlpFsm::DoCancel()
	{
	SUPLLOG(ELogP1, "CSuplRrlpFsm::DoCancel() Begin\n");
	ASSERT(EFalse);
	SUPLLOG(ELogP1, "CSuplRrlpFsm::DoCancel() End\n");
	}

