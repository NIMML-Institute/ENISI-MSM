#include "EpithelialCellGroup.h"

#include "grid/Borders.h"
#include "compartment/Compartment.h"

using namespace ENISI;

int p_EpiCellDeath;//Rule 11
int p_EpiProliferation;//Rule 8
int p_rule10a;
int p_rule10b;
int p_rule12;//Rule 12


EpithelialCellGroup::EpithelialCellGroup(Compartment * pCompartment, const size_t & count):
  mpCompartment(pCompartment)
{
  size_t LocalCount = mpCompartment->localCount(count);

  for (size_t i = 0; i < LocalCount; i++)
    {
      mpCompartment->addAgentToRandomLocation(new Agent(Agent::EpithelialCell, EpithelialCellState::HEALTHY));
    }
}

void EpithelialCellGroup::act()
{
  for (Iterator it = mpCompartment->begin(); it; it.next())
    {
      act(*it);
    }
}


void EpithelialCellGroup::act(const repast::Point<int> & pt)
{
  std::vector<double> Location(2, 0.0);

  std::vector< Agent * > EpithelialCells;
  mpCompartment->getAgents(pt, Agent::EpithelialCell, EpithelialCells);
  std::vector< Agent * >::iterator it = EpithelialCells.begin();
  std::vector< Agent * >::iterator end = EpithelialCells.end();

  // We only request information if we are at the border
  std::vector< Agent * > Bacteria;
  Concentration BacteriaConcentration;

  std::vector< Agent * > Tcells;
  Concentration TcellsCellConcentration;

  double IL10 = 0.0;

  if (mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::HIGH) < 1.0)
    {
      mpCompartment->getAgents(pt, 0, 1, Agent::Bacteria, Bacteria);
      concentrations(Agent::Bacteria, Bacteria, BacteriaConcentration);

      mpCompartment->getAgents(pt, 0, 1, Agent::Tcell, Tcells);
      concentrations(Agent::Tcell, Tcells, TcellsCellConcentration);

      IL10 = mpCompartment->cytokineValue("IL10", pt, 0, 1);
    }
  else if (mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::LOW) < 1.0)
    {
      mpCompartment->getAgents(pt, 0, -1, Agent::Bacteria, Bacteria);
      concentrations(Agent::Bacteria, Bacteria, BacteriaConcentration);
    }

  for (; it != end; ++it)
    {
      Agent * pAgent = *it;
      EpithelialCellState::State state = (EpithelialCellState::State) pAgent->getState();

      if (state == EpithelialCellState::DEAD) continue;

      EpithelialCellState::State newState = state;

      double infectiousBacteriaConcentration = BacteriaConcentration[BacteriaState::INFECTIOUS];
      double tolegenicBacteriaConcentration = BacteriaConcentration[BacteriaState::TOLEROGENIC];

      //Rules 9 and 10
      double th17Concentration = TcellsCellConcentration[TcellState::TH17]; //Rule 10 when Th17 is in contact
      double th1Concentration = TcellsCellConcentration[TcellState::TH1]; //RUle 9 when Th1 is in contact

      if (infectiousBacteriaConcentration && state == EpithelialCellState::HEALTHY && (p_rule10a > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()))
        {
          newState = EpithelialCellState::DAMAGED;
        }
      else if (tolegenicBacteriaConcentration && state == EpithelialCellState::HEALTHY)
        {
          newState = EpithelialCellState::HEALTHY;
        }
      else if ((th17Concentration || th1Concentration) && state == EpithelialCellState::HEALTHY
               && mpCompartment->getType() == Compartment::epithilium && (p_rule10b > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) // TODO CRITICAL This will never be true-FIXED
        {
          newState = EpithelialCellState::DAMAGED; /*Rule 10*/
          /* CHECK : Here there should be a function for information regarding the Layer,
          for eg. This rule requires the state transition when TH17 in LaminaPropria is in contact with E at 'Epithelium and LaminaPropria' membrane*/
        }
      if (p_EpiCellDeath > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())
        {
          mpCompartment->removeAgent(pAgent);/*Rule 11*/
          continue;
        }
      if (mpCompartment->getType() == Compartment::epithilium
          && (p_EpiProliferation > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()))
        {
          mpCompartment->getLocation(pAgent->getId(), Location); /*Rule 8*/
          mpCompartment->addAgent(new Agent(Agent::EpithelialCell, pAgent->getState()), Location);
        }
      else if (mpCompartment->getType() == Compartment::epithilium
               && state == EpithelialCellState::DAMAGED
               && IL10 > ENISI::Threshold
               && (p_rule12 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()))
        {
          newState = EpithelialCellState::HEALTHY;/* If IL10 is in contact with E at the Ep and Lm border, E-> Edamaged slowed donw by some factor*/
        }

      //else if (mpCompartment->getType() == Compartment::epithilium) // TODO CRITICAL This will always be true -FIXED
        //{
          // addCellAt(state, newLoc);/* Rule 8*/
          //mpCompartment->removeAgent(pAgent); /*Rule 11*/
        //}
      //else if
      if (newState == EpithelialCellState::DAMAGED)
        {
          mpCompartment->cytokineValue("IL6", pt) = 70;
          mpCompartment->cytokineValue("IL12", pt) = 70;
        }

      pAgent->setState(newState);

      // TODO CRITICAL Determine the maximum speed
      double MaxSpeed = 1.0;
      mpCompartment->moveRandom(pAgent->getId(), MaxSpeed);
    }
}
