namespace yarp roboticslab

service SelfPresentationCommandsIDL
{
    oneway void doGreet();
    oneway void doHoming();
    oneway void doExplanation1();
    oneway void doExplanation2();
    oneway void doExplanation3();
    oneway void doExplanation4();
    oneway void doExplanationHead();
    oneway void doExplanationRightPC();
    oneway void doExplanationLeftPC();
    oneway void doExplanationInsidePC();
    oneway void doExplanationHDD();
    oneway void doExplanationSensors();
    bool checkMotionDone();
    bool stop();
}
