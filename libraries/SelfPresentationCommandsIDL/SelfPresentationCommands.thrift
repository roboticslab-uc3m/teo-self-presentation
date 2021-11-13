namespace yarp roboticslab

service SelfPresentationCommandsIDL
{
    void doGreet();
    void doHoming();
    void doExplanation1();
    void doExplanation2();
    void doExplanation3();
    void doExplanationHead();
    void doExplanationRightPC();
    void doExplanationLeftPC();
    void doExplanationInsidePC();
    void doExplanationHDD();
    void doExplanationSensors();
    bool checkMotionDone();
    bool stop();
}
