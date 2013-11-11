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
 @publishedPartner
 @released
*/

#if (!defined COMMSDAT_PARTNER_H)
#define       COMMSDAT_PARTNER_H

#include <e32property.h>
#include <commsdat.h>

namespace Meta
    {
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBNumVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBLinkNumVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBTextVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBMedTextVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBLongTextVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBBinVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBLinkVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBRecordSetVisitorFactoryL(const TAny* mem, const TAny* data);
/**
@publishedPartner
@released
*/
    IMPORT_C MMetaType* TMDBGenericRecordVisitorFactoryL(const TAny* mem, const TAny* data);

    }


namespace CommsDat
{
/**
@publishedPartner
@released
*/
const TInt KMaxTextLength		= 50; 		//< Specifies the max length (characters) for short text.
/**
@publishedPartner
@released
*/
const TInt KMaxMedTextLength	= 255; 		//< Specifies the max length (characters) for medium text.
/**
@publishedPartner
@released
*/
const TInt KMaxLongTextLength	= 1024;		//< Specifies the max length (characters) for long text.
/**
@publishedPartner
@released
*/
const TInt KMaxNumLength		= 1;		//< Specifies the max length for an integer and a boolean.

// Length override on a normal text field
/**
@publishedPartner
@released
*/
const TInt KShortTextLength		= 32;

/**
Mask for all attribute settings that apply to read and write operations
see TCDAttributeFlags

  ECDNoWriteButDelete = 0x0000010                             
  ECDHidden           = 0x0000020
  ECDPrivate          = 0x0000040
  ECDProtectedWrite   = 0x0000080
  
@publishedPartner
@released
*/
#define KCDMaskShowReadWriteAttributes  (ECDNoWriteButDelete |ECDHidden | ECDPrivate | ECDProtectedWrite)

/**
Mask for all attribute settings that apply to read operations
see TCDAttributeFlags

  ECDHidden           = 0x0000020
  ECDPrivate          = 0x0000040

@publishedPartner
@released
*/
#define KCDMaskShowReadAttributes       (ECDHidden| ECDPrivate)     

/**
Combine with table id to give the location of table type info

@publishedPartner
@released
*/
#define KCDTableTypeInfo            0x007fff00

/*
Combine with table id to give location of record type info
*/
//#define KCDRecordTypeInfo			0x007f0000


/**
Use to request allocation of a new table id.
Should only be called when creating a new user-defined table

@publishedPartner
@released
*/
#define KCDNewTableRequest           KCDMaskGenericTableName


/**
Combine with table id to give request for a new column in a table.
The table must already exist.  New columns should only normally be 
requested when creating a new user-defined table

@publishedPartner
@released
*/
#define KCDNewColumnRequest			0x0000ff01


/**
@publishedPartner
@released
*/
#define KCDMaxRecordId     0x0000ff00
/**
@publishedPartner
@released
*/
#define KCDMaxColumnId     0x007f0000
/**
@publishedPartner
@released
*/
#define KCDMaxTableId      0x7f800000


/** UIDs for CommDB event notification */

/**
@publishedPartner
@released
*/
const TUid KUidCommDbNotificationEvent = {0x1020762E};
/**
@publishedPartner
@released
*/
const TUid KUidCommsDatStatusEvent = {KUidSystemCategoryValue};
/**
@publishedPartner
@released
*/
const TInt KCommsDatStatusEventCommitSeq = 0x1020762F;

/**
@publishedPartner
@released
*/
#define END_ATTRIBUTE_TABLE_BASE_N( baseMetaClass, baseId, entry ) \
 { (TInt)((TAny*)(baseMetaClass::GetVDataTableStatic() + entry)), NULL }};

} // namespace CommsDat

#endif //COMMSDAT_PARTNER_H

