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
#include "BTGPSPairingHandler.h"
#include "BTGPSConnectManagerExt.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSHandlerFactory.h"
#include "BTGPSLogging.h"
#include "btgpssimstubs.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSPairingHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSPairingHandler* CBTGPSPairingHandler::NewL(
        MBTGPSConnectManagerExt& aManagerExt)
    {
    CBTGPSPairingHandler* self = new (ELeave) CBTGPSPairingHandler(
        aManagerExt);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSPairingHandler::~CBTGPSPairingHandler
// -----------------------------------------------------------------------------
CBTGPSPairingHandler::~CBTGPSPairingHandler()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CBTGPSPairingHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSPairingHandler::ConstructL()
    {
    TRACESTRING("CBTGPSPairingHandler::ConstructL start...")

    TSockAddr sockAddr;
    TUint protocol =  KRFCOMM;
    sockAddr.SetFamily(KBTAddrFamily);
    
    //Get socket address
    TBTSockAddr& sockAddrBt = TBTSockAddr::Cast(sockAddr);
    sockAddrBt.SetBTAddr(iManagerExt.DeviceManager().BtDevice().BDAddr());
    sockAddrBt.SetPort(iManagerExt.DeviceManager().Port());
    //Set authentication
    TBTServiceSecurity security;
    security.SetAuthentication(ETrue);
    sockAddrBt.SetSecurity(security);

    //Get pointer to socket server and socket
    iManagerExt.GetSocket(iSocketServ, iSocket);

    //Connect to socket server
    if(iSocketServ->Handle()==0)
        {
        User::LeaveIfError(iSocketServ->Connect());
        }

#ifdef	LBS_BTGPSPSY_SIM
    GetSimSockAddr(sockAddr, protocol);
#endif
    
    iSocket->Close();
    User::LeaveIfError(iSocket->Open(*iSocketServ,
        sockAddr.Family(),KSockStream,protocol));

    iSocket->Connect(
        sockAddr, 
        iStatus);
        
    SetActive();

    TRACESTRING("CBTGPSPairingHandler::ConstructL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSPairingHandler::CBTGPSPairingHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSPairingHandler::CBTGPSPairingHandler(
        MBTGPSConnectManagerExt& aManagerExt)
    : CActive(CActive::EPriorityStandard),
      iManagerExt(aManagerExt)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CBTGPSPairingHandler::RunL
// -----------------------------------------------------------------------------
void CBTGPSPairingHandler::RunL()
    {
    TRACESTRING2("CBTGPSPairingHandler::RunL start...", iStatus.Int());
    if(iStatus.Int()==KErrNone)
        {
        iManagerExt.DeviceManager().SetPaired(ETrue);
        }
    iManagerExt.HandlerComplete(EPairingHandler, iStatus.Int());
    TRACESTRING("CBTGPSPairingHandler::RunL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSPairingHandler::DoCancel
// -----------------------------------------------------------------------------
void CBTGPSPairingHandler::DoCancel()
    {
    if(iSocket!=NULL)
        {
        iSocket->CancelConnect();
        }
    }


//  End of File



