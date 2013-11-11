// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/

#if !defined(__ET_PHONE_SPLIT_H__)
#define __ET_PHONE_SPLIT_H__


/**
The request with this handle should not be completed by TSY
@internalTechnology
*/
#define TSY_HANDLE_INIT_VALUE (0)

/**
@internalComponent
*/
const TInt KDeltaTimerInterval=100000;
const TInt KPhoneTimerPriority=50;



/********************************************************************/
//
// CLibUnloader class
//
class CPhoneFactoryBase;
class CLibUnloader : public CAsyncOneShot
/**
@internalComponent
*/
	{
friend class CPhoneFactoryBase;
public:
	static CLibUnloader* NewL(RLibrary &aLib);
	~CLibUnloader();
protected:
	CLibUnloader();
	virtual void RunL();
private:
	RLibrary iLib;
	};


/********************************************************************/
#endif // __FAXSTORE_SPLIT_H__
