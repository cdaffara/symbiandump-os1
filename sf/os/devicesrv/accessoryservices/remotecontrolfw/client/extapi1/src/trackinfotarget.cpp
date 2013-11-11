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
#include <remcontrackinfotarget.h>
#include <remcontrackinfotargetobserver.h>
#include <remconinterfaceselector.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_EXTAPI1);
#endif

EXPORT_C CRemConTrackInfoTarget* CRemConTrackInfoTarget::NewL(CRemConInterfaceSelector& aInterfaceSelector, 
																	MRemConTrackInfoTargetObserver& aObserver)
	{
	LOG_STATIC_FUNC

	CRemConTrackInfoTarget* self = new(ELeave) CRemConTrackInfoTarget(aInterfaceSelector, aObserver);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRemConTrackInfoTarget::CRemConTrackInfoTarget(CRemConInterfaceSelector& aInterfaceSelector, 
													 MRemConTrackInfoTargetObserver& aObserver)
:	CRemConInterfaceBase(TUid::Uid(KRemConTrackInfoApiUid), 
						 KMaxName,
						 aInterfaceSelector,
						 ERemConClientTypeTarget),
	iObserver(aObserver)
	{
	}

EXPORT_C CRemConTrackInfoTarget::~CRemConTrackInfoTarget()
	{
	LOG_FUNC
	}

TAny* CRemConTrackInfoTarget::GetInterfaceIf(TUid aUid)
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

#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
void CRemConTrackInfoTarget::MrcibNewMessage(TUint aOperationId, const TDesC8& aData)
#else
void CRemConTrackInfoTarget::MrcibNewMessage(TUint aOperationId, const TDesC8& /*aData*/)
#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
	{
	LOG_FUNC
	LOG1(_L("\taOperationId = 0x%02x"), aOperationId);

	switch ( aOperationId )
		{
	case ERemConGetTrackName:
		iObserver.MrctitoGetTrackName();
		break;

	case ERemConGetArtist:
		iObserver.MrctitoGetArtist();
		break;

	case ERemConGetTrackDuration:
		iObserver.MrctitoGetTrackDuration();
		break;

//Only used in the internal pan-shared back to back test builds. Should never be in a released product.
#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
	case ERemConSetTrackName:
		HandleSetTrackName(aData);
		break;
#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
	
	default:
		break;
		}
	}

EXPORT_C void CRemConTrackInfoTarget::GetTrackNameResponse(TRequestStatus& aStatus, const TDesC& aTrackName, TInt aError)
	{
	LOG_FUNC

	// First copy aTrackName into a specific descriptor type to 
	// nail down the structure as we don't know what type of 
	// descriptor has been passed in. 
	
	// TBuf is restricted to 58 characters, this relates to the 
	// amount of available space in iOutData.
	TBuf<58> temp;
	temp.Copy(aTrackName);

	iOutData.Copy((TUint8*)&aError, KRemConExtApi1ResultDataLength);
	
	TPckgBuf<TName> buf(temp);
	//Set buf to minimum required length
	//Size of iLength + Size of iMaxLength + Size of aTrackName
	//      4 Bytes   +        4 Bytes     +(Max. 58x2) 116 Bytes 
	//                                     = 124 Bytes Max Size
	buf.SetLength(sizeof(TUint) + sizeof(TUint) + temp.Size());
	
	iOutData.Append(buf);

	// We pass iNumRemotes even though we're not interested in its value but 
	// RemCon will write to this location so we need it to be somewhere safe.
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConTrackInfoApiUid),
		(TUint)ERemConGetTrackName, 
		iNumRemotes,
		ERemConResponse,
		iOutData);
	}

EXPORT_C void CRemConTrackInfoTarget::GetArtistResponse(TRequestStatus& aStatus, const TDesC& aArtist, TInt aError)
	{
	LOG_FUNC

	// First copy aArtist into a specific descriptor type to 
	// nail down the structure as we don't know what type of 
	// descriptor has been passed in. 
	
	// TBuf is restricted to 58 characters, this relates to the 
	// amount of available space in iOutData.
	TBuf<58> temp;
	temp.Copy(aArtist);

	iOutData.Copy((TUint8*)&aError, KRemConExtApi1ResultDataLength);
	
	TPckgBuf<TName> buf(temp);
	//Set buf to minimum required length
	//Size of iLength + Size of iMaxLength + Size of aTrackName
	//      4 Bytes   +        4 Bytes     +(Max. 58x2) 116 Bytes 
	//                                     = 124 Bytes Max Size
	buf.SetLength(sizeof(TUint) + sizeof(TUint) + temp.Size());
	
	iOutData.Append(buf);

	// We pass iNumRemotes even though we're not interested in its value but 
	// RemCon will write to this location so we need it to be somewhere safe.
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConTrackInfoApiUid),
		(TUint)ERemConGetArtist, 
		iNumRemotes,
		ERemConResponse,
		iOutData);
	}

EXPORT_C void CRemConTrackInfoTarget::GetTrackDurationResponse(TRequestStatus& aStatus, const TTime& aDuration, TInt aError)
	{
	LOG_FUNC

	iOutData.Copy((TUint8*)&aError, KRemConExtApi1ResultDataLength);
	TInt64 int64 = aDuration.Int64();
	TPckgBuf<TInt64> buf(int64);
	iOutData.Append(buf);

	// We pass iNumRemotes even though we're not interested in its value but 
	// RemCon will write to this location so we need it to be somewhere safe.
	InterfaceSelector().Send(aStatus, 
		TUid::Uid(KRemConTrackInfoApiUid),
		(TUint)ERemConGetTrackDuration, 
		iNumRemotes,
		ERemConResponse,
		iOutData);
	}


//Only used in the internal pan-shared back to back test builds. Should never be in a released product.
#ifdef SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
/**
Sends a response to a 'set track name' command.
	
@param aStatus Used by RemCon to indicate completion of the send request.
@param aError The response error.
*/
EXPORT_C void CRemConTrackInfoTarget::SetTrackNameResponse(TRequestStatus& aStatus, TInt aError)
	{
	LOG_FUNC
	
	iOutData.Copy((TUint8*)&aError, KRemConExtApi1ResultDataLength);

	InterfaceSelector().Send(aStatus,
		TUid::Uid(KRemConTrackInfoApiUid),
		(TUint)ERemConSetTrackName,
		iNumRemotes,
		ERemConResponse,
		iOutData);	
	}

/**
Extracts the track name from the 'set track name' command data and calls 
the relevant mixin function on the observer.
	
@param aData The data passed with the command.
*/	
void CRemConTrackInfoTarget::HandleSetTrackName(const TDesC8& aData)
	{
	LOG_FUNC

	TPckgBuf<TName> buf;
	buf.Copy((aData.Mid(KRemConExtApi1ResultDataLength)));
	iObserver.MrctitoSetTrackName(buf());
	}
#endif //SYMBIAN_ENABLE_TRACKINFO_BACKTOBACK_TEST_FUNCT
