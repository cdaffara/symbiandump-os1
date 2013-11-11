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
//

/**
 @file
 @internalComponent
*/

#ifndef __SYNC_SENDER_H__
#define __SYNC_SENDER_H__

#include <in_sock.h>
#include <es_sock.h>
#include <e32cons.h>
#include <badesca.h>

#include "rpsasciirqstdefs.h"

class CRpsMsg;

const TUint KRespFromSlaveTimeout = 1000000000;	// 1000 sec

class CSender : public CBase
	{
	public:
		static CSender* NewL();
		~CSender();

		TInt SendRequestL(CRpsMsg& aCRpsMsg, HBufC8** aResp);
			
	protected:
		void ConstructL();
		
		TInt GetRequestOrTimeOut(	TRequestStatus& aRequestStatus, 
									TTimeIntervalMicroSeconds32 aTimeOut);
	private:
		RSocket 	iSendSocket;	// Sending socket as specified in iOptions.
		RSocketServ iSocketServ;	// Socket server
		TInetAddr  	iDestAddr;
		
		TBuf8<KRespSocketPreReadSize> 	iBufferPreRead;	// Recvs msg data for Msg Header and Msg Len
		RBuf8							iBuffer;		// Recvs remaining msg data
		RBuf8							iBufferAll;		// Holds the entire msg (iBufferPreRead+iBuffer)
	};

#endif // __SYNC_SENDER_H__
