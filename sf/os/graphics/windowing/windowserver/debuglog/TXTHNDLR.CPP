// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Handles text for the window server debug-logger - platform independent
// 
//

#include "../SERVER/w32cmd.h"
#include "DEBUGLOG.H"

void TDebugLogTextHandler::Append(TDes &aBuf1, const TDesC &aBuf2)
	{
	if (aBuf1.Length()+aBuf2.Length()>aBuf1.MaxLength()-1)
		{
		TUint offset=aBuf1.Length();
		aBuf1.SetLength(aBuf1.MaxLength()-3-1);
		for (TInt i=offset; i<(aBuf1.MaxLength()-3-1); i++)
			aBuf1[i]=aBuf2[i-offset];
		aBuf1+=_L("...");
		}
	else
		aBuf1+=aBuf2;
	}

void TDebugLogTextHandler::TestAppend()
	{
#if defined(_DEBUG)
	TBuf<0x10> buf1=_L("123456789012345");
	TBuf<0x10> buf2(KNullDesC);

	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("123456789012345"), panic(1));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("123456789012345"), panic(2));

	buf1=_L("12345678901234");
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("12345678901234"), panic(3));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("12345678901234"), panic(4));

	buf1=_L("1234567890123");
	buf2=_L("**********");
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("123456789012..."), panic(5));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("123456789012..."), panic(6));

	buf1=_L("123456789012");
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("123456789012..."), panic(7));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("123456789012..."), panic(8));

	buf1=_L("12345678901");
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("12345678901*..."), panic(9));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("12345678901*..."), panic(10));

	buf1=_L("1234567890");
	buf2=_L("*");
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("1234567890*"), panic(11));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("1234567890**"), panic(12));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("1234567890***"), panic(12));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("1234567890****"), panic(12));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("1234567890*****"), panic(12));
	TDebugLogTextHandler::Append(buf1, buf2);
	__ASSERT_ALWAYS(buf1==_L("1234567890**..."), panic(12));
#endif
	}

TBuf<20> TDebugLogTextHandler::FormatBool(TBool aBool)
	{
	TBuf<6> text;
	if (aBool)
		text.Append(_L("ETrue"));
	else
		text.Append(_L("EFalse"));
	return text;
	}

TBuf<32> TDebugLogTextHandler::FormatPoint(const TPoint &aPoint)
	{
	TBuf<32> text;
	text.Format(_L("{%d,%d}"), aPoint.iX, aPoint.iY);
	return text;
	}

TBuf<32> TDebugLogTextHandler::FormatSize(const TSize &aSize)
	{
	TBuf<32> text;
	text.Format(_L("{%d,%d}"), aSize.iWidth, aSize.iHeight);
	return text;
	}

TBuf<64> TDebugLogTextHandler::FormatRect(const TRect &aRect)
	{
	TBuf<64> text;
	text.Format(_L("{{%d,%d},{%d,%d}}"), aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY);
	return text;
	}

TBuf<40> TDebugLogTextHandler::FormatRgb(const TRgb &aRgb)
	{
	TBuf<40> text;
	text.Format(_L("{%u,%u,%u}"), (TUint)aRgb.Red(), (TUint)aRgb.Green(), (TUint)aRgb.Blue());
	return text;
	}

TBuf<20> TDebugLogTextHandler::PointerEventType(TPointerEvent::TType aType)
	{
	TBuf<20> text;
	switch(aType)
		{
		case TPointerEvent::EButton1Down:
			text.Copy(_L("EButton1Down"));
			break;
		case TPointerEvent::EButton1Up:
			text.Copy(_L("EButton1Up"));
			break;
		case TPointerEvent::EButton2Down:
			text.Copy(_L("EButton2Down"));
			break;
		case TPointerEvent::EButton2Up:
			text.Copy(_L("EButton2Up"));
			break;
		case TPointerEvent::EButton3Down:
			text.Copy(_L("EButton3Down"));
			break;
		case TPointerEvent::EButton3Up:
			text.Copy(_L("EButton3Up"));
			break;
		case TPointerEvent::EButtonRepeat:
			text.Copy(_L("EButtonRepeat"));
			break;
		case TPointerEvent::EDrag:
			text.Copy(_L("EDrag"));
			break;
		case TPointerEvent::EMove:
			text.Copy(_L("EMove"));
			break;
		case TPointerEvent::ESwitchOn:
			text.Copy(_L("ESwitchOn"));
			break;
		}
	return text;
	}

TBuf<LogTBufSize> TDebugLogTextHandler::FormatArray(TArrayType aArrayType, const TUint8 *aArrayPtr, TUint aNumElems)
	{
	TBuf<LogTBufSize> text;
	TUint elemSize=0;

	switch (aArrayType)
		{
	case EInt:
		elemSize=sizeof(TInt);
		break;
	case ERgb:
		elemSize=sizeof(TRgb);
		break;
	default:
		panic(1);
		}

	if (aNumElems>0)
		{
		Append(text, formatArrayElement(aArrayType, aArrayPtr));
		while (--aNumElems>0)
			{
			Append(text, _L(","));
			aArrayPtr+=elemSize;
			Append(text, formatArrayElement(aArrayType, aArrayPtr));
			}
		}
	Append(text, _L("}"));
	return text;
	}

TBuf<0x20> TDebugLogTextHandler::formatArrayElement(TArrayType aArrayType, const TUint8 *aArrayPtr)
	{
	TBuf<0x20> text;

	switch (aArrayType)
		{
	case EInt:
		text.Format(_L("%d"), *(TInt *)aArrayPtr);
		break;
	case ERgb:
		{
		TLongBuf buf(FormatRgb(*(TRgb *)aArrayPtr));
		text.Format(_L("%S"), &buf);
		}
		break;
	default:
		panic(2);
		}
	return text;
	}

void TDebugLogTextHandler::panic(TInt aReason)
	{
	User::Panic(_L("WservDebugLog"), aReason);
	}

