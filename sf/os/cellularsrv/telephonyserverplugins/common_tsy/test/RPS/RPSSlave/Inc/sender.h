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


//#include <ctransfer.h>
#include <in_sock.h>
#include <e32cons.h>


class CSender : public  CActive  // NB: Sender does not implement an AO at the moment!!
	{
	public:
		static CSender* NewL(RSocketServ& aSockServ, TInetAddr& aDestAddr, CConsoleBase* aConsole);
		~CSender();
		void DoCancel();
		
		void SendData(TDesC8& aChar);

		//void SetConsole(CConsoleBase* aConsole);
		void PrintConsole(const TDesC& aDes);

		CConsoleBase* Console()
			{ return iConsole; }
			
		void WaitForKeyPress();
			
	protected:
		CSender(RSocketServ& aSockServ, TInetAddr& aDestAddr, CConsoleBase* aConsole);
		void RunL();
		void ConstructL();

	private:
		RSocket iSendSocket;		// Sending socket as specified in iOptions.
		RSocketServ iSockServ;		// Reference to socket server.
		TInetAddr  iDestAddr;

		TBuf8<1> iBuffer;   // Recv data buffer
		
		CConsoleBase* iConsole;
	};

#endif // __SYNC_SENDER_H__
