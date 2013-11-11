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
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <e32base.h>
#include <remcon/remconextapi1.h>
#include <remcontrackinfocontroller.h>
#include <remcontrackinfocontrollerobserver.h>
#include <remconinterfaceselector.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_EXTAPI1);
#endif

// Used to pad over the results field in the operation-specific data.
_LIT8(KResultsPad, "    ");

EXPORT_C CRemConTrackInfoController* CRemConTrackInfoController::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
																	MRemConTrackInfoControllerObserver& aObserver)
	{
	LOG_STATIC_FUNC

	CRemConTrackInfoController* self = new(ELeave) CRemConTrackInfoController(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRemConTrackInfoController::CRemConTrackInfoController(CRemConInterfaceSelector& aInterfaceSelector, 
													 MRemConTrackInfoControllerObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConTrackInfoApiUid), 
						 KMaxName,
						 aInterfaceSelector,
						 ERemConClientTypeController),
	iObserver(aObserver)
	{
	}

EXPORT_C CRemConTrackInfoController::~CRemConTrackInfoController()
	{
	LOG_FUNC
	}

TAny* CRemConTrackInfoController::GetInterfaceIf(TUid aUid)
	{
	TAny* ret = NULL;
	if ( aUid == TUid::Uid(KRemConInterfaceIf1) )
		{
		ret = reinterpret_cast<TAny*>(
			static_cast<MRemConInterfaceIf*>(this)
			);
		}

	return ret;
	}

void CRemConTrackInfoController::MrcibNewMessage(TUint aOperationId, const TDesC8& aData)
	{
	LOG_FUNC
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);
	LOG1(_L("\taData.Length = %d"), aData.Length());
	
	// Get the response error out of aData.
	if ( aData.Length() < KRemConExtApi1MinimumDataLength )
		{
		return; // ditch malformed messages
		}
		
	TInt err = static_cast<TInt>(aData.Ptr()[0]);

	switch ( aOperationId )
		{
	case ERemConSetTrackName:
		iObserver.MrcticoSetTrackNameResponse(err);
		break;

//Only used in the pan-shared back to back test builds.
#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
	case ERemConGetTrackName:
		HandleGetTrackNameResponse(err, aData);
		break;
	case ERemConGetArtist:
		HandleGetArtistResponse(err, aData);
		break;
	case ERemConGetTrackDuration:
		HandleGetTrackDurationResponse(err, aData);
		break;
#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT

	default:
		break;
		}
	}

EXPORT_C void CRemConTrackInfoController::SetTrackName(TRequestStatus& aStatus, const TDesC& aTrackName, TUint& aNumRemotes)
	{
	LOG_FUNC

	// First copy aTrackName into a specific descriptor type to 
	// nail down the structure as we don't know what type of 
	// descriptor has been passed in. 
	
	// TBuf is restricted to 58 characters, this relates to the 
	// amount of available space in iOutData.
	TBuf<58> temp;
	temp.Copy(aTrackName);

	iOutData.Copy(KResultsPad()); //leaves 124 bytes of space
	
	TPckgBuf<TName> buf(temp);
	
	//Set buf to minimum required length
	//Size of iLength + Size of iMaxLength + Size of aTrackName
	//      4 Bytes   +        4 Bytes     +(Max. 58x2) 116 Bytes 
	//                                     = 124 Bytes Max Size
	buf.SetLength(sizeof(TUint) + sizeof(TUint) + temp.Size());
	iOutData.Append(buf);

	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConTrackInfoApiUid),
		(TUint)ERemConSetTrackName, 
		aNumRemotes,
		ERemConCommand,
		iOutData);
	}


//Included for internal testing purposes only. Should never be in a released product.
#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
/**
Sends a 'get track name' command.
	
@param aStatus Used by RemCon to indicate completion of the send request.
@param aNumRemotes On success, will contain the number of remotes the 
command was sent to.
*/
EXPORT_C void CRemConTrackInfoController::GetTrackName(TRequestStatus& aStatus, TUint& aNumRemotes)
	{
	LOG_FUNC

	iOutData.Zero();
	
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConTrackInfoApiUid),
		(TUint)ERemConGetTrackName, 
		aNumRemotes,
		ERemConCommand,
		iOutData);
	}

/**
Sends a 'get artist' command.
	
@param aStatus Used by RemCon to indicate completion of the send request.
@param aNumRemotes On success, will contain the number of remotes the 
command was sent to.
*/
EXPORT_C void CRemConTrackInfoController::GetArtist(TRequestStatus& aStatus, TUint& aNumRemotes)
	{
	LOG_FUNC
	
	iOutData.Zero();
	
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConTrackInfoApiUid),
		(TUint)ERemConGetArtist, 
		aNumRemotes,
		ERemConCommand,
		iOutData);
	}

/**
Sends a 'get track duration' command.
	
@param aStatus Used by RemCon to indicate completion of the send request.
@param aNumRemotes On success, will contain the number of remotes the 
command was sent to.
*/	
EXPORT_C void CRemConTrackInfoController::GetTrackDuration(TRequestStatus& aStatus, TUint& aNumRemotes)
	{
	LOG_FUNC
	
	iOutData.Zero();
	
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConTrackInfoApiUid),
		(TUint)ERemConGetTrackDuration, 
		aNumRemotes,
		ERemConCommand,
		iOutData);	
	}
	
/**
Extracts the track name from the 'get track name' response data and calls 
the relevant mixin function on the observer.	
	
@param The data passed with the response.
*/	
void CRemConTrackInfoController::HandleGetTrackNameResponse(TInt aError, const TDesC8& aData)
	{
	LOG_FUNC	
	
	TPckgBuf<TName> buf;
	buf.Copy((aData.Mid(KRemConExtApi1ResultDataLength)));
	iObserver.MrcticoGetTrackNameResponse(aError, buf());
	}

/**
Extracts the artist from the 'get artist' response data and calls 
the relevant mixin function on the observer.	
	
@param The data passed with the response.
*/	
void CRemConTrackInfoController::HandleGetArtistResponse(TInt aError, const TDesC8& aData)
	{
	LOG_FUNC

	TPckgBuf<TName> buf;
	buf.Copy((aData.Mid(KRemConExtApi1ResultDataLength)));
	iObserver.MrcticoGetArtistResponse(aError, buf());
	}

/**
Extracts the track duration from the 'get track duration' response data and calls 
the relevant mixin function on the observer.	
	
@param The data passed with the response.
*/	
void CRemConTrackInfoController::HandleGetTrackDurationResponse(TInt aError, const TDesC8& aData)
	{
	LOG_FUNC	
	
	TPckgBuf<TInt64> buf;
	buf.Copy((aData.Mid(KRemConExtApi1ResultDataLength)));
	TTime duration(buf());
	iObserver.MrcticoGetTrackDurationResponse(aError, duration);
	}

#pragma message ("The remote control Track Info extension API is being built with the extra back to back test code functionality included.  This should only happen in DEBUG build and never be released.")

#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
