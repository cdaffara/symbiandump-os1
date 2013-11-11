// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SPEECHRECOGNITIONDATATEST_H__
#define __SPEECHRECOGNITIONDATATEST_H__

//  INCLUDES
#include <e32base.h>

/**
@publishedAll
@released

Class to encapsulate diagnostic parameter values.

@since  2.0
*/

class TDiagnostic
{
public:// Constructors and destructor
	/**
    Two-phased constructor.
	*/
	IMPORT_C TDiagnostic();

	/**
	Destructor.
	*/
	IMPORT_C ~TDiagnostic();

public:
	/** Signal to noise ratio.
	*/
	TInt32 iSNR;
	/** Whether the recognition result was accepted.
	*/
	TBool iAccepted;
	/** Controller specific diagnostic data related to the recognition result.
	*/
	TPtr iData;
};

#endif // __SPEECHRECOGNITIONDATATEST_H__
