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
 @internalTechnology
*/

#if (!defined METACONTAINER_H)
#define METACONTAINER_H

#include <e32base.h>
#include <e32std.h>
#include <elements/metadata.h>


namespace Meta
{

class RMetaDataContainerBase : public RPointerArray<SMetaData>
/**

Container of meta objects.
Base class.
Derived containers need to implement static LoadL.

@internalComponent
@released since v9.0 */
    {
public:
	IMPORT_C explicit RMetaDataContainerBase();

    IMPORT_C void Copy(const RMetaDataContainerBase& aMetaContainer);
    IMPORT_C TInt Store(TDes8& aDes) const;

    IMPORT_C TInt Load(TPtrC8& aDes);
	IMPORT_C TInt Length() const;

	IMPORT_C STypeId GetTypeId() const;

protected:
	virtual SMetaData* LoadElementL(TPtrC8& aDes) = 0;
    };

template<class ELEMENT>
class RMetaDataContainer : public RMetaDataContainerBase
/**

Templated container of meta objects that implements static LoadL.
Meta object type (template parameter) needs to implement its own static LoadL.

@internalComponent
@released since v9.0 */
	{
public:
	static RMetaDataContainer<ELEMENT>* LoadL(TPtrC8& aDes);

	explicit inline RMetaDataContainer();

protected:
	virtual SMetaData* LoadElementL(TPtrC8& aDes);
	};

typedef RMetaDataContainer<SMetaDataECom> RMetaDataEComContainer;

} //namespace Meta

#include <elements/metacontainer.inl>

#endif //METACONTAINER_H


