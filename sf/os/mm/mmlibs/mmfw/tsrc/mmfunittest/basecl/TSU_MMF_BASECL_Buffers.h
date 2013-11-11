// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSU_MMF_BASECL_BUFFERS_H__
#define __TSU_MMF_BASECL_BUFFERS_H__

#include <mmf/server/mmfdatabuffer.h>


#include "TSU_MMF_BASECL.h"

const TUint KMMFTestBufferSize = 4096;
const TUint KMMFTestBufferLengthSize = 2048;
const TUint KMMFTestFrameNumber = 123;
const TUint KMMFTestPosition = 1234;
const TUint KMMFTestRequestSize = 2048;
const TUint KMMFDefaultTransferWindowSize = 0x10000;
const TUint KMMF1sServerShutdownTime = 1000000;

/**
 * Test step base class for transfer buffer tests
 */
class CTestStep_MMF_BASECL_TransferBuffer : public CTestStep_MMF_BASECL
	{
public:
	CTestStep_MMF_BASECL_TransferBuffer();
	~CTestStep_MMF_BASECL_TransferBuffer();
protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	
protected:
	TPtr8 iTestData;
	};


/**
 * Test step base class for CMMFPtrBuffer tests
 */
class CTestStep_MMF_BASECL_PtrBuffer : public CTestStep_MMF_BASECL
	{
public:
	CTestStep_MMF_BASECL_PtrBuffer();
	~CTestStep_MMF_BASECL_PtrBuffer();
protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
protected:
	TPtr iTestData;
	};

class CTestStep_MMF_BASECL_U_0001 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0001() ;
 ~CTestStep_MMF_BASECL_U_0001(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0002 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0002() ;
 ~CTestStep_MMF_BASECL_U_0002(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0003 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0003() ;
 ~CTestStep_MMF_BASECL_U_0003(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0004 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0004() ;
 ~CTestStep_MMF_BASECL_U_0004(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0005 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0005() ;
 ~CTestStep_MMF_BASECL_U_0005(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0006 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0006() ;
 ~CTestStep_MMF_BASECL_U_0006(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0007 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0007() ;
 ~CTestStep_MMF_BASECL_U_0007(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0008 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0008() ;
 ~CTestStep_MMF_BASECL_U_0008(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0009 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0009() ;
 ~CTestStep_MMF_BASECL_U_0009(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0010 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0010() ;
 ~CTestStep_MMF_BASECL_U_0010(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0011 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0011() ;
 ~CTestStep_MMF_BASECL_U_0011(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0012 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0012() ;
 ~CTestStep_MMF_BASECL_U_0012(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0100 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0100() ;
 ~CTestStep_MMF_BASECL_U_0100(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0101 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0101() ;
 ~CTestStep_MMF_BASECL_U_0101(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0102 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0102() ;
 ~CTestStep_MMF_BASECL_U_0102(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0103 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0103() ;
 ~CTestStep_MMF_BASECL_U_0103(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0104 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0104() ;
 ~CTestStep_MMF_BASECL_U_0104(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0105 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0105() ;
 ~CTestStep_MMF_BASECL_U_0105(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0106 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0106() ;
 ~CTestStep_MMF_BASECL_U_0106(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0107 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0107() ;
 ~CTestStep_MMF_BASECL_U_0107(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0108 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0108() ;
 ~CTestStep_MMF_BASECL_U_0108(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0109 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0109() ;
 ~CTestStep_MMF_BASECL_U_0109(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0110 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0110() ;
 ~CTestStep_MMF_BASECL_U_0110(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0111 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0111() ;
 ~CTestStep_MMF_BASECL_U_0111(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0112 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0112() ;
 ~CTestStep_MMF_BASECL_U_0112(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0113 : public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0113() ;
 ~CTestStep_MMF_BASECL_U_0113(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0114 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0114() ;
 ~CTestStep_MMF_BASECL_U_0114(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0115: public CTestStep_MMF_BASECL
 {
public:
 CTestStep_MMF_BASECL_U_0115() ;
 ~CTestStep_MMF_BASECL_U_0115(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0116 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0116() ;
 ~CTestStep_MMF_BASECL_U_0116(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0117 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0117() ;
 ~CTestStep_MMF_BASECL_U_0117(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0118 : public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0118() ;
 ~CTestStep_MMF_BASECL_U_0118(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0119: public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0119() ;
 ~CTestStep_MMF_BASECL_U_0119(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0120: public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0120() ;
 ~CTestStep_MMF_BASECL_U_0120(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0121: public CTestStep_MMF_BASECL_TransferBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0121() ;
 ~CTestStep_MMF_BASECL_U_0121(){} ;
 virtual TVerdict DoTestStepL();
 } ;

//CMMFPtr

class CTestStep_MMF_BASECL_U_0150 : public CTestStep_MMF_BASECL_PtrBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0150() ;
 ~CTestStep_MMF_BASECL_U_0150(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0151 : public CTestStep_MMF_BASECL_PtrBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0151() ;
 ~CTestStep_MMF_BASECL_U_0151(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0152 : public CTestStep_MMF_BASECL_PtrBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0152() ;
 ~CTestStep_MMF_BASECL_U_0152(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0153 : public CTestStep_MMF_BASECL_PtrBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0153() ;
 ~CTestStep_MMF_BASECL_U_0153(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0154 : public CTestStep_MMF_BASECL_PtrBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0154() ;
 ~CTestStep_MMF_BASECL_U_0154(){} ;
 virtual TVerdict DoTestStepL();
 } ;



class CTestStep_MMF_BASECL_U_0157 : public CTestStep_MMF_BASECL_PtrBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0157() ;
 ~CTestStep_MMF_BASECL_U_0157(){} ;
 virtual TVerdict DoTestStepL();
 } ;

class CTestStep_MMF_BASECL_U_0159 : public CTestStep_MMF_BASECL_PtrBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0159() ;
 ~CTestStep_MMF_BASECL_U_0159(){} ;
 virtual TVerdict DoTestStepL();
 } ;


class CTestStep_MMF_BASECL_U_0160 : public CTestStep_MMF_BASECL_PtrBuffer
 {
public:
 CTestStep_MMF_BASECL_U_0160() ;
 ~CTestStep_MMF_BASECL_U_0160(){} ;
 virtual TVerdict DoTestStepL();
 } ;



#endif /* __TSU_MMF_BASECL_BUFFERS_H__ */
