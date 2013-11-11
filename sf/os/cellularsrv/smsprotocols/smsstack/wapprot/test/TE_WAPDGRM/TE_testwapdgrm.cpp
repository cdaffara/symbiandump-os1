// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// Testcode for wappstor.cpp, wapdgrm.cpp, wapthdr.cpp, wapmain.cpp
// Basic CWapDatagram & CWapReassemblyStore class testing.
// It covers
// - constructing CWapDatagram object from user data
// - encoding CSmsMessages from CWapDatagram object
// - constructing CWapDatagram object from CSmsMessage
// - parsing array of CSmsMessages creating CWapDatagram object.
// - the previous process uses CWapReassemblyStore in this process
// The test prints intermediate result of tests to the EPOC emulator screen.
// The test run can be controlled in several ways by giving some values
// to some internal parameters before compiling the test program.
// // This variable controls whether CSmsMessage are pushed for decoding
// TBool IsRunParsing = ETrue;
// // This variable controls whether CSmsMessages are printed out
// TBool IsPrintEncoded = EFalse;
// It is also possible to emulate a real situation, in which
// short message of different wap datagrams are received interleaved:
// // insertSms values > 0 means that all of the CWapDatagram object are not saved
// // into the store in the _Parse2() method. They are saved into the
// // ReserveArray. Later the objects from ReserveArray are pushed to the store.
// // This mechanism emulates as if CSmsMessage of different
// // wap datagrams were received interleaved.
// // If insertSms>0, the CSmsMessages are inserted to the ReserveArray automatically.
// // However, to flush the contents of ReserveArray into the store, you must
// // set IsFlushReserveArray true at run time.
// TBool IsFlushReserveArray=EFalse;
// TInt insertSms=0;
// testcase:                                                      using store:
// 0 TestWapProt_NoData_7_8_BitDatagramL                          no store
// 1 TestWapProt_Short_7_8_BitDatagramL,                          no store
// 2 TestWapProt_Long_7_8_BitDatagramL,                           store
// 3 TestWapProt_Long_7_BitDatagramWithFullLastSegmentL           store
// 4 TestWapProt_Short_7_8_BitDatagramWithImplicit16bitPortNumber store
// 5 TestWapProt_Short_7_8_BitDatagramWithExplicit16bitPortNumber store
// 6 TestWapProt_Short_7_8_BitDatagramWithCharacterValue127       no store
// 
//

#include "TE_wapdgrmbase.h"
#include "TE_testwapdgrm.h"

TVerdict CTestWapProtDatagram::doTestStepL()
/*
 * implementation of Wap Protocol Datagram test case
 */
{
	INFO_PRINTF1(_L("TE_WAPDGRM Wap Prot Datagram TEST - Starting ..."));
	INFO_PRINTF1(_L(""));

    HBufC8*    Buffer = HBufC8::NewL(4096);
    TBuf<32>   ToAddress;
    TInt       ToPort;
    TBuf<32>   FromAddress;
    TInt       FromPort;

    // Set General Values
    ToAddress =    _L("+358501234567");
    FromAddress =  _L("+358502345678");
    TSmsDataCodingScheme::TSmsAlphabet alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
    TBool Is16BitPorts = EFalse;

    TInt index=0;
    while (iWapDgrmTable[index]!=NULL)
    {
        // Set this true before last case, if ReserveArray is in use
        iIsFlushReserveArray = EFalse;

        Is16BitPorts = EFalse;

        ToPort   = 12;
        FromPort = 24;
        alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
        DoTestWapProtDatagramL(  *iWapDgrmTable[index],
                                ToAddress, ToPort, FromAddress, FromPort,
                                alphabet, Is16BitPorts);
        alphabet = TSmsDataCodingScheme::ESmsAlphabet8Bit;
        DoTestWapProtDatagramL(  *iWapDgrmTable[index],
                                ToAddress, ToPort, FromAddress, FromPort,
                                alphabet, Is16BitPorts);

        Is16BitPorts = ETrue;

        ToPort = 4098;
        FromPort = 24;
        alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
        DoTestWapProtDatagramL(  *iWapDgrmTable[index],
                                ToAddress, ToPort, FromAddress, FromPort,
                                alphabet, Is16BitPorts);
        alphabet = TSmsDataCodingScheme::ESmsAlphabet8Bit;
        DoTestWapProtDatagramL(  *iWapDgrmTable[index],
                                ToAddress, ToPort, FromAddress, FromPort,
                                alphabet, Is16BitPorts);
        ToPort   = 0;
        FromPort = 0;
        alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;
        DoTestWapProtDatagramL(  *iWapDgrmTable[index],
                                ToAddress, ToPort, FromAddress, FromPort,
                                alphabet, Is16BitPorts);
        alphabet = TSmsDataCodingScheme::ESmsAlphabet8Bit;
        DoTestWapProtDatagramL(  *iWapDgrmTable[index],
                                ToAddress, ToPort, FromAddress, FromPort,
                                alphabet, Is16BitPorts);
        index++;
    }

    delete Buffer;

	return TestStepResult();
}
