// Socket.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#if !defined(AFX_SOCKET_H__EE7BB9E5_00B0_4007_A36C_A2251979F83F__INCLUDED_)
#define AFX_SOCKET_H__EE7BB9E5_00B0_4007_A36C_A2251979F83F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Base.h"
#include <winsock2.h>
#include "Window.h"


class CSocket;


class MSocketClosureObserver
	{
public:
	virtual void HandleSocketClosure(CSocket& aSocket) = 0;
	};


class CSocket : public CBase, public MWindowMessageHandler
	{
public:
	virtual ~CSocket();
	void SetClosureObserver(MSocketClosureObserver* aClosureObserver);
protected:
	CSocket(CWindow& aWindow);
	CSocket(CWindow& aWindow, SOCKET aSocket);
	void Construct();
	void HandleClosure();
private: // From MWindowMessageHandler.
	virtual LRESULT HandleWindowMessage(UINT aMessage, WPARAM aWParam, LPARAM aLParam);
protected: // New.
	virtual LRESULT HandleSocketMessage(LPARAM aLParam) = 0;
protected:
	CWindow& iWindow;
	SOCKET iSocket;
private:
	MSocketClosureObserver* iClosureObserver;
	};


#endif // !defined(AFX_SOCKET_H__EE7BB9E5_00B0_4007_A36C_A2251979F83F__INCLUDED_)
