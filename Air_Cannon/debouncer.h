#ifndef DEBOUNCER_H
#define DEBOUNCER_H
#include <Arduino.h>

class db{
  private:
  enum BUTTON_STATE{STABLE, UNSTABLE};
  BUTTON_STATE state = STABLE;

  int tempButtonPosition = buttonPosition;
  BUTTON_STATE statePrev = state;
  
  int buttonPin = -1;
  int buttonPosition = 0;
  unsigned long timeSince;
  unsigned long timeStart;
  unsigned long debouncePeriod = 10; 
  
  public:
  db(int bp, unsigned long db = 10);
  bool checkButtonPress();
  void Init();
};

#endif
