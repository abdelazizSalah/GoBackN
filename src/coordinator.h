#ifndef __NETWORKS_PROJECT_COORDINATOR_H_
#define __NETWORKS_PROJECT_COORDINATOR_H_

#include <omnetpp.h>
#include "defs.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Coordinator : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif
