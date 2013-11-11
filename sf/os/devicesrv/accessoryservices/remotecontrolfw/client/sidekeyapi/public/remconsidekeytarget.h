// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/

#ifndef REMCONSIDEKEYTARGET_H
#define REMCONSIDEKEYTARGET_H

#include <e32base.h>
#include <remcon/remconinterfacebase.h>
#include <remcon/remconinterfaceif.h>

class MRemConSideKeyTargetObserver;
class CRemConInterfaceSelector;

/**
Client-instantiable type supporting sending SideKey API responses.
*/
NONSHARABLE_CLASS(CRemConSideKeyTarget) : public CRemConInterfaceBase, 
								         public MRemConInterfaceIf
	{
public:
	/**
	Factory function.
	@param aInterfaceSelector The interface selector. The client must have 
	created one of these first.
	@param aObserver The observer of this interface.
	@return A new CRemConSideKeyTarget, owned by the interface selector.
	*/
	IMPORT_C static CRemConSideKeyTarget* NewL(CRemConInterfaceSelector& aInterfaceSelector, 
		MRemConSideKeyTargetObserver& aObserver);
	
	IMPORT_C ~CRemConSideKeyTarget();

public:
	/**
	Sends a response to a 'side key up' command.
	@param aError The response error.
	*/
	IMPORT_C void SideKeyUpResponse(TInt aError);

	/**
	Sends a response to a 'side key down' command.
	@param aError The response error.
	*/
	IMPORT_C void SideKeyDownResponse(TInt aError);


private:
	/** 
	Constructor.
	@param aInterfaceSelector The interface selector.
	@param aObserver The observer of this interface.
	*/
	CRemConSideKeyTarget(CRemConInterfaceSelector& aInterfaceSelector, 
		MRemConSideKeyTargetObserver& aObserver);

private: // from CRemConInterfaceBase
	TAny* GetInterfaceIf(TUid aUid);

private: // from MRemConInterfaceIf
	void MrcibNewMessage(TUint aOperationId, const TDesC8& aData);

private: // unowned
	MRemConSideKeyTargetObserver& iObserver;

private: // owned
	// For operation-specific data.
	static const TUint KMaxOperationDataSize = 0x04;
	TBuf8<KMaxOperationDataSize> iOutData;
	};

#endif // REMCONSIDEKEYTARGET_H
