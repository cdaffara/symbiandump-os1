// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REMCONTRACKINFOCONTROLLER_H
#define REMCONTRACKINFOCONTROLLER_H

#include <e32base.h>
#include <remcon/remconinterfacebase.h>
#include <remcon/remconinterfaceif.h>

class MRemConTrackInfoControllerObserver;
class CRemConInterfaceSelector;

/**
Client-instantiable type supporting sending TrackInfo API commands.
*/
NONSHARABLE_CLASS(CRemConTrackInfoController) : public CRemConInterfaceBase, 
								       	        public MRemConInterfaceIf
	{
public:
	/**
	Factory function.
	@param aInterfaceSelector The interface selector. The client must have 
	created one of these first.
	@param aObserver The observer of this interface.
	@return A new CRemConTrackInfoController, owned by the interface selector.
	*/
	IMPORT_C static CRemConTrackInfoController* NewL(CRemConInterfaceSelector& aInterfaceSelector, 
		MRemConTrackInfoControllerObserver& aObserver);
	
	/** Destructor. */
	IMPORT_C ~CRemConTrackInfoController();

public:
	/** 
	Sends a 'set track name' command.
	Only one command per controller session can be outstanding at any one time.
	@param aStatus Used by RemCon to indicate completion of the send request.
	@param aTrackName The track name.
	@param aNumRemotes On success, will contain the number of remotes the 
	command was sent to.
	@panic USER 23, if the length of aTrackName is greater than 58 Characters.
	*/
	IMPORT_C void SetTrackName(TRequestStatus& aStatus, const TDesC& aTrackName, TUint& aNumRemotes);

/**
The following section of code is included to allow internal back to back 
testing of this component within Symbian Software Ltd. It will only be 
compiled in specially configured test builds. It will never be available 
in any release and so should never be used outside of Symbian owned test 
code.
*/
#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.
	*/
	IMPORT_C void GetTrackName(TRequestStatus& aStatus, TUint& aNumRemotes);
	
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.
	*/	
	IMPORT_C void GetArtist(TRequestStatus& aStatus, TUint& aNumRemotes);
	
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.
	*/
	IMPORT_C void GetTrackDuration(TRequestStatus& aStatus, TUint& aNumRemotes);
	
private:
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.
	*/
	void HandleGetTrackNameResponse(TInt aError, const TDesC8& aData);
	
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.
	*/
	void HandleGetArtistResponse(TInt aError, const TDesC8& aData);
	
	/**
	This method is only available in specially configured test builds and will 
	not be found in any released code.
	*/
	void HandleGetTrackDurationResponse(TInt aError, const TDesC8& aData);	
#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT

private:
	/** 
	Constructor.
	@param aInterfaceSelector The interface selector.
	@param aObserver The observer of this interface.
	*/
	CRemConTrackInfoController(CRemConInterfaceSelector& aInterfaceSelector, 
		MRemConTrackInfoControllerObserver& aObserver);

private: // from CRemConInterfaceBase
	TAny* GetInterfaceIf(TUid aUid);

private: // from MRemConInterfaceIf
	void MrcibNewMessage(TUint aOperationId, const TDesC8& aData);

private: // unowned
	MRemConTrackInfoControllerObserver& iObserver;

private: // owned
	TBuf8<KMaxName> iOutData;
	};

#endif // REMCONTRACKINFOCONTROLLER_H
