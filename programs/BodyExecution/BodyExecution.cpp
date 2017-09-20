// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "BodyExecution.hpp"

namespace teo
{

/************************************************************************/

bool BodyExecution::configure(yarp::os::ResourceFinder &rf)
{
    std::string robot = rf.check("robot",yarp::os::Value(DEFAULT_ROBOT),"name of /robot to be used").asString();

    printf("--------------------------------------------------------------\n");
    if (rf.check("help"))
    {
        printf("BodyExecution options:\n");
        printf("\t--help (this help)\t--from [file.ini]\t--context [path]\n");
        printf("\t--robot: %s [%s]\n",robot.c_str(),DEFAULT_ROBOT);
        ::exit(0);
    }

    state = VOCAB_STATE_HOME;

    std::string bodyExecutionStr("/bodyExecution");



    yarp::os::Property leftArmOptions;
    leftArmOptions.put("device","remote_controlboard");
    leftArmOptions.put("remote",robot+"/leftArm");
    leftArmOptions.put("local",bodyExecutionStr+robot+"/leftArm");
    leftArmDevice.open(leftArmOptions);
    if(!leftArmDevice.isValid()) {
      printf("robot leftArm device not available.\n");
      leftArmDevice.close();
      yarp::os::Network::fini();
      return false;
    }

    if ( ! leftArmDevice.view(leftArmIPositionControl) ) {
        printf("[warning] Problems acquiring leftArmPos interface\n");
        return false;
    } else printf("[success] Acquired leftArmPos interface\n");
    leftArmIPositionControl->setPositionMode();

    yarp::os::Property rightArmOptions;
    rightArmOptions.put("device","remote_controlboard");
    rightArmOptions.put("remote",robot+"/rightArm");
    rightArmOptions.put("local",bodyExecutionStr+robot+"/rightArm");
    rightArmDevice.open(rightArmOptions);
    if(!rightArmDevice.isValid()) {
      printf("robot rightArm device not available.\n");
      rightArmDevice.close();
      yarp::os::Network::fini();
      return false;
    }

    if ( ! rightArmDevice.view(rightArmIPositionControl) ) {
        printf("[warning] Problems acquiring rightArmPos interface\n");
        return false;
    } else printf("[success] Acquired rightArmPos interface\n");
    rightArmIPositionControl->setPositionMode();




    //phase = false;

    inDialogPort.open("/bodyExecution/rpc:s");
    inDialogPort.setReader(*this);  //-- Callback reader: avoid need to call inSrPort.read().

    return this->start();  //-- Start the thread (calls run).
}

/************************************************************************/

bool BodyExecution::interruptModule()
{
    this->stop();
    //inDialogPort.interrupt();
    leftArmDevice.close();
    return true;
}

/************************************************************************/

double BodyExecution::getPeriod()
{
    return 4.0; // Fixed, in seconds, the slow thread that calls updateModule below
}

/************************************************************************/

bool BodyExecution::updateModule()
{
    printf("Entered updateModule...\n");

    return true;
}

/************************************************************************/

bool BodyExecution::armJointsMoveAndWait(std::vector<double>& leftArmQ, std::vector<double> &rightArmQ)
{
    // -- Configuring Speeds and Accelerations
    std::vector<double> armSpeeds(7,30.0);
    std::vector<double> armAccelerations(7,30.0);

    rightArmIPositionControl->setRefSpeeds(armSpeeds.data());
    leftArmIPositionControl->setRefSpeeds(armSpeeds.data());
    rightArmIPositionControl->setRefAccelerations(armAccelerations.data());
    leftArmIPositionControl->setRefAccelerations(armAccelerations.data());
    rightArmIPositionControl->positionMove( rightArmQ.data() );
    leftArmIPositionControl->positionMove( leftArmQ.data() );

    //printf("Waiting for right arm.");
    bool doneRight = false;
    bool doneLeft = false;
    while((!doneRight)&&(!Thread::isStopping()))
    {
        rightArmIPositionControl->checkMotionDone(&doneRight);
        yarp::os::Time::delay(0.1);
    }

    while((!doneLeft)&&(!Thread::isStopping()))
    {
         leftArmIPositionControl->checkMotionDone(&doneLeft);
          yarp::os::Time::delay(0.1);
    }

    //printf("\n");
    return true;
}

/************************************************************************/

bool BodyExecution::read(yarp::os::ConnectionReader& connection)
{
     yarp::os::Bottle b;
     b.read(connection);
     // process data in b
     state = b.get(0).asVocab();

     //printf("[BodyExecution] Got %s\n", b.toString().c_str());

     //if( (VOCAB_STATE_SALUTE ==b.get(0).asVocab()) )
     //{
     //    state = VOCAB_STATE_SALUTE;
     //}
     //else if (VOCAB_STATE_HOME ==b.get(0).asVocab())
     //    state = VOCAB_STATE_HOME;

     //else if (VOCAB_STATE_SIGNALIZE_RIGHT == b.get(0).asVocab())
     //    state = VOCAB_STATE_SIGNALIZE_RIGHT;

     //else if (VOCAB_STATE_SIGNALIZE_LEFT == b.get(0).asVocab())
     //    state = VOCAB_STATE_SIGNALIZE_LEFT;

     return true;
}

/************************************************************************/

void BodyExecution::run()
{
    while( !Thread::isStopping() )
    {
        switch (state)
        {        

        case VOCAB_STATE_SALUTE:
            printf("Salute\n");
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[1] = 4;    // Tray security position
                rightArmQ[0] = 45;
                rightArmQ[2] = 20;
                rightArmQ[3] = 80;
                armJointsMoveAndWait(leftArmQ,rightArmQ);
            }
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[1] = 4;    // Tray security position
                rightArmQ[0] = 45;             
                rightArmQ[2] = -20;
                rightArmQ[3] = 80;
                armJointsMoveAndWait(leftArmQ,rightArmQ);
            }
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[1] = 4;    // Tray security position
                rightArmQ[0] = 45;
                rightArmQ[2] = 20;
                rightArmQ[3] = 80;
                armJointsMoveAndWait(leftArmQ,rightArmQ);
            }
            //state = VOCAB_STATE_ARM_SWINGING;
            break;     

        case VOCAB_STATE_HOME:
            printf("Home\n");
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[1] = 4;    // Tray security position
                armJointsMoveAndWait(leftArmQ,rightArmQ);
            }
            break;

        case VOCAB_STATE_EXPLANATION_1:
            printf("Explanation 1");
            {
               std::vector<double> leftArmQ(7,0.0);
               std::vector<double> rightArmQ(7,0.0);
               rightArmQ[0] = -17.029877;
               rightArmQ[1] = -22.653778;
               rightArmQ[2] = 1.493849;
               rightArmQ[3] = 88.752197;
               rightArmQ[4] = -2.355011;
               rightArmQ[5] = 53.778557;
               armJointsMoveAndWait(leftArmQ,rightArmQ);
            }
        {
            std::vector<double> leftArmQ(7,0.0);
            std::vector<double> rightArmQ(7,0.0);
            rightArmQ[0] = 65;
            rightArmQ[1] = -79.420044;
            rightArmQ[2] = 6.239016;
            rightArmQ[3] = 88.752197;
            rightArmQ[4] = -31.441132;
            rightArmQ[5] = -31.177521;
            armJointsMoveAndWait(leftArmQ,rightArmQ);
        }


        default:
            printf("Doing nothing....\n");
            break;
        }
    }

    return;
}

/************************************************************************/

}  // namespace teo

