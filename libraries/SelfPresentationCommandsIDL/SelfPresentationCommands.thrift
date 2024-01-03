namespace yarp roboticslab

service SelfPresentationCommands
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
    oneway void doExplanationSensors();
    bool checkMotionDone();
    bool stop();
}
