// command_base.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __COMMAND_BASE_H__
#define __COMMAND_BASE_H__

#include <e32base.h>

namespace IoUtils
	{

	NONSHARABLE_CLASS(CCommandCompleter) : public CActive
		{
	public:
		CCommandCompleter(IoUtils::CCommandBase& aCmd);
		~CCommandCompleter();
		void Complete(TInt aError);
		void Complete(TInt aError, const TDesC& aText, RIoWriteHandle& aWhere);
		void Complete(TInt aError, const CTextBuffer* aTextBuffer, RIoWriteHandle& aWhere);
	private:
		virtual void RunL();
		virtual void DoCancel();
	private:
		IoUtils::CCommandBase& iCmd;
		TInt iError;
		const CTextBuffer* iTextBuffer;
		TBuf<0x100> iBuf;
		};

	NONSHARABLE_CLASS(CReaderChangeNotifier) : public CActive
		{
	public:
		CReaderChangeNotifier(IoUtils::CCommandBase& aCmd, RIoReadHandle& aReadHandle);
		~CReaderChangeNotifier();
		void Notify();
	private:
		virtual void RunL();
		virtual void DoCancel();
	private:
		IoUtils::CCommandBase& iCmd;
		RIoReadHandle& iReadHandle;
		TPckgBuf<TUint> iChangeType;
		};

	void Panic(TCmdBasePanic aReason);

	NONSHARABLE_CLASS(CKeypressWatcher) : public CActive
		{
	public:
		CKeypressWatcher(IoUtils::MCommandExtensionsV2& aCmd, RIoConsoleReadHandle& aReadHandle);
		~CKeypressWatcher();
		void Notify();
	private:
		virtual void RunL();
		virtual void DoCancel();
	private:
		IoUtils::MCommandExtensionsV2& iCmd;
		RIoConsoleReadHandle& iReadHandle;
		};
	}
	
#endif //__COMMAND_BASE_H__
