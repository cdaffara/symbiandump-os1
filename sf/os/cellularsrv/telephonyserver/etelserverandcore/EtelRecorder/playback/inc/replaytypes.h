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
//

#ifndef __REPLAYTYPES_H__
#define __REPLAYTYPES_H__


//offsets

enum TArgTypes
	{
	ENull=0,
	EDes8=1,
	EDes16=2,
	EInt=3,
	KMaxNoTypes //add other types before this one
	};

enum TDispatchType
	{
	EIsaNullNullNull,
	EIsaNullNullDes8,
	EIsaNullNullDes16,
	EIsaNullNullInt,
	EIsaNullDes8Null,
	EIsaNullDes8Des8,
	EIsaNullDes8Des16,
	EIsaNullDes8Int,
	EIsaNullDes16Null,
	EIsaNullDes16Des8,
	EIsaNullDes16Des16,
	EIsaNullDes16Int,
	EIsaNullIntNull,
	EIsaNullIntDes8,
	EIsaNullIntDes16,
	EIsaNullIntInt,

	EIsaDes8NullNull,
	EIsaDes8NullDes8,
	EIsaDes8NullDes16,
	EIsaDes8NullInt,
	EIsaDes8Des8Null,
	EIsaDes8Des8Des8,
	EIsaDes8Des8Des16,
	EIsaDes8Des8Int,
	EIsaDes8Des16Null,
	EIsaDes8Des16Des8,
	EIsaDes8Des16Des16,
	EIsaDes8Des16Int,
	EIsaDes8IntNull,
	EIsaDes8IntDes8,
	EIsaDes8IntDes16,
	EIsaDes8IntInt,
	
	EIsaDes16NullNull,
	EIsaDes16NullDes8,
	EIsaDes16NullDes16,
	EIsaDes16NullInt,
	EIsaDes16Des8Null,
	EIsaDes16Des8Des8,
	EIsaDes16Des8Des16,
	EIsaDes16Des8Int,
	EIsaDes16Des16Null,
	EIsaDes16Des16Des8,
	EIsaDes16Des16Des16,
	EIsaDes16Des16Int,
	EIsaDes16IntNull,
	EIsaDes16IntDes8,
	EIsaDes16IntDes16,
	EIsaDes16IntInt,
	
	EIsaIntNullNull,
	EIsaIntNullDes8,
	EIsaIntNullDes16,
	EIsaIntNullInt,
	EIsaIntDes8Null,
	EIsaIntDes8Des8,
	EIsaIntDes8Des16,
	EIsaIntDes8Int,
	EIsaIntDes16Null,
	EIsaIntDes16Des8,
	EIsaIntDes16Des16,
	EIsaIntDes16Int,
	EIsaIntIntNull,
	EIsaIntIntDes8,
	EIsaIntIntDes16,
	EIsaIntIntInt		
	};
	
	//TO DO RTelSubSessionBase::Set(const TInt aIpc, const RFile& aFile, TReqPriorityType aType) 
	// createkernelobject
	
struct TIpcToName
	{
	TInt iIpc;
	TPtrC8 iName;
	};
	
_LIT8(KCloseSessionIpc, "EEtelCloseSession");



		
const TDispatchType KDispatchTypeLookup	[KMaxNoTypes/*arg0 type offset*/]
										[KMaxNoTypes/*arg1 type offset*/]
										[KMaxNoTypes/*arg2 type offset*/] = 
	{	    /*arg2=0,                   1,                2                  3*/
			{
/*arg0=0, arg1=0*/{EIsaNullNullNull,	EIsaNullNullDes8, EIsaNullNullDes16, EIsaNullNullInt}, 
/*arg0=0, arg1=1*/{EIsaNullDes8Null,	EIsaNullDes8Des8, EIsaNullDes8Des16, EIsaNullDes8Int}, 
/*arg0=0, arg1=2*/{EIsaNullDes16Null,	EIsaNullDes16Des8,EIsaNullDes16Des16,EIsaNullDes16Int}, 
/*arg0=0, arg1=3*/{EIsaNullIntNull,	    EIsaNullIntDes8,  EIsaNullIntDes16,  EIsaNullIntInt}, 
			},	
			{
/*arg0=1, arg1=0*/{EIsaDes8NullNull,	EIsaDes8NullDes8, EIsaDes8NullDes16, EIsaDes8NullInt}, 
/*arg0=1, arg1=1*/{EIsaDes8Des8Null,	EIsaDes8Des8Des8, EIsaDes8Des8Des16, EIsaDes8Des8Int}, 
/*arg0=1, arg1=2*/{EIsaDes8Des16Null,	EIsaDes8Des16Des8,EIsaDes8Des16Des16,EIsaDes8Des16Int}, 
/*arg0=1, arg1=3*/{EIsaDes8IntNull,	    EIsaDes8IntDes8,  EIsaDes8IntDes16,  EIsaDes8IntInt},
			},
			{
/*arg0=2, arg1=0*/{EIsaDes16NullNull,	EIsaDes16NullDes8, EIsaDes16NullDes16, EIsaDes16NullInt}, 
/*arg0=2, arg1=1*/{EIsaDes16Des8Null,	EIsaDes16Des8Des8, EIsaDes16Des8Des16, EIsaDes16Des8Int}, 
/*arg0=2, arg1=2*/{EIsaDes16Des16Null,	EIsaDes16Des16Des8,EIsaDes16Des16Des16,EIsaDes16Des16Int}, 
/*arg0=2, arg1=3*/{EIsaDes16IntNull,    EIsaDes16IntDes8,  EIsaDes16IntDes16,  EIsaDes16IntInt},
			},
			{
/*arg0=3, arg1=0*/{EIsaIntNullNull,	    EIsaIntNullDes8, EIsaIntNullDes16, EIsaIntNullInt}, 
/*arg0=3, arg1=1*/{EIsaIntDes8Null,	    EIsaIntDes8Des8, EIsaIntDes8Des16, EIsaIntDes8Int}, 
/*arg0=3, arg1=2*/{EIsaIntDes16Null,	EIsaIntDes16Des8,EIsaIntDes16Des16,EIsaIntDes16Int}, 
/*arg0=3, arg1=3*/{EIsaIntIntNull,	    EIsaIntIntDes8,  EIsaIntIntDes16,  EIsaIntIntInt},
			}
	};
	

#endif
