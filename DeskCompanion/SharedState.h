#ifndef STATE_H
#define STATE_H

struct SharedState {
  bool offline_mode;
  bool active_stopwatch;
  String queued_main_button_event;
};

#endif
