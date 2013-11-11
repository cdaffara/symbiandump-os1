// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __MMFBTAUDIOSERVERSTART_H
#define __MMFBTAUDIOSERVERSTART_H

#include <e32std.h>

_LIT(KAudioServerFileName,"MMFBtAudioServer.exe");
_LIT(KAudioServerName,"!MMFBtAudioServer");

const TUint KMMFAudioServerVersion=8;
const TUint KMMFAudioServerMinorVersionNumber=0;
const TUint KMMFAudioServerBuildVersionNumber=1;


class TAudioServerStart
/**
*@internalTechnology
*/
	{
public:
	TAudioServerStart() {};
	TAudioServerStart(TRequestStatus& aStatus);
	TAudioServerStart(const TProcessId& aProcessId, TRequestStatus& aStatus);
	TPtrC AsCommand() const;
	TInt GetCommand();
	TInt Signal();
public:
	TProcessId iProcessId;
private:
	TThreadId iId;
	TRequestStatus* iStatus;
	};

#endif
