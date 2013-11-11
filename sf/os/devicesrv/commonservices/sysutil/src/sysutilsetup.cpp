// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <bafl/sysutil.h>
#include "sysutilpatchdata.h"
#include "sysutildebug.h"			// debug helper
#include "sysutilsetup.h"

/**
Calls a function in SysUtil.dll which sets up one or more Publish and Subscribe values. 
The function called is determined by the flags passed to sysutilsetup.exe when it is run. 
The valid flags can be found in sysutilsetup.h.
*/
TInt MainL()
	{
	TBuf<16> flags;
	User::CommandLine( flags );
	TLex lex( flags );
	
	for ( TPtrC token = lex.NextToken(); token.Length() != 0; token.Set( lex.NextToken() ) )
		{
		if ( token.CompareF(KSetVersionPropertiesFlag) == 0 )
			{
			__SYSUTIL_TRACE("Calling SetVersionPropertiesL().");
			SetVersionPropertiesL();
			__SYSUTIL_TRACE("Called SetVersionPropertiesL().");
			}
		else if ( token.CompareF(KSetDeviceTypeInfoPropertyFlag) == 0 )
			{
			__SYSUTIL_TRACE("Calling SetDeviceTypeInfoPropertyL().");
			SetDeviceTypeInfoPropertyL();
			__SYSUTIL_TRACE("Called SetDeviceTypeInfoPropertyL().");
			}
		else if ( token.CompareF(KDeletePropertiesFlag) == 0 )
			{
			DeletePSPropertiesL();
			break;
			}
		else
			{
			return KErrArgument;
			}
		}
	
	return KErrNone;
	}

TInt E32Main()
	{
	__UHEAP_MARK;
	__SYSUTIL_TRACE("Entered sysutilsetup.exe E32Main.");
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	TInt err = KErrNoMemory;
	if ( cleanup != NULL )
		{
		TRAP( err, MainL() );
		delete cleanup;
		}
	
	__SYSUTIL_TRACE("Leaving sysutilsetup.exe E32Main.");
	__UHEAP_MARKEND;
	return err;
	}
