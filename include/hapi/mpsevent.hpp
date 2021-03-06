// DOCUMENTATION: mpsevent.hpp {{{
/*! \file
 * This file declares the MpsEvent class used to represent the events omitted
 * by the processes during evaluation. These events include message sending and reveiving,
 * session creation, synchronization etc.
 *
 * MpsQueue is defined as a queue of events, and used to represent the message buffers.
 *
 * MpsEnv is defined as a map from channels to queues, and used to represent
 * the buffer environment of a process.
 */
// }}}
#ifndef MPSEVENT_HPP
#define MPSEVENT_HPP

#include <hapi/mpschannel.hpp>
#include <string>
#include <list>
#include <map>
#include <iostream>

namespace hapi
{

// DOCUMENTATION: event_type {{{
/*! event_type is an enumeration of the different types of events.
 *
 * tau is the "nothing happens" event. <br/>
 * snd is the sending of a value event. <br/>
 * rcv is the reception of a value event. <br/>
 * bsnd is the sending of a label (choice). <br/>
 * brcv is the reception of a label (choice). <br/>
 * link is the creation of a session. <br/>
 * sync is the symmetric synchronization. <br/>
 * guivalue is adding information to a UI. <br/>
 * fundef is the definition of a procedure. <br/>
 * funcall is the calling of a procedure. <br/>
 * nu is the creation of a global channel. <br/>
 */
// }}}
enum event_type {tau=0, snd=10, rcv=11, bsnd=12, brcv=13, link=20, sync=21, guivalue=30, fundef=40, funcall=41, nu=42, cond=43, assign=44, create=45, host=101, sndtype=210, rcvtype=211};

// DOCUMENTATION: MpsEvent {{{
/*! MpsEvent represents an event that can be omitted by a process during evaluation.
 * The different kinds of events are: <br/>
 * sending to and receiving from a queue including branching, creating a
 * multiparty session and synchronizing on a session.
 *
 * The content of an event is <br/>
 * The event type <br/>
 * The channel for communication <br/>
 * The value or label being communicated <br/>
 * For session init the participants currently accepted, and the session name <br/>
 * For session sync the number of participants, and the number of required participants
 *
 * The Subtype property was added in order to send events to the GUI module
 * when certain steps are performed (linking, syncing and guivalue)
 */
// }}}
class MpsEvent // {{{
{
  public:
    MpsEvent();
    MpsEvent(const MpsEvent &rhs);
    virtual ~MpsEvent();

    MpsEvent &operator=(const MpsEvent &rhs);
    std::string ToString() const;

    // Data
    event_type myType; // Holds the event type
    event_type mySubType; // Holds the event type
    MpsChannel myChannel; // For snd, rcv, bsnd, brcv, link and sync
    MpsExp *myValue; // For snd, rcv and guivalue
    std::string myLabel; // For bsnd, brcv and sync
    std::string mySession; // For link, guivalue, sndtype and rcvtype
    int myMaxpid; // For link and sync
    int myPid; // For guivalue,link
    std::string myName; // For guivalue
    int myArgs; // For Call event
    int myStateArgs; // For Call event
}; // }}}

// DOCUMENTATION: MpsQueue {{{
/*!
 * MpsQueue represents a message queue in a runtime process
 */
// }}}
typedef std::list<MpsEvent> MpsQueue;

// DOCUMENTATION: MpsEnv {{{
/*!
 * MpsEnv represents a buffer environment in which to evaluate a process
 */
// }}}
typedef std::map<MpsChannel,MpsQueue> MpsEnv;

inline std::string EnvToString(const MpsEnv &env) // {{{
{
  std::string result="[[";
  bool first = true;
  for (MpsEnv::const_iterator chn=env.begin(); chn!=env.end(); ++chn)
  {
    if (chn->second.size() > 0)
    {
      if (!first)
        result += "\n |";
      else 
        first = false;
      result+=chn->first.ToString() + ": ";
      for (MpsQueue::const_iterator msg=chn->second.begin(); msg!=chn->second.end(); ++msg)
      {
        if (msg!=chn->second.begin())
          result += ",";
        result += msg->ToString();
      }
    }
  }
  result += "]]";
  return result;
} // }}}

}
#endif
