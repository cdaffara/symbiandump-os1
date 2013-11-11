// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Constants for use in storing comms data via CommsDat
// 
//

/**
 @file
 @internalComponent
*/

#if (!defined COMMSDAT_INTERNAL_H)
#define       COMMSDAT_INTERNAL_H

namespace Meta
    {
/**
Visitor factory functions
@internalComponent
*/
    MMetaType* TMDBNumBaseVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@internalComponent
*/
    MMetaType* TMDBTextBaseVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@internalComponent
*/
    MMetaType* TMDBBinBaseVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@internalComponent
*/
    MMetaType* TMDBDeprecatedVisitorFactoryL(const TAny* mem, const TAny* data);
    }

namespace CommsDat
{
/**
T C R   Mask for Central Repository to find a single field

@internalComponent
*/
#define KCDMaskFindSingleField		0x7FFFFFFE



/*
Mask to show only basic type info, 
Masks out all info on links
@internalComponent
*/
#define KCDShowBasicTypeInfo  0x000000ff



/**
Typedefs for convenience in meta data definitions
@internalComponent
*/
using namespace Meta;

template <class TYPE> class TMDBVisitor;
class TMDBRecordLinkVisitor;
class TMDBRecordSetVisitor;
class TMDBGenericRecordVisitor;
class CMDBElement;

/*
Visitors for field values
*/

/**
Typedefs for convenience in meta data definitions
@internalComponent
*/
 typedef TMDBVisitor<TInt>                           TMDBNum;

/**
Typedefs for convenience in meta data definitions
@internalComponent
*/	
 typedef TMDBVisitor<TInt>                           TMDBBool;
 
/**
Typedefs for convenience in meta data definitions
@internalComponent
*/	
 typedef TMDBVisitor<TDesC>                          TMDBDes;	
 
/**
Typedefs for convenience in meta data definitions
@internalComponent
*/
 typedef TMDBVisitor<TDesC8>                         TMDBDes8;	

/*
Visitors for containers
*/

/**
Typedefs for convenience in meta data definitions
@internalComponent
*/
typedef TMDBRecordLinkVisitor						TMDBLink;	

/**
Typedefs for convenience in meta data definitions
@internalComponent
*/		
typedef TMDBRecordSetVisitor                        TMDBRecordSet;

/**
Typedefs for convenience in meta data definitions
@internalComponent
*/		
typedef TMDBGenericRecordVisitor                    TMDBGenRecord;		


/**
Metadatabase container type ids

@internalComponent
*/
#define KCDTIdMDBElement            123

/**
Metadatabase container type ids

@internalComponent
*/
#define KCDTIdMDBNumFieldBase       223

/**
Metadatabase container type ids

@internalComponent
*/
#define KCDTIdMDBTextFieldBase      224

/**
Metadatabase container type ids

@internalComponent
*/
#define KCDTIdMDBBinFieldBase       225

/**
Metadatabase container type ids

@internalComponent
*/
#define KCDTIdMDBRecordLinkBase     323

/**
Metadatabase container type ids

@internalComponent
*/
#define KCDTIdMDBRecordSetBase      523

/**
Metadatabase container type ids

@internalComponent
*/
#define KCDTIdMDBGenericRecord      585
	
} //end namespace CommsDat

#endif

