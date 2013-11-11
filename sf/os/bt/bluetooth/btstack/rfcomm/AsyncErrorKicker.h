// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ASYNCERRORKICKER_H
#define ASYNCERRORKICKER_H


//	Class used by (currently RFCOMM) SAPs to asynchronously
//	error themselves during writes and reads which do not
//	allow for synchronous errors.
//
//	The error kicker should last for the lifetime of the SAP, 
//	and no longer


#include <e32base.h>

class CRfcommSAP;

NONSHARABLE_CLASS(CAsyncErrorKicker) : public CAsyncOneShot
	{
public:
	//	Construction
	CAsyncErrorKicker(CRfcommSAP& aSAP, TInt aPriority);
	void SetError(TInt aErrorCode, TInt aErrorType);	//	aType used for TErrorTypes, 
														//	trying to keep this vaguely generic.
private:
	void RunL();

	CRfcommSAP& iSAP;
	TInt iErrorCode;
	TInt iErrorType;
	};

#endif //ASYNCERRORKICKER_H
