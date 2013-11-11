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



// INCLUDE FILES
#include <e32std.h>
#include <bt_sock.h>
#include <btdevice.h>
#include "BTGPSPanic.h"
#include "BTGPSConnectManager.h"
#include "BTGPSSettingManager.h"
#include "BTGPSHandlerFactory.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSMessageSender.h"
#include "BTGPSMessageReceiver.h"
#include "BTGPSPanic.h"
#include "BTGPSLogging.h"
#include "btgpsdevicelistmanager.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

//Process state enumeration
enum THandlerProcessState
    {
    EExitSuccess = -1,
    EExitFailed = -2,  //Connection failed, device is error
    EExitDisconnected = -3 //Connection can't be made, device is disconnected
    };

//There are two different connecting state machines 
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
const struct THandlerProcessItem KConnectingProcess[] = 
    {
        {EGetNextDevice, 	 	1,             7},           //0
        {ECheckDevicePairing,	2,			   3},			 //1
        {ESocketHandler,     	EExitSuccess,  0}, 		  	 //2
        {ESdpHandler,        	4,             0},           //3
        {EPairingHandler,    	6,             5},           //4
        {ESocketHandler,     	6,             0},           //5
        {EInitHandler,      	EExitSuccess,  0},           //6
        {EDeviceSelector,   	8,             EExitFailed}, //7
        {ESdpHandler,        	9,             7},           //8
        {EPairingHandler,    	11,            10},          //9
        {ESocketHandler,     	11,            7},           //10
        {EInitHandler,       	EExitSuccess,  7},           //11
    };
#else
//This struct defines the decision tree of the connection process
const struct THandlerProcessItem KConnectingProcess[] = 
    {
        {EDeviceInfoHandler, 1,             2},           //0
        {ESocketHandler,     EExitSuccess,  2}, 		  //1
        {EDeviceSelector,    3,             EExitFailed}, //2
        {ESdpHandler,        4,             2},           //3
        {EPairingHandler,    6,             5},           //4
        {ESocketHandler,     6,             2},           //5
        {EInitHandler,       EExitSuccess,  2},           //6
    };

//Reconnecting if socket error
const struct THandlerProcessItem KErrorRecoveryProcess[]=
    {
        {ESocketHandler, EExitSuccess, EExitDisconnected}
    };
#endif

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSConnectManager::NewL
// -----------------------------------------------------------------------------
CBTGPSConnectManager* CBTGPSConnectManager::NewL(
        CBTGPSSettingManager& aSettingManager,
        CBTGPSDeviceManager& aDeviceManager,
        CBTGPSConstantsManager& aConstantsManager,
        CBTGPSDeviceListManager& aDeviceListManager)
    {
    CBTGPSConnectManager* self = new (ELeave) CBTGPSConnectManager(
        aSettingManager,
        aDeviceManager,
        aConstantsManager,
        aDeviceListManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::~CBTGPSConnectManager
// -----------------------------------------------------------------------------
CBTGPSConnectManager::~CBTGPSConnectManager()
    {
    delete iMessageReceiver;
    delete iMessageSender;
    
    delete iActiveHandler;

    if(iIdle!=NULL)
        {
        iIdle->Cancel();
        delete iIdle;
        }

    //Close socket connection
    iSocket.Close();
    iSocketServ.Close();
    }


// -----------------------------------------------------------------------------
// CBTGPSConnectManager::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::ConstructL()
    {
    iMessageSender = CBTGPSMessageSender::NewL(iSocket, iDeviceManager);
    iMessageReceiver = CBTGPSMessageReceiver::NewL(iSocket, iDeviceManager);
    iIdle = CIdle::NewL(CActive::EPriorityStandard);
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::CBTGPSConnectManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSConnectManager::CBTGPSConnectManager(
        CBTGPSSettingManager& aSettingManager,
        CBTGPSDeviceManager& aDeviceManager,
        CBTGPSConstantsManager& aConstantsManager,
        CBTGPSDeviceListManager& aDeviceListManager)
    :iSettingManager(aSettingManager),
     iDeviceManager(aDeviceManager),
     iConstantsManager(aConstantsManager),
     iDeviceListManager(aDeviceListManager)
    {
    }


// -----------------------------------------------------------------------------
// CBTGPSConnectManager::Connecting
// -----------------------------------------------------------------------------
TInt CBTGPSConnectManager::Connecting()
    {
    TRACESTRING("CBTGPSConnectManager::ConnectingL start...")
    //Get device connect status
    TBTDeviceConnectStatus connectStatus = 
        iDeviceManager.DeviceConnectStatus();

    switch (connectStatus)
        {
        case EBTDeviceDisconnected:
            //start to connect to BT device
            break;
        case EBTDeviceConnecting:
        case EBTDeviceConnected:
            //do nothing if it's in connecting or connected status
            return KErrNone;
        case EBTDeviceConnectError:
            //If device error then fail the request with KErrQualityLoss
            return KErrCouldNotConnect;
        default:
            //should never hapen
            Panic(EPanicInvalidCase);
            break;
        }

    //Start to connect to BT Device
    StartConnectDevice();
    TRACESTRING("CBTGPSConnectManager::ConnectingL end")
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::Reconnecting
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::Reconnecting()
    {
    //Get device connect status
    TBTDeviceConnectStatus connectStatus = 
        iDeviceManager.DeviceConnectStatus();

    switch (connectStatus)
        {
        case EBTDeviceDisconnected:
        case EBTDeviceConnectError:
            //start to connect to BT device
            break;
        case EBTDeviceConnecting:
        case EBTDeviceConnected:
            //do nothing if it's in connecting or connected status
            return;
        default:
            //should never hapen
            Panic(EPanicInvalidCase);
            break;
        }
        
    //Start to connect to BT Device
    StartConnectDevice();
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::StartConnectDevice
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::StartConnectDevice()
    {
    //This function should be return imediately. Thus real functoinalities
    //are moved to idle callback function

    //Set connection status
    iDeviceManager.SetBTDeviceConnectStatus(EBTDeviceConnecting);
    

    if(!iIdle->IsActive())
        {
        iIdle->Start(TCallBack(StaticIdleCallback, this));
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::StartConnectIdle
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::StartConnectIdle()
    {
    TRAPD(err, StartConnectIdleL());
    if(err!=KErrNone)
        {
        iDeviceManager.SetBTDeviceConnectStatus(EBTDeviceConnectError, err);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::StartConnectIdleL
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::StartConnectIdleL()
    {
    //Cancel idle object
    iIdle->Cancel();
    
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
    //Reset the BT GPS device list
    iDeviceListManager.RefreshDeviceList();
    
    //Always start the full connecting process even when attempting to reconnect.
    // This will cause the PSY to attempt to reconnect to the device it was just connected
    // to (as it will be at the top of the list).  Failure to reconnect to this device once
    // will cause the PSY to continue attempting to connect to the remaining devices in the list.
    StartConnectingProcessL(KConnectingProcess);

#else
   
    if(iDeviceManager.IsGpsConnected())
        {
        //Connection error case. Device has already connected, but
        //might some error happens in socket connection.
        //Set device disconnected
        DisconnectBtDevice();
        StartConnectingProcessL(KErrorRecoveryProcess);
        }
    else
        {
        //start connecting process
        StartConnectingProcessL(KConnectingProcess);
        }
#endif
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::StaticIdleCallback
// -----------------------------------------------------------------------------
TInt CBTGPSConnectManager::StaticIdleCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSConnectManager*>(aAny)->StartConnectIdle();
    return KErrNone; //shall not be called again
    }


// -----------------------------------------------------------------------------
// CBTGPSConnectManager::CancelConnecting
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::CancelConnecting()
    {
    //Stop idle
    iIdle->Cancel();
    
    //Stop current handler
    delete iActiveHandler;
    iActiveHandler = NULL;
    
    //Stop current process
    iActiveProcess = NULL;

    DisconnectBtDevice();
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::DisconnectBtDevice
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::DisconnectBtDevice()
    {
    TRACESTRING("CBTGPSConnectManager::DisconnectBtDevice start...")
    TInt connectStatus = iDeviceManager.DeviceConnectStatus();
    if(connectStatus==EBTDeviceConnected ||
        connectStatus==EBTDeviceConnectError
        && iSocket.SubSessionHandle()!=0)
        {
        //Disconnect from GPS
        //Update device connection status
        iDeviceManager.SetBTDeviceConnectStatus(
            EBTDeviceDisconnected,
            KErrCancel);

        //Close socket connection
        iSocket.Close();
        
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::StartConnectingProcessL
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::StartConnectingProcessL(const THandlerProcessItem* aProcess)
    {
    //Just construct the first item in the process
    if(iActiveHandler!=NULL)
        {
        delete iActiveHandler;
        iActiveHandler = NULL;
        }

    iActiveProcess = aProcess;

    TBTGPSHandlerFactory handlerFactory(
        *this);

    iCurrentNode = 0;
    iActiveHandler = handlerFactory.ConstructHandlerL(aProcess->iHandlerId);
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::DeviceManager
// -----------------------------------------------------------------------------
CBTGPSDeviceManager& CBTGPSConnectManager::DeviceManager() const
    {
    return iDeviceManager;
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::ConstantsManager
// -----------------------------------------------------------------------------
CBTGPSConstantsManager& CBTGPSConnectManager::ConstantsManager() const
    {
    return iConstantsManager;
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::SettingManager
// -----------------------------------------------------------------------------
CBTGPSSettingManager& CBTGPSConnectManager::SettingManager() const
    {
    return iSettingManager;
    }

// -----------------------------------------------------------------------------
// CBTGPSSettingManager::DeviceListManager
// -----------------------------------------------------------------------------
CBTGPSDeviceListManager& CBTGPSConnectManager::DeviceListManager() const
	{
	return iDeviceListManager;
	}

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::GetSocket
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::GetSocket(
            RSocketServ*& aSocketServ,
            RSocket*& aSocket)
    {
    aSocketServ = &iSocketServ;
    aSocket = &iSocket;
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::HandlerComplete
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::HandlerComplete(TInt aId, TInt aErr)
    {
    TRACESTRING("CBTGPSConnectManager::HandlerComplete start..")
    TRACESTRING2("node = %d", iCurrentNode)
    TRACESTRING2("err= %d", aErr)
    //Find node in process decision tree
    const THandlerProcessItem* item = iActiveProcess+iCurrentNode;

    //Panic in debug if process defination is incorrect
    __ASSERT_DEBUG(item!=NULL, Panic(EPanicProcessDefination));
    
    //If socket handler succeeds, then set device as connected
    if( ( aId == ESocketHandler || aId == EPairingHandler ) && aErr==KErrNone)
        {
        iDeviceManager.SetBTDeviceConnectStatus(
            EBTDeviceConnected,
            aErr);
        }

    iLastError = aErr;
    
    if(aErr==KErrNone)
        {
        iCurrentNode = item->iSuccessNodeIndex;
        }
    else
        {
        iCurrentNode = item->iFailedNodeIndex;
        }
    
    StartNextHandler();
    TRACESTRING("CBTGPSConnectManager::HandlerComplete end")
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::StartNextHandler()
    {
    //Delete current handler
    if(iActiveHandler != NULL)
        {
        delete iActiveHandler;
        iActiveHandler = NULL;
        }

    switch(iCurrentNode)
        {
        case EExitFailed:
        case EExitSuccess:
        case EExitDisconnected:
            ConnectComplete(iCurrentNode);
            break;
        default:
            {
            //Start next item
            const THandlerProcessItem* item = iActiveProcess+iCurrentNode;
            TBTGPSHandlerFactory handlerFactory(*this);
            TRAPD(handlerErr, 
                iActiveHandler = handlerFactory.ConstructHandlerL(
                    item->iHandlerId));
            if(handlerErr != KErrNone)
                {
                iLastError = handlerErr;
                //handler construction failed, start next handler
                iCurrentNode = item->iFailedNodeIndex;
                StartNextHandler();
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::ConnectComplete
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::ConnectComplete(TInt aErr)
    {
    //Clear active process
    iActiveProcess = NULL;

    switch( aErr )
        {
        case EExitSuccess:
            //Do nothing here. the device is connected
            break;
        case EExitDisconnected:
            //Set device as disconected. Next location
            //request will trigger the connecting procedure.
            iDeviceManager.SetBTDeviceConnectStatus(
                EBTDeviceDisconnected,
                iLastError);
            break;
        case EExitFailed:
        default:
            //Set device as connect error. Next new 
            //subsession will trigger the connecting procedure.
            iDeviceManager.SetBTDeviceConnectStatus(
                EBTDeviceConnectError,
                iLastError);
            break;
        }
    }


// -----------------------------------------------------------------------------
// CBTGPSConnectManager::AddMessageListenerL
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::AddMessageListenerL(MBTGPSMessageListener& aListener)
    {
    iMessageReceiver->AddMessageListenerL(aListener);
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::RemoveMessageListener
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::RemoveMessageListener(MBTGPSMessageListener& aListener)
    {
    iMessageReceiver->RemoveMessageListener(aListener);
    }
    
// -----------------------------------------------------------------------------
// CBTGPSConnectManager::SendMessageL
// -----------------------------------------------------------------------------
void CBTGPSConnectManager::SendMessageL(const TDesC8& aMsg)
    {
    iMessageSender->SendL(aMsg);
    }

// -----------------------------------------------------------------------------
// CBTGPSConnectManager::SendMessage
// -----------------------------------------------------------------------------
TInt CBTGPSConnectManager::SendMessage(const TDesC8& aMsg)
    {
    TRAPD(err, SendMessageL(aMsg));
    return err;
    }

//  End of File



