// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class declaration for the serial device specific logging code
// 
//

#if !defined(__DEBLOGSR_H__)
#define __DEBLOGSR_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__DEBUGLOG_H__)
#include "DEBUGLOG.H"
#endif

#if !defined(__D32COMM_H__)
#include <d32comm.h>
#endif

class CDebugLogSerial: public CDebugLogDevice
	{
public:
	CDebugLogSerial();
	~CDebugLogSerial();
	//Pure virtual functions from CDebugLogDevice
	void ConstructL(TBool aIsFirst, TDesC &aParams);
	void WriteToLogL(const TDesC &aDes, const TDesC &aDes2);
	void WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2);
private:
	RBusDevComm iSerialPort;
	};

const TUint KHeapSize=0x8000;

#endif

