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

    std::string bodyExecutionStr("/bodyExecution");


    // ------ HEAD -------
    yarp::os::Property headOptions;
    headOptions.put("device","remote_controlboard");
    headOptions.put("remote",robot+"/head");
    headOptions.put("local",bodyExecutionStr+robot+"/head");
    headDevice.open(headOptions);
    if(!headDevice.isValid()) {
      printf("robot head device not available.\n");
      headDevice.close();
      yarp::os::Network::fini();
      return false;
    }

    if (!headDevice.view(headIControlMode2) ) { // connecting our device with "control mode 2" interface, initializing which control mode we want (position)
        printf("[warning] Problems acquiring headIControlMode2 interface\n");
        return false;
    } else printf("[success] Acquired headIControlMode2 interface\n");

    if (!headDevice.view(headIPositionControl2) ) { // connecting our device with "position control 2" interface (configuring our device: speed, acceleration... and sending joint positions)
        printf("[warning] Problems acquiring headIPositionControl2 interface\n");
        return false;
    } else printf("[success] Acquired headIPositionControl2 interface\n");


    // ------ LEFT ARM -------
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

    if (!leftArmDevice.view(leftArmIControlMode2) ) { // connecting our device with "control mode 2" interface, initializing which control mode we want (position)
        printf("[warning] Problems acquiring leftArmIControlMode2 interface\n");
        return false;
    } else printf("[success] Acquired leftArmIControlMode2 interface\n");

    if (!leftArmDevice.view(leftArmIPositionControl2) ) { // connecting our device with "position control 2" interface (configuring our device: speed, acceleration... and sending joint positions)
        printf("[warning] Problems acquiring leftArmIPositionControl2 interface\n");
        return false;
    } else printf("[success] Acquired leftArmIPositionControl2 interface\n");


    // ------ RIGHT ARM -------
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

    if (!rightArmDevice.view(rightArmIControlMode2) ) { // connecting our device with "control mode 2" interface, initializing which control mode we want (position)
        printf("[warning] Problems acquiring rightArmIControlMode2 interface\n");
        return false;
    } else printf("[success] Acquired rightArmIControlMode2 interface\n");


    if (!rightArmDevice.view(rightArmIPositionControl2) ) { // connecting our device with "position control 2" interface (configuring our device: speed, acceleration... and sending joint positions)
        printf("[warning] Problems acquiring rightArmIPositionControl2 interface\n");
        return false;
    } else printf("[success] Acquired rightArmIPositionControl2 interface\n");

    //-- Set control modes
    int headAxes;
    headIPositionControl2->getAxes(&headAxes);
    std::vector<int> headControlModes(headAxes,VOCAB_CM_POSITION);
    if(! headIControlMode2->setControlModes( headControlModes.data() )){
        printf("[warning] Problems setting position control mode of: head\n");
        return false;
    }
    
    int leftArmAxes;
    leftArmIPositionControl2->getAxes(&leftArmAxes);
    std::vector<int> leftArmControlModes(leftArmAxes,VOCAB_CM_POSITION);
    if(! leftArmIControlMode2->setControlModes( leftArmControlModes.data() )){
        printf("[warning] Problems setting position control mode of: left-arm\n");
        return false;
    }

    int rightArmAxes;
    rightArmIPositionControl2->getAxes(&rightArmAxes);
    std::vector<int> rightArmControlModes(rightArmAxes,VOCAB_CM_POSITION);
    if(! rightArmIControlMode2->setControlModes(rightArmControlModes.data())){
        printf("[warning] Problems setting position control mode of: right-arm\n");
        return false;
    }

    // -- Configuring ports
    inDialogPort.open("/bodyExecution/rpc:s");
    inDialogPort.setReader(*this);  //-- Callback reader: avoid need to call inSrPort.read().

    return this->start();  //-- Start the thread (calls run).
}

/************************************************************************/

bool BodyExecution::interruptModule()
{
    this->stop();
    inDialogPort.interrupt();
    rightArmDevice.close();
    leftArmDevice.close();
    headDevice.close();
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

bool BodyExecution::jointsMoveAndWait(std::vector<double>& leftArm, std::vector<double> &rightArm, std::vector<double> &head)
{
    // -- Configuring Speeds and Accelerations

    // -- Arms
    std::vector<double> armSpeeds(7,25.0); // 7,30.0
    std::vector<double> armAccelerations(7,25.0); // 7,30.0
    // -- Head
    std::vector<double> headSpeed(2,25.0); // 7,30.0
    std::vector<double> headAcceleration(2,25.0); // 7,30.0

    // -- configuring..

    if(!headIPositionControl2->setRefSpeeds(headSpeed.data())){
        printf("[Error] Problems setting reference speed on head joints.\n");
        return false;
    }
    
    if(!rightArmIPositionControl2->setRefSpeeds(armSpeeds.data())){
        printf("[Error] Problems setting reference speed on right-arm joints.\n");
        return false;
    }
    if(!leftArmIPositionControl2->setRefSpeeds(armSpeeds.data())){
        printf("[Error] Problems setting reference speed on left-arm joints.\n");
        return false;
    }
    if(!headIPositionControl2->setRefAccelerations(headAcceleration.data())){
        printf("[Error] Problems setting reference acceleration on head joints.\n");
        return false;
    }
    if(!rightArmIPositionControl2->setRefAccelerations(armAccelerations.data())){
        printf("[Error] Problems setting reference acceleration on right-arm joints.\n");
        return false;
    }
    if(!leftArmIPositionControl2->setRefAccelerations(armAccelerations.data())){
        printf("[Error] Problems setting reference acceleration on left-arm joints.\n");
        return false;
    }


    // -- move to position
    if(!headIPositionControl2->positionMove( head.data() )){
            printf("[Error: positionMove] Problems setting new reference point for head axes.\n");
            return false;
    }
    if(!rightArmIPositionControl2->positionMove( rightArm.data() )){
        printf("[Error: positionMove] Problems setting new reference point for right-arm axes.\n");
        return false;
    }
    if(!leftArmIPositionControl2->positionMove( leftArm.data() )){
            printf("[Error: positionMove] Problems setting new reference point for left-arm axes.\n");
            return false;
    }


    // -- checking movement done...
    bool doneRight = false;
    bool doneLeft = false;
    bool doneHead = false;

    while(!doneRight)
    {
        yarp::os::Time::delay(0.1);
        rightArmIPositionControl2->checkMotionDone(&doneRight);
    }

    while(!doneLeft)
    {
        yarp::os::Time::delay(0.1);
        leftArmIPositionControl2->checkMotionDone(&doneLeft);
    }    

/*  // to avoid problems, we have commented checkMotionDone for the head
    while(!doneHead)
    {
        yarp::os::Time::delay(0.1);
        headIPositionControl2->checkMotionDone(&doneHead);
    }
*/
    //printf("\n");
    return true;
}

/************************************************************************/

bool BodyExecution::read(yarp::os::ConnectionReader& connection)
{
     yarp::os::Bottle in, out; // in: the VOCAB_STATE, out: boolean to check if the movement has finished
     bool ok = in.read(connection);
     //if (!ok) return false;

     state = in.get(0).asVocab();

     if(state == VOCAB_RETURN_MOVEMENT_STATE){

         // -- Gets a way to reply to the message, if possible.
         ConnectionWriter *returnToSender = connection.getWriter();

         if(done) {
             out.addInt(1); // done = 1 (true)
         }
         if (returnToSender!=NULL)
             out.write(*returnToSender);
     }
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
                double rightArmPoss[7] = {45, 0.0, 20.0, 80, 0.0, 0.0, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(7,0.0);
                std::vector<double> head(2,0.0);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            {
                double rightArmPoss[7] = {45, 0.0, -20.0, 80, 0.0, 0.0, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(7,0.0);                
                std::vector<double> head(2,0.0);               
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            {                
                double rightArmPoss[7] = {45.0, 0.0, 20.0, 80, 0.0, 0.0, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(7,0.0);
                std::vector<double> head(2,0.0);
                jointsMoveAndWait(leftArm,rightArm,head);

            }
            state = VOCAB_STATE_HOME;            
            break;     

        case VOCAB_STATE_HOME:
            printf("Home\n");
            {
                std::vector<double> leftArm(7,0.0);
                std::vector<double> rightArm(7,0.0);
                std::vector<double> head(2,0.0);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            done = true;
            state = 0; //default
            break;

        case VOCAB_STATE_EXPLANATION_1:
            printf("Explanation 1\n");
            {
               double rightArmPoss[7] = {-17.029877, -22.653778, 1.493849, 88.752197, -2.355011, 53.778557, 0.0};
               std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
               std::vector<double> leftArm(7,0.0);               
               std::vector<double> head(2,0.0);
               jointsMoveAndWait(leftArm,rightArm,head);
            }
            {
                double rightArmPoss[7] = {65, -79.420044, 6.239016, 88.664322, -31.441132, -31.177521, 0.0};
                double headPoss[2] = {-60.0, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(7,0.0);                
                std::vector<double> head(&headPoss[0], &headPoss[0]+2);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            {
                double rightArmPoss[7] = {-17.029877, -22.653778, 1.493849, 88.752197, -2.355011, 53.778557, 0.0};
                double lefArmPoss[7] = {-68.750282, 10.531192, 16.5944, -96.568978, 34.899421, -9.019511, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(2,0.0);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            done = true;
            state = 0; //default
            break;

        case VOCAB_STATE_EXPLANATION_2:
            printf("Explanation 2\n");
            {
                double rightArmPoss[7] = {39.12303, -3.598853, 5.90115, 64.952431, -29.75017, 56.955845, 0.0};
                double lefArmPoss[7] = {-39.462073, -0.496053, -5.092714, -58.080423, 29.750108, -52.803162, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(2,0.0);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            {
                double rightArmPoss[7] = {63.796131, -3.848846, 3.602812, 65.20211, 12.653778, 38.57645, 0.0};
                double lefArmPoss[7] = {-63.12303, -3.598853, 5.90115, -80.952431, -10.249928, -19.750286, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(2,0.0);
                jointsMoveAndWait(leftArm,rightArm,head);
            }

            done = true;
            state = 0; //default
            break;

        case VOCAB_STATE_EXPLANATION_3:
            printf("Explanation 3\n");
            {
                double rightArmPoss[7] = {39.12303, -3.598853, 5.90115, 64.952431, -29.75017, 56.955845, 0.0};
                double lefArmPoss[7] = {-39.462073, -0.496053, -5.092714, -58.080423, 29.750108, -52.803162, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(2,0.0);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            {
                double rightArmPoss[7] = {63.796131, -3.848846, 3.602812, 65.20211, 12.653778, 38.57645, 0.0};
                double lefArmPoss[7] = {-63.12303, -3.598853, 5.90115, -80.952431, -10.249928, -19.750286, 0.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(2,0.0);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            {
                double rightArmPoss[7] = {63.796131, -3.848846, 3.602812, 65.20211, 12.653778, 38.57645, 0.0};
                double lefArmPoss[7] = {-63.12303, -3.598853, 5.90115, -80.952431, -10.249928, -19.750286, 0.0};
                double headPoss[2] = {0.0, 10.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(&headPoss[0], &headPoss[0]+2);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            {
                double rightArmPoss[7] = {63.796131, -3.848846, 3.602812, 65.20211, 12.653778, 38.57645, 0.0};
                double lefArmPoss[7] = {-63.12303, -3.598853, 5.90115, -80.952431, -10.249928, -19.750286, 0.0};
                double headPoss[2] = {0.0, -10.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(&headPoss[0], &headPoss[0]+2);
                jointsMoveAndWait(leftArm,rightArm,head);
            }

            done = true;
            state = 0; //default
            break;

       case VOCAB_STATE_EXPLANATION_HEAD:
            printf("Explanation Head\n");
            {

                double lefArmPoss[7] = {-70.896301, -1.300537, 48.506149, -93.040436, 8.963093, -52.803162, 0.0};
                double headPoss[2] = {20.0, 10.0};
                std::vector<double> rightArm(7,0.0);
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(&headPoss[0], &headPoss[0]+2);
                jointsMoveAndWait(leftArm,rightArm,head);
            }          
            done = true;
            state = 0; //default
            break;

       case VOCAB_STATE_EXPLANATION_PC_RIGHT:
            printf("Explanation PC 1 \n");
            {
                double rightArmPoss[7] = {63.532513, -8.154663, -34.516693, 72.231987, -32.319855, 85.166962, 0.0};
                double headPoss[2] = {-20.0, 10.0};
                std::vector<double> leftArm(7,0.0);                
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> head(&headPoss[0], &headPoss[0]+2);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            done = true;
            state = 0; //default
            break;

       case VOCAB_STATE_EXPLANATION_PC_LEFT:
            printf("Explanation PC 1 \n");
            {
                double rightArmPoss[7] = {64.938484, 8.963093, -54.288239, 72.319862, -40.404236, 50, 0.0};
                double headPoss[2] = {20.0, 10.0};
                std::vector<double> leftArm(7,0.0);                
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> head(&headPoss[0], &headPoss[0]+2);
                jointsMoveAndWait(leftArm,rightArm,head);
            }            
            done = true;
            state = 0; //default
            break;

       case VOCAB_STATE_EXPLANATION_PC_INSIDE:
            printf("Explanation PC 1 \n");
            {
                double lefArmPoss[7] = {-37.957572, 15.479351, 29.750346, -94.636667, 20.312505, -52.803162, 0.0}; // without plate (wrist = -79.961016)
                double headPoss[2] = {20.0, 10.0};
                std::vector<double> rightArm(7,0.0);
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(&headPoss[0], &headPoss[0]+2);
                jointsMoveAndWait(leftArm,rightArm,head);
            }
            done = true;
            state = 0; //default
            break;

       case VOCAB_STATE_EXPLANATION_HDD:
            printf("Explanation HDD \n");
        {
            double rightArmPoss[7] = {42.706501, -8.418274, -30.826019, 81.370827, -68.787354, 93.673111, 0.0};
            double lefArmPoss[7] = {-36.801422, 14.850615, 37.961334, -85.834808, 60.720562, -52.803162, 0.0};
            double headPoss[2] = {0.0, 10.0};
            std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
            std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
            std::vector<double> head(&headPoss[0], &headPoss[0]+2);
            jointsMoveAndWait(leftArm,rightArm,head);
        }            
            done = true;
            state = 0; //default
            break;

        case VOCAB_STATE_EXPLANATION_SENSOR:
             printf("Explanation Sensors \n");
         {
             double rightArmPoss[7] = {42.003513, 5.975395, -38.031647, 76.97715, -48.927948, 36.203865, 0.0};
             double lefArmPoss[7] = {-55.869965, -11.318115, 27.065025, -87.328644, 38.840069, -43.040436, 0.0};
             double headPoss[2] = {15, 10.0};
             std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
             std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
             std::vector<double> head(&headPoss[0], &headPoss[0]+2);
             jointsMoveAndWait(leftArm,rightArm,head);
         }
         {
             double rightArmPoss[7] = {42.003513, 5.975395, -38.031647, 76.97715, -48.927948, 36.203865, 0.0};
             double lefArmPoss[7] = {-44.797882, 14.323374, 9.314587, -79.947266, 44.639717, -52.803162, 0.0};
             double headPoss[2] = {15, 10.0};
             std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
             std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
             std::vector<double> head(&headPoss[0], &headPoss[0]+2);
             jointsMoveAndWait(leftArm,rightArm,head);
         }
            {
                double rightArmPoss[7] = {64.323372, 13.70826, -38.119507, 76.97715, -48.840088, 43.14587, 0.0};
                double lefArmPoss[7] = {-44.797882, 14.323374, 9.314587, -79.947266, 44.639717, -52.803162, 0.0};
                double headPoss[2] = {15, 10.0};
                std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
                std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
                std::vector<double> head(&headPoss[0], &headPoss[0]+7);
                jointsMoveAndWait(leftArm,rightArm,head);
            }

        {
            double rightArmPoss[7] = {65.465729, 14.850615, -36.449921, 76.97715, -21.335693, 56.151142, 0.0};
            double lefArmPoss[7] = {-62.108978, -3.497375, 28.910368, -66.678375, 55.272408, -52.803162, 0.0};
            double headPoss[2] = {-15, 10.0};
            std::vector<double> rightArm(&rightArmPoss[0], &rightArmPoss[0]+7); //teoSim (+6) teo (+7)
            std::vector<double> leftArm(&lefArmPoss[0], &lefArmPoss[0]+7);
            std::vector<double> head(&headPoss[0], &headPoss[0]+7);
            jointsMoveAndWait(leftArm,rightArm,head);
        }
            done = true;
            state = 0; //default
            break;

        default:
            yarp::os::Time::delay(1);
            printf("Doing nothing....\n");
            done = false;
            break;
        }

    }
}

/************************************************************************/

}  // namespace teo

