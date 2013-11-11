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
#include "BTGPSSocketHandler.h"
#include "BTGPSHandlerFactory.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSConnectManagerExt.h"
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
// CBTGPSSocketHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSSocketHandler* CBTGPSSocketHandler::NewL(
            MBTGPSConnectManagerExt& aManagerExt)
    {
    CBTGPSSocketHandler* self = new (ELeave) CBTGPSSocketHandler(
        aManagerExt);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSSocketHandler::~CBTGPSSocketHandler
// -----------------------------------------------------------------------------
CBTGPSSocketHandler::~CBTGPSSocketHandler()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CBTGPSSocketHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSSocketHandler::ConstructL()
    {
    TRACESTRING("CBTGPSSocketHandler::ConstructL start...")
    //Connect to socket server
    if(iSocketServ->Handle()==0)
        {
        User::LeaveIfError(iSocketServ->Connect());
        }
    
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

    //Connect to socket server
    if(iSocketServ->Handle()==0)
        {
        User::LeaveIfError(iSocketServ->Connect());
        }
    
#ifdef	LBS_BTGPSPSY_SIM
    GetSimSockAddr(sockAddr, protocol);
#endif
    
    //For safe first close subssion.
    iSocket->Close();
    User::LeaveIfError(iSocket->Open(*iSocketServ,
    		sockAddr.Family(),KSockStream,protocol));
    
    iSocket->Connect(
        sockAddr, 
        iStatus);
        
    SetActive();
    
    TRACESTRING("CBTGPSSocketHandler::ConstructL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSSocketHandler::CBTGPSSocketHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSSocketHandler::CBTGPSSocketHandler(
    MBTGPSConnectManagerExt& aManagerExt)
    : CActive(EPriorityStandard),
      iManagerExt(aManagerExt)
    {
    aManagerExt.GetSocket(iSocketServ, iSocket);
    
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CBTGPSSocketHandler::RunL
// -----------------------------------------------------------------------------
void CBTGPSSocketHandler::RunL()
    {
    TRACESTRING2("CBTGPSSocketHandler::RunL start...%d", iStatus.Int())
    iManagerExt.HandlerComplete(ESocketHandler, iStatus.Int());
    TRACESTRING("CBTGPSSocketHandler::RunL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSSocketHandler::DoCancel
// -----------------------------------------------------------------------------
void CBTGPSSocketHandler::DoCancel()
    {
    iSocket->CancelConnect();
    }

// -----------------------------------------------------------------------------
// CBTGPSSocketHandler::RunError
// -----------------------------------------------------------------------------
TInt CBTGPSSocketHandler::RunError(TInt /*aError*/)
    {
    //Should never be called
    return KErrNone;
    }


//  End of File



