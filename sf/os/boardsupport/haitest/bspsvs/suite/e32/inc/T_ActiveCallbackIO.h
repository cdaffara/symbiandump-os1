/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
@test
@internalComponent

This contains CT_ActiveCallbackIO
*/

#if (!defined __T_ACTIVECALLBACK_IO_H__)
#define __T_ACTIVECALLBACK_IO_H__

// User includes
#include "ActiveCallbackBase.h"

/**
* Callback class for playing and recording processes
*/
class CT_ActiveCallbackIO : public CActiveCallbackBase
	{
public:
	static CT_ActiveCallbackIO*	NewL(CDataWrapperBase& aCallback, TInt aPriority=EPriorityStandard);
	~CT_ActiveCallbackIO();

	TInt			BufferLength();
	const TDesC8&	Buffer();
	void			PrepareFromStringL(TInt aRepeat, const TDesC& aString);
	void			PrepareFromFileL(TInt aRepeat, const TDesC& aFilename);

protected:
	CT_ActiveCallbackIO(CDataWrapperBase& aCallback, TInt aPriority);

private:
	void	ConvertAndRepeatBuffer(TInt aRepeat);
	void	ConvertEscapeChars(TPtr8 aBuffer);

private:
	HBufC8*	iBuffer;
	};

#endif /* __T_ACTIVECALLBACK_IO_H__ */
