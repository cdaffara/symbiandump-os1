// vtc_serial.h
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __VTC_SERIAL_H__
#define __VTC_SERIAL_H__

#include <e32std.h>
#include <e32cons.h>
#include <c32comm.h>
#include <e32math.h>
#include <fshell/consoleextensions.h>
#include <fshell/vtc_base.h>
#include <fshell/vtc_controller.h>

class TPortConfig
	{
public:
	TPortConfig() : iRate(EBpsAutobaud), iDebug(EFalse) {}

	TPtrC iPdd;
	TPtrC iLdd;
	TPtrC iCsy;
	TPtrC iPort;
	TBps iRate;
	TBool iDebug;
	};
	

class CVtcSerialConsole : public CVtcConsoleBase
	{
public:
	IMPORT_C CVtcSerialConsole();
	IMPORT_C virtual ~CVtcSerialConsole();
protected: // From CVtcSerialConsole.
	IMPORT_C virtual void ConstructL(const TDesC& aTitle);
private: // From MConsoleOutput.
	IMPORT_C virtual TInt Output(const TDesC8& aDes);
private: // From MConsoleInput.
	IMPORT_C virtual void Input(TDes8& aDes, TRequestStatus& aStatus);
	IMPORT_C virtual void CancelInput(TRequestStatus& aStatus);
private:
	TInt ReadConfig(const TDesC& aConfigDes, TPortConfig& aConfig);
private:
	RCommServ iCommServ;
	RComm iCommPort;
	};

#endif //__VTC_SERIAL_H__
