// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// An utility class for specifying the error pattern
// 
//

#if (!defined __TE_LBSSUPLPUSHERRORPATTERN__)
#define __TE_LBSSUPLPUSHERRORPATTERN__

#include <e32base.h>


/**
The error pattern class used for specifying the set of errors to happen, simulating these errors 
while running the test code, and checking the results later.
 
@see CTe_LbsSuplErrorPattern
*/
class RTe_LbsSuplErrorPattern
	{		
public:

	static void OpenL();
	static void Close();

	static void Reset();
	static void AppendNextErrorL(TInt aFunction, TInt aCount, TInt aError, TBool aLeave);
	static TInt ErrorReached();
	
	static TInt CheckNextError(TInt aFunction);
	static TInt CheckNextErrorL(TInt aFunction);
	
private:
	/** The internal implementation of the RTe_LbsSuplErrorPattern class */
	static class CTe_LbsSuplErrorPattern* iImpl;
	};

	
#endif //__TE_LBSSUPLPUSHERRORPATTERN__
