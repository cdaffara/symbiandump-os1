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

#ifndef REMCONSERIALBEARER_H
#define REMCONSERIALBEARER_H


#include <e32base.h>
#include <c32comm.h>
#include <e32property.h>
#include <remcon/remconbearerplugin.h>
#include <remcon/remconbearerinterface.h>
#include <remcon/messagetype.h>

#include "RemConStubObserver.h"
#include "RemConPSstubObserver.h"
#include <accremconpstypes.h>
#include <RemConExtensionApi.h>

class CTestCaseControl;

class CRemConStub : public CRemConBearerPlugin, 
					public MRemConBearerInterfaceV2,
					public MRemConStubObserver 
							
	{
public:
	static CRemConStub* NewL(TBearerParams& aParams);
	~CRemConStub();
	
public:	// from MRemConStubObserver
    void TestCaseCommandReceived(TInt aTFControlKey, TRemConPckgStubBuf& aKeyEvents );


private: // from CRemConBearerPlugin
	TAny* GetInterface(TUid aUid);

private: // from MRemConBearerInterface
	TInt GetResponse(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr);
	TInt SendCommand(TUid aInterfaceUid, 
		TUint aCommand, 
		TUint aTransactionId,  
		RBuf8& aData, 
		const TRemConAddress& aAddr);
	TInt GetCommand(TUid& aInterfaceUid, 
		TUint& aTransactionId, 
		TUint& aCommand, 
		RBuf8& aData, 
		TRemConAddress& aAddr);
	TInt SendResponse(TUid aInterfaceUid, 
		TUint aOperationId, 
		TUint aTransactionId, 
		RBuf8& aData, 
		const TRemConAddress& aAddr);
	void ConnectRequest(const TRemConAddress& aAddr);
	void DisconnectRequest(const TRemConAddress& aAddr);
	void ClientStatus(TBool aControllerPresent, TBool aTargetPresent);
	TSecurityPolicy SecurityPolicy() const;

private: // from MRemConBearerInterfaceV2
    
    TInt GetNotifyCommand(TUid& aInterfaceUid, 
        TUint& aTransactionId, 
        TUint& aOperationId, 
        RBuf8& aData, 
        TRemConAddress& aAddr);

    void SendReject(TUid aInterfaceUid, 
            TUint aOperationId, 
            TUint aTransactionId, 
            const TRemConAddress& aAddr);

private:
	CRemConStub(TBearerParams& aParams);
	void ConstructL();

private: // utility
	TInt DoSend(TUid aInterfaceUid, 
		TUint aOperationId, 
		RBuf8& aData, 
		TRemConMessageType aMsgType);
	void ConnectL();
	void Receive();
	void ClosePort();
	void DoGetResponseL(TUid& aInterfaceUid, 
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr);
	void DoGetCommandL(TUid& aInterfaceUid,
		TUint& aOperationId, 
		RBuf8& aData, 
		TRemConAddress& aAddr);

private: 

	TUid iInterfaceUid;
	TUint iOperationId;
	TRemConMessageType iMsgType;
	TBuf8<KRemConExtParamMaxLength> iData; // This is the length of the associated data in our naive 
	
	TUint iTransactionId;
	RProperty iSubcribeToControl;
	
	CRemConPSstubObserver* iNewCmdSubscribe;  
	CRemConPSstubObserver* iNewNotifyCmdSubscribe;  
	};

#endif // REMCONSERIALBEARER_H
