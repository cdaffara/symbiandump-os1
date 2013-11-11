The purpose of this dummy transport implementation is to abstract away
the thread management process for simple demonstration purposes and perhaps
also some simple test scenarios.

Normally we would of course use the root server to manage our threads
and a proper, efficient, non-fallible transport supporting in-thread
as well as cross-thread as well as cross-process communication.

As well as lack of support for the above mentioned areas, this transport
is purposely slow, resource hungry and most of all fallible.

THEREFORE IT CAN NOT BE USED FOR ANY OTHER PURPOSES
THAN THE ONES STATED ABOVE.

IT IS ALSO NOT SUITABLE FOR ANY KIND OF OOM TESTING.