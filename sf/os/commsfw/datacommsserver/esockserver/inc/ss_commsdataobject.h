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

/**
 @file
 @internalTechnology
*/

#ifndef SS_COMMSDATAOBJECT_H
#define SS_COMMSDATAOBJECT_H


#include <comms-infras/api_ext_list.h>
#include <comms-infras/ss_platsec_apiext.h>
#include <comms-infras/ss_commsprov.h>
#include <comms-infras/metavirtctor.h>
#include <metadatabase.h>
#include <comms-infras/ss_nodemessages_legacy_internal.h>


namespace ESock
{
class XCommsDataObject;


class MQueryConnSettingsApiExt
    {
public:
	static const TInt KInterfaceId = 0x102864D4;

	virtual TInt GetInt(CommsDat::TMDBElementId aElementId, TUint32& aValue, MPlatsecApiExt* aPlatsecApiExt) = 0;
	virtual TInt GetInt(CommsDat::TMDBElementId aElementId, TInt& aValue, MPlatsecApiExt* aPlatsecApiExt) = 0;
	virtual TInt GetBool(CommsDat::TMDBElementId aElementId, TBool& aValue, MPlatsecApiExt* aPlatsecApiExt) = 0;
	virtual TInt GetText(CommsDat::TMDBElementId aElementId, HBufC8*& aValue, MPlatsecApiExt* aPlatsecApiExt) = 0;
	virtual TInt GetText(CommsDat::TMDBElementId aElementId, HBufC16*& aValue, MPlatsecApiExt* aPlatsecApiExt) = 0;
    };



/**
A self-propagating extension message. It advances down the stack looking for
a MeshMachine::AMMNodeBase that implements the interface requested by the comms data object
@internalComponent
*/
class TGetOrSetParameters : public ESock::TCFSigLegacyRMessage2PlatsecExt,
							public Messages::TSignatureBase::TTypeToMessageId<TCFLegacyMessageInternal::EGetOrSetParameters, TCFLegacyMessage::ERealmId>
    {
public:
    inline TGetOrSetParameters() {};
	TGetOrSetParameters(const Den::RSafeMessage& aMessage, XCommsDataObject*& aDataObject)
    :	ESock::TCFSigLegacyRMessage2PlatsecExt(Messages::TNodeSignal::TMessageId(EId, ERealm), aMessage),
        iDataObject(&aDataObject)
        {
        }

protected:
	virtual TBool CanProcess(MeshMachine::TNodeContextBase& aContext);
    virtual void ForwardL(MeshMachine::TNodeContextBase& aContext);
    virtual void ProcessL(MeshMachine::TNodeContextBase& aContext);

	virtual TInt CheckError(MeshMachine::TNodeContextBase& aContext, TInt aError);

private:
	void Complete(MeshMachine::TNodeContextBase& aContext, TInt aError);

public:
    DECLARE_MVIP_CTR(TGetOrSetParameters)
    EXPORT_DATA_VTABLE_AND_FN

private:
	XCommsDataObject** iDataObject;
    };



} // namespace ESock

#endif
// SS_COMMSDATAOBJECT_H

