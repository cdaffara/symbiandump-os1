/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/** @file
@internalComponent
*/

#include "remconstub.h"
#include "remconsourcehandler.h"
#include <remcon/remconbearerobserver.h>
#include <remcon/remconconverterplugin.h>
#include <remconaddress.h>

#include "siftrace.h"

const TInt KBearerUid = 0x102069AA;

/**
Factory function.
@return Ownership of a new CRemConStub.
*/
CRemConStub* CRemConStub::NewL(TBearerParams& aParams)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::NewL() - Enter" ) ) );
	CRemConStub* self = new(ELeave) CRemConStub(aParams);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); 
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::NewL() - Return" ) ) );
	
	return self;
	}

/**
Destructor.
*/
CRemConStub::~CRemConStub()
	{
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::~CRemConStub() - Enter" ) ) );
	
	iNewCmdSubscribe->Cancel();	
	
	delete iNewCmdSubscribe;

	iNewNotifyCmdSubscribe->Cancel();	
	delete iNewNotifyCmdSubscribe;
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::~CRemConStub() - Return" ) ) );
	}

/**
Constructor.
*/
CRemConStub::CRemConStub(TBearerParams& aParams)
:	CRemConBearerPlugin(aParams)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::CRemConBearerPlugin() - Enter" ) ) );
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::CRemConBearerPlugin() - Return" ) ) );
	
	}

/**
2nd-phase construction.
*/
void CRemConStub::ConstructL()
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::ConstructL() - Enter" ) ) );
	
	TRemConAddress addr;
	addr.BearerUid() = Uid();	// implementation UID of this bearer 
	Observer().ConnectIndicate(addr);
	//
	// Construct of Key Event listener. Event is received from remconkeyeventapp
	//		
	iNewCmdSubscribe = CRemConPSstubObserver::NewL( *this, KTFRemConTriggerCmdKey );
	iNewCmdSubscribe->Start();

	//
	// Construct of notifyCommand Event listener. Event is received from remconkeyeventapp
	//		
	iNewNotifyCmdSubscribe = CRemConPSstubObserver::NewL( *this, KTFRemConTriggerNotifyCmdKey );
	iNewNotifyCmdSubscribe->Start();	
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::ConstructL() - Return" ) ) );
	}

/**
RemCon server calls to get a pointer to an object which implements the 
Bearer API with UID aUid
*/
TAny* CRemConStub::GetInterface(TUid aUid)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::GetInterface() - Enter" ) ) );
	
	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KRemConBearerInterface1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConBearerInterface*>(this)
			);
		}
    else if ( aUid == TUid::Uid(KRemConBearerInterface2) )
        {
        ret = reinterpret_cast<TAny*>(
            static_cast<MRemConBearerInterfaceV2*>(this)
            );
        }
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::GetInterface() - Return" ) ) );
	
	return ret;
	}

/**


*/
void CRemConStub::ConnectRequest(const TRemConAddress& aAddr)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::ConnectRequest() - Enter" ) ) );
	Observer().ConnectConfirm( aAddr, KErrNone );
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::ConnectRequest() - Return" ) ) );
	}

/**


*/
void CRemConStub::DisconnectRequest(const TRemConAddress& aAddr)
	{
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::DisconnectRequest() - Enter" ) ) );
	Observer().DisconnectConfirm(aAddr, KErrNone);
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::DisconnectRequest() - Return" ) ) );
	}

/**


*/
TInt CRemConStub::SendResponse(TUid /*aInterfaceUid*/, 
		TUint aOperationId, 
		TUint /*aTransactionId*/, // we don't care about this transaction ID
		RBuf8& /*aData*/, 
		const TRemConAddress& /*aAddr*/)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SendResponse() - Enter" ) ) );
	
	TPhoneCmdHandlerKeyEventBuf cmdDataBuf;
	cmdDataBuf().iOperationId = aOperationId;
	TUid ExtAPIUid;
    
    ExtAPIUid.iUid = KRemConExtCallHandlingApiUid;
    
    if(ExtAPIUid.iUid == iInterfaceUid.iUid)
       {
    
    	switch ( aOperationId  )
    		{
    		case ERemConExtAnswerCall:
    		case ERemConExtEndCall:
    		case ERemConExtVoiceDial:
    		case ERemConExtLastNumberRedial:
    		case ERemConExtDialCall:
    		case ERemConExt3WaysCalling:
    		case ERemConExtGenerateDTMF:
    		case ERemConExtSpeedDial:
    		case ERemConExtAnswerEnd:
    			{
    			(void)iSubcribeToControl.Set(KTFPhoneCmdHandlerProperty, KTFPhoneCmdHandlerKey,cmdDataBuf);
    			break;    
    			}

    		default:
    			
    			break;
    		}
        COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SendResponse() - Return" ) ) );			
	    //aData.Close();
        
        return KErrNotFound;
        }
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SendResponse() - Return" ) ) );			
	
	return KErrNone;
	}


TInt CRemConStub::SendCommand(TUid /*aInterfaceUid*/, 
		TUint /*aOperationId*/, 
		TUint aTransactionId,  
		RBuf8& /*aData*/, 
		const TRemConAddress& /*aAddr*/)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SendCommand() - Enter" ) ) );			
	TInt ret(KErrNone);
	
	// Identifies command - response pairs, needed if response is given 
	iTransactionId = aTransactionId; 
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SendCommand() - Return" ) ) );			
	
	return ret;
	}

/**


*/
void CRemConStub::TestCaseCommandReceived(TInt aTFControlKey, TRemConPckgStubBuf& akeyEventBuf )
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::TestCaseCommandReceived() - Enter" ) ) );			
	
	TInt aError(KErrNone);
	iTransactionId = Observer().NewTransactionId();
	
	iData.Zero();
	// Inform server about command accessory has sent
	
	TRemConAddress addr;	 	
	addr.BearerUid() = Uid();
	addr.Addr() = KNullDesC8();

    TBool aUseCon = akeyEventBuf().iUseConverter;
    
    // Use RemConKeyEventConverter in test case ?
    if( aUseCon )
    	{

    	TRemConKeyEventDataBuf aKeyData;			
    	aKeyData = akeyEventBuf().iKeyEvent;
    					
    	aError = Observer().BearerToInterface(TUid::Uid(KBearerUid), 
    						aKeyData,
    						aKeyData, 
    						iInterfaceUid, 
    						iOperationId,
    						iMsgType, 
    						iData);
    															
    	// Evaluate results
    	if ( aError == KErrNone )
			{
	   		// Key event was converted to interface API format, now interface UID and
	   		// operation ID contains the appropriate values defined for this specific key
	   		// event.
			   		
			//iData.Zero();
			//iData.Append((TInt)akeyEventBuf().iButtonAction);
					
			addr.BearerUid();					
			addr.BearerUid() = Uid();
			addr.Addr() = KNullDesC8();
			if ( aTFControlKey == KTFRemConTriggerCmdKey )
			    {
			    Observer().NewCommand(addr);
			    }
			else if ( aTFControlKey == KTFRemConTriggerNotifyCmdKey )
			    {
			    Observer().NewNotifyCommand(addr);
			    }
			}
		else if ( aError == KErrNotSupported )
			{
			// The key event wasn't identified by the converter.
			
			// Note, iOperation has to be != any RemConExtensionApi command.
			// This branch is used for following test cases
						
			TPhoneCmdHandlerKeyEventBuf cmdDataBuf;
			cmdDataBuf().iOperationId = (TUint)-1;
			(void)iSubcribeToControl.Set(KTFPhoneCmdHandlerProperty, KTFPhoneCmdHandlerKey,cmdDataBuf);
	    				
			}
		else if( aError == KErrGeneral )
			{
			// Other error cases
			}
        }

	//Converter not used, send command directly to RemCon
    else
    	{
        TUid extensionAPIUid;
        extensionAPIUid.iUid = KRemConExtCallHandlingApiUid;
        iInterfaceUid        = extensionAPIUid;
        iOperationId         = akeyEventBuf().iOperationId;
        
       	iData.Copy( akeyEventBuf().iData );
                   
	    addr.BearerUid();					
    	addr.BearerUid() = Uid();
    	addr.Addr() = KNullDesC8();
        if ( aTFControlKey == KTFRemConTriggerCmdKey )
            {
            Observer().NewCommand(addr);
            }
        else if ( aTFControlKey == KTFRemConTriggerNotifyCmdKey )
            {
            Observer().NewNotifyCommand(addr);
            }  			
        }
        
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::TestCaseCommandReceived() - Return" ) ) );			
							
	}

/**
Called by RemCon to retrieve a response on a connection. Must only be 
called as a result of a NewResponse upcall.
*/
TInt CRemConStub::GetResponse(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr)
	{

	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::GetResponse() - Enter" ) ) );			
	
	TRAPD(err, DoGetResponseL(aInterfaceUid, aOperationId, aData, aAddr));

	// The serial bearer only has one incoming message in it at once, so we 
	// don't need to do any queueing or anything fancy to get the transaction 
	// ID- it's just the last one RemCon told us.
	aTransactionId = iTransactionId;

	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::GetResponse() - Return" ) ) );			

	return err;
	}

/**


*/
void CRemConStub::DoGetResponseL(TUid& aInterfaceUid, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::DoGetResponseL() - Enter" ) ) );			

	aInterfaceUid = iInterfaceUid;
	aOperationId = iOperationId;
	//ASSERT(aData.MaxLength() == 0);
	// Pass ownership of this to RemCon.
	aData.CreateL(iData);	

	aAddr.BearerUid() = Uid();
	aAddr.Addr() = KNullDesC8();
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::DoGetResponseL() - Return" ) ) );			

	}
/**
Called by RemCon to get the command that accesory 
has sent to terminal ?
*/
TInt CRemConStub::GetCommand(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr)
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::GetCommand() - Enter" ) ) );			
	
	//ASSERT(iMsgType == ERemConMessageTypeUndefined );
	// The message should be a command, unless RemCon is being buggy and 
	// calling GetCommand at bad times.
	aInterfaceUid = iInterfaceUid;
	// The serial bearer only has one incoming message in it at once, so we 
	// don't need to do any queueing or anything fancy to get the transaction 
	// ID- it's just the last one RemCon told us.
	aTransactionId = iTransactionId;
	aOperationId = iOperationId;
	// Pass ownership of this to RemCon.
    
	aData.CreateL(iData);
	
	aAddr.BearerUid() = Uid();
	aAddr.Addr() = KNullDesC8();

	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::GetCommand() - Return" ) ) );			

	return KErrNone;
	}

/**


*/
TSecurityPolicy CRemConStub::SecurityPolicy() const
	{
	
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SecurityPolicy() - Enter" ) ) );			
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SecurityPolicy() - Return" ) ) );			
	return TSecurityPolicy(ECapabilityLocalServices);
	}

/**


*/
void CRemConStub::ClientStatus(TBool aControllerPresent, TBool aTargetPresent)
	{
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::ClientStatus() - Enter" ) ) );			
	(void)aControllerPresent;
	(void)aTargetPresent;
	COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::ClientStatus() - Return" ) ) );			
	}

TInt CRemConStub::GetNotifyCommand(TUid& aInterfaceUid, 
    TUint& aTransactionId, 
    TUint& aOperationId, 
    RBuf8& aData, 
    TRemConAddress& aAddr)
    {
    COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::GetNotifyCommand() - Enter" ) ) ); 

    aInterfaceUid = iInterfaceUid;

    aTransactionId = iTransactionId;
    aOperationId = iOperationId;
    // Pass ownership of this to RemCon.
    
    aData.CreateL(iData);
    
    aAddr.BearerUid() = Uid();
    aAddr.Addr() = KNullDesC8();    
    COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::GetNotifyCommand() - Return" ) ) ); 
    return KErrNone;
    }

void CRemConStub::SendReject(TUid aInterfaceUid, 
        TUint aOperationId, 
        TUint aTransactionId, 
        const TRemConAddress& aAddr)
    {
    COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SendReject() - Enter" ) ) );  
    (void) aInterfaceUid;
    (void) aOperationId;
    (void) aTransactionId;
    (void) aAddr;
    COMPONENT_TRACE( ( _L( "REMCONSTUB - CRemConStub::SendReject() - Return" ) ) );        
    }

//
// End of file
