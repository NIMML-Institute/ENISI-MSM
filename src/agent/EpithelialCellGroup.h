#ifndef ENISI_EpithelialCellGroup
#define ENISI_EpithelialCellGroup

#include "agent/AgentStates.h"
#include "repast_hpc/Point.h"
#include "agent/GroupInterface.h"

namespace ENISI
{

class EpithelialCellGroup: public GroupInterface
{
public:
  EpithelialCellGroup(Compartment * pCompartment, const double & concentrations);
  virtual void act();
  virtual std::string classname() const {return "EpithelialCellGroup";}

private:
  void act(const repast::Point<int> &);

  double p_EpiCellDeath;//Rule 11
  double p_EpiProliferation;//Rule 8
  double p_rule10a;
  double p_rule10b;
  double p_rule12;//Rule 12
};

} // namespace ENISI

#endif // ENISI_EpithelialCellGroup