#include "debouncer.h"

db::db(int bp, unsigned long dp = 10){
  buttonPin = bp;
  debouncePeriod = dp;
}

void db::Init(){
  pinMode(buttonPin, INPUT_PULLUP);
}

bool db::checkButtonPress(){
  bool flag = false;
  buttonPosition = digitalRead(buttonPin);

  switch(state){
    case STABLE:
    if(statePrev == UNSTABLE && buttonPosition == HIGH && tempButtonPosition == LOW){
      tempButtonPosition = buttonPosition;
      flag = true;
    }
    else if (buttonPosition != tempButtonPosition){
      timeStart = millis();
      tempButtonPosition = buttonPosition;
      state = UNSTABLE;
    }
    break;

    case UNSTABLE:
    //Pin changes again but not enough time as elapsed -> restart timer
    if(buttonPosition != tempButtonPosition && millis()-timeStart < debouncePeriod){
      timeStart = millis();
      tempButtonPosition = buttonPosition;
      }
    //Pin changes AND enough time elapsed -> state is stable
    if(buttonPosition != tempButtonPosition && millis()-timeStart >= debouncePeriod){
      statePrev = UNSTABLE;
      state = STABLE;
      }
    break;
  }

  return flag;
}
