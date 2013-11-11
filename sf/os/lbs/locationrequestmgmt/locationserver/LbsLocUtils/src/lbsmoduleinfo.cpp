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

// System
#include <e32base.h>

// Lbs
#include "lbsdevloggermacros.h"
#include <lbscommon.h>
#include "LbsInternalInterface.h"

// Component
#include <lbs/lbsmoduleinfo.h>



/** Open


*/
EXPORT_C void RLbsModuleInfo::OpenL()
	{
	LBSLOG(ELogP9, "->S RLbsModuleInfo::OpenL() LbsModuleInfo\n");
	}

/** Close
*/
EXPORT_C void RLbsModuleInfo::Close()
	{
	LBSLOG(ELogP9, "->S RLbsModuleInfo::Close() LbsModuleInfo\n");
	}

/** Default constructor


*/
EXPORT_C RLbsModuleInfo::RLbsModuleInfo():
	iReserved(NULL)
	{
	LBSLOG(ELogP9, "->S RLbsModuleInfo::RLbsModuleInfo() LbsModuleInfo\n");
	}

/** Get the number of modules by module count key.

@param aNumModules Return the number of modules.
@return Symbian Location service Error code


*/
EXPORT_C TInt RLbsModuleInfo::GetNumModules(TUint& aNumModules)
	{
	LBSLOG(ELogP9, "->S RLbsModuleInfo::GetNumModules() LbsModuleInfo\n");

	TInt err = LbsModuleInfo::GetNumModules(aNumModules);
	LBSLOG2(ELogP9, "  < TUint aNumModules =  %u", aNumModules);
	LBSLOG2(ELogP9, "  Return =  %d\n", err);
	return err;
	}
	
/** Get module info data by module index.

@param aModuleIndex Module index (zero based).
@param aModuleInfo A reference to a TPositionModuleInfoBase object.
@see TPositionModuleInfoBase
@return Symbian Location service Error code


*/
EXPORT_C TInt RLbsModuleInfo::GetModuleInfoByIndex(TInt aModuleIndex, TPositionModuleInfoBase& aModuleInfo)
	{
	LBSLOG(ELogP9, "->S RLbsModuleInfo::GetModuleInfoByIndex() LbsModuleInfo\n");
	LBSLOG2(ELogP9, "  > TInt aModuleIndex  = %d\n", aModuleIndex);
	
	TInt err = LbsModuleInfo::GetModuleInfoByIndex(aModuleIndex, &aModuleInfo, aModuleInfo.PositionClassSize());
	
	LBSLOG(ELogP9, "  < TPositionModuleInfoBase aModuleInfo  =\n");
	LBSLOG_TPOSITIONMODULE(aModuleInfo);
	LBSLOG2(ELogP9, "  Return  = %d\n", err);	
	return err;
	}

/** Get module info data by module ID.

@param aModuleId Position module ID.
@param aModuleInfo A reference to a TPositionModuleInfoBase object.
@see TPositionModuleInfoBase
@return Symbian Location service Error code


*/
EXPORT_C TInt RLbsModuleInfo::GetModuleInfoById(TPositionModuleId aModuleId, TPositionModuleInfoBase& aModuleInfo)
	{
	LBSLOG(ELogP9, "->S RLbsModuleInfo::GetModuleInfoById() LBSModuleInfo\n");
	LBSLOG2(ELogP9, "  < TPositionModuleId aDataSaModuleId  = 0x%08X\n", aModuleId.iUid);

	TInt err = LbsModuleInfo::GetModuleInfoById(aModuleId, &aModuleInfo, aModuleInfo.PositionClassSize());
	
	LBSLOG(ELogP9, "  < TPositionModuleInfoBase aModuleInfo  =\n");
	LBSLOG_TPOSITIONMODULE(aModuleInfo);
	LBSLOG2(ELogP9, "  Return  = %d\n", err);	
	return err;
	}
	
/** Get the Id of the data source plugin.

@param aModuleId Position module ID.
@param aDataSourceId a reference to TUid for the result
@return Symbian Location service Error code


*/
EXPORT_C TInt RLbsModuleInfo::GetDataSourceId(TPositionModuleId aModuleId, TUid& aDataSourceId)
	{
	LBSLOG(ELogP9, "->S RLbsModuleInfo::GetDataSourceId() LbsModule\n");
	LBSLOG2(ELogP9, "  < TPositionModuleId aDataSaModuleId  = 0x%08X\n", aModuleId.iUid);

	TInt err = LbsModuleInfo::GetDataSourceId(aModuleId, aDataSourceId);
	
	LBSLOG2(ELogP9, "  < TUid aDataSource  = 0x%08X\n", aDataSourceId.iUid);
	LBSLOG2(ELogP9, "  Return  = %d\n", err);	
	return err;
	}
