#include "HPyloriGroup.h"

#include "grid/Borders.h"
#include "compartment/Compartment.h"

using namespace ENISI;

int p_rule3;
int p_rule4a;
int p_rule4b;
int p_rule5;
int p_HPyloriDeath;

HPyloriGroup::HPyloriGroup(Compartment * pCompartment, const size_t & count):
  mpCompartment(pCompartment)
{
  size_t LocalCount = mpCompartment->localCount(count);

  for (size_t i = 0; i < LocalCount; i++)
    {
      mpCompartment->addAgentToRandomLocation(new Agent(Agent::HPylori, HPyloriState::NAIVE));
    }
}

void HPyloriGroup::act()
{
  for (Iterator it = mpCompartment->begin(); it; it.next())
    {
      act(*it);
    }
}

void HPyloriGroup::act(const repast::Point<int> & pt)
{
  std::vector< double > Location(2, 0);

  std::vector< Agent * > HPylori;
  mpCompartment->getAgents(pt, Agent::HPylori, HPylori);
  std::vector< Agent * >::iterator it = HPylori.begin();
  std::vector< Agent * >::iterator end = HPylori.end();

  std::vector< Agent * > Tcells;
  mpCompartment->getAgents(pt, Agent::Tcell, Tcells);

  Concentration TcellConcentration;
  concentrations(Agent::Tcell, Tcells, TcellConcentration);

  // We only request information if we are at the border
  std::vector< Agent * > EpithelialCells;
  Concentration EpithelialCellConcentration;

  if (mpCompartment->getType() == Compartment::lumen &&
      mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::HIGH) < 1.0)
    {
      mpCompartment->getAgents(pt, 0, 1, Agent::EpithelialCell, EpithelialCells);
      concentrations(Agent::EpithelialCell, EpithelialCells, EpithelialCellConcentration);
    }
  else if (mpCompartment->getType() == Compartment::lamina_propria &&
           mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::LOW) < 1.0)
    {
      mpCompartment->getAgents(pt, 0, -1, Agent::EpithelialCell, EpithelialCells);
      concentrations(Agent::EpithelialCell, EpithelialCells, EpithelialCellConcentration);
    }

  for (; it != end; ++it)
    {
      Agent * pAgent = *it;
      HPyloriState::State state = (HPyloriState::State) pAgent->getState();

      if (state == HPyloriState::DEAD)
        continue;

      /*identify states of Epithelial Cells counted */
      double damagedEpithelialCellConcentration = EpithelialCellConcentration[EpithelialCellState::DAMAGED];

      /* move HPylori across epithelial border if in contact with damaged Epithelial cell *Rule 3*/
      if ((p_rule3 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()) && damagedEpithelialCellConcentration && mpCompartment->getType() == Compartment::lumen)
        {
          std::vector< double > Location;
          mpCompartment->getLocation(pAgent->getId(), Location);
          Location[Borders::Y] +=
              1.01 * mpCompartment->spaceBorders()->distanceFromBorder(Location, Borders::Y, Borders::HIGH) // Move accross the border
              + Compartment::instance(Compartment::epithilium)->dimensions().extents(Borders::Y); // Move all the way through the epithelium

          mpCompartment->moveTo(pAgent->getId(), Location);
        }

      double th1Concentration = TcellConcentration[TcellState::TH1];
      double th17Concentration = TcellConcentration[TcellState::TH17];

      /* HPylori dies is nearby damaged epithelial cell, th1 or th17* *Rule 5,6,7*/
      if ((damagedEpithelialCellConcentration || th1Concentration || th17Concentration) &&
          (p_rule5 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()))
        {
          // newState = HPyloriState::DEAD;
          mpCompartment->removeAgent(pAgent);
        }
      if ((p_HPyloriDeath > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()))
        {
          mpCompartment->removeAgent(pAgent);
          continue;
        }

      if (mpCompartment->getType() == Compartment::lamina_propria && (p_rule4a > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()))
        {
          mpCompartment->getLocation(pAgent->getId(), Location);
          mpCompartment->addAgent(new Agent(Agent::HPylori, pAgent->getState()), Location);
        }

      if (mpCompartment->getType() == Compartment::lumen && (p_rule4b > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()))
        {
          mpCompartment->getLocation(pAgent->getId(), Location);
          mpCompartment->addAgent(new Agent(Agent::HPylori, pAgent->getState()), Location);
        }

      /* TODO: H Pylori are removed when macrophage uptake/differentiate */

      // TODO CRITICAL Determine the maximum speed
      double MaxSpeed = 1.0;
      mpCompartment->moveRandom(pAgent->getId(), MaxSpeed);
    }
}
