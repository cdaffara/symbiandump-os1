// remote_console.h
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __REMOTE_CONSOLE_H__
#define __REMOTE_CONSOLE_H__

#include <e32cons.h>
#include <es_sock.h>
#include "remote_console_config.h"


#ifdef EKA2
NONSHARABLE_CLASS(CRemoteConsole)
#else
class CRemoteConsole
#endif
	 : public CConsoleBase
	{
public:
	CRemoteConsole();
public: // From CConsoleBase.
	virtual ~CRemoteConsole();
	virtual TInt Create(const TDesC& aTitle, TSize aSize);
	virtual void Read(TRequestStatus& aStatus);
	virtual void ReadCancel();
	virtual void Write(const TDesC& aDes);
	virtual TPoint CursorPos() const;
	virtual void SetCursorPosAbs(const TPoint& aPoint);
	virtual void SetCursorPosRel(const TPoint& aPoint);
	virtual void SetCursorHeight(TInt aPercentage);
	virtual void SetTitle(const TDesC& aTitle);
	virtual void ClearScreen();
	virtual void ClearToEndOfLine();
	virtual TSize ScreenSize() const;
	virtual TKeyCode KeyCode() const;
	virtual TUint KeyModifiers() const;
	virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
private:
	TInt Connect(const TDesC& aTitle, TSize aSize);
	void CleanupConnection();
	void WriteCommand(const TDesC8& aDes) const;
	void ReadCommandResponse(TDes8& aDes) const;
private:
	class TKeyEvent
		{
	public:
		TUint16 iChar;
		TUint32 iModifiers;
		};
private:
	TRemoteConsoleConfig iConfig;
	RSocketServ iSocketSession;
#ifdef USE_RCONNECTION
	RConnection iConnection;
#endif
	mutable RSocket iCommandSocket;
	RSocket iKeyEventSocket;
	TKeyEvent iKeyEvent;
	TPckg<TKeyEvent> iKeyEventPckg;
	CConsoleBase* iUnderlyingConsole;
	};


#endif // __REMOTE_CONSOLE_H__
