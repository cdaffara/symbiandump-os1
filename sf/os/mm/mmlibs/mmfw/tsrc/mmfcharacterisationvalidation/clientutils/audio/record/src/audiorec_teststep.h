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


#ifndef AUDIORECTEST_H
#define AUDIORECTEST_H

#include "char_audiorec_common.h"


/*******************************
* Creating Meta Data Variables
********************************/

_LIT(name,"<Copyright>");
_LIT(data,"<Symbian (c) 2006 - 2007>");
_LIT(name2,"Artist");
_LIT(data2,"Test");

/*******************************
* Base Class derived From RAsync
********************************/
class RAudioRecTestStep : public RAudioRecTest
	{
public:
     static RAudioRecTestStep* NewL(const TDesC& aStepName, const TDesC& aFileName);
     
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RAudioRecTestStep(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	};


/**********************************
class for Open an audio file in Opened State.
***********************************/
class RTestStepAudioRecOpen : public RAudioRecTest
	{
public:
     static RTestStepAudioRecOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecOpen(const TDesC& aStepName, const TDesC& aFilename);
	};

/**********************************
class for Play an audio file in Playing State.
***********************************/
class RTestStepAudioRecPlayInPlaying : public RAudioRecTest
	{
public:
     static RTestStepAudioRecPlayInPlaying* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecPlayInPlaying(const TDesC& aStepName, const TDesC& aFilename);
	};
	
/**********************************
class for Play an audio file Before Open.
***********************************/
class RTestStepAudioRecPlayBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecPlayBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecPlayBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	};

/**********************************
class for Record an audio file Before Open.
***********************************/
class RTestStepAudioRecRecordBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecRecordBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecRecordBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	};

/**********************************
 Record an audio file with maximum size.
 ***********************************/
class RTestStepAudioRecRecordMaxSize: public RAudioRecTest
	{
public:
     static RTestStepAudioRecRecordMaxSize* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecRecordMaxSize(const TDesC& aStepName, const TDesC& aFilename);
	TInt iSetMaxWriteLength;
	};
	
/*************************************************
 * Record an audio file While in Recording State.
 *************************************************/
class RTestStepAudioRecRecordWhileInRecording: public RAudioRecTest
	{
public:
     static RTestStepAudioRecRecordWhileInRecording* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecRecordWhileInRecording(const TDesC& aStepName, const TDesC& aFilename);
	TInt iSetMaxWriteLength;
	};

/*************************************************
*
*     Getting MaxVolume Before Open an Audio file
*
**************************************************/
class RTestStepAudioRecMaxVolBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecMaxVolBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecMaxVolBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	TInt iMaxVol;
	};

/*************************************************
*
*     Set and Get Volume Before Open an Audio file
*
**************************************************/
class RTestStepAudioRecSetGetVolBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetGetVolBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetGetVolBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	TInt iSetVolume;
	};

/*************************************************
*
*     Set Volume to Less than Zero while in Open state
*
**************************************************/
class RTestStepAudioRecSetVolInOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetVolInOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetVolInOpen(const TDesC& aStepName, const TDesC& aFilename);\
	TInt iSetVolume;
 	TInt iSetNegativeVol;
	TInt iSetMaxVol;
	};

/*************************************************
*
*     Setting SetVolumeRamp Before Open an Audio file
*
**************************************************/
class RTestStepAudioRecSetVolRampBeforeOpen: public RAudioRecTest
					  
	{
public:
     static RTestStepAudioRecSetVolRampBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetVolRampBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);\
	TInt iSetRampDuration;
	};

/*************************************************
*
*     Getting Maximum Gain Before Open an Audio file
*
**************************************************/
class RTestStepAudioRecMaxGainBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecMaxGainBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecMaxGainBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iMaxGain;
	};

/*************************************************
*
*   Set And Get gain in All Conditions in All States
*
**************************************************/
class RTestStepAudioRecSetAndGetGaininAllCond: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetAndGetGaininAllCond* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetAndGetGaininAllCond(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetGain;
	};
	
/*************************************************
*
*   Set And Get Position in All Conditions in All States
*
**************************************************/
class RTestStepAudioRecSetAndGetPositionInAllCond: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetAndGetPositionInAllCond* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetAndGetPositionInAllCond(const TDesC& aStepName, const TDesC& aSectName);
	void DoTimerCallback();
	TInt iSetPos;
	};

/*******************************************************************************
*
*   Record Time Available while in Recording and while before open an audio file.
*
********************************************************************************/
class RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecRecodTimeAvailInRecordingAndBeforeOpen(const TDesC& aStepName, const TDesC& aSectName);
	void DoTimerCallback();
	TInt iSetMaxWriteLength;
	};

/**********************************************************************************
*
*   Record Time Available while in before Open,Opened and Recording of an audio file.
*   with SetMaxWriteLength
*
***********************************************************************************/
class RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength: public RAudioRecTest
	{
public:
     static RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecRecodTimeAvailInSetMaxWriteLength(const TDesC& aStepName, const TDesC& aSectName);
	void DoTimerCallback();
	TInt iSetMaxWriteLength;
	};


/*******************************************************************************
*
*   Getting Duration in all states i,e. while in before open,Recording and Playing an audio file.
*
********************************************************************************/
class RTestStepAudioRecDurationBeforeOpenRecordingPlaying: public RAudioRecTest
	{
public:
     static RTestStepAudioRecDurationBeforeOpenRecordingPlaying* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecDurationBeforeOpenRecordingPlaying(const TDesC& aStepName, const TDesC& aSectName);
	void DoTimerCallback();
	};

/*******************************************************************************
*
*   User wants to define the window(SetPlayWindow()) on the audio sample data i,e. while in before open,Opened and Playing an audio file.
*
********************************************************************************/
class RTestStepAudioRecSetPlayWindowOpenAndPlaying: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetPlayWindowOpenAndPlaying* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetPlayWindowOpenAndPlaying(const TDesC& aStepName, const TDesC& aSectName);
	void DoTimerCallback();
	
	TInt iSetPlaywindowStart;
	TInt iSetPlaywindowEnd;
	//These variables added for operational purpose of SetPlayWindow
	TUint iPlayduration1;
	TTime iStart1,iStop1;
	TUint iPlayduration2;
  	TTime iStart2,iStop2;
	//Counter variable to count number of PlayL() is called
	TInt iCount;
	};
	
/*******************************************************************************
*
*   User wants to define the window(SetPlayWindow()) on the audio sample data i,e. while in before open.
*
********************************************************************************/
class RTestStepAudioRecSetPlayWindowbeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetPlayWindowbeforeOpen* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetPlayWindowbeforeOpen(const TDesC& aStepName, const TDesC& aSectName);
	TInt iSetPlaywindowStart;
	TInt iSetPlaywindowEnd;
	//These variables added for operational purpose of SetPlayWindow
	TUint iPlayduration1;
	TTime iStart1,iStop1;
	};

/*******************************************************************************
*
*   User wants to Remove the window(ClearPlayWindow()) on the audio sample data i,e. while in before open,Opened and Playing an audio file.
*
********************************************************************************/
class RTestStepAudioRecClearPlayWindow: public RAudioRecTest
	{
public:
     static RTestStepAudioRecClearPlayWindow* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecClearPlayWindow(const TDesC& aStepName, const TDesC& aSectName);
	
	TInt iSetPlaywindowStart;
	TInt iSetPlaywindowEnd;
	//These variables added for operational purpose of SetPlayWindow
	TUint iPlayduration;
	TTime iStart1,iStop1;
  	TTime iStart2,iStop2;
	//Counter variable to count number of PlayL called
	TInt iCount;
	};
	
/*******************************************************************************
*
*   User wants to Remove the window(ClearPlayWindow()) on the audio sample data i,e. while in before open.
*
********************************************************************************/
class RTestStepAudioRecClearPlayWindowBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecClearPlayWindowBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecClearPlayWindowBeforeOpen(const TDesC& aStepName, const TDesC& aSectName);
	TInt iSetPlaywindowStart;
	TInt iSetPlaywindowEnd;
	//These variables added for operational purpose of SetPlayWindow
	TUint iPlayduration1;
	TTime iStart1,iStop1;
	};
	
/*****************************************************************************************
* 				SetRepeats()
*Sets the number of times the audio sample is to be repeated while in Opened state.
*
******************************************************************************************/
class RTestStepAudioRecSetRepeatsInOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetRepeatsInOpen* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetRepeatsInOpen(const TDesC& aStepName, const TDesC& aSectName);
	
	TInt iRepeatNumberOfTimes;
	TInt iDuration;
	TInt iPlayduration1;
	TTime iStart1,iStop1;
	};

/*****************************************************************************************
* 				SetRepeats()
*Sets the number of times the audio sample is to be repeated while in Playing state.
*
******************************************************************************************/
class RTestStepAudioRecSetRepeatsInPlay: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetRepeatsInPlay* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetRepeatsInPlay(const TDesC& aStepName, const TDesC& aSectName);
	
	TInt iRepeatNumberOfTimes;
	TInt iDuration;
	TInt iPlayduration2;
	TTime iStart2,iStop2;
	TInt iCount;
	};

/*****************************************************************************************
* 				SetRepeats()
*Sets the number of times the audio sample is to be repeated while Before Open an Audio file..
*
******************************************************************************************/
class RTestStepAudioRecSetRepeatsBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetRepeatsBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aSectName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetRepeatsBeforeOpen(const TDesC& aStepName, const TDesC& aSectName);
	
	TInt iRepeatNumberOfTimes;
	TInt iDuration;
	TInt iPlayduration1;
	TTime iStart1,iStop1;
	};

/**********************************************************
*
*  Sets the maximum size for a file that is being recorded 
*  while in Open State And Boundary condition(Less than Zero).
*
***********************************************************/
class RTestStepAudioRecMinSizeInOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecMinSizeInOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecMinSizeInOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetMaxWriteLength;
	};

/**********************************************************
*
*  Sets the maximum size for a file that is being recorded 
*  while in Open State And Boundary condition(MAx value).
*
***********************************************************/
class RTestStepAudioRecMaxSizeInOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecMaxSizeInOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecMaxSizeInOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetMaxWriteLength;
	};
	
/**********************************************************
*
*  Sets the maximum size for a file that is being recorded 
*  while in Recording.
*
***********************************************************/
class RTestStepAudioRecMaxSizeInRecord: public RAudioRecTest
	{
public:
     static RTestStepAudioRecMaxSizeInRecord* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecMaxSizeInRecord(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetMaxWriteLength;
	};
	
/**********************************************************
*
*  Sets the maximum size for a file that is being recorded 
*  Before Open an Audio file.
*
***********************************************************/
class RTestStepAudioRecMaxSizeInBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecMaxSizeInBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecMaxSizeInBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetMaxWriteLength;
	};
	
/**********************************************************
*	SetPriority()
*  Sets the Priority a file that is being recorded while in Open state.
*  
*
***********************************************************/
class RTestStepAudioRecSetPriorityInOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetPriorityInOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetPriorityInOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TMMFPrioritySettings iPrior;
	};
/**********************************************************
*
*  Set and Get Play Back Balance for a file that is being recorded 
*  Before Open an Audio file.
*
***********************************************************/
class RTestStepAudioRecSetPlayBackBalBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetPlayBackBalBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetPlayBackBalBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetPlayBackBalance;
	TInt iReturn;
	};
	
/**********************************************************
*
*  Set and Get Play Back Balance for a file that is being recorded 
*  While in Open State(Setting to Maximum Value).
*
***********************************************************/
class RTestStepAudioRecSetPlayBackBalInOpenedMaxValue: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetPlayBackBalInOpenedMaxValue* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetPlayBackBalInOpenedMaxValue(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetPlayBackMaxBalance;
	TInt iReturn;
	};

/**********************************************************
*
*  Set and Get Play Back Balance for a file that is being recorded 
*  While in Open State(Setting To Minimum Value).
*
***********************************************************/
class RTestStepAudioRecSetPlayBackBalInOpenedMinValue: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetPlayBackBalInOpenedMinValue* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetPlayBackBalInOpenedMinValue(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetPlayBackMinBalance;
	TInt iReturn;
	};
	
/**********************************************************
*
*  Set and Get the current recording balance setting between the left and right stereo microphones 
*  Before Open an Audio file.
*
***********************************************************/
class RTestStepAudioRecSetRecordBalBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetRecordBalBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	//from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetRecordBalBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetRecordBalance;
	TInt iReturn;
	};
	
/**********************************************************
* Set and Get the current recording balance setting between the left and right stereo microphones 
*  While in Open State(Setting to Maximum Value).
*
***********************************************************/
class RTestStepAudioRecSetRecordBalInOpenMaxValue: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetRecordBalInOpenMaxValue* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetRecordBalInOpenMaxValue(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetRecordBalMaxValue;
	TInt iReturn;
	};
	
/**********************************************************
* Set and Get the current recording balance setting between the left and right stereo microphones 
*  While in Open State(Setting to Minimum Value).
*
***********************************************************/
class RTestStepAudioRecSetRecordBalInOpenMinValue: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetRecordBalInOpenMinValue* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetRecordBalInOpenMinValue(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetRecordBalMinValue;
	TInt iReturn;
	};
	
/**********************************************************
* Set and Get the current recording balance setting between the left and right stereo microphones 
*  While in Recording State.
*
***********************************************************/
class RTestStepAudioRecSetRecordBalInRecording: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetRecordBalInRecording* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetRecordBalInRecording(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetRecordBalance;
	TInt iReturn;
	};	
		
/**********************************************************
* Returns a list of the supported codecs for the conversion destination.
*  While in Recording State.
*
***********************************************************/
class RTestStepAudioRecGetSupportDestDataTypeInRecording: public RAudioRecTest
	{
public:
     static RTestStepAudioRecGetSupportDestDataTypeInRecording* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecGetSupportDestDataTypeInRecording(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt ReadExpectedFourCC(RArray<TFourCC>& aExpectedFourCC);	
	RArray<TFourCC> supFourCC;
	RArray<TFourCC> iExpectedFourCCToBeSupported;
	};
		
/**********************************************************
* Returns a list of the supported codecs for the conversion destination.
*  Before Open an Audio file.
*
***********************************************************/
class RTestStepAudioRecGetSupportDestDataTypeBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecGetSupportDestDataTypeBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecGetSupportDestDataTypeBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt ReadExpectedFourCC(RArray<TFourCC>& aExpectedFourCC);
	RArray<TFourCC> supFourCC;
	RArray<TFourCC> iExpectedFourCCToBeSupported;
	};		
	
/**********************************************************
*  Set and Get the codec to use with the record controller.
*  Before Open an Audio file(SetDestinationDataTypeL(),DestinationDataTypeL).
*
***********************************************************/
class RTestStepAudioRecSetGetDestnDataTypeBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetGetDestnDataTypeBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetGetDestnDataTypeBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt ReadExpectedFourCC(RArray<TFourCC>& aExpectedFourCC);
	RArray<TFourCC>	iExpectedFourCCToBeSupported;
	};		
	
/**********************************************************
* Get the codec to use with the record controller.
*  While in Recording State(DestinationDataTypeL()).
*
***********************************************************/
class RTestStepAudioRecGetDestnDataTypeInRecording: public RAudioRecTest
	{
public:
     static RTestStepAudioRecGetDestnDataTypeInRecording* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecGetDestnDataTypeInRecording(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	};	
	
/**********************************************************
* Set and Get the bit rate of the data sink in bits per second
*  Before Open an Audio file(SetDestinationBitRateL(),DestinationBitRateL).
*
***********************************************************/
class RTestStepAudioRecSetGetDestnBitRateBeforeOpen: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetGetDestnBitRateBeforeOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetGetDestnBitRateBeforeOpen(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetDestinationBitRate;
	};		
	
/**********************************************************
* Gets the bit rate of the data sink in bits per second
*  While in recording State(DestinationBitRateL).
*
***********************************************************/
class RTestStepAudioRecGetDestnBitRateInRecording: public RAudioRecTest
	{
public:
     static RTestStepAudioRecGetDestnBitRateInRecording* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecGetDestnBitRateInRecording(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iSetDestinationBitRate;
	RArray<TUint> supBitRates;
	};		
	
/**********************************************************
* Returns a list of the supported bit rates for recording to the data sink.
* While in Before Open an Audio file(GetSupportedBitRatesL())
***********************************************************/
class RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec: public RAudioRecTest
	{
public:
     static RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecGetSupportedBitRateBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	};		

/**********************************************************
* Returns the bit rate of the audio clip While Before Open an 
* Audio file and While in Playing State.
* SourceBitRateL()
**********************************************************/
class RTestStepAudioRecSourceBitRateBeforeOpenAndPalying: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSourceBitRateBeforeOpenAndPalying* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSourceBitRateBeforeOpenAndPalying(const TDesC& aStepName, const TDesC& aFilename);
	TUint iRetSource;
	};	
		
/****************************************************************************************
* Sets the sample rate for the conversion destination data sink 
* Gets the sample rate in samples/second that the destination data sink 
* currently set to an Audio file ,While Before Open State and While in Recording State.
* 
* SetDestinationSampleRateL()/DestinationSampleRateL()
*
**************************************************************************88*************/
class RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
private:
	RTestStepAudioRecSetGetDestnSampleRateBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iRetSource;
	RArray<TUint> iSupSampleRates;
	TInt iNumCodecsOpen;
  	TInt iNumCodecsRecording;
	};
			
/****************************************************************************************
* Returns a list of supported recording sample rates
* While Before Open State and While in Recording State.
* 
* GetSupportedSampleRatesL()
*
**************************************************************************88*************/
class RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec: public RAudioRecTest
	{
public:
     static RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecGetSupportedSampleRateBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	RArray<TUint> iSupSampleRates;
	};		
	
/****************************************************************************************
* Sets the format of the audio clip. This can only be done if the audio clip does not exist.
* While Before Open State and While in Recording State.
* 
* SetDestinationFormatL()/DestinationFormatL()
*
**************************************************************************88*************/
class RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetGetDestnFormatBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TUid iFormat;
	};		
	
/****************************************************************************************
* Sets the number of channels for the recorded audio clip.
* While Before Open and while Recording State.
* 
* SetDestinationNumberOfChannelsL()/DestinationNumberOfChannelsL()
*
**************************************************************************88*************/
class RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec: public RAudioRecTest
	{
public:
     static RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestStepAudioRecSetGetDestnNumberChannelBeforeOpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TUint iChannel;
	};		
	
/****************************************************************************************
* Returns a list of the supported number of channels for recording.
* While Before Open and While in Recording State.
* 
* GetSupportedNumberOfChannelsL()
*
**************************************************************************88*************/
class RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec: public RAudioRecTest
	{
public:
     static RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestAudioRecGetSuptedNoOfChanlsB4OpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TUint iChannel;
	};	
				
/****************************************************************************************
* Returns the number of meta data entries associated with this clip.
* While Before Open, Opened, Recording and While in Playing State.
* 
* GetNumberOfMetaDataEntries()
*
**************************************************************************88*************/
class RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec: public RAudioRecTest
	{
public:
     static RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestAudioRecGetNumOfMetaDataEntB4OpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iMetaData;
	TInt iExpErrorWhenIdle;
	TInt iExpErrorWhenOpened;
	TInt iExpErrorWhenPlaying;
	TInt iExpErrorWhenRecording;
	};
			
/****************************************************************************************
* Returns the specified meta data entry from the current audio clip.
* While Before Open,Recording and While in Playing State.
* 
* GetMetaDataEntryL()
*
**************************************************************************88*************/
class RTestAudioRecGetMetaDataEntB4OpenAndPly: public RAudioRecTest
	{
public:
     static RTestAudioRecGetMetaDataEntB4OpenAndPly* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestAudioRecGetMetaDataEntB4OpenAndPly(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iExpErrorWhenIdle;
	TInt iExpErrorWhenPlaying;
	TInt iExpErrorWhenRecording;
	};		
			
/****************************************************************************************
* Adds a meta data entry to an Audio file ,While Before Open and While in Recording State.
* 
* AddMetaDataEntryL()
*
**************************************************************************88*************/
class RTestAudioRecAddMetaDataEntB4OpenAndRec: public RAudioRecTest
	{
public:
     static RTestAudioRecAddMetaDataEntB4OpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestAudioRecAddMetaDataEntB4OpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	CMMFMetaDataEntry* iMetadata[2];
	TInt iExpErrorWhenIdle;
	TInt iExpErrorWhenRecording;
	};	
				
/****************************************************************************************
* Removes a specified meta data entry from the audio clip.While Before Open and While in Recording State.
* 
* RemoveMetaDataEntry()
*
**************************************************************************88*************/
class RTestAudioRecRemvMetaDataEntB4OpenAndRec: public RAudioRecTest
	{
public:
     static RTestAudioRecRemvMetaDataEntB4OpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestAudioRecRemvMetaDataEntB4OpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TInt iExpErrorWhenIdle;
	};	
				
/****************************************************************************************
* Replaces the specified meta data entry with a new entry.While Before Open and While in Recording State.
* 
* ReplaceMetaDataEntryL()
*
**************************************************************************88*************/
class RTestAudioRecRepMetaDataEntB4OpenAndRec: public RAudioRecTest
	{
public:
     static RTestAudioRecRepMetaDataEntB4OpenAndRec* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestAudioRecRepMetaDataEntB4OpenAndRec(const TDesC& aStepName, const TDesC& aFilename);
	void DoTimerCallback();
	TUid iFormat;
	TInt iExpError;
	};		
/****************************************************************************************
* Waits for the client to resume the play even after the default timer expires.
* While in Open State
* WillResumePlay()
*
**************************************************************************88*************/
class RTestAudioRecWillResumePlayInOpen: public RAudioRecTest
	{
public:
     static RTestAudioRecWillResumePlayInOpen* NewL(const TDesC& aStepName, const TDesC& aFileName);
	// from RAsyncTestStep
	 void DoKickoffTestL();  //-- for the derived class to implement
	//State Machine Functions
	void Fsm(TAudioRecorderState aEventCode);
			
private:
	RTestAudioRecWillResumePlayInOpen(const TDesC& aStepName, const TDesC& aFilename);
	};

#endif // AUDIORECTEST_H
