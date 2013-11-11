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
// Class declaration for the windows-dependent derived class
// 
//

#if !defined(__DEBLOGWN_H__)
#define __DEBLOGWN_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__DEBUGLOG_H__)
#include "DEBUGLOG.H"
#endif

#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif

class CDebugLogFile: public CDebugLogDevice
	{
public:
	CDebugLogFile();
	~CDebugLogFile();
	//Pure virtual functions from CDebugLogDevice
	virtual void ConstructL(TBool aIsFirst, TDesC &aParams);
	virtual void WriteToLogL(const TDesC &aDes, const TDesC &aDes2);
	virtual void WriteToLog8L(const TDesC8 &aDes, const TDesC8 &aDes2);
private:
	TPtrC8 iEol;
	TBuf16<2> iEol16;
	TBuf8<2> iEol8;
	RFs iFs;
	RFile iFile;
	};

#endif

