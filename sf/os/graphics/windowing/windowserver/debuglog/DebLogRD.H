// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class declaration for the class that logs using RDebug::Print
// 
//

#if !defined(__DEBLOGRD_H__)
#define __DEBLOGRD_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__DEBUGLOG_H__)
#include "DEBUGLOG.H"
#endif

class CDebugLogPrint: public CDebugLogDevice
	{
public:
	CDebugLogPrint();
	~CDebugLogPrint();
	//Pure virtual functions from CDebugLogDevice
	virtual void ConstructL(TBool aIsFirst, TDesC &aParams);
	virtual void WriteToLogL(const TDesC &aDes, const TDesC &aDes2);
	virtual void WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2);
	};

#endif

