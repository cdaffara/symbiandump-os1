// kill.h
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

#include <fshell/memoryaccesscmd.h>

using namespace IoUtils;

class CCmdKill : public CMemoryAccessCommandBase
	{
public:
	static CCommandBase* NewLC();
	~CCmdKill();
private:
	CCmdKill();
	void KillIdL(TUint aId);
	template <class T> void DoKillByHandleL(TUint aId);
	void DoKillThreadL(TUint aId);
	void FindIdAndKillL(const TDesC& aPattern);
	template <class RProcessOrThread, class TFindProcessOrThread> void DoFindIdAndKillL(const TDesC& aPattern);
private: // From CCommandBase.
	virtual const TDesC& Name() const;
	virtual void DoRunL();
	virtual void OptionsL(RCommandOptionList& aOptions);
	virtual void ArgumentsL(RCommandArgumentList& aArguments);
private:
	TUint iId;
	TInt iReason;
	TBool iThread;
	TBool iTerminate;
	HBufC* iPanicCategory;
	HBufC* iMatch;
	TBool iAll;
	};
