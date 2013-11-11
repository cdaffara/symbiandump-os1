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
// Constants for use in storing comms data via CommsDat
// 
//

/**
 @file
 @publishedAll
 @released
*/


#if (!defined COMMSDAT_H)
#define       COMMSDAT_H

#include <comms-infras/metatype.h>
#include <e32property.h>


namespace CommsDat
{

/**
Use this constant for Debug panic calls

@publishedAll
@released
*/
_LIT(KCommsDatName,"CommsDat");

/**
Use this constant for Debug panic calls

@publishedAll
@released
*/
enum
	{
	ECommitBeforeOpenTransaction = 1,
	ERollBackBeforeOpenTransaction = 2,
	ESessionClosed = 3,
	ENotifierNotCreated = 4,
    EWrongAPPrioritySelPolRecord = 5,
    EWrongIAPServiceType = 6,
    EInvalidIAP = 7
	};
	
using namespace Meta;

//
// VERSION INFO
//
/* Please note:
 * ------------ 
 * Concrete schema version should be used in the client applicaiton (curently available
 * KCDVersion1_1 or KCDVersion1_2). 
 * 
 * Until the 9.5 release there was only one schema version, KCDVersion1_1, which was 
 * available. From the 9.5 relase there is a possibility to use other schema version,
 * KCDVersion1_2. Important that it's still possible to use the KCVersion1_1 
 * constant, and use the deprecated records/tables, - CommsDat will invoke it's 
 * mapping functionality in this case to be backward compatible. 
 * 
 * For backward compatibility reason the KCDLatestVersion and KCDCurrentVersion 
 * constants are mapped to the KCDVersion1_1 constant. These constants are 
 * deprecated.
 * 
 * ! PLEASE USE EXPLICIT SCHEMA VERSION WHEN CREATING A COMMSDAT SESSION OBJECT !
 */

/**
Versions of the dataset in use

Initial Version 
@publishedAll
@released
*/

#define KCDVersion1_1               TVersion(1,1,1)

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
/**
Some elements in Version KCDVersion1_1 now deprecated
These elements will be mapped if the client starts a session with KCDVersion1_1
They will be ignored or rejected as not supported if client starts a session with 
KCDVersion1_2

@publishedAll
@released
*/
#define KCDVersion1_2               TVersion(1,2,1)
#endif

/**
For BC reasons this constant maps to the KCDVersion1_1 dataschema version.

@publishedAll
@deprecated
*/
#define KCDLatestVersion            KCDVersion1_1

/**
For BC reasons this constant maps to the KCDVersion1_1 dataschema version.

@publishedAll
@deprecated
*/
#define KCDCurrentVersion           KCDVersion1_1


//
//  Attribute Flags
//

/**
Access control attribute flags.
When these are SET in the mask the attribute is obeyed
When they are CLEARED from the mask, the attribute is ignored
By default attributes are obeyed.

@publishedAll
@released
*/
typedef enum
{
  ECDNoWriteButDelete = 0x0000010,     // R/W Read Only but deleteable      Caller should explicitly choose to modify
                                       //                                   Enforced by convention only, not with Platform Security
  ECDHidden           = 0x0000020,     // R/W Read-Write Hidden data        Not policed - hides public utility info such as default values
                                       //                                   Enforced by convention only, not with Platform Security  
  ECDPrivate          = 0x0000040,     // R/W Read-Write Private data       For sensitive user data such as passwords
                                       //                                   Policed at storage server API with platsec capability
  ECDProtectedWrite   = 0x0000080,     // W   Write-Protected data          To allow data to be read only to most clients but modifiable by a few privileged clients
                                       //                                   Policed at storage server API with platsec capability
} TCDAttributeFlags;

//
// Masks for Element Type info
//


/**
Mask to use to hide attribute reserved bits

@publishedAll
@released
*/
#define KCDMaskHideAttrAndRes       0xffffff00


/**
Mask to use to show reserved bits

@publishedAll
@released
*/
#define KCDMaskShowRes              0x8000000f


/**
Mask to use to hide reserved bits

@publishedAll
@released
*/
#define KCDMaskHideRes              0x7ffffff0


/**
T x x   Mask to show Table Type info

@publishedAll
@released
*/
#define KCDMaskShowRecordType       0x7f800000


/**
x C x   Mask to show Column Type 

@publishedAll
@released
*/
#define KCDMaskShowFieldType        0x007f0000


/**
T C x   Mask to show Table and Column Type 

@publishedAll
@released
*/
#define KCDMaskShowType             0x7fff0000


/**
T x R   Mask to show Record id

@publishedAll - don't use this.  Replace with KCDMaskShowRecordId
@deprecated
*/
#define KCDMaskShowInstance         0x0000ff00


/**
T x R   Mask to show Record id
Use in place of KCDMaskShowInstance
@publishedAll
@released
*/
#define KCDMaskShowRecordId         0x0000ff00


/**
T C R   Mask to show Field instance without attributes or reserved bits

@publishedAll
@released
*/
#define KCDMaskShowField            0x7fffff00


/**
T x R   Mask to show Record Type and Record Id without column type, attributes or reserved bits

@publishedAll
@released
*/
#define KCDMaskShowRecordTypeAndId  (KCDMaskShowRecordType | KCDMaskShowRecordId)

/**
x C R   Mask to show Column Type and Record Id without Record type, attributes or reserved bits

@publishedAll
@released
*/
#define KCDMaskShowColumnTypeAndRecordId  (KCDMaskShowFieldType |  KCDMaskShowRecordId)


/**
Mask to show Attributes

@publishedAll
@released
*/
#define KCDMaskShowAttributes       0x000000f0

/**
Mask to use to show attributes and reserved bits

@publishedAll
@released
*/
#define KCDMaskShowAttrAndRes       0x000000ff


/**
Id for the Initial record in a record set

@publishedAll
@released
*/
#define KCDInitialRecordId			0x00000100


/**
Id for the Initial table in the database

@publishedAll
@released
*/
#define KCDInitialTableId			0x00800000


/**
Id for the Initial User defined table in the database
clients have 64 table ids available to them

@publishedAll
@released
*/
#define KCDInitialUDefRecordType   0x5F800000


/**
Id for the Last User defined table in the database.

@publishedAll
@released
*/
#define KCDLastUDefRecordType      0x7F800000


/**
Id for the Initial column in a record set

@publishedAll
@released
*/
#define KCDInitialColumnId			0x00010000


/**
T C 0   location for default field values

@publishedAll
@released
*/
#define KCDDefaultRecord            0x00000000


/**
Combine with table id and Column id to give the location of column type info

@publishedAll
@released
*/
#define KCDColumnTypeInfo           0x0000ff00



/**
Combine with table id to give request for a new record in a table.
The table must already exist.

@publishedAll
@released
*/
#define KCDNewRecordRequest			0x807f0001


/**
Combined with CMDBField, it finds the TableId given the table name

@publishedAll
@released
*/
#define KCDMaskGenericTableName			0x007fff00



/**
Maximum number of tables that can be created in one repository

@publishedAll
@released
*/
#define KCDMaxTables                254


/**
Maximum number of User defined tables that can be created in one repository

@publishedAll
@released
*/
#define KCDMaxUserDefTables         64


/**
Maximum number of columns that can be created in one table

@publishedAll
@released
*/
#define KCDMaxColumns               126  


/**
Maximum number of records that can be stored in one table

@publishedAll
@released
*/
#define KCDMaxRecords               254


/**
Maximum number of characters in a field name

@publishedAll
@released
*/
#define KCDMaxFieldNameLength        64


/**
Macro for interaction with netmeta
@publishedAll
@released
*/
#define X_REGISTER_ATTRIBUTE( thisMetaClass, var, metaType ) \
 { _FOFF( thisMetaClass, var ), Meta::metaType##VisitorFactoryL },


#define EXP_DATA_VTABLE \
 IMPORT_C virtual Meta::SVDataTableEntry const* GetVDataTable() const;	\
 IMPORT_C virtual TUint8* GetAttribPtr(const TInt aOffset) const;	\
 IMPORT_C static Meta::SVDataTableEntry const* GetVDataTableStatic(); \
 static const Meta::SVDataTableEntry iVDataTable[];


/**
Macros used to implement virtual functions from MMetaData interface and construct the v data table
@publishedAll
@released
*/
#define EXP_START_ATTRIBUTE_TABLE( thisMetaClass, uid, typeId ) \
 EXPORT_C Meta::SVDataTableEntry const* thisMetaClass::GetVDataTable() const { return &iVDataTable[0]; };	\
 EXPORT_C TUint8* thisMetaClass::GetAttribPtr(const TInt aOffset) const { return (TUint8*)this + aOffset; };	\
 EXPORT_C Meta::SVDataTableEntry const* thisMetaClass::GetVDataTableStatic() { return &iVDataTable[0]; }; \
 const Meta::SVDataTableEntry thisMetaClass::iVDataTable[] = { \
 { uid, reinterpret_cast<Meta::TMetaTypeNewL>(typeId) },


/**
@publishedAll
@released
*/
typedef enum
{
	EText,          //< Text field - descriptor with a maximum length of 50 unicode characters.
 	EDesC8,         //< Buffer of TUInt8. Maximum length is 1024 * 8. 
 	EUint32,		//< Unsigned 32 bit integer field.
 	EInt,			//< Signed integer field
 	EBool,			//< Boolean field
 	EMedText,       //< Text field - descriptor with a maximum length of 256 unicode characters.
 	ELongText, 		//< Text field - descriptor with a maximum length of 1024 unicode characters.
    ELink			//< This field is an integer that identifies a record in another table.

} TCDFieldValueTypes;

/**
Flag to indicate change in field
Not for use in database.  Just with fields

@publishedAll
*/
#define KCDChangedFlag      0x00000001
    
} //end namespace CommsDat

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <commsdat_internal.h>
#include <commsdat_partner.h>
#endif

#endif
