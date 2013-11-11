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
// Encapsulates the debug logging mechanism.
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __SSMDEBUG_H__
#define __SSMDEBUG_H__

#ifdef _DEBUG
#include <e32debug.h>

#define SSMLOGLEAVE(_p1) {TInt _val=_p1; RDebug::Printf("%s %d: Unconditional leave %s evaluates to %d ", __FILE__,__LINE__, #_p1, _val); User::Leave(_val); }
#define SSMLOGLEAVEIFERROR(_p1) {TInt _err=_p1; if(KErrNone!=_err) RDebug::Printf("%s %d: Leaving because %s evaluates to (%d)", __FILE__,__LINE__, #_p1, _err); User::LeaveIfError(_err); }
#define SSMLOGLEAVEIFNOTERROR(_p1) {TInt _err=_p1; if(KErrNone==_err) { RDebug::Printf("%s %d: Leaving (with no error) because %s evaluates to (%d)", __FILE__,__LINE__, #_p1, _err); User::Leave(_err); } }
#define SSMLOGLEAVEIFNULL(_p1) {TAny* _p=_p1; if(NULL==_p) RDebug::Printf("%s %d: Leaving because %s evaluates to NULL", __FILE__,__LINE__, #_p1); User::LeaveIfNull(_p); }
#define SSMLOGLEAVEIFFALSE(_p1, _p2) {TInt _val=_p1; if(!_val) RDebug::Printf("%s %d: Leaving with %s because %s evaluates to FALSE", __FILE__,__LINE__, #_p2, #_p1); if(!_val) User::Leave(_p2); }
#define SSMLOGLEAVEIFTRUE(_p1, _p2) {TInt _val=_p1; if(_val) RDebug::Printf("%s %d: Leaving with %s because %s evaluates to TRUE", __FILE__,__LINE__, #_p2, #_p1); if(_val) User::Leave(_p2); }

#define DEBUGPRINT1(_p1) RDebug::Print(_p1)
#define DEBUGPRINT2(_p1,_p2) RDebug::Print(_p1,_p2)
#define DEBUGPRINT3(_p1,_p2,_p3) RDebug::Print(_p1,_p2,_p3)
#define DEBUGPRINT4(_p1,_p2,_p3,_p4) RDebug::Print(_p1,_p2,_p3,_p4)
#define DEBUGPRINT5(_p1,_p2,_p3,_p4,_p5) RDebug::Print(_p1,_p2,_p3,_p4,_p5)
#define DEBUGPRINT6(_p1,_p2,_p3,_p4,_p5,_p6) RDebug::Print(_p1,_p2,_p3,_p4,_p5,_p6)
#define DEBUGPRINT7(_p1,_p2,_p3,_p4,_p5,_p6,_p7) RDebug::Print(_p1,_p2,_p3,_p4,_p5,_p6,_p7)

#define DEBUGPRINT1A(_p1) RDebug::Printf("%s %d: %s", __FILE__, __LINE__, _p1)
#define DEBUGPRINT2A(_p1,_p2) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2)
#define DEBUGPRINT3A(_p1,_p2,_p3) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3)
#define DEBUGPRINT4A(_p1,_p2,_p3,_p4) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3, _p4)
#define DEBUGPRINT5A(_p1,_p2,_p3,_p4,_p5) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3, _p4,_p5)
#define DEBUGPRINT6A(_p1,_p2,_p3,_p4,_p5,_p6) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3, _p4,_p5,_p6)
#define DEBUGPRINT7A(_p1,_p2,_p3,_p4,_p5,_p6,_p7) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3, _p4,_p5,_p6,_p7)

#ifdef SSM_VERBOSE_DEBUG
#define DEBUGVERBOSE1A(_p1) RDebug::Printf("%s %d: %s", __FILE__, __LINE__, _p1)
#define DEBUGVERBOSE2A(_p1,_p2) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2)
#define DEBUGVERBOSE3A(_p1,_p2,_p3) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3)
#define DEBUGVERBOSE4A(_p1,_p2,_p3,_p4) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3, _p4)
#define DEBUGVERBOSE5A(_p1,_p2,_p3,_p4,_p5) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3, _p4,_p5)
#define DEBUGVERBOSE6A(_p1,_p2,_p3,_p4,_p5,_p6) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3, _p4,_p5,_p6)
#define DEBUGVERBOSE7A(_p1,_p2,_p3,_p4,_p5,_p6,_p7) RDebug::Printf("%s %d: "_p1, __FILE__, __LINE__, _p2, _p3, _p4,_p5,_p6,_p7)
#else
#define DEBUGVERBOSE1A(_p1)
#define DEBUGVERBOSE2A(_p1,_p2)
#define DEBUGVERBOSE3A(_p1,_p2,_p3)
#define DEBUGVERBOSE4A(_p1,_p2,_p3,_p4)
#define DEBUGVERBOSE5A(_p1,_p2,_p3,_p4,_p5)
#define DEBUGVERBOSE6A(_p1,_p2,_p3,_p4,_p5,_p6)
#define DEBUGVERBOSE7A(_p1,_p2,_p3,_p4,_p5,_p6,_p7)
#endif	//SSM_VERBOSE_DEBUG

#else

#define SSMLOGLEAVE(_p1) 			User::Leave(_p1)
#define SSMLOGLEAVEIFERROR(_p1) 		User::LeaveIfError(_p1)
#define SSMLOGLEAVEIFNOTERROR(_p1) 	{ TInt _err=_p1; if(KErrNone==_err) User::Leave(_err); }
#define SSMLOGLEAVEIFNULL(_p1) 		User::LeaveIfNull(_p1)
#define SSMLOGLEAVEIFFALSE(_p1, _p2) {TInt _val=_p1; if(!_val) User::Leave(_p2); }
#define SSMLOGLEAVEIFTRUE(_p1, _p2) 	{TInt _val=_p1; if(_val) if(_val) User::Leave(_p2); }

#define DEBUGPRINT1(_p1)
#define DEBUGPRINT2(_p1,_p2)
#define DEBUGPRINT3(_p1,_p2,_p3)
#define DEBUGPRINT4(_p1,_p2,_p3,_p4)
#define DEBUGPRINT5(_p1,_p2,_p3,_p4,_p5)
#define DEBUGPRINT6(_p1,_p2,_p3,_p4,_p5,_p6)
#define DEBUGPRINT7(_p1,_p2,_p3,_p4,_p5,_p6,_p7)

#define DEBUGPRINT1A(_p1)
#define DEBUGPRINT2A(_p1,_p2)
#define DEBUGPRINT3A(_p1,_p2,_p3)
#define DEBUGPRINT4A(_p1,_p2,_p3,_p4)
#define DEBUGPRINT5A(_p1,_p2,_p3,_p4,_p5)
#define DEBUGPRINT6A(_p1,_p2,_p3,_p4,_p5,_p6)
#define DEBUGPRINT7A(_p1,_p2,_p3,_p4,_p5,_p6,_p7)

#define DEBUGVERBOSE1A(_p1)
#define DEBUGVERBOSE2A(_p1,_p2)
#define DEBUGVERBOSE3A(_p1,_p2,_p3)
#define DEBUGVERBOSE4A(_p1,_p2,_p3,_p4)
#define DEBUGVERBOSE5A(_p1,_p2,_p3,_p4,_p5)
#define DEBUGVERBOSE6A(_p1,_p2,_p3,_p4,_p5,_p6)
#define DEBUGVERBOSE7A(_p1,_p2,_p3,_p4,_p5,_p6,_p7)

#endif //_DEBUG

#endif //__SSMDEBUG_H__

