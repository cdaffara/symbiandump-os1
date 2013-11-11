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

#ifndef AFHSETTER_H
#define AFHSETTER_H



const TUint8 KSendHCC = 0xff;


const TUint8 KPairs = 0x01; // take numbers in for ranges
const TUint8 KLast = 0x02; // ready to accept last (or only) number
const TUint8 KSetChannelsBad = 0x04; //ready to use (either a pair or a single)
class CAFHSetter : public CActive
/**
Example subscriber
@internalComponent
*/
	{
public:
	static CAFHSetter* NewL(RTest& aTest);
	void Start();
	void GetClassificationBySettingBadChannels();
	TInt SendBluetoothAFHHostChannelClassification();

	void RunL();
	void DoCancel();

	~CAFHSetter();

private:
	CAFHSetter(RTest& aTest);
	void ConstructL();
	TInt GetNextChannelNum();

private:
	RTest&					iTest;
	RProperty				iProperty;
	TBTAFHHostChannelClassification	iHCC;
	TUint8					iCounter;
	TBool					iTens;
	RTimer					iTimer;
	RSocketServ				iSS;
	TUint8					iState;
	TUint8					iChannel1;
	TUint8					iChannel2;
	};



#endif //AFHSETTER
