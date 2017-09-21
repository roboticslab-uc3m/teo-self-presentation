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

    //state = VOCAB_STATE_HOME;

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

    std::vector<double> armSpeeds(7,20.0); // 7,30.0
    std::vector<double> armAccelerations(7,20.0); // 7,30.0

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
                printf("SALUTE (1)\n");
            }
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[1] = 4;    // Tray security position
                rightArmQ[0] = 45;             
                rightArmQ[2] = -20;
                rightArmQ[3] = 80;
                armJointsMoveAndWait(leftArmQ,rightArmQ);
                printf("SALUTE (2)\n");
            }
            {
                std::vector<double> leftArmQ(7,0.0);
                std::vector<double> rightArmQ(7,0.0);
                leftArmQ[1] = 4;    // Tray security position
                rightArmQ[0] = 45;
                rightArmQ[2] = 20;
                rightArmQ[3] = 80;
                armJointsMoveAndWait(leftArmQ,rightArmQ);
                printf("SALUTE (3)\n");
            }
            state = VOCAB_STATE_HOME;
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
            printf("Explanation 1\n");
            {
               double rightArmPoss[7] = {-17.029877, -22.653778, 1.493849, 88.752197, -2.355011, 53.778557, 0.0};
               std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
               std::vector<double> leftArm(7,0.0);
               armJointsMoveAndWait(leftArm,rightArm);
            }
            {
                double rightArmPoss[7] = {65, -79.420044, 6.239016, 88.664322, -31.441132, -31.177521, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(7,0.0);
                armJointsMoveAndWait(leftArm,rightArm);
            }
            {
            double rightArmPoss[7] = {-17.029877, -22.653778, 1.493849, 88.752197, -2.355011, 53.778557, 0.0};
            double lefArmPoss[7] = {-68.750282, 10.531192, 16.5944, -96.568978, 34.899421, -9.019511, 0.0};
            std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
            std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
            armJointsMoveAndWait(leftArm,rightArm);
            }
            state = VOCAB_STATE_HOME;
            break;

        case VOCAB_STATE_EXPLANATION_2:
            printf("Explanation 2\n");
            {
                double rightArmPoss[7] = {39.36731, -3.848846, 6.151142, 65.20211, 12.741652, 57.205624, 0.0};
                double lefArmPoss[7] = {-39.701233, -0.246063, -5.342712, -58.330414, -15.18454, -71.687164, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
                armJointsMoveAndWait(leftArm,rightArm);
            }
            {
                double rightArmPoss[7] = {63.796131, -3.848846, 3.602812, 65.20211, 12.653778, 38.57645, 0.0};
                double lefArmPoss[7] = {-39.701233, -0.246063, -5.342712, -58.330414, -15.18454, -71.687164, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
                armJointsMoveAndWait(leftArm,rightArm);
            }
            break;

       case VOCAB_STATE_EXPLANATION_HEAD:
            printf("Explanation Head\n");
            {

                double lefArmPoss[7] = {-70.896301, -1.300537, 48.506149, -93.040436, 8.963093, -55.518463, 0.0};
                std::vector<double> rightArm(7,0.0);
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
                armJointsMoveAndWait(leftArm,rightArm);
            }
            break;

       case VOCAB_STATE_EXPLANATION_PC_RIGHT:
            printf("Explanation PC 1 \n");
            {
                double rightArmPoss[7] = {63.532513, -8.154663, -34.516693, 72.231987, -32.319855, 95.166962, 0.0};
                std::vector<double> leftArm(7,0.0);
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
                armJointsMoveAndWait(leftArm,rightArm);
            }
            break;

       case VOCAB_STATE_EXPLANATION_PC_LEFT:
            printf("Explanation PC 1 \n");
            {
                double rightArmPoss[7] = {64.938484, 8.963093, -54.288239, 72.319862, -40.404236, 53.866432, 0.0};
                std::vector<double> leftArm(7,0.0);
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
                armJointsMoveAndWait(leftArm,rightArm);
            }
            break;

       case VOCAB_STATE_EXPLANATION_PC_INSIDE:
            printf("Explanation PC 1 \n");
            {
                double lefArmPoss[7] = {-37.957572, 15.479351, 29.750346, -94.636667, 20.312505, -79.961016, 0.0};
                std::vector<double> rightArm(7,0.0);
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
                armJointsMoveAndWait(leftArm,rightArm);
            }
            break;

       case VOCAB_STATE_EXPLANATION_HDD:
            printf("Explanation HDD \n");
        {
            double rightArmPoss[7] = {42.706501, -8.418274, -30.826019, 81.370827, -68.787354, 93.673111, 0.0};
            double lefArmPoss[7] = {-36.801422, 14.850615, 37.961334, -85.834808, 60.720562, -80.298767, 0.0};
            std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
            std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
            armJointsMoveAndWait(leftArm,rightArm);
        }

        case VOCAB_STATE_EXPLANATION_SENSOR:
             printf("Explanation Sensors \n");
         {
             double rightArmPoss[7] = {42.003513, 5.975395, -38.031647, 76.97715, -48.927948, 36.203865, 0.0};
             double lefArmPoss[7] = {-55.869965, -11.318115, 27.065025, -87.328644, 38.840069, -43.040436, 0.0};
             std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
             std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
             armJointsMoveAndWait(leftArm,rightArm);
         }
         {
             double rightArmPoss[7] = {42.706501, -8.418274, -30.826019, 81.370827, -68.787354, 93.673111, 0.0};
             double lefArmPoss[7] = {-44.797882, 14.323374, 9.314587, -79.947266, 44.639717, -54.024597, 0.0};
             std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
             std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
             armJointsMoveAndWait(leftArm,rightArm);
         }
        {
            double rightArmPoss[7] = {64.323372, 13.70826, -38.119507, 76.97715, -48.840088, 43.14587, 0.0};
            double lefArmPoss[7] = {-44.797882, 14.323374, 9.314587, -79.947266, 44.639717, -54.024597, 0.0};
            std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
            std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
            armJointsMoveAndWait(leftArm,rightArm);
        }

        {
            double rightArmPoss[7] = {64.323372, 13.70826, -38.119507, 76.97715, -48.840088, 43.14587, 0.0};
            double lefArmPoss[7] = {-62.108978, -3.497375, 28.910368, -66.678375, 55.272408, -62.6362, 0.0};
            std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+6); //teoSim (+6) teo (+7)
            std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+6);
            armJointsMoveAndWait(leftArm,rightArm);
        }
            state = VOCAB_STATE_HOME;
            break;

        default:
            printf("Doing nothing....\n");
            break;
        }

    }
}

/************************************************************************/

}  // namespace teo

