#ifndef ENISI_MSM_BACTERIA_COMPONENT_H
#define ENISI_MSM_BACTERIA_COMPONENT_H

#include "Bacteria.h"
#include "TcellGroup.h"
#include "Properties.h"

class BacteriaGroup: public AgentGroup {
public:
  enum State { DEAD, INFECTIOUS, TOLEGENIC, LAST_STATE_DO_NOT_MOVE};

  struct StateCount { unsigned int state[LAST_STATE_DO_NOT_MOVE]; };
  typedef std::map<repast::Point<int>, StateCount> CoordMap;

  BacteriaGroup(const boost::uintmax_t, ICompartmentLayer<ENISIAgent, AgentGroupPackage, AgentGroupPackageProvider, AgentGroupPackageReceiver> * p_layer);

  virtual void act();

  const std::vector<TcellGroup::StateCount> 
    getTcellNeighbors(const repast::Point<int> &);

  const StateCount &
    getCellsAt(const repast::Point<int> & loc) { return coordMap[loc]; }

  static std::vector<BacteriaGroup *> & instances()
  {
    static std::vector<BacteriaGroup *> instances;
    return instances;
  }

  virtual Color getColor() { return black; }
  virtual std::string classname() { return "BacteriaGroup"; }

protected:
  static std::vector<double> randomMove(const double &, const repast::Point<int> &);
private:
  void act(State, const repast::Point<int> &);
  void init(const boost::uintmax_t);
  CoordMap coordMap;
};

#endif