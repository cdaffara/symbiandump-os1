// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Generic parameter set base classes.
// XParameterSetBase exists to support interworking with old CSubConParameterSet classes.
// XParameterSet has richer functionality so should be used for all new applications of Parameter Sets.
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __ES_PARAMETERSET_H
#define __ES_PARAMETERSET_H

#include <comms-infras/metadata.h>

class XParameterSetBase : public Meta::SMetaDataECom
	{
	};


const TInt KParameterSetTypeUid = 0x10272F44;

class XParameterSet : public XParameterSetBase
	{
public:
	IMPORT_C virtual ~XParameterSet();

	// Updates this parameter set with new information. The interpretation and 
	//  importance of new information may depend on type-specific interpretation
	//  of the fields in the object (e.g. a field somehow marked "unknown" in the new
	//  set should not be copied to this set, or some kind of lock on a
	//  field in this set should prevent that field being copied from the
	//  new set).
	//
	// This base implementation asserts, so as to force the consideration of this
	//  function for derived types.
	IMPORT_C virtual void UpdateWithL(const XParameterSet& aRhsBase);
	IMPORT_C virtual TInt AppendPrintableTo(TDes& aDestination) const;
	IMPORT_C virtual TInt _extension(TUid aExtensionId, TAny*& aReturnedExtension);

protected:
    // It's necessary to expose the typeid table to child classes so they can refer to it in their attribute tables (so IsTypeOf() works)
	EXPORT_TYPEID_TABLE
	};

#endif // __ES_PARAMETERSET_H

