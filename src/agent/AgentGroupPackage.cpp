#include "AgentGroupPackage.h"
#include "agent/CellGroup.h"

/* Serializable Agent Package Data */

AgentGroupPackage::AgentGroupPackage(){ }

AgentGroupPackage::AgentGroupPackage(
    int _id, int _rank, int _type, int _currentRank, 
    std::string _classname, 
    std::map<int, std::vector<std::pair<int, int> > > _transfers
    ):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), 
  classname(_classname), transfers(_transfers)
{ }
AgentGroupPackageProvider::AgentGroupPackageProvider(repast::SharedContext<ENISIAgent>* agentPtr): agents(agentPtr){ }

void AgentGroupPackageProvider::providePackage(
    ENISIAgent * agent, std::vector<AgentGroupPackage>& out)
{
  repast::AgentId id = agent->getId();

  AgentGroupPackage package(
    id.id(), 
    id.startingRank(), 
    id.agentType(), 
    id.currentRank(), 
    agent->classname(),
    ((CellGroup*)agent)->getTransfers()
  );

  out.push_back(package);
}

void AgentGroupPackageProvider::provideContent(
    repast::AgentRequest req, std::vector<AgentGroupPackage>& out)
{
  std::vector<repast::AgentId> ids = req.requestedAgents();

  for(size_t i = 0; i < ids.size(); i++)
  {
    /* RepastProcess doesn't clear the AgentRequest when executing
     * requestAgents() multiple times. This leads to agent id "bleedover" from
     * other contexts, causing getAgent() to return null agents if not checked */
    if (agents->getAgent(ids[i]))
      providePackage(agents->getAgent(ids[i]), out);
  }
}


AgentGroupPackageReceiver::AgentGroupPackageReceiver(
    repast::SharedContext<ENISIAgent>* agentPtr): agents(agentPtr){}

ENISIAgent * 
AgentGroupPackageReceiver::createAgent(AgentGroupPackage package)
{
  repast::AgentId id(package.id, package.rank, package.type, package.currentRank);

  repast::Point<double> pt(0,0);
  repast::GridDimensions dims(pt, pt);

  CellLayer layer(dims);

  ENISIAgent * agent = new TransferGroup(&layer);

  agent->setId(id);

  return agent;
}

void AgentGroupPackageReceiver::updateAgent(AgentGroupPackage package)
{
  repast::AgentId id(package.id, package.rank, package.type);
  ENISIAgent * agent = agents->getAgent(id);
  agent->setId(id);
  ((CellGroup*)agent)->setTransfers(package.transfers);
}
