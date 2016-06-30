#include "TcellGroup.h"
#include "agent/ENISIAgent.h"
#include "compartment/Compartment.h"
#include "grid/Properties.h"
#include "DataWriter/LocalFile.h"
#include "agent/TcellODE.h"
#include "grid/Borders.h"

using namespace ENISI;

TcellGroup::TcellGroup(Compartment * pCompartment, const double & concentrations) :
  GroupInterface(pCompartment)
{
  size_t LocalCount = mpCompartment->localCount(concentrations);

  for (size_t i = 0; i < LocalCount; i++)
    {
      mpCompartment->addAgentToRandomLocation(new Agent(Agent::Tcell, TcellState::NAIVE));
    }
  const Properties * pModel = Properties::instance(Properties::model);

  pModel->getValue("p_rule18", p_rule18);
  pModel->getValue("p_rule19", p_rule19);
  pModel->getValue("p_rule20", p_rule20);
  pModel->getValue("p_rule21", p_rule21);
  pModel->getValue("p_rule22", p_rule22);
  pModel->getValue("p_rule23", p_rule23);
  pModel->getValue("p_rule24", p_rule24);
  pModel->getValue("p_rule26", p_rule26);
  pModel->getValue("p_rule27", p_rule27);
  pModel->getValue("p_rule31", p_rule31);
  pModel->getValue("p_rule31a",p_rule31a);
  pModel->getValue("p_rule32", p_rule32);
  pModel->getValue("p_rule33", p_rule33);
  pModel->getValue("p_rule35", p_rule35);
  pModel->getValue("p_rule36", p_rule36);
  pModel->getValue("p_rule37", p_rule37);
  pModel->getValue("p_rule38", p_rule38);
  pModel->getValue("p_rule39", p_rule39);
  pModel->getValue("p_rule40", p_rule40);
  pModel->getValue("p_rule41", p_rule41);
  pModel->getValue("p_rule53", p_rule53);
  pModel->getValue("p_rule55a", p_rule55a);
  pModel->getValue("p_rule55b", p_rule55b);
  pModel->getValue("neardistane_border", neardistane_border);

  pModel->getValue("p_IL17", p_IL17);
  pModel->getValue("p_IFNg", p_IFNg);
  pModel->getValue("p_IL10", p_IL10);
}

void TcellGroup::act(const repast::Point<int> & pt)
{
	//LocalFile::debug() << "I am in Tcells act()" << std::endl;
	std::vector< double > Location(2, 0.0);

	std::vector< Agent * > Tcells;
	mpCompartment->getAgents(pt, Agent::Tcell, Tcells);

	Concentration TcellConcentration;
	concentrations(Agent::Tcell, Tcells, TcellConcentration);
	mpCompartment->getAgents(pt, Agent::Tcell, Tcells);


	std::vector< Agent * > Macrophages;
	mpCompartment->getAgents(pt, Agent::Macrophage, Macrophages);
	Concentration MacrophageConcentration;

	std::vector< Agent * > Dentritics;
	mpCompartment->getAgents(pt, Agent::Dentritics, Dentritics);
	Concentration DentriticsConcentration;
	concentrations(Agent::Dentritics, Dentritics, DentriticsConcentration);

	std::vector< Agent * > EpithelialCells; // We only request information if we are at the border
	Concentration EpithelialCellConcentration;
	mpCompartment->getAgents(pt, Agent::EpithelialCell, EpithelialCells);

	if (mpCompartment->getType() == Compartment::lamina_propria) {
		mpCompartment->getAgents(pt, Agent::Macrophage, Macrophages);
		mpCompartment->getAgents(pt, Agent::Dentritics, Dentritics);
		mpCompartment->getAgents(pt, Agent::Tcell, Tcells);
	}
	else if (mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::HIGH) < 1.5) {
		//mpCompartment->getAgents(pt, 0, 1, Agent::EpithelialCell, EpithelialCells);
		mpCompartment->getAgents(pt, 0, 2, Agent::Dentritics, Dentritics);
	}
	else if (
			mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::LOW) < 1.5) {
		mpCompartment->getAgents(pt, 0, -2, Agent::EpithelialCell, EpithelialCells);
		mpCompartment->getAgents(pt, 0, -2, Agent::Dentritics, Dentritics);
	}
	else if (mpCompartment->getType() == Compartment::gastric_lymph_node){
		mpCompartment->getAgents(pt, Agent::Dentritics, Dentritics);
	}
	/*else if (mpCompartment->getType() == Compartment::gastric_lymph_node &&
			mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::LOW) < 1.5) {
		mpCompartment->getAgents(pt, 0, -2, Agent::Dentritics, Dentritics);
	}*/
	concentrations(Agent::EpithelialCell, EpithelialCells, EpithelialCellConcentration);

	concentrations(Agent::Macrophage, Macrophages, MacrophageConcentration);

	double IL6_pool = mpCompartment->cytokineValue("eIL6", pt);
	double TGFb_pool = mpCompartment->cytokineValue("eTGFb", pt);
	double IL12_pool = mpCompartment->cytokineValue("eIL12", pt);

	TcellODE & odeModel = TcellODE::getInstance();

	odeModel.setInitialConcentration("IL6_pool", IL6_pool);
	odeModel.setInitialConcentration("TGFb_pool", TGFb_pool);
	odeModel.setInitialConcentration("IL12_pool", IL12_pool);

	/* run time course */
	if (!odeModel.runTimeCourse()){
	LocalFile::debug() << pt << std::endl;
	}

	double dIFNg = odeModel.getConcentration("dIFNg");
	double dIL17 = odeModel.getConcentration("dIL17");
	double dIL10 = odeModel.getConcentration("dIL10");

  /*LocalFile::debug() << "dIFNg = " << dIFNg << std::endl;
  LocalFile::debug() << "dIL17 = " << dIL17 << std::endl;
  LocalFile::debug() << "dIL10 = " << dIL10 << std::endl;*/

	double macrophageregConcentration = MacrophageConcentration[MacrophageState::REGULATORY];
	double th17Concentration = TcellConcentration[TcellState::TH17]; //Rules 22, 23, 36-39 when Th17 is in contact
	double itregConcentration = TcellConcentration[TcellState::iTREG]; //Rules 19-21 when iTreg is in contact
	double th1Concentration = TcellConcentration[TcellState::TH1];
	double eDCConcentration = DentriticsConcentration[DendriticState::EFFECTOR]; //Rule 39 eDC count that is in contact with nT
	double tDCConcentration = DentriticsConcentration[DendriticState::TOLEROGENIC]; //Rule 23 tDC count
	double damagedEpithelialCellConcentration = EpithelialCellConcentration[EpithelialCellState::DAMAGED];// Rule 18 damagedEpithelialCellConcentration
	//double immatureDentritics = DentriticsConcentration[DendriticState::IMMATURE];

	//double damagedEpithelialCellConcentration = 1000;
	//LocalFile::debug() << "liveHPyloriConcentration=          " << liveHPyloriConcentration << std::endl;
			/* LocalFile::debug() << "eDCConcentration			      =" << eDCConcentration << std::endl;
			 LocalFile::debug() << "tDCConcentration				  =" << tDCConcentration << std::endl;
			 LocalFile::debug() << "damagedEpithelialCellConcentration=" << damagedEpithelialCellConcentration << std::endl;
			 LocalFile::debug() << "macrophageregConcentration		  =" << macrophageregConcentration << std::endl;*/
			 //LocalFile::debug() << "th17Concentration			  	  =" << th17Concentration << std::endl;
			 //LocalFile::debug() << "th1Concentration			      =" << th1Concentration << std::endl;
	//LocalFile::debug() << "immatureDentritics		      =" << immatureDentritics << std::endl;
	//		  LocalFile::debug() << "macrophageinfConcentration=        " << macrophageinfConcentration << std::endl;
	//		  LocalFile::debug() << "infectiousBacteriaConcentration=   " << infectiousBacteriaConcentration << std::endl << std::endl;

	std::vector< Agent * >::iterator it = Tcells.begin();
	std::vector< Agent * >::iterator end = Tcells.end();

	for (; it != end; ++it){
		Agent * pAgent = *it;
		TcellState::State state = (TcellState::State) pAgent->getState();

		if (state == TcellState::DEAD) continue;

		TcellState::State newState = state;

		/*if (state != TcellState::Tr) //Rule 58
			{
			//LocalFile::debug() << "I am here 00" << std::endl;
			if (dIL17 > p_IL17) {
				newState = TcellState::TH17;
				mpCompartment->cytokineValue("eIL17", pt) += 70;
				LocalFile::debug() << "I am here 01" << std::endl;
			}
			else if (dIFNg > p_IFNg) {
				newState = TcellState::TH1;
				mpCompartment->cytokineValue("eIFNg", pt) += 70;
				LocalFile::debug() << "I am here 02" << std::endl;
			}
			else if (dIL10 > p_IL10) {
				newState = TcellState::iTREG;
				mpCompartment->cytokineValue("eIL10", pt) += 70;
				LocalFile::debug() << "I am here 03" << std::endl;
			}
		}*/
		if (mpCompartment->getType() == Compartment::lamina_propria){
			if (p_rule41 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()) {
				mpCompartment->getLocation(pAgent->getId(), Location);
				mpCompartment->addAgent(new Agent(Agent::Tcell, pAgent->getState()), Location);
				//LocalFile::debug() << "I am here 04" << std::endl;
				continue;
				// TODO CRITICAL Proliferation can always happen it is not condition dependent - FIXED
				// addCellAt(TcellState::NAIVE, loc); /*Rule 41* - nT can 'proliferate' when in contact with nT in Propria */
			}
			if (state == TcellState::NAIVE) {
				if ((mpCompartment->cytokineValue("eIL10", pt) > p_rule31a * mpCompartment->cytokineValue("eIFNg", pt))
				   && (macrophageregConcentration > ENISI::Threshold)
				   && (p_rule31 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
					newState = TcellState::Tr; //Rule 31: if nT is in contact with regulatory macrophages, and if IL10> a* IFNg, then nT -> Tr
					LocalFile::debug() << "I am here 0" << std::endl;
				}
			}
			else if (state == TcellState::iTREG){
				if ((th1Concentration > ENISI::Threshold)
				   && (p_rule19 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()) && (dIL10 > p_IL10)) {
					newState = TcellState::TH17; /*Rule 19*/
					pAgent->setState(newState);
					LocalFile::debug() << "I am here 3" << std::endl;
				}
				else if (eDCConcentration > ENISI::Threshold
						&& (p_rule20 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
					//LocalFile::debug() << "eDC changed iTREG to Th17" << std::endl;
					newState = TcellState::TH17; /*Rule 20*/
					pAgent->setState(newState);
					LocalFile::debug() << "I am here 6" << std::endl;
				}
				else if (th17Concentration > ENISI::Threshold
						&& (p_rule21 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
					newState = TcellState::TH17; /*Rule 22*/
					pAgent->setState(newState);
					LocalFile::debug() << "I am here 4" << std::endl;
				}
				else if (p_rule26 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()) {
					mpCompartment->removeAgent(pAgent); /*Rule 26*- iTREG can die in LP*/
					continue;
				}
			}
			else if (state == TcellState::TH17) {
				if (p_rule24 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()) {
					mpCompartment->removeAgent(pAgent); /*Rule 24*- TH17 can die in LP*/
					continue;
				}
				else if ((tDCConcentration  > ENISI::Threshold)
					   && (p_rule23 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
					newState = TcellState::iTREG; /*Rule 23*/
					pAgent->setState(newState);
					//LocalFile::debug() << "I am here 5" << std::endl;
				}
			}
			else if (state == TcellState::TH1){
				if (itregConcentration > ENISI::Threshold
						&& (p_rule22 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
					newState = TcellState::iTREG; /*Rule 22*/
					pAgent->setState(newState);
					//LocalFile::debug() << "I am here 4" << std::endl;
				}
				else if (p_rule27 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()) {
					mpCompartment->removeAgent(pAgent); /*Rule 27*- Th1 can die in LP*/
					continue;
				}
			}
			else if ((state == TcellState::iTREG)
					&& (damagedEpithelialCellConcentration  > ENISI::Threshold)
					&& (p_rule18> repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
				newState = TcellState::TH17; /*Rule 18* - //This rule requires the state transition when iTREG is in contact with Edamaged at 'Epithelium and LaminaPropria' membrane*/
				pAgent->setState(newState);
				LocalFile::debug() << "I am here 13" << std::endl;
			}
		}//END of LP
		else if (mpCompartment->getType() == Compartment::gastric_lymph_node) {
			if (p_rule40 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()) {
				mpCompartment->getLocation(pAgent->getId(), Location);
				mpCompartment->addAgent(new Agent(Agent::Tcell, pAgent->getState()), Location);
				continue;
			}
			else if (eDCConcentration > ENISI::Threshold) {
				if(p_rule55a > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next() && (dIL17 > p_IL17)) {
					LocalFile::debug() << "eDC changed state to Th17 cell" << std::endl;
					newState = TcellState::TH17;
					pAgent->setState(newState);
					mpCompartment->cytokineValue("eIL17", pt) += dIL17;
				}
				else if (p_rule55b > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next() && (dIFNg > p_IFNg)) {
					newState = TcellState::TH1;
					pAgent->setState(newState);
					mpCompartment->cytokineValue("eIFNg", pt) += dIFNg;
					LocalFile::debug() << "eDC changed state to Th1 cell" << std::endl;
				}
			}
			else if (state == TcellState::TH17){
				if  ((tDCConcentration > ENISI::Threshold)
					  && (p_rule36 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
					newState = TcellState::iTREG; /*Rule 36*/
					pAgent->setState(newState);
					LocalFile::debug() << "I am here 7" << std::endl;
				}
				else if ((itregConcentration > ENISI::Threshold)
						 && (p_rule35 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
					newState = TcellState::iTREG; /*Rule 35*/
					pAgent->setState(newState);
					LocalFile::debug() << "I am here 8" << std::endl;
				}
			}
			else if (state == TcellState::iTREG) {
				if ((mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::LOW))< mpCompartment->spaceToGrid(Borders::Y, 1.5) //TODO - CRITICAL Determine this value
					 && (p_rule33 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {/*Rule 33*/
					std::vector<double> Location;
					mpCompartment->getLocation(pAgent->getId(), Location);
					Location[Borders::Y] -= 1.01 * mpCompartment->spaceBorders()->distanceFromBorder(Location, Borders::Y, Borders::LOW);
					mpCompartment->moveTo(pAgent->getId(), Location);
					continue;
				}
				else if ((th17Concentration > ENISI::Threshold)
						 && (p_rule37 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
				   newState = TcellState::TH17; /*Rule 37*/
				   pAgent->setState(newState);
				   LocalFile::debug() << "I am here 9" << std::endl;
				}
				else if((th1Concentration > ENISI::Threshold)
					  && (p_rule38 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
					newState = TcellState::TH17;/*Rule 38*//*When iTREG is in contact with TH1 in GLN, iTREG changes to TH17*/
					pAgent->setState(newState);
					LocalFile::debug() << "I am here 10" << std::endl;
				}
				else if(state == TcellState::NAIVE) {
					if ((eDCConcentration  > ENISI::Threshold)
					   && (p_rule39 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
						newState = TcellState::TH1; /*Rule 39*/
						pAgent->setState(newState);
						LocalFile::debug() << "I am here 11" << std::endl;
					}
					else if ((tDCConcentration > ENISI::Threshold)
						   && (p_rule53 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next())) {
						newState = TcellState::iTREG; /*Rule 53*/
						pAgent->setState(newState);
						LocalFile::debug() << "I am here 12" << std::endl;
					}
				}
				else if (state == TcellState::TH1) {
					if (mpCompartment->gridBorders()->distanceFromBorder(pt.coords(), Borders::Y, Borders::LOW) < mpCompartment->spaceToGrid(Borders::Y, 1.5)
					  && (p_rule32 > repast::Random::instance()->createUniDoubleGenerator(0.0, 1.0).next()))/*Rule 32*/ {
						std::vector<double> Location;
						mpCompartment->getLocation(pAgent->getId(), Location);
						Location[Borders::Y] -= 1.01 * mpCompartment->spaceBorders()->distanceFromBorder(Location, Borders::Y, Borders::LOW);
						mpCompartment->moveTo(pAgent->getId(), Location);
						continue;
					}
				}
			}
		} // end of the Compartment Type = Compartment::GLN

		pAgent->setState(newState);
		if (newState == TcellState::TH1) { //Rule 29 If T cell state is TH1, then release IFNg
			mpCompartment->cytokineValue("eIFNg", pt) += dIFNg; // production based on ODE for IFNg
		}
		else if (newState == TcellState::Tr || newState == TcellState::iTREG) {
			//Rule 30, If T cell state is Tr then release IL10 [or iTREG]
			mpCompartment->cytokineValue("eIL10", pt) += dIL10; // production based on ODE for IL10
		}
	}
}
// virtual
void TcellGroup::move()
{
  // TODO CRITICAL Determine the maximum speed
  double MaxSpeed = 1.0;
  // Find all local agents and move them
  Compartment::LocalIterator itLocal = mpCompartment->localBegin();
  Compartment::LocalIterator endLocal = mpCompartment->localEnd();

  for (; itLocal != endLocal; ++itLocal)
	{
      mpCompartment->moveRandom((*itLocal)->getId(), MaxSpeed);
	}
}

// virtual
void TcellGroup::write(const repast::Point<int> & pt)
{
  std::ostream & o = LocalFile::instance(mpCompartment->getName())->stream();

  std::vector< Agent * > Tcells;
  mpCompartment->getAgents(pt, Agent::Tcell, Tcells);
  Concentration TcellConcentration;
  concentrations(Agent::Tcell, Tcells, TcellConcentration);

  Concentration::const_iterator it = TcellConcentration.begin();
  Concentration::const_iterator end = TcellConcentration.end();

  for (int i = 0; it != end; ++it, ++i)
    {
	  if (i) o << ", ";

	  o << "[" << i << "] = " << *it;
  }
}
