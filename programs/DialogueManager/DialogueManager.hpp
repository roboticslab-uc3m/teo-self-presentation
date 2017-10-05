// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __FM_DIALOGUE_MANAGER_HPP__
#define __FM_DIALOGUE_MANAGER_HPP__

#include <yarp/os/all.h>
#include <stdlib.h>

#include "ScriptManager.hpp"

#define DEFAULT_LANGUAGE "spanish"

namespace teo
{

/**
 * @ingroup teo-self-presentation_programs
 *
 * @brief Dialogue Manager.
 */
class DialogueManager : public yarp::os::RFModule {
  private:
    ScriptManager scriptManager;
    yarp::os::RpcClient outTtsPort; // tts port
    yarp::os::RpcClient outCmdMovementsPort; // robot movements

    bool interruptModule();
    double getPeriod();
    bool updateModule();

  public:
    bool configure(yarp::os::ResourceFinder &rf);
    bool close();

};

}  // namespace teo

#endif  // __FM_DIALOGUE_MANAGER_HPP__
