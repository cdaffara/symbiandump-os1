// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include <btsdp.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_SDPDATABASE);
#endif

#ifdef _DEBUG
PANICCATEGORY("attrrang");
#endif

EXPORT_C TAttrRange::TAttrRange()
	{
	}

EXPORT_C TAttrRange::TAttrRange(TSdpAttributeID aAttrId)
 : iStart(aAttrId), iEnd(aAttrId)
 	{
 	}
	
EXPORT_C TAttrRange::TAttrRange(TSdpAttributeID aStart, TSdpAttributeID aEnd)
 : iStart(aStart), iEnd(aEnd)
	{
	ASSERT_DEBUG(iStart <= iEnd);
	}
	
EXPORT_C TBool TAttrRange::IsInRange(TSdpAttributeID aAttrId) const
	{
	return aAttrId >= iStart && aAttrId <= iEnd;
	}

EXPORT_C TBool TAttrRange::IsContiguousWith(TSdpAttributeID aAttrId) const 
	{
	return (aAttrId >= iStart - 1) && (aAttrId <= iEnd + 1);
	}

EXPORT_C TBool TAttrRange::IsContiguousWith(TAttrRange aRange) const
	{
	return IsContiguousWith(aRange.iStart) || IsContiguousWith(aRange.iEnd);
	}

	

// EOF
