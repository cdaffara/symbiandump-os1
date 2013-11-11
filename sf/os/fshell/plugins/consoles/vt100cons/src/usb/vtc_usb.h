// vtc_usb.h
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
#include <usbman.h>
#include "vtc_serial.h"

class TUsbPortConfig
	{
public:
	TUsbPortConfig();

	TPtrC iPort;
	TInt iPersonality;
	};

NONSHARABLE_CLASS(CUsbConsole) : public CVtcSerialConsole
	{
public:
	CUsbConsole();
	virtual ~CUsbConsole();
private: // From CVtcSerialConsole.
	virtual void ConstructL(const TDesC& aTitle);
private:
	TInt ReadConfig(const TDesC& aConfigDes, TUsbPortConfig& aConfig);

private:
	RUsb iUsb;
	};


#endif //__VTC_BT_H__
