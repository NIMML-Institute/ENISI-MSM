#ifndef ENISI_MSM_AGENT_BACTERIA_H
#define ENISI_MSM_AGENT_BACTERIA_H

#include <stdexcept>
#include <vector>
#include <math.h>
#include "ENISIAgent.h"
#include "Tcell.h"
#include "repast_hpc/SharedContinuousSpace.h"
#include "repast_hpc/GridComponents.h" // repast::StickyBorders repast::SimpleAdder
#include "repast_hpc/Random.h" // repast::Random

class Bacteria: public ENISI::Agent
{
public:
  Bacteria(ENISI::CellLayer * p_layer) : _p_layer(p_layer)
  {
    setState(ENISI::AgentState::INFECTIOUS);
  }

  Bacteria(ENISI::CellLayer * p_layer, ENISI::AgentState::State st) : _p_layer(p_layer)
  {
    setState(st);
  }

  virtual Color getColor();
  virtual std::string classname() {return "Bacteria";}
  virtual void act();

  void move(double x, double y);
  double randomMove(double = 1);
  std::vector<double> getLocation();
  std::vector<ENISI::Agent *> getNeighbors(const std::string);

private:
  ENISI::CellLayer * _p_layer;
};

#endif