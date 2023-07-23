//
//  CellTypes.h
//  ClusterBotz
//
//  Created by Scott Schafer on 12/5/17.
//
//

#ifndef CellTypes_h
#define CellTypes_h


// Cell actions, each with a byte parameter.
// Note: a cell has a state, which includes:
// - Sleeping / Not sleeping
// - Counter
enum {
  actionMove = 1,   // moves
  actionBite,       // transfers energy from one organism to another
  actionEye,        // fires when facing the specified target
  actionNerve,      // fires when cell is compressed (according to param)
  actionTimer       // fires periodically (according to param)
};

// a cell can have a rule for each connection controlling how it will respond to a signal from
enum {
  sigIncrement = 1,     // increment some counter when receiving a signal
  sigDecrement,
  sigToggle,
  sigClear,
  sigRebroadcast,
};

// cells can pass signals

// a connection can send a signal to connected cells.


// connection types
enum {
  connectionPassthrough = 1,
  connectionInvert,
  connectionNull
};


#endif /* CellTypes_h */
