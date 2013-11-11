/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file TransportSpecificDefs.h
*/
 
 
 #if (!defined TRANSPORTSPECIFICDEFS_H)
 #define TRANSPORTSPECIFICDEFS_H
 
 //config task information
 _LIT(KTe_TaskSection, "TaskSection");
 _LIT(KTe_Tasks, "Tasks");
 _LIT(KTe_Slots, "QueueSlots");
 _LIT(KTe_TimeInterval, "TimeInterval");
 
 //config peer information
 _LIT(KTe_PeerSection, "Peer");
 _LIT(KTe_PeerId, "PeerId");
 _LIT(KTe_WorkerId, "WorkerId");
 
 //config thread information
 _LIT(KTe_ThreadSection, "Thread");
 _LIT(KTe_ThreadId, "ThreadId");
 _LIT(KTe_ThreadPriority, "ThreadPriority");
 
 //Blob contents
 _LIT(KBlob, "This is a Large Blob Message: In the merry month of June from my home I started left the girls of Taum nearly brokenhearted saluted me father dear, kissed me darling mother drank a pint of beer, my grief and tears to smother then off to reap the corn, leave where I was born cut a stout blackthorn to banish ghosts and goblin, brand-new pair of brogues, rattling o'er the bogs frightening all the dogs  on the rocky road to Dublin. In Mullingar last night, I rested limbs so weary started by daylight next morning bright and early took a drop of the pure to keep me heart from sinking that's the daddy's cure when he's on the drinking see the lassies smile, laughing all the while at me darling style, would set your heart a-bubblin' asked me was I hired, wages I required 'til I was almost tired of the rocky road to Dublin. Hunt the hare and turn her down the rocky road and all the way to Dublin, whack-fol-la-de-da!"); 

 _LIT(KBlob1, "1This is a Large Blob Message1: Symbian OS v9 is helping lower Symbian OS licensee development costs and accelerate time to market for smaller, less expensive and even more capable Symbian OS smartphones.  As a robust, secure, open and standards-based platform, Symbian OS v9 will support network operators' cost-effective deployment of revenue-generating services, content and applications......Symbian OS is the global industry standard operating system for smartphones, and is licensed to the world's leading handset manufacturers, which account for over 85 per cent of annual worldwide mobile phone sales....Symbian OS v8.0 is designed to meet the needs of Symbian OS licensees, network operators and enterprises by driving down Symbian OS phone build cost, providing upgraded Java and multimedia capabilities, as well as advanced device management functionality....ymbian believes that the mobile phone market has five key characteristics");

 _LIT(KBlob2, "2This is a Large Blob Message2: There are more things in heaven and earth, Horatio, Than are dreamt of in your philosophy, To be or not to be, --that is the question:--Whether 'tis nobler in the mind to suffer The slings and arrows of outrageous fortune Or to take arms against a sea of troubles,And by opposing end them?, How now? A rat? Dead, for a ducat, dead!, Cowards die many times before their deaths, The valiant never taste of death but once, How poor are they that have not patience! What wound did ever heal but by degrees? Thou know'st we work by wit, and not by witchcraft; And wit depends on dilatory time, O villain, villain, smiling, damned villain!, The fault, dear Brutus, is not in our stars, But in ourselves, that we are underlings: All the world's a stage,And all the men and women merely players.They have their exits and their entrances,And one man in his time plays many parts,His acts being seven ages." ); 

 _LIT(KBlob3, "3This is a Large Blob Message3: 12:57:45:120 z:\\te_commsfw_transport1.script RUN_TEST_STEP 10000 TE_CommsFW_Transport CTransportTestStep7_2 z:\\config\\CTransportTestStepStandard.ini12:57:45:125 INFO -  99 TestStepBase.Cpp 190 INI READ : z:\\config\\CTransportTestStepStandard.ini TaskSection Tasks Peer1,Peer212:57:45:125 INFO -  99 TestStepBase.Cpp 171 INI READ : z:\\config\\CTransportTestStepStandard.ini Peer1 PeerId 112:57:45:125 INFO -  99 TestStepBase.Cpp 171 INI READ : z:\\config\\CTransportTestStepStandard.ini Peer1 WorkerId 012:57:45:125 INFO -  99 TestStepBase.Cpp 171 INI READ : z:\\config\\CTransportTestStepStandard.ini Peer2 PeerId 012:57:45:125 INFO -  99 TestStepBase.Cpp 171 INI READ : z:\\config\\CTransportTestStepStandard.ini Peer2 WorkerId 112:57:45:125 INFO -  99 TestStepBase.Cpp 171 INI READ : z:\\config\\CTransportTestStepStandard.ini TaskSection QueueSlots 312:57:45:125 INFO -  99 TestStepBase.Cpp 171 INI READ");

 _LIT(KBlob4, "4This is a Large Blob Message4: The unpurged images of day recede; The Emperor's drunken soldiery are abed; Night resonance recedes, night walkers' song After great cathedral gong; A starlit or a moonlit dome disdains All that man is, All mere complexities, The fury and the mire of human veins. Before me floats an image, man or shade, Shade more than man, more image than a shade; For Hades' bobbin bound in mummy-cloth May unwind the winding path; A mouth that has no moisture and no breath Breathless mouths may summon; I hail the superhuman; I call it death-in-life and life-in-death. Miracle, bird or golden handiwork, More miraclc than bird or handiwork, Planted on the star-lit golden bough, Can like the cocks of Hades crow, Or, by the moon embittered, scorn aloud In glory of changeless metal Common bird or petal And all complexities of mire or blood. At midnight on the Emperor's pavement flit Flames that no faggot feeds, nor steel has lit" );  
 
 // Dispatch msg info
_LIT(KSmall1, "1");
_LIT(KSmall2, "2");
_LIT(KSmall3, "3");
_LIT(KSmall4, "4");

_LIT(KMeta,"metadata");

_LIT(KThread1, "t1");
_LIT(KThread2, "t2");

_LIT(KSem1,"t1sem");
_LIT(KSem2,"t2sem");

const TInt KTotalThreads = 2;
const TInt KJournalSlots = 40;

const TInt KThread1Hash = 0;
const TInt KThread2Hash = 1;
const CommsFW::TId KRequestID = 100;

const TInt KMemorySlots = 2;
const TInt KMemoryTestDimensions = 2;
const TInt KMemoryTestResultHash = 0;
const TInt KOrphanedMemoryHash = 1;
const TInt KHookRegFailureHash = 15;
const TInt KSimpleDispatchData = 10;
const TInt KNullDispatchData = 100;
const TInt KStandardHeapSize = 1048576;

const CommsFW::TId KReqId = 21232;
#endif
