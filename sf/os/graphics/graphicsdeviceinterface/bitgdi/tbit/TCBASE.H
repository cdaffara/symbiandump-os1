// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

enum TCBaseDummy {ETCBaseDummy};
template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8> 			
class TCMem 
	{
public:
	typedef C CT;
	TCMem(); 
	TCMem(TCBaseDummy,SR(*)());
	TCMem(CT* apOb,MR(CT::*)());
	TCMem(P1,SR(*)(P1));
	TCMem(CT* apOb,P1,MR(CT::*)(P1));
	TCMem(P1,P2,SR(*)(P1,P2));
	TCMem(CT* apOb,P1,P2,MR(CT::*)(P1,P2));
	TCMem(P1,P2,P3,SR(*)(P1,P2,P3));
	TCMem(CT* apOb,P1,P2,P3,MR(CT::*)(P1,P2,P3));
	TCMem(P1,P2,P3,P4,SR(*)(P1,P2,P3,P4));
	TCMem(CT* apOb,P1,P2,P3,P4,MR(CT::*)(P1,P2,P3,P4));
	TCMem(P1,P2,P3,P4,P5,SR(*)(P1,P2,P3,P4,P5));
	TCMem(CT* apOb,P1,P2,P3,P4,P5,MR(CT::*)(P1,P2,P3,P4,P5));
	TCMem(P1,P2,P3,P4,P5,P6,SR(*)(P1,P2,P3,P4,P5,P6));
	TCMem(CT* apOb,P1,P2,P3,P4,P5,P6,MR(CT::*)(P1,P2,P3,P4,P5,P6));
	TCMem(P1,P2,P3,P4,P5,P6,P7,SR(*)(P1,P2,P3,P4,P5,P6,P7));
	TCMem(CT* apOb,P1,P2,P3,P4,P5,P6,P7,MR(CT::*)(P1,P2,P3,P4,P5,P6,P7));
	TCMem(P1,P2,P3,P4,P5,P6,P7,P8,SR(*)(P1,P2,P3,P4,P5,P6,P7,P8));
	TCMem(CT* apOb,P1,P2,P3,P4,P5,P6,P7,P8,MR(CT::*)(P1,P2,P3,P4,P5,P6,P7,P8));		
private:
	enum {EDuringDestruction};
	void DoTest(TInt (*aF)(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*));
	void Panic(TUint aAddress,TUint aNum);
	static TInt DoStatFun0(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoStatFun1(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoStatFun2(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoStatFun3(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoStatFun4(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoStatFun5(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoStatFun6(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoStatFun7(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoStatFun8(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun0(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun1(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun2(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun3(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun4(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun5(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun6(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun7(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	static TInt DoMemFun8(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*);
	SR (*iSF0)();
	SR (*iSF1)(P1);
	SR (*iSF2)(P1,P2);
	SR (*iSF3)(P1,P2,P3);
	SR (*iSF4)(P1,P2,P3,P4);
	SR (*iSF5)(P1,P2,P3,P4,P5);
	SR (*iSF6)(P1,P2,P3,P4,P5,P6);
	SR (*iSF7)(P1,P2,P3,P4,P5,P6,P7);
	SR (*iSF8)(P1,P2,P3,P4,P5,P6,P7,P8);
	MR (CT::*iMF0)();
	MR (CT::*iMF1)(P1);
	MR (CT::*iMF2)(P1,P2);
	MR (CT::*iMF3)(P1,P2,P3);
	MR (CT::*iMF4)(P1,P2,P3,P4);
	MR (CT::*iMF5)(P1,P2,P3,P4,P5);
	MR (CT::*iMF6)(P1,P2,P3,P4,P5,P6);
	MR (CT::*iMF7)(P1,P2,P3,P4,P5,P6,P7);
	MR (CT::*iMF8)(P1,P2,P3,P4,P5,P6,P7,P8);
	P1 iP1;
	P2 iP2;
	P3 iP3;
	P4 iP4;
	P5 iP5;
	P6 iP6;
	P7 iP7;
	P8 iP8;
	SR iStatOb;
	CT*	iOb;
	TBool iStat;
	void operator=(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>){;};
	};

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>	
void TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoTest(TInt (*aF)(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>*))
	{
	User::Heap().iTestCodeRunning=ETrue;
	for (TUint ii=1;;++ii)
		{
		__UHEAP_MARK;
		__UHEAP_SETFAIL(RHeap::EDeterministic,ii);
		TInt ret=(*aF)(this);
		if (ret!=KErrNone) 
			{
			__UHEAP_MARKEND;
			if (User::Heap().iTestNumAllocCells!=0)
				{
				if (iStat)
					delete iStatOb;
				else					
					delete iOb;
				Panic(User::Heap().iTestAddress,ii);
				}
			}
		else
			{
			if (iStat)
				delete iStatOb;
			else
				delete iOb;
			__UHEAP_MARKEND;
			if (User::Heap().iTestNumAllocCells!=0)
				Panic(User::Heap().iTestAddress,EDuringDestruction);
			else
				{
				User::Heap().iTestCodeRunning=EFalse;
				__UHEAP_RESET;
				return;
				}
			}
		__UHEAP_RESET;
		}
	User::Heap().iTestCodeRunning=EFalse;
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8> 
void TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::Panic(TUint aAddress,TUint aNum)
	{
	TBuf<20> info = (aNum==EDuringDestruction) ? _L("DES: ") : _L("CON: ");
	info.AppendFormat(_L("%x\n"),aAddress);
	User::Panic(info,aNum);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun0(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF0))()));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun1(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	aOb->iStatOb=NULL;
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF1))(aOb->iP1)));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun2(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF2))(aOb->iP1,aOb->iP2)));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun3(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF3))(aOb->iP1,aOb->iP2,aOb->iP3)));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun4(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF4))(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4)));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun5(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF5))(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5)));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun6(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF6))(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6)));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun7(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF7))(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6,aOb->iP7)));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun8(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iStatOb=((*(aOb->iSF8))(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6,aOb->iP7,aOb->iP8)));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun0(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF0)()); 
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun1(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF1)(aOb->iP1)); 
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun2(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF2)(aOb->iP1,aOb->iP2)); 
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun3(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF3)(aOb->iP1,aOb->iP2,aOb->iP3)); 
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun4(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF4)(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4)); 
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun5(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF5)(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5)); 
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun6(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF6)(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6)); 
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun7(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF7)(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6,aOb->iP7)); 
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun8(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,(aOb->iOb->*aOb->iMF8)(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6,aOb->iP7,aOb->iP8)); 
	return(ret);
	}




/*template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon0(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C());
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon1(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C(aOb->iP1));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon2(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C(aOb->iP1,aOb->iP2));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon3(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C(aOb->iP1,aOb->iP2,aOb->iP3));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon4(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon5(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon6(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6));
	return(ret);
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon7(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6,aOb->iP7));
	return(ret);
	}  

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TInt TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoCon8(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>* aOb)
	{
	TRAPD(ret,aOb->iOb=new(ELeave)C(aOb->iP1,aOb->iP2,aOb->iP3,aOb->iP4,aOb->iP5,aOb->iP6,aOb->iP7,aOb->iP8));
	return(ret);
	}
*/
template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,MR(CT::*aF)())
	: iMF0(aF),iOb(apOb),iStat(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun0));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,P1 aP1,MR(CT::*aF)(P1))
	: iP1(aP1),iMF1(aF),iOb(apOb),iStat(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun1));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,P1 aP1,P2 aP2,MR(CT::*aF)(P1,P2))
	: iP1(aP1),iP2(aP2),iMF2(aF),iOb(apOb),iCon(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun2));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,P1 aP1,P2 aP2,P3 aP3,MR(CT::*aF)(P1,P2,P3))
	: iP1(aP1),iP2(aP2),iP3(aP3),iMF3(aF),iOb(apOb),iStat(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun3));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,P1 aP1,P2 aP2,P3 aP3,P4 aP4,MR(CT::*aF)(P1,P2,P3,P4))
	: iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iMF4(aF),iOb(apOb),iStat(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun4));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,P1 aP1,P2 aP2,P3 aP3,P4 aP4,P5 aP5,MR(CT::*aF)(P1,P2,P3,P4,P5))
	: iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5),iMF5(aF),iOb(apOb),iStat(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun5));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,P1 aP1,P2 aP2,P3 aP3,P4 aP4,P5 aP5,P6 aP6,MR(CT::*aF)(P1,P2,P3,P4,P5,P6))
	: iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5),iP6(aP6),iMF6(aF),iOb(apOb),iStat(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun6));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,P1 aP1,P2 aP2,P3 aP3,P4 aP4,P5 aP5,P6 aP6,P7 aP7,MR(CT::*aF)(P1,P2,P3,P4,P5,P6,P7))
	: iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5),iP6(aP6),iP7(aP7),iMF7(aF),iOb(apOb),iStat(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun7));
	}

#pragma warning ( disable : 4786 )	 //identifier was truncated to '255' characters in the debug information
template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(CT* apOb,P1 aP1,P2 aP2,P3 aP3,P4 aP4,P5 aP5,P6 aP6,P7 aP7,P8 aP8,MR(CT::*aF)(P1,P2,P3,P4,P5,P6,P7,P8))
	: iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5),iP6(aP6),iP7(aP7),iP8(aP8),iMF8(aF),iOb(apOb),iStat(EFalse)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoMemFun8));
	}
#pragma warning ( default : 4786 )
template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(TCBaseDummy,SR(*aF)())
	:iSF0(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun0));
	}	

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(P1 aP1,SR(*aF)(P1))
	:iP1(aP1),iSF1(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun1));
	}	

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(P1 aP1,P2 aP2,SR(*aF)(P1,P2))
	:iP1(aP1),iP2(aP2),iSF2(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun2));
	}	

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(P1 aP1,P2 aP2,P3 aP3,SR(*aF)(P1,P2,P3))
	:iP1(aP1),iP2(aP2),iP3(aP3),iSF3(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun3));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(P1 aP1,P2 aP2,P3 aP3,P4 aP4,SR(*aF)(P1,P2,P3,P4))
	:iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iSF4(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun4));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(P1 aP1,P2 aP2,P3 aP3,P4 aP4,P5 aP5,SR(*aF)(P1,P2,P3,P4,P5))
	:iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5),iSF5(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun5));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(P1 aP1,P2 aP2,P3 aP3,P4 aP4,P5 aP5,P6 aP6,SR(*aF)(P1,P2,P3,P4,P5,P6))
	:iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5),iP6(aP6),iSF6(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun6));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(P1 aP1,P2 aP2,P3 aP3,P4 aP4,P5 aP5,P6 aP6,P7 aP7,SR(*aF)(P1,P2,P3,P4,P5,P6,P7))
	:iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5),iP6(aP6),iP7(aP7),iSF7(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun7));
	}

template<class C,class SR,class MR,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::TCMem(P1 aP1,P2 aP2,P3 aP3,P4 aP4,P5 aP5,P6 aP6,P7 aP7, P8 aP8,SR(*aF)(P1,P2,P3,P4,P5,P6,P7,P8))
	:iP1(aP1),iP2(aP2),iP3(aP3),iP4(aP4),iP5(aP5),iP6(aP6),iP7(aP7),iP8(aP8),iSF8(aF),iStat(ETrue)
	{
	DoTest(&(TCMem<C,SR,MR,P1,P2,P3,P4,P5,P6,P7,P8>::DoStatFun8));
	}



// CT = class type, PT = parameter type, P = parameter, F = function, SRT = static return type
// pCT pointer to a class of type CT, MRT = member return type

#define T_STATIC0(CT,SRT,F) {TCMem<CT,SRT,TAny*,TInt,TInt,TInt,TInt,TInt,TInt,TInt,TInt> temp(ETCBaseDummy,F);};
#define T_STATIC1(CT,SRT,F,PT1,P1) {TCMem<CT,SRT,TAny*,PT1,TInt,TInt,TInt,TInt,TInt,TInt,TInt> temp(P1,F);};
#define T_STATIC2(CT,SRT,F,PT1,P1,PT2,P2) {TCMem<CT,SRT,TAny*,PT1,PT2,TInt,TInt,TInt,TInt,TInt,TInt> temp(P1,P2,F);};
#define T_STATIC3(CT,SRT,F,PT1,P1,PT2,P2,PT3,P3) {TCMem<CT,SRT,TAny*,PT1,PT2,PT3,TInt,TInt,TInt,TInt,TInt> temp(P1,P2,P3,F);};
#define T_STATIC4(CT,SRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4) {TCMem<CT,SRT,TAny*,PT1,PT2,PT3,PT4,TInt,TInt,TInt,TInt> temp(P1,P2,P3,P4,F);};
#define T_STATIC5(CT,SRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4,PT5,P5) {TCMem<CT,SRT,TAny*,PT1,PT2,PT3,PT4,PT5,TInt,TInt,TInt> temp(P1,P2,P3,P4,P5,F);};
#define T_STATIC6(CT,SRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4,PT5,P5,PT6,P6) {TCMem<CT,SRT,TAny*,PT1,PT2,PT3,PT4,PT5,PT6,TInt,TInt> temp(P1,P2,P3,P4,P5,P6,F);};
#define T_STATIC7(CT,SRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4,PT5,P5,PT6,P6,PT7,P7) {TCMem<CT,SRT,TAny*,PT1,PT2,PT3,PT4,PT5,PT6,PT7,TInt> temp(P1,P2,P3,P4,P5,P6,P7,F); };
#define T_STATIC8(CT,SRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4,PT5,P5,PT6,P6,PT7,P7,PT8,P8) {TCMem<CT,SRT,TAny*,PT1,PT2,PT3,PT4,PT5,PT6,PT7,PT8> temp(P1,P2,P3,P4,P5,P6,P7,P8,F);};
			 
#define T_MEMBER0(CT,pCT,MRT,F) {TCMem<CT,CT*,MRT,TInt,TInt,TInt,TInt,TInt,TInt,TInt,TInt> temp(pCT,F);};
#define T_MEMBER1(CT,pCT,MRT,F,PT1,P1) {TCMem<CT,CT*,MRT,PT1,TInt,TInt,TInt,TInt,TInt,TInt,TInt> temp(pCT,P1,F);};
#define T_MEMBER2(CT,pCT,MRT,F,PT1,P1,PT2,P2) {TCMem<CT,CT*,MRT,PT1,PT2,TInt,TInt,TInt,TInt,TInt,TInt> temp(pCT,P1,P2,F);};
#define T_MEMBER3(CT,pCT,MRT,F,PT1,P1,PT2,P2,PT3,P3) {TCMem<CT,CT*,MRT,PT1,PT2,PT3,TInt,TInt,TInt,TInt,TInt> temp(pCT,P1,P2,P3,F);};
#define T_MEMBER4(CT,pCT,MRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4) {TCMem<CT,CT*,MRT,PT1,PT2,PT3,PT4,TInt,TInt,TInt,TInt> temp(pCT,P1,P2,P3,P4,F);};
#define T_MEMBER5(CT,pCT,MRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4,PT5,P5) {TCMem<CT,CT*,MRT,PT1,PT2,PT3,PT4,PT5,TInt,TInt,TInt> temp(pCT,P1,P2,P3,P4,P5,F);};
#define T_MEMBER6(CT,pCT,MRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4,PT5,P5,PT6,P6) {TCMem<CT,CT*,MRT,PT1,PT2,PT3,PT4,PT5,PT6,TInt,TInt> temp(pCT,P1,P2,P3,P4,P5,P6,F);};
#define T_MEMBER7(CT,pCT,MRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4,PT5,P5,PT6,P6,PT7,P7) {TCMem<CT,CT*,MRT,PT1,PT2,PT3,PT4,PT5,PT6,PT7,TInt> temp(pCT,P1,P2,P3,P4,P5,P6,P7,F); };
#define T_MEMBER8(CT,pCT,MRT,F,PT1,P1,PT2,P2,PT3,P3,PT4,P4,PT5,P5,PT6,P6,PT7,P7,PT8,P8) {TCMem<CT,CT*,MRT,PT1,PT2,PT3,PT4,PT5,PT6,PT7,PT8> temp(pCT,P1,P2,P3,P4,P5,P6,P7,P8,F);};

