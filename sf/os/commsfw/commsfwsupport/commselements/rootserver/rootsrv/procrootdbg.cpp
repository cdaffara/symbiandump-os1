// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file 
 @internalTechnology
 @released
*/

#include <procroot.h>

using namespace RootServer; // Allowing the usage of definitions from the RS
using namespace CommsFW; // Allowing the usage of definitions from the Comms Framework

#if defined (_DEBUG_ROOTSERVER_FUNCTIONS)
EXPORT_C TInt RProcessRootDbg::__DbgMarkHeap()
	/** Sets a heap mark in the root server
	@internalComponent 
    */
	{
	return iProcessRoot.SendReceive(RSDbgMarkHeap,TIpcArgs());
	}

EXPORT_C TInt RProcessRootDbg::__DbgCheckHeap(TInt aCount)
	/** Sets a heap mark in the root server
	@internalComponent 
    */
	{
	return iProcessRoot.SendReceive(RSDbgCheckHeap,TIpcArgs(aCount));//check if it's right 
	}

EXPORT_C TInt RProcessRootDbg::__DbgMarkEnd(TInt aCount)
	/** Sets a heap mark in the root server
	@internalComponent 
    */
	{
	return iProcessRoot.SendReceive(RSDbgMarkEnd,TIpcArgs(aCount));//check if it's right 
	}
EXPORT_C TInt RProcessRootDbg::__DbgFailNext(TInt aCount)
	/** Sets a heap mark in the root server
	@internalComponent 
    */
	{
	return iProcessRoot.SendReceive(RSDbgFailNext,TIpcArgs(aCount));//check if it's right 
	}

#else
EXPORT_C TInt RProcessRootDbg::__DbgMarkHeap()
	/** Sets a heap mark in the root server
	@internalComponent 
    */
	{
	return KErrNone;
	}

EXPORT_C TInt RProcessRootDbg::__DbgCheckHeap(TInt /*aCount*/)
	/** Sets a heap mark in the root server
	@internalComponent 
    */
	{
   return KErrNone;
	}

EXPORT_C TInt RProcessRootDbg::__DbgMarkEnd(TInt /*aCount*/)
	/** Sets a heap mark in the root server
	@internalComponent 
    */
	{
   return KErrNone;
	}
EXPORT_C TInt RProcessRootDbg::__DbgFailNext(TInt /*aCount*/)
	/** Sets a heap mark in the root server
	@internalComponent 
    */
	{
   return KErrNone;
	}

#endif // _DEBUG


