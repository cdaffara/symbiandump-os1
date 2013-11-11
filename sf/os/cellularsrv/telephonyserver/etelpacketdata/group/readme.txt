Please note that the current test for this component is TS_EtelPacket. This harness uses Scheduletest, and so the user should compile the latest version of ScheduleTest before compiling this test.

ScheduleTest can be found in networking/integrationtest and is built from the networking/integrationtest/group directory. TS_EtelPacket can be found  in telephony/etelpckt/ts_etelpacket. The test is complied from the telephony/etelpckt/group directory. Please note that in order to run this test you also need to compile the dummy tsy.

For ease of merging test code defect fixes from Hurricane to 3GComms, the old test code is still present in the 3GComms_Hurricane branch but is not built.
