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
//

#ifndef __MB_THREAD_H__
#define __MB_THREAD_H__

#include <e32base.h>
#include <cflog.h>
#include <comms-infras/commsbufpond.h>

namespace CommsFW {

class MCommsBufManagerIntf;

class RCFThread : public RThread
/** Utility class to control the creation of a new thread.  Implements it's own
main thread function in order to process the startup correctly. Used
to make sure that e.g. the MBufMgr TLS is transferred properly to the new thread, 
thus enabling it to use the process shared MBuf Manager.
@internalTechnology 
@released
*/
	{
public:

	IMPORT_C TInt Create(const TDesC& aName, TThreadFunction aThreadFunction,
		TInt aStackSize, RHeap* aHeap, TAny* aArg);				
	IMPORT_C TInt Create(const TDesC& aName, TThreadFunction aThreadFunction, TInt aStackSize,
		TInt aMinHeapSize, TInt aMaxHeapSize, TAny* aArg);

private:	
	static TInt RCFThreadFunction(TAny* aStartupInfo);

private:
	struct ThreadStartupInfo
		{
		RCommsBufPond iCommsBufPond;
		__CFLOG_STMT(CCFLogIf* iCFLogIf;)
		TThreadFunction iOtherThreadFunction;
		TAny* iModuleArgs;
		} iStartupInfo;
	};



} // namespace CommsFW

#endif // __CFTHREAD_H__

