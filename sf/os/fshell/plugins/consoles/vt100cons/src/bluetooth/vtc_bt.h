// vtc_bt.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __VTC_BT_H__
#define __VTC_BT_H__

#include <e32cons.h>
#include <fshell/btserialclient.h>
#include <fshell/vtc_base.h>
#include <fshell/vtc_controller.h>

NONSHARABLE_CLASS(CBtConsole) : public CVtcConsoleBase
	{
public:
	CBtConsole();
	virtual ~CBtConsole();
private: // From CVtcSerialConsole.
	virtual void ConstructL(const TDesC& aTitle);
private: // From MConsoleOutput.
	virtual TInt Output(const TDesC8& aDes);
private: // From MConsoleInput.
	virtual void Input(TDes8& aDes, TRequestStatus& aStatus);
	virtual void CancelInput(TRequestStatus& aStatus);
private:
	RBtSerialSession iBtConnection;
	};


#endif //__VTC_BT_H__
