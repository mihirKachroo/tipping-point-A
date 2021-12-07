#include "main.h"
#include "globals.h"
#include "macros.h"

/**
 * \brief Scale joystick output to cubic graph
 * 
 * Scales raw value in range [-127, 127] to range [-1, 1] and apply exponent 3 in order to allow for more driver accuracy
*/
double joystickCubicDrive(int raw) {
    double scaledRaw = ((double)raw) / 127.0;
    return pow(scaledRaw, 3) * 127;
}

void myOpControl() {

    // Enable all systems
    intake.enable();
    forklift.enable();

    // 0 -> nothing, 1 -> clockwise, -1 -> counter clockwise
    int macroToggle = 0;

    // Internal state for forklift 1
    // 0 -> Down, 1 -> Middle, 2 -> Up
    int forkliftState = 0;

    while (true) {
        // Basic op control using arcade drive
        int forward = masterController.get_analog(ANALOG_LEFT_Y);
        int sideways = masterController.get_analog(ANALOG_LEFT_X);

        driveTrain->arcade(joystickCubicDrive(forward), joystickCubicDrive(sideways), 0);

        // Intake mapped to right joystick
        int intakeSpeed = masterController.get_analog(ANALOG_RIGHT_Y);

        // Forklift mapped to left shoulder buttons
        int forkliftUp = masterController.get_digital(DIGITAL_L1);
        int forkliftDown = masterController.get_digital(DIGITAL_L2);

        int intakeMacroCW = masterController.get_digital_new_press(DIGITAL_UP);
        int intakeMacroCCW = masterController.get_digital_new_press(DIGITAL_DOWN);

        // Intake macro handler
        if (macroToggle == -1 && intakeMacroCCW) {
            // Turn off macro
            macroToggle = 0;
        } else if (macroToggle == 1 && intakeMacroCW) {
            // Turn off macro
            macroToggle = 0;
        } else {
            // State does not match the button pressed, enable the respective macro
            if (intakeMacroCW) {
                // Change macro state to clockwise
                macroToggle = 1;
            } else if (intakeMacroCCW) {
                // Change macro state to counter clockwise
                macroToggle = -1;
            }
        }

        // Apply macro control to intake system
        switch (macroToggle) {
            case 1: {
                startIntakeSmoothMove(false, false);
                break;
            }
            case -1: {
                startIntakeSmoothMove(true, true);
                break;
            }
            case 0: {
                // Operator control
                intake.control();

                intake.setPower(intakeSpeed);
            }
            default: {
                break;
            }
        }
        intake.update();

        // Forklift control
        forklift.control();
        
        int forkliftSpeed = 127;

        if (forkliftUp) {
            forklift.setPower(forkliftSpeed);
        }
        else if (forkliftDown) {
            forklift.setPower(-forkliftSpeed);
        }
        else {
            forklift.setPower(0);
        }
        // Run update funcs on sysmans
        forklift.update();

        // Operator control
        if (!macroToggle) {
            stopIntakeSmoothMove();
        }

        pros::delay(10);
    }    
}