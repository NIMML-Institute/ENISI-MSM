#ifndef ENISI_MSM_AGENT_TCELLS_H
#define ENISI_MSM_AGENT_TCELLS_H

#include <string>
#include <map>
#include "ImmuneCell.h"
#include "TcellODE.h"
#include "../compartment/Compartment.h"

class Tcell: public ImmuneCell 
{
public:
  Tcell(ICompartmentLayer<ENISIAgent, AgentGroupPackage, AgentGroupPackageProvider, AgentGroupPackageReceiver> * p_layer) 
    : ImmuneCell(p_layer) { setState(AgentState::NAIVE); }

  Tcell(ICompartmentLayer<ENISIAgent, AgentGroupPackage, AgentGroupPackageProvider, AgentGroupPackageReceiver> * p_layer, AgentState::State st) 
    : ImmuneCell(p_layer) { setState(st); }

  virtual Color getColor();
  virtual std::string classname() { return "Tcell"; }

  virtual void act();
};

#endif