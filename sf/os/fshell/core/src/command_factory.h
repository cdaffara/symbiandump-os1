// command_factory.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#ifndef __COMMAND_FACTORY_H__
#define __COMMAND_FACTORY_H__

#include <e32base.h>
#include "command_wrappers.h"
class CCommandConstructorBase;
class CThreadPool;
#include "error.h"

class RFs;
class MCommand;
class CCommandSet;

class CCommandFactory : public CActive
	{
public:
	static CCommandFactory* NewL(RFs& aFs);
	~CCommandFactory();
	MCommand* CreateCommandL(const TDesC& aCommandName, TError& aErrorContext, const TDesC& aArguments);
	void ListCommandsL(RArray<TPtrC>& aList);
	TInt CountUniqueCommandsL();
	void GetCommandInfoL(const TDesC& aCommand, RLtkBuf16& aDescriptionBuf);
private:
	CCommandFactory(RFs& aFs);
	void ConstructL();
	void WatchFileSystem();
	void AddCommandL(CCommandConstructorBase* aCommandConstructor);
	void AddThreadCommandL(TCommandConstructor aConstructor, TUint aFlags = 0);
	void AddThreadCommandL(const TDesC& aCommandName, TCommandConstructor aConstructor, TUint aAttributes, TUint aFlags = 0);
	void AddAliasCommandL(const TDesC& aAliasName, TCommandConstructor aConstructor, const TDesC* aAdditionalArguments, const TDesC* aReplacementArguments, TUint aAttributes, TUint aFlags = 0);
	void CheckExternalCommands();
	void FindExternalCommandsL();
	void AppendExternalCommandsL(const TUidType& aUidType, const TDesC& aExtension);
	void DoAppendExternalCommandsL(const TEntry& aEntry, const TUidType& aUidType, const TDesC& aExtension);
	void WaitLC() const;
	TInt FindCommandL(const TDesC& aCommandName);
	MCommand* DoCreateCommandL(const TDesC& aCommandName, const TDesC& aArguments, RProcess& aProcess);
	static void Signal(TAny* aSelf);
private: // From CActive.
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private:
	RFs& iFs;
	mutable RMutex iLock;
	RPointerArray<CCommandConstructorBase> iCommands;
	TDriveList iDriveList;
	TBool iFileSystemScanned;
	TBool iFailedToScanFileSystem;
	CThreadPool* iThreadPool;
	};


#endif // __COMMAND_FACTORY_H__
