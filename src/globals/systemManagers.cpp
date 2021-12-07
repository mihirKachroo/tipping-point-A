#include "globals.h"

Intake intake(Intake::HOLD_STATE, PIDInfo(1, 1, 1));

Forklift forklift(Forklift::DOWN_STATE, forkliftMotor, PIDInfo(1, 1, 1));