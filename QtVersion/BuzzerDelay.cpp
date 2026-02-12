#include "mainwindow.h"

void MainWindow::stateBuzzerDelay()
{
    //Enter BuzzerDelay State
    previousState = currentState;
    currentState = states::BuzzerDelayState;

    qInfo() << "BuzzerDelayState";

    buzzerTimerDuration = 3;

    //Start buzzer timer
    if(timerBuzzer.isActive())
        timerBuzzer.stop();
    timerBuzzer.start(1000);

}

void MainWindow::buzzerTimerDecrement()
{
    if (buzzerTimerDuration == 3)
    {
        //Ring buzzer
        writeDigitalOutput(m_buzzerDigitalOutAddress, 1);
    }
    else if (buzzerTimerDuration == 1)
    {
        //Stop buzzer
        writeDigitalOutput(m_buzzerDigitalOutAddress, 0);
    }
    else if (buzzerTimerDuration <= 0)
    {
        if (previousState == states::StopState)
        {
            timerBuzzer.stop();
            stateRun1();
        }
        else if (previousState == states::EstopState)
        {
            if (estopReturnToState == states::Run1State)
            {
                timerBuzzer.stop();
                stateRun1();
            }
            else if (estopReturnToState == states::Run2State)
            {
                timerBuzzer.stop();
                stateRun2();
            }
            else stateStop();
        }
    }
    buzzerTimerDuration--;
}
