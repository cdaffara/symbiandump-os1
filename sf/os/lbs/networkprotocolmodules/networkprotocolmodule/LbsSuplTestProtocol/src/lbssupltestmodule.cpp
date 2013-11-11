// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file provides the class implementation of the ECOM plug-in 
// interface for the LBS test Network Protocol Module.
// 
//

#include "lbssupltestmodule.h"
#include "lbssuplsocketlistener.h"
#include <ecom/implementationproxy.h>

//const TUint32 KInetAddr1 = INET_ADDR(209,172,70,68);//SiRF server
const TUint32 KInetAddr2 = INET_ADDR(66,230,192,56);//SiRF sever
const TUint32 KLoopBackAddr = INET_ADDR(127,0,0,1);//Used by SiRF in session ID of SUPL RESPONSE -proxy mode-
const TUint KSuplPort =  7276; // standard SUPL TCP port.
const TInt KNetworkTimeout = 45000000;
const TInt KProtocolTimeout = 50000000;
const TInt KMaxTxBufferSize = 2048; // 2k Max size of SUPL messages sent
const TInt KMaxRxBufferSize = 8192; // 8k Max size of SUPL messages received

// --------------------- SUPL TEST MODULE (ECom plug-in) --------------------------

/** The unique ID of this plug-in interface.
This corresponds to the interface UID specified in the .rss file
for this SUPL test module.
*/
const TInt KPluginUidValue = 0x1028225B;

CSuplProtocolModule* CSuplProtocolModule::NewL(TAny* aInitParams)
	{
	CSuplProtocolModule* self=new(ELeave) CSuplProtocolModule(aInitParams);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSuplProtocolModule::~CSuplProtocolModule()
	{
	iSocketServerSession.Close();
	}


CSuplProtocolModule::CSuplProtocolModule(TAny* aInitParams): 
	iObserver(REINTERPRET_CAST(
	          TAssistanceDataSourceModuleConstructionParams*,
	          aInitParams)->iAssistanceDataSourceObserver),
	iNextSessionNumber(0)
	{
	}

void CSuplProtocolModule::ConstructL()
	{
	// Open channel to Socket Server
	User::LeaveIfError(iSocketServerSession.Connect());
	}

CAssistanceDataSourceBase* CSuplProtocolModule::NewDataSourceL()
	{
	// Create a new number for the session
	if (iNextSessionNumber == 1000)
	{
		iNextSessionNumber = 0;
	}
	
	iNextSessionNumber++;
	
	return CSuplProtocolSession::NewL(iObserver, iSocketServerSession, iNextSessionNumber);
	}


/** Defines the plug-in interface implementation UIDs, required by ECOM.
*/
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KPluginUidValue, CSuplProtocolModule::NewL)
    };
    

/** Exported function for ECOM to retrieve implementations table
*/
EXPORT_C 
const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }


//--------------------- SUPL Session ---------------------------------------------------------

CSuplProtocolSession::CSuplProtocolSession(MLbsAssistanceDataSourceObserver& aObserver, const TInt16& aSessionNumber): 
	CActive(EPriorityStandard),
    iObserver(&aObserver),
    iBuilderSet(NULL),
    iSessionNumber(aSessionNumber)
	{
	// Attach to active scheduler
	//
	CActiveScheduler::Add(this);
	
	iAddress.SetPort(KSuplPort);
	iAddress.SetAddress(KInetAddr2);
	}

void CSuplProtocolSession::ConstructL(RSocketServ& aSocketServer)
	{
	iTxBuf.CreateMaxL(KMaxTxBufferSize);
	iRxBuf.CreateMaxL(KMaxRxBufferSize);
	// Open a TCP socket
	User::LeaveIfError(iSocket.Open(aSocketServer, KAfInet, KSockStream, KProtocolInetTcp));


    // Set SUPL port in socket
    //
    iSocket.SetLocalPort(KSuplPort);
    
	// Create a socket listener for SUPL messages
	iSuplSocketListener = CSuplSocketListener::NewL(iSocket, iRxBuf);

	// Create the timer
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	}

CSuplProtocolSession::~CSuplProtocolSession()
	{
		Cancel();
		iTxBuf.Close();
		iRxBuf.Close();
		delete iTimer;
		delete iSuplSocketListener;
		iSocket.Close();
	}

/**
GetAssistanceData() Called by Network Simulator to request assitance data from the remote
server.

@parameter aBuilderSet. The CSuplProtocolSession will set the fields of this builder with
			the assistance data.
*/
void CSuplProtocolSession::GetAssistanceData(RLbsAssistanceDataBuilderSet& aBuilderSet, const TLbsAssistanceDataGroup& aAssistanceDataFilter, const TCoordinate& aPosition)
	{

	iBuilderSet = &aBuilderSet;
	iAssistanceDataFilter = aAssistanceDataFilter;
	iPosition = aPosition;

    // Validate parameters
	if(aAssistanceDataFilter == 0)
		{
		iObserver->DataSourceNotification(KErrArgument);
		}
	else
		{
		// Set the correct state and force a transition of the state machine
		if(iSessionState == EConnectedAwaitingRequest)
			{
			// Transition out of wait state
			iSessionState = EAssistanceDataRequested;
			}
		else
			{
			// If in the process of collecting assistance data, cancel previous request and
			// start over.
			// Do not notify NetSim that previous assistance data retrieval won't be completed
			// (it should know)
			CancelGetAssistanceData();

			// Flag that a request is pending
			iAssistanceDataRequestPending = true;
			}
			
		// Transition out of inactive states (EIdle and EConnectedAwaitingState)
		if(!IsActive())
			{
			PrepareSelfTransition();
			}
		}
	}


CSuplProtocolSession* CSuplProtocolSession::NewL(MLbsAssistanceDataSourceObserver& aObserver,RSocketServ& aSocketServer, const TInt16& aSessionNumber)
	{
	CSuplProtocolSession* self= new(ELeave) CSuplProtocolSession(aObserver,aSessionNumber);
	CleanupStack::PushL(self);
	self->ConstructL(aSocketServer);
	CleanupStack::Pop();
	return self;
	}

void CSuplProtocolSession::RunL()
	{
	iTimer->Cancel();
	
	// Deal with errors in socket operations
	if (iStatus != KErrNone)
	{
		if ((iSessionState == EConnectingToRemoteServer) && (!iAssistanceDataRequestPending))
			{
			// Disconnect without reporting to NetSim
			iSessionState = EDisconnectingFromServer;
			}
		else if ((iAssistanceDataRequestPending) && (iStatus == KErrCancel))
			{
			// Assume the cancellation has been due to a new request from NetSim
			// Start over without reporting to NetSim.
			iSessionState = EConnectingToRemoteServer;
			}
		else
			{
			// start disconnection and report to NetSim
			iSessionState = ENetworkTimeout;
			}
	}
	
	switch(iSessionState)
		{
		
		case EIdle:
			ConnectToRemoteServer();	
			break;
		
		case EConnectingToRemoteServer:
			TransitionAccordingToRequestAvailability();
			break;
		
		case EConnectedAwaitingRequest:
			// Nothing to be done in this state.
			// This state is left when the Network
			// Simulator requests assistance data
			// (see GetAssistanceData())
			break;
			
		case EAssistanceDataRequested:
			iAssistanceDataRequestPending = EFalse;
			iSessionState = ESendingSuplStart;
			SendSuplMessageL(ESuplStart);
			break;
		
		case ESendingSuplStart:
			// SuplStart sent. Wait for SuplResponse
			iSessionState = EAwaitingSuplResponse;
			ReadSuplMessage();
			break;
			
		case EAwaitingSuplResponse:
		    iSessionState = ESuplResponseAvailable;
			DecodeSuplMessageL(ESuplResponse); //May set error state
		    break;
		    
		case ESuplResponseAvailable:
			iSessionState = ESendingSuplPosInit;
			SendSuplMessageL(ESuplPosInit);	
			break;
			
		case ESendingSuplPosInit:
			iSessionState = EAwaitingSuplPos;
			ReadSuplMessage();
			break;
			
		case EAwaitingSuplPos:
			iSessionState = ESuplPosAvailable;
			DecodeSuplMessageL(ESuplPos);	//May set error state
			break;
			
		case ESuplPosAvailable:
		    iSessionState = ERrcPayloadDecodeComplete;
			DecodeRRCPayloadL();
			PrepareSelfTransition();
			break;
			
		case ERrcPayloadDecodeComplete:
			iSessionState = EEndingSession;
			// Notify succesful exit to NetSim
			iObserver->DataSourceNotification(KErrNone);
			PrepareSelfTransition();
			break;
			
		case EProtocolError:
			//Unexpected message. Notify unsuccesfull exit to NetSim
			iObserver->DataSourceNotification(KErrCancel);
			iAssistanceDataRequestPending = EFalse;
			iSessionState = EDisconnectingFromServer;
			PrepareSelfTransition();
			break;
			
		case ENetworkTimeout:
			// Report to NetSim that something went wrong
			iObserver->DataSourceNotification(iStatus.Int());
			iAssistanceDataRequestPending = EFalse;
			iSessionState = EDisconnectingFromServer;
			PrepareSelfTransition();
			break;
			
		case EDisconnectingFromServer:
			iSessionState = EIdle;
			iSocket.Shutdown(RSocket::ENormal,iStatus);
			SetActive();
			break;
		
		case EEndingSession:
			iSessionState = ESuplEndSent;
			SendSuplMessageL(ESuplEnd);
			break;

		case ESuplEndSent:
 			iSessionState = EConnectingToRemoteServer;
			ReadSuplMessage();
			break;

		default:
			Cancel();
			User::Leave(KErrGeneral);
			break;
		}
	}

/**
CancelGetAssistanceData. Cancels a previous request
for Assistance Data and starts actions to bring the state
machine to EConnectedAwaitingRequest again, where a new
request for assistance data can be serviced.

If the session had been started with the remote server 
to obtain Assistance Data, the session is first brought
down and the state machine will autonomously reconnect and
move on to EConnectedAwaitingRequest state.

The Nework Simulator will not receive a notification of
cancellation of the previous request for assistance data.

Note this cancelling method is unrelated to
CActive::Cancel()/DoCancel(). The state machine will remain
active and method RunL is expected to be called after this
method completes (unlike DoCancel()).

@return Symbian error code. KErrNone if OK.
*/
void CSuplProtocolSession::CancelGetAssistanceData()
	{

	iTimer->Cancel();
	// Do not service pending requests
	iAssistanceDataRequestPending = false;
	switch (iSessionState)
		{
		case EIdle:
		case EConnectingToRemoteServer:
		case EConnectedAwaitingRequest:
		case EDisconnectingFromServer:
		case ENetworkTimeout:
		case EProtocolError:
		case EEndingSession:
		// nothing to do (session not getting assistance data)
		break;

		case EAssistanceDataRequested:
		// Session not started with remote server yet.
		// RunL will be called shortly as a self transition
		// was prepared when this state was entered.
		// Put the state machine in a position of
		// transitioning towards EConnectedAwaitingRequest state.
		iSessionState = EConnectingToRemoteServer;			
		break;

		case ESendingSuplStart:
		case ESendingSuplPosInit:
		// Cancel send operation.
		// RunL will process KErrCancel case,
		// setting the state to EDisconnectingFromServer.
		iSocket.CancelWrite();
		break;

		case EAwaitingSuplResponse:
		case EAwaitingSuplPos:
		case ESuplEndSent:
		// Cancel receive operation
		// RunL will process KErrCancel case,
		// setting the state to EDisconnectingFromServer.
		iSuplSocketListener->Cancel();
		break;

		case ESuplResponseAvailable:
		case ESuplPosAvailable:
		case ERrcPayloadDecodeComplete:
		// These three states have prepared
		// self-transaction. When RunL gets
		// executed, disconnect:
		iSessionState = EDisconnectingFromServer;
		break;

		default:
		// Not a valid state
		User::Invariant();
		break;
		}
	}
	
	
TInt CSuplProtocolSession::OnTimerEventL(TAny* aPtr)
	{
	CSuplProtocolSession* aSession = STATIC_CAST(CSuplProtocolSession*,aPtr);
	if (aSession)
		{
		aSession->DoProcessTimeoutL();
		}
	return KErrNone;
	}

void CSuplProtocolSession::DoProcessTimeoutL()
{
	iTimer->Cancel();
	
	// Cancel async tasks to re-enter RunL with
	// error code KErrCancel
	switch(iSessionState)
	{
		case EConnectingToRemoteServer:
			iSocket.CancelConnect();
			break;
		case ESendingSuplStart:
		case ESendingSuplPosInit:
			iSocket.CancelWrite();
			break;
		case EAwaitingSuplPos:
		case EAwaitingSuplResponse:
			iSuplSocketListener->Cancel();
			break;
		default:
			// No other state should have a timer running
			Cancel();
			User::Leave(KErrGeneral);
			break;
	}
}


void CSuplProtocolSession::CreateSuplStartMessageL()
	{
	
	CSuplMessageContent* suplMsgContent=CSuplMessageContent::NewLC();
	
	// Build common part
	suplMsgContent->iMessageChoice=ESuplStart;	
	BuildCommonPartL(suplMsgContent);
	
	suplMsgContent->iSuplStartContent=CSuplStartContent::NewL();

	// Qop field is optional BUT required for the SiRF server to work with RRC
	TBool qopPresent = ETrue;
	suplMsgContent->iSuplStartContent->iQopPresent=qopPresent;
	
	// Set capabilities (mandatory)
	suplMsgContent->iSuplStartContent->iSetCapabilitiesContent=CSuplSetCapabilitiesContent::NewL();
	BuildSetCapabilitiesL(suplMsgContent->iSuplStartContent->iSetCapabilitiesContent);
	
	//LocationId --- CellInfo
	suplMsgContent->iSuplStartContent->iLocationIdContent=CSuplLocationIdContent::NewL();
	suplMsgContent->iSuplStartContent->iLocationIdContent->iCellInfoContent=CSuplCellInfoContent::NewL();
	BuildCellInfoL(suplMsgContent->iSuplStartContent->iLocationIdContent->iCellInfoContent, EGsmCellInformation); // Verify EGsmCellInformation
	suplMsgContent->iSuplStartContent->iLocationIdContent->iLocationStatus= ECurrent; //TSuplLocationStatus

	// QoP
	suplMsgContent->iSuplStartContent->iQoPContent=CSuplQoPContent::NewL();
	suplMsgContent->iSuplStartContent->iQoPContent->iVeraccPresent = EFalse;
	suplMsgContent->iSuplStartContent->iQoPContent->iMaxLocAgePresent = EFalse;
	suplMsgContent->iSuplStartContent->iQoPContent->iDelayPresent = EFalse;
	suplMsgContent->iSuplStartContent->iQoPContent->iHoracc= 20; // Random value !!!


	CSuplMessage* suplMessageEncoder=CSuplMessage::NewLC(*suplMsgContent);
	
    // Encode message
    iTxBuf.SetLength(0);
	suplMessageEncoder->EncodeL(iTxBuf);
	
	CleanupStack::PopAndDestroy(suplMessageEncoder);
	CleanupStack::PopAndDestroy(suplMsgContent);
	}

void CSuplProtocolSession::CreateSuplEndMessageL()
	{
	
	CSuplMessageContent* suplMsgContent=CSuplMessageContent::NewLC();
	
	// Build common part
	suplMsgContent->iMessageChoice=ESuplEnd;	
	BuildCommonPartL(suplMsgContent);
	suplMsgContent->iSuplEndContent=CSuplEndContent::NewL();
	
	// Position field should be false
	suplMsgContent->iSuplEndContent->iPositionPresent=EFalse;
	
	// Status Code field should be false
	suplMsgContent->iSuplEndContent->iStatusCodePresent=EFalse;

	// Ver field should be false
	suplMsgContent->iSuplEndContent->iVerPresent=EFalse;		

	CSuplMessage* suplMessageEncoder=CSuplMessage::NewLC(*suplMsgContent);
	
    // Encode message
    iTxBuf.SetLength(0);
	suplMessageEncoder->EncodeL(iTxBuf);
	
	CleanupStack::PopAndDestroy(suplMessageEncoder);
	CleanupStack::PopAndDestroy(suplMsgContent);
	}

void CSuplProtocolSession::CreatePosInitMessageL()
	{
	CSuplMessageContent* suplMsgContent=CSuplMessageContent::NewLC();
	
	// Build common part
	suplMsgContent->iMessageChoice = ESuplPosInit;
	BuildCommonPartL(suplMsgContent);
	suplMsgContent->iSuplPosInitContent = CSuplPosInitContent::NewL();

	// Request Assist Data field should be true
	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataPresent=ETrue;
	
	// PosInit field should be false
	suplMsgContent->iSuplPosInitContent->iSuplPosPresent=EFalse;
	
	// Position field should be true
	suplMsgContent->iSuplPosInitContent->iPositionPresent=ETrue;
	
	// Ver field should be false
	suplMsgContent->iSuplPosInitContent->iVerPresent=EFalse;
	suplMsgContent->iSuplPosInitContent->iSetCapContent=CSuplSetCapabilitiesContent::NewL();
	
	// Build Set Capabilities
	BuildSetCapabilitiesL(suplMsgContent->iSuplPosInitContent->iSetCapContent);
	
	// Set Assistance Data types requested
	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent=CSuplRequestedAssistDataContent::NewL();
	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iNavigationModelDataPresent=EFalse;// SiRF unsupported
	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iDgpsCorrectionsRequested=EFalse; // NOT SUPPORTED
	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iRealTimeIntegrityRequested=EFalse;// NOT SUPPORTED
	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iUtcModelRequested=EFalse; // NOT SUPPORTED BY SIRF
	
    if(iAssistanceDataFilter & EAssistanceDataAlmanac)
    	{
    	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iAlmanacRequested=ETrue;
   		}
   	else
   		{
   		suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iAlmanacRequested=EFalse;	
   		}
   		
	if(iAssistanceDataFilter & EAssistanceDataIonosphericModel)
    	{
    	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iIonosphericModelRequested=ETrue;
   		}
   	else
   		{
		suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iIonosphericModelRequested=EFalse;
   		}
   		
    if(iAssistanceDataFilter &  EAssistanceDataReferenceLocation)
    	{
    	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iReferenceLocationRequested=ETrue;
   		}
   	else
   		{
		suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iReferenceLocationRequested=ETrue;
   		}

    if(iAssistanceDataFilter &  EAssistanceDataReferenceTime)
    	{
    	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iReferenceTimeRequested=ETrue;
   		}
   	else
   		{
		suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iReferenceTimeRequested=EFalse;
   		}

    if(iAssistanceDataFilter &  EAssistanceDataAquisitionAssistance)
    	{
    	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iAcquisitionAssistanceRequested=ETrue;
   		}
   	else
   		{
		suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iAcquisitionAssistanceRequested=EFalse;
   		}

    if(iAssistanceDataFilter &  EAssistanceDataNavigationModel)
    	{
    	suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iNavigationModelRequested=ETrue;
   		}
   	else
   		{
		suplMsgContent->iSuplPosInitContent->iRequestedAssistDataContent->iNavigationModelRequested=EFalse;
   		}   

	// build cell info	
	suplMsgContent->iSuplPosInitContent->iLocationIdContent = CSuplLocationIdContent::NewL();
	suplMsgContent->iSuplPosInitContent->iLocationIdContent->iCellInfoContent=CSuplCellInfoContent::NewL();
	suplMsgContent->iSuplPosInitContent->iLocationIdContent->iLocationStatus = EStale; //TSuplLocationStatus
	BuildCellInfoL(suplMsgContent->iSuplPosInitContent->iLocationIdContent->iCellInfoContent,EGsmCellInformation);
	
	// build Position info
	suplMsgContent->iSuplPosInitContent->iPositionContent = CSuplPositionContent::NewL();
	BuildPositionInfoL(suplMsgContent->iSuplPosInitContent->iPositionContent);
	
	CSuplMessage* suplMessageEncoder=CSuplMessage::NewLC(*suplMsgContent);

	// Encode message
    iTxBuf.SetLength(0);
	suplMessageEncoder->EncodeL(iTxBuf);
	
	CleanupStack::PopAndDestroy(suplMessageEncoder);
	CleanupStack::PopAndDestroy(suplMsgContent);
	}

TSuplMessageType CSuplProtocolSession::DoDecodeMessageL()
	{
    CSuplMessage* suplMessageDecoder=CSuplMessage::NewLC();

    CSuplMessageContent* suplMessageResultContent=suplMessageDecoder->DecodeL(iRxBuf);
    CleanupStack::PushL(suplMessageResultContent);

	// Keep in the session the bits of information needed from the messages sent from the remote
	// server
	//
	switch (suplMessageResultContent->iMessageChoice)
		{
		case ESuplResponse:
			// Keep the SLP part of the session ID
			iSlpSessionId.Copy(suplMessageResultContent->iSuplSessionIdContent->iSlpSessionIdContent->iSessionId);
			break;
		case ESuplPos:
			// Keep the offset into the rx buffer where the RRC payload is
			iRrcOffset = suplMessageResultContent->iSuplPosContent->iPosPayloadContent->iPayloadStartBitPos;
			break;
		}
	
	TSuplMessageType messageType = static_cast<TSuplMessageType>(suplMessageResultContent->iMessageChoice);

    CleanupStack::PopAndDestroy(2, suplMessageDecoder);

	return messageType;
	}

void CSuplProtocolSession::DecodeRRCPayloadL()
	{
	CAssistanceDataDeliveryR3IEs* rrcPayLoad=CAssistanceDataDeliveryR3IEs::NewLC(iBuilderSet);
    rrcPayLoad->DecodeL(iRxBuf, iRrcOffset);
    CleanupStack::PopAndDestroy(rrcPayLoad);
	}

void CSuplProtocolSession::PrepareSelfTransition()
	{
	TRequestStatus* localStatus = &iStatus;
	iStatus = KRequestPending;
	SetActive();
	User::RequestComplete(localStatus, KErrNone);
	}

void CSuplProtocolSession::DoCancel()
	{
	iSocket.CancelWrite();
	iSuplSocketListener->Cancel();
	iTimer->Cancel();
	}

void CSuplProtocolSession::BuildSetCapabilitiesL(CSuplSetCapabilitiesContent* aContent)
	{
	// SETCAPABILITY --- PosTechnology (Set based and autonomous only)
	aContent->iPosTechnologyContent=CSuplPosTechnologyContent::NewL();
	aContent->iPosTechnologyContent->iAgpsSETassisted=EFalse;
	aContent->iPosTechnologyContent->iAgpsSETBased=ETrue;
	aContent->iPosTechnologyContent->iAutonomousGPS=ETrue;
	aContent->iPosTechnologyContent->iAFLT=EFalse;	
	aContent->iPosTechnologyContent->iECID=EFalse;
	aContent->iPosTechnologyContent->iEOTD=EFalse;	
	aContent->iPosTechnologyContent->iOTDOA=EFalse;

	// SETCAPABILITY --- PosMethod	 (prefer Set based)
	aContent->iPrefMethod = EAGpsSETBasedPreferred; //TSuplPrefMethod	
	
	// SETCAPABILITY --- PosProtocol (only RRC)
	aContent->iPosProtocolContent=CSuplPosProtocolContent::NewL();	
	aContent->iPosProtocolContent->iTia801=EFalse;	
	aContent->iPosProtocolContent->iRrlp=EFalse;	
	aContent->iPosProtocolContent->iRrc=ETrue;		
	}

void CSuplProtocolSession::BuildCellInfoL(CSuplCellInfoContent* aContent, TCellInformationChoice aCellInfoChoice)
	{
	
	aContent->iCellInformation = aCellInfoChoice;

	switch (aCellInfoChoice)
		{
		case EGsmCellInformation:
			{
			aContent->iGsmCellInformationContent=CSuplGsmCellInformationContent::NewL();
			TBool nmrPresent = EFalse; //Optional
			aContent->iGsmCellInformationContent->iNMRPresent=nmrPresent;

			TBool taPresent = EFalse; //Optional
			aContent->iGsmCellInformationContent->iTAPresent=taPresent;

			TInt refMCC = 234; // UK
			aContent->iGsmCellInformationContent->iRefMCC=refMCC;

			TInt refMNC = 101; // 101 (O2), 151 (voda), 301(t-mobile), 331 (orange)
			aContent->iGsmCellInformationContent->iRefMNC=refMNC;

			TInt refLAC = 12049; // Area code (it seems to be dependant on the network!)
			aContent->iGsmCellInformationContent->iRefLAC=refLAC;

			TInt refCI = 1103; // Entirely dependant on the operator.
			aContent->iGsmCellInformationContent->iRefCI=refCI;			
			}
			break;
		case EWcdmaCellInformation:
			{
			aContent->iWcdmaCellInformationContent=CSuplWcdmaCellInformationContent::NewL();
			TBool frequencyInfoPresent = EFalse;
			aContent->iWcdmaCellInformationContent->iFrequencyInfoPresent=frequencyInfoPresent;
			
			TBool primaryScramblingCodePresent = EFalse;
			aContent->iWcdmaCellInformationContent->iPrimaryScramblingCodePresent=primaryScramblingCodePresent;

			TBool measuredResultsListPresent = EFalse;
			aContent->iWcdmaCellInformationContent->iMeasuredResultsListPresent=measuredResultsListPresent;

			TInt refMCC = 34;
			aContent->iWcdmaCellInformationContent->iRefMCC=refMCC;

			TInt refMNC = 34; 
			aContent->iWcdmaCellInformationContent->iRefMNC=refMNC;

			TInt refUC =  1;
			aContent->iWcdmaCellInformationContent->iRefUC=refUC;				
			}
			break;
		case ECdmaCellInformation:
			{
			// All parameters are mandatory
			//
			aContent->iCdmaCellInformationContent=CSuplCdmaCellInformationContent::NewL();
			TInt refNID = 1;
			aContent->iCdmaCellInformationContent->iRefNID=refNID;
			
			TInt refSID = 1;
			aContent->iCdmaCellInformationContent->iRefSID=refSID;
			
			TInt refBASEID = 1;
			aContent->iCdmaCellInformationContent->iRefBASEID=refBASEID;

			TInt refBASELAT = 1;
			aContent->iCdmaCellInformationContent->iRefBASELAT=refBASELAT;
			
			TInt reBASELONG = 1;
			aContent->iCdmaCellInformationContent->iReBASELONG=reBASELONG;
			
			TInt refREFPN = 1;
			aContent->iCdmaCellInformationContent->iRefREFPN=refREFPN;
			
			TInt refWeekNumber = 1;
			aContent->iCdmaCellInformationContent->iRefWeekNumber=refWeekNumber;
			
			TInt refSeconds = 1;
			aContent->iCdmaCellInformationContent->iRefSeconds=refSeconds;				
			}
			break;
		}
	}

void CSuplProtocolSession::BuildPositionInfoL(CSuplPositionContent* posContent)
	{
	posContent->iPositionEstimateContent = CSuplPositionEstimateContent::NewL();
	
	if (iPosition.Latitude() >= 0)
		{
		posContent->iPositionEstimateContent->iLatitudeSign = ENorth;
		}
	else
		{
		posContent->iPositionEstimateContent->iLatitudeSign = ESouth;
		}
	
	posContent->iVelocityPresent = EFalse;
	posContent->iUtcTimeStamp.UniversalTime(); // Use the current time. SUPL server ignores actual value.
	
	posContent->iPositionEstimateContent->iUncertainty = EFalse;
	posContent->iPositionEstimateContent->iConfidence = EFalse;
	posContent->iPositionEstimateContent->iAltitudInfo = EFalse;
	
	// Set Location (in the future, get the NetSim to provide this)
	posContent->iPositionEstimateContent->iLatitude = static_cast<TUint32>((Abs(iPosition.Latitude()) * 8388608.0 + 0.5) / 90.0);
	posContent->iPositionEstimateContent->iLongitude = static_cast<TInt32>((iPosition.Longitude() * 2097152.0 + 0.5) / 45.0);
	}

void CSuplProtocolSession::BuildCommonPartL(CSuplMessageContent* aSuplMsgContent)
	{
	
	//Message Version
	TInt majorVersion =1;
	TInt minVersion = 0;
	TInt servindVersion =0;
	aSuplMsgContent->iSuplVersionContent=CSuplVersionContent::NewL();
	aSuplMsgContent->iSuplVersionContent->iMaj=majorVersion;
	aSuplMsgContent->iSuplVersionContent->iMin=minVersion;
	aSuplMsgContent->iSuplVersionContent->iServind=servindVersion;
	
	//SET Session ID
	TBool setsessionPresent = ETrue;
	aSuplMsgContent->iSuplSessionIdContent=CSuplSessionIdContent::NewL();
    aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdPresent=setsessionPresent;

	
	aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdContent=CSuplSetSessionIdContent::NewL();
	aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdContent->iSessionId=iSessionNumber;
	aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent=CSuplSetIdContent::NewL();
	aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iSetIdType= ESuplIPAddress; //TSuplSetIdType
	aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iIpAddressContent=CSuplIpAddressContent::NewL();
	aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iIpAddressContent->iIpAddressType=ESuplIpV4AddressChoice;//TSuplIpAddressType;
	
	
	// Find out own address from socket
	//
	TInetAddr localAddress;
	iSocket.LocalName(localAddress);
    TUint32 address = localAddress.Address();
	//aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iIpAddressContent->iIpAddress.Copy(address);
	TBuf8<4> theLocalAddress(4);
	TUint8 aByteOfTheAddress = 0;
	aByteOfTheAddress = address;
	theLocalAddress[3]= aByteOfTheAddress;
	aByteOfTheAddress = address >>8;
	theLocalAddress[2]= aByteOfTheAddress;
	aByteOfTheAddress = address >> 16;
	theLocalAddress[1]= aByteOfTheAddress;
	aByteOfTheAddress = address >> 24;
	theLocalAddress[0]= aByteOfTheAddress;
	aSuplMsgContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iIpAddressContent->iIpAddress.Copy(theLocalAddress);

	//SLP Session ID -not sent in START- 
	if (aSuplMsgContent->iMessageChoice == ESuplStart)
		{
		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdPresent=EFalse;
		}
	else
		{
		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdPresent=ETrue;
		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdContent=CSuplSlpSessionIdContent::NewL();
		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdContent->iSessionId.Copy(iSlpSessionId);

		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent=CSuplSlpAddressContent::NewL();

		// Address provided by SiRF is IPv4
		//
		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent->iSlpAddressType= ESuplSlpIpAddressChoice;
		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent->iIpAddressContent=CSuplIpAddressContent::NewL();
		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent->iIpAddressContent->iIpAddressType = ESuplIpV4AddressChoice;

    	// Use loopback address 127.0.0.1 (received in RESPONSE, proxy mode).
		TUint32 serverAddress = KLoopBackAddr;
		TBuf8<4> theRemoteAddress(4);
		TUint8 aByteOfTheRemoteAddress = 0;
		aByteOfTheRemoteAddress = serverAddress;
		theRemoteAddress[3]= aByteOfTheRemoteAddress;
		aByteOfTheRemoteAddress = serverAddress >>8;
		theRemoteAddress[2]= aByteOfTheRemoteAddress;
		aByteOfTheRemoteAddress = serverAddress >> 16;
		theRemoteAddress[1]= aByteOfTheRemoteAddress;
		aByteOfTheRemoteAddress = serverAddress >> 24;
		theRemoteAddress[0]= aByteOfTheRemoteAddress;
		aSuplMsgContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent->iIpAddressContent->iIpAddress.Copy(theRemoteAddress);
		}
	}


void CSuplProtocolSession::ReadSuplMessage()
	{
	iTimer->Cancel();
	// Start "listen" timer
	iTimer->Start(KProtocolTimeout, KProtocolTimeout, TCallBack(OnTimerEventL,this));
	iSuplSocketListener->ReadMessage(iStatus);
	SetActive();
	}


void CSuplProtocolSession::SendSuplMessageL(TSuplMessageType aSuplMessageType)
	{
	iTimer->Cancel();
	
	switch(aSuplMessageType)
		{
		case ESuplStart:
			CreateSuplStartMessageL();
			break;

		case ESuplPosInit:
			CreatePosInitMessageL();
			break;

		case ESuplEnd:
			CreateSuplEndMessageL();
			break;
						
		default:
			// unexpected message type 
			User::Leave(KErrGeneral);
			break;
		}

	// The encoding process does no set the final length of the encoded message
	// in the first two bytes of the SUPL message. Do it now.
    TUint16 encodedMessageLength = iTxBuf.Length();
	TUint8 oneByteOftheLength = encodedMessageLength;
	iTxBuf[1] = oneByteOftheLength; //Less significant byte
	oneByteOftheLength = (encodedMessageLength >> 8);
	iTxBuf[0] = oneByteOftheLength;

	// Start send timer
	iTimer->Start(KNetworkTimeout, KNetworkTimeout, TCallBack(OnTimerEventL,this));
	// Send message
	iSocket.Write(iTxBuf, iStatus);
	SetActive();	
	}

void CSuplProtocolSession::DecodeSuplMessageL(TSuplMessageType aExpectedSuplMessageType)
	{
	iTimer->Cancel();
	if (aExpectedSuplMessageType != DoDecodeMessageL())
		{
		iSessionState = EProtocolError;
		}
	PrepareSelfTransition();
	}

void CSuplProtocolSession::ConnectToRemoteServer()
	{
	iTimer->Cancel();
	iTimer->Start(KNetworkTimeout, KNetworkTimeout, TCallBack(OnTimerEventL,this));
	iSocket.Connect(iAddress,iStatus);
	iSessionState = EConnectingToRemoteServer;
	SetActive();
	}

/**
TransitionAccordingToRequestStatus()
This method is called by the CSuplProtocolSession after
having connected to the remote server to decide if the
next state must be a waiting state (when there is no request
for assistance data from the NetSim to service).
*/
void CSuplProtocolSession::TransitionAccordingToRequestAvailability()
	{
	iTimer->Cancel();
	if(iAssistanceDataRequestPending)
		{
		// Start servicing pending request
		iAssistanceDataRequestPending = EFalse;
		iSessionState = EAssistanceDataRequested;
		PrepareSelfTransition();
		}
	else
		{
		// Go into a waiting state. Do not self-transition.
		// CSuplProtocolSession will remain inactive until 
		// GetAssistanceData() is called by NetSim.
		iSessionState = EConnectedAwaitingRequest;
		}
	}

TInt CSuplProtocolSession::RunError(TInt aError)
{
	iObserver->DataSourceNotification(aError);
	return KErrNone;
}
