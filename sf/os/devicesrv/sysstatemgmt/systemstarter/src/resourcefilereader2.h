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

#if !defined (__RESOURCEFILEREADER2_H__)
#define __RESOURCEFILEREADER2_H__

#include <e32base.h>
#include <barsc.h>
#include <barsread.h>

#include "startsafe.h"
#include "sysmonclisess.h"
#include "StartupStateInfo.h"
#include "startuputilprovider.h"


class MStartupStateInfo;
class CSystemStartupStateInfo;
class MStartupCommand;

/** This class is responsible for reading the supplied resource file and
creating all the objects described by it.
@internalComponent
@deprecated
*/
class CResourceFileReader : public CBase
	{
public:
	static CResourceFileReader* NewL(TInt aBootMode, RFs& aFs);
	static CResourceFileReader* NewLC(TInt aBootMode, RFs& aFs);
	~CResourceFileReader();
	
	MStartupStateInfo* GetStateInfoL();
		
private:
	CResourceFileReader(TInt aBootMode, RFs& aFs);
	void ConstructL(const TDesC& aResourceFile);
	void FindFirstStateEntryL();
	CSystemStartupStateInfo* ReadStateInformationL();
 	CommandList ReadCommandListL(TUint32 aCommandListId);	
 	MStartupCommand* ReadDllInfoL(TResourceReader& aReader);
	MStartupCommand* ReadSplashScreenL(TResourceReader& aReader);
	MStartupCommand* ReadAppArcInitL();
	MStartupCommand* ReadMultipleWaitInfoL(TResourceReader& aReader, CommandList& aDeferredList);
	MStartupCommand* ReadAmaInfoL(TResourceReader& aReader);
	void ValidateRestartMode(TInt aStartupMode);
 	
private:
	RFs iFs;	
	TInt32 iState;
	TInt iBootMode;
	
	RResourceFile iResourceFile;
	CStartupUtilProvider *iStartupUtilProvider;
	TInt iMaxStartupMode;
	};

#endif // __RESOURCEFILEREADER2_H__
