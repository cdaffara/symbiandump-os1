// sandbox.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#include <e32base.h>
#include <APMSTD.H>

class RMemoryAccess;

namespace Sandbox
	{
	void GetRecogniserInfoL(TUid aImplementationUid, RArray<TDataType>& aMimeTypes);
	void GetDllNameFromEcomUidL(RMemoryAccess& aMemAccess, TUid aUid, TFileName& aFileName);
	}
