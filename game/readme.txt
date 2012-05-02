State Machine Usage
Ben Cleveland
April 28th, 2010

Global Message Response - traveler.cpp   line 43
OnTimeInState  		- projectile.cpp line 104
ChangeStateDelayed 	- projectile.cpp line 137
Data passed in Msg Used - projectile.cpp line 63
Data passed in Msg 	- attacker.cpp 	 line 93
SendMsgDelayedToState 	- attacker.cpp 	 line 85
PopStateMachine 	- attacker.cpp 	 line 128
RequeueStateMachine 	- patroller.cpp  line 71
PushStateMachine 	- traveler.cpp   line 83
OnPeriodicTimeInState   - traveler.cpp 	 line 76
Substates 		- traveler.cpp   line 104 - 132


State Machine Language v3.0
Designed by Steve Rabin
Nov 20th, 2008


============================================================
1.0 Special Instructions
============================================================

Note: Place the USERTYPE.DAT file in the 
C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE 
directory of Microsoft Visual Studio. This will highlight 
the state machine keywords.


============================================================
2.0 Overview
============================================================

The State Machine Language is a hybrid approach to creating 
a friendly FSM scripting language for game AI that is fully 
contained within the game's native source language, namely 
C++. This results in a consistent enforced structure that is 
easy to read, easy to program, and easy to debug.

The State Machine Language is tightly coupled with a 
messaging system that provides much of the power of the 
system. For example, timers and delays are implemented 
within the state machine using the messaging system. Also, 
all communication between AI agents can easily be 
accomplished via the SendMsg and OnMsg constructs that send 
and capture messages within states.

In this implementation, game objects contain a queue of 
state machines, running the top one on the queue. State
machines can be pushed, popped, queued, and requeued. The
game object maintains a state machine manager which funnels
initialization, update messages, and general messages to the 
current state machine.

Messages are special packages that can be sent within a 
state machine or to state machines within other game 
objects. A message can be delayed so that it is delivered at 
a specific time in the future. It can also hold a small 
amount of data. Message names are defined in the file 
msgnames.h. There are many various helper functions for 
sending messages that can be seen in statemch.h.


============================================================
3.0 State Machine Language Keywords
============================================================

The state machine language has several keywords that
interchangably fit together to compose a state machine. All 
state machines must start with the "BeginStateMachine" 
keyword and end with the "EndStateMachine" keyword.

Between those two keywords, states can be defined by using 
the "DeclareState()" keyword and placing the name of the 
state as the argument. The state names are defined at the 
top of the derived state machine cpp file as a StateName 
enum. Note: The very first state to execute will be the 
first one defined in the StateName enum.

Under a DeclareState(), you can catch events such as 
entering the state for the first time ("OnEnter"), exiting 
the state ("OnExit"), an update notification every frame 
("OnUpdate"), and a message notification ("OnMsg()"). 
Whatever C++ code appears below these keywords will be 
executed on those events.

The space before the very first DeclareState() declaration is 
reserved for global message responses. This is where you can 
put "OnMsg()" to capture message events regardless of which 
state is active. However, if an "OnMsg()" listening for the 
same message appears in the current active state, then it 
will have priority and the global "OnMsg()" will not be 
triggered (if possible, the message is consumed by the most
specific substate or state).

Within a DeclareState(), you can have "DeclareSubstate()" 
keywords to define substates. The default substate is NULL. 
So if there are three DeclareSubstate() keywords defined 
within a DeclareState(), when the DeclareState() is 
initially entered it is in none of the substates. Through 
the use of "ChangeSubstate" in an "OnEnter", the current 
NULL substate can be changed to any of the defined substates.


============================================================
4.0 State Machine Control Flow
============================================================

The state machine is driven by events. By understanding the
events, it is easier to conceptualize the control flow.

When a state machine is created and pushed onto a game 
object's state machine queue, it is initialized by sending 
it an OnEnter event. This can be captured in the first
state that is defined in the StateName enum.

Every frame, the database is sent an update message that
flows to the game object and then gets pumped into the
active state machine as an OnUpdate event.

State machines can send messages that are routed immediately
to itself or other state machines. State machines can also
send delayed messages that are sent in the future.

In the main game loop, after objects in the database are 
updated, queued delayed messages are checked to see if it 
is time for them to be delivered. Any ripe messages are
then sent.

Within a state machine, it is possible to change states
using the "ChangeState" or "ChangeSubstate" function. 
These are requests to change states. The state change takes 
place once control exits the state machine. Upon a state 
change, an OnExit event is sent to the state machine, the 
state is then changed, and then an OnEnter event is sent. 
State changes can be delayed by using the "ChangeStateDelayed" 
or "ChangeSubstateDelayed" functions.

Message scoping is an important technique to keep particular
delayed messages from inappropriately being delivered.
A delayed message (and delayed state changes as well) can
be scoped to the substate, state, or state machine. If a
delayed message is sent within state A and it is scoped to
that state, then the message will not be delivered if the
state changes. The function "SendMsgDelayedToState" will 
scope the message to only be valid if the current state
does not change. Similarly, "SendMsgDelayedToSubstate" will
scope the message to only be valid if the current substate
does not change. Use "SendMsgDelayedToStateMachine" to scope 
a delayed message to the entire state machine, so that it's 
delivered regardless of substate or state changes. Note that
scoped messages can still be captured by global message
responses as long as the state or substate hasn't changed to
invalidate the message.


============================================================
5.0 State Machine Implementation
============================================================

The state machine keywords are implemented as macros. The 
macro definitions can be found at the top of the file
statemch.h. In essence, a state machine is a series of "if" 
statements coupled with the state machine class that pumps 
events into it.

By keeping the messy details hidden behind macros, the state 
machine is kept clean, easy to read, and easy to debug.

An additional benefit of the macros is that logging info and 
error checking can be hidden within the macros. There are 
two sets of macros, with the default group containing the 
extra debugging assistance. When this debugging info is not 
desired, the define DEBUG_STATE_MACHINE_MACROS as the top of 
statemch.h can be commented out.


============================================================
6.0 Additional Info
============================================================

Several articles/chapters have been written about various 
earlier versions of the State Machine Language. Please 
see the following articles for more information:

Rabin, Steve, "Designing a General Robust AI Engine," 
Game Programming Gems, Charles River Media, 2000.

Rabin, Steve, "Implementing a State Machine Language," 
AI Game Programming Wisdom, Charles River Media, 2002.

Rabin, Steve, "Enhancing a State Machine Language through 
Messaging," AI Game Programming Wisdom, Charles River 
Media, 2002.

Fu, Dan and Houlette, Ryan, "The Ultimate Guide to FSMs 
in Games," Charles River Media, 2003.

Rabin, Steve, Introduction to Game Development, Charles 
River Media, 2005.


============================================================
7.0 Updates and Bug Fixes
============================================================

The latest version of the State Machine Language can be
downloaded from www.introgamedev.com or www.aiwisdom.com.


============================================================
8.0 Contact
============================================================

Comments, suggestions, and bug fixes should be e-mailed to
steve.rabin@gmail.com



THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.