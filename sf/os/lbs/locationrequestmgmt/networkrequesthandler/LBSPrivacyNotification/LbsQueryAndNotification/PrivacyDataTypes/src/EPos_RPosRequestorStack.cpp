// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <s32mem.h>
#include <lbs/epos_cposrequestor.h>
#include <lbs/epos_cposservicerequestor.h>
#include <lbs/epos_cposcontactrequestor.h>
#include <lbs/epos_rposrequestorstack.h>

const TInt KPosMaxRequestorLength = 255;


// ---------------------------------------------------------------------------
/**
@internalComponent
@released
*/
void ResetAndDestroyRequestorStack(TAny* aRequestorStack)
    {
    (reinterpret_cast<RPosRequestorStack*>(
        aRequestorStack))->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
/**
Internalizes the CPosRequestors from a stream.

@param aStream The read stream.
*/
EXPORT_C void RPosRequestorStack::ExternalizeL(RWriteStream& aStream) const
    {
    const TInt count = Count();
    aStream.WriteInt32L(count);

    for (TInt i = 0; i < count; i++)
        {
        CPosRequestor* req = (*this)[i];

        __ASSERT_DEBUG(req, User::Leave(KErrArgument));

        CPosRequestor::TRequestorType type = req->RequestorType();

        if (type != CPosRequestor::ERequestorService &&
             type != CPosRequestor::ERequestorContact)
            {
            User::Leave(KErrNotSupported);
            }

        aStream.WriteInt32L(type);
        aStream.WriteInt32L(req->RequestorIdFormat());
        aStream.WriteInt32L(req->NetworkType());
        aStream.WriteInt32L(req->RequestType());
        TPtrC id = req->RequestorIdString();
        aStream.WriteInt32L(id.Length());
        aStream.WriteL(id);
        }
    }

// ---------------------------------------------------------------------------
/**
Externalizes the CPosRequestors to a stream
@param aStream The write stream.
*/
EXPORT_C void RPosRequestorStack::InternalizeL(RReadStream& aStream)
    {
    ResetAndDestroy();

    CleanupStack::PushL(TCleanupItem(
        ResetAndDestroyRequestorStack, this));
    const TInt count = aStream.ReadInt32L();

    __ASSERT_ALWAYS(count >= 0, User::Leave(KErrArgument));

    for (TInt i = 0; i < count; i++)
        {
        CPosRequestor::TRequestorType type =
            static_cast<CPosRequestor::TRequestorType>(aStream.ReadInt32L());

        __ASSERT_ALWAYS(type == CPosRequestor::ERequestorService ||
                        type == CPosRequestor::ERequestorContact,
                        User::Leave(KErrArgument)
                        );

        CPosRequestor::TRequestorIdFormat format =
            static_cast<CPosRequestor::TRequestorIdFormat>(
                aStream.ReadInt32L());

        __ASSERT_ALWAYS(format >= CPosRequestor::EIdFormatGenericName &&
                        format <= CPosRequestor::EIdFormatMDN,
                        User::Leave(KErrArgument)
                        );

		CPosRequestor::TNetworkType networkType =
			static_cast<CPosRequestor::TNetworkType>(
				aStream.ReadInt32L());
				
		__ASSERT_ALWAYS(networkType >= CPosRequestor::ENetworkGSM &&
            			networkType <= CPosRequestor::ENetworkWCDMA,
            			User::Leave(KErrArgument)
            			);

		CPosRequestor::TRequestType requestType =
			static_cast<CPosRequestor::TRequestType>(
				aStream.ReadInt32L());

        __ASSERT_ALWAYS(TInt32(requestType) >= CPosRequestor::ERequestTypeUnknown &&
            			requestType <= CPosRequestor::ERequestAreaEvent,
            			User::Leave(KErrArgument)
        				);

        TInt idSize = aStream.ReadInt32L();

        __ASSERT_ALWAYS(idSize >= 0 &&
                        idSize <= KPosMaxRequestorLength,
                        User::Leave(KErrArgument)
                        );

        HBufC* id = HBufC::NewLC(idSize);
        TPtr idPtr(id->Des());
        aStream.ReadL(idPtr, idSize);

        CPosRequestor* requestor = NULL;

        if (type == CPosRequestor::ERequestorService)
            {
            requestor = CPosServiceRequestor::NewLC(format, *id);
            }
        else // (type == CPosRequestor::ERequestorContact)
            {
            requestor = CPosContactRequestor::NewLC(format, *id);
            }

		requestor->SetNetworkType(networkType);
		requestor->SetRequestType(requestType);

        User::LeaveIfError(Append(requestor));
        CleanupStack::Pop(requestor);
        CleanupStack::PopAndDestroy(id);
        }

    CleanupStack::Pop(this); // TCleanupItem(CleanupResetAndDestroy, this)
    }
