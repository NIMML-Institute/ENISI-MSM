#include "MacrophageODE2.h"
#include "COPASI.h"

using namespace ENISI;

MacrophageODE2* MacrophageODE2::instance = NULL;

MacrophageODE2::MacrophageODE2() : DEBUG(false)
{
  std::string modelFileName("./modelfiles/MSM_CD4.cps");

  // create a new datamodel
  dataModel = COPASI::loadDataModel(modelFileName);

  if (dataModel == NULL)
    {
      std::cerr << "Error while loading the model from file named \""
                << modelFileName << "\"." << std::endl;

      std::exit(1);
    }

  model = dataModel->getModel();
  assert(model != NULL);

  if (DEBUG)
    std::cout << "Model statistics for model \""
              << model->getObjectName() << "\"." << std::endl;

  // output number and names of all compartments
  size_t i, iMax = model->getCompartments().size();

  if (DEBUG)
    {
      std::cout << "Number of Compartments: " << iMax << std::endl;
      std::cout << "Compartments: " << std::endl;
    }

  for (i = 0; i < iMax; ++i)
    {
      CCompartment* pCompartment = model->getCompartments()[i];
      assert(pCompartment != NULL);

      if (DEBUG)
        std::cout << "\t" << pCompartment->getObjectName() << std::endl;
    }

  // output number and names of all metabolites
  iMax = model->getMetabolites().size();

  if (DEBUG)
    {
      std::cout << "Number of Metabolites: " << iMax << std::endl;
      std::cout << "Metabolites: " << std::endl;
    }

  for (i = 0; i < iMax; ++i)
    {
      CMetab* metab = model->getMetabolites()[i];
      assert(metab != NULL);

      nameMetabs[metab->getObjectName()] = metab;

      if (DEBUG)
        std::cout << "\t" << metab->getObjectName() << "\t"
                  << metab->getInitialConcentration() << "\t"
                  << metab->getInitialValue() << std::endl;
    }

  // output number and names of all reactions
  iMax = model->getReactions().size();

  if (DEBUG)
    {
      std::cout << "Number of Reactions: " << iMax << std::endl;
      std::cout << "Reactions: " << std::endl;
    }

  for (i = 0; i < iMax; ++i)
    {
      CReaction* pReaction = model->getReactions()[i];
      assert(pReaction != NULL);

      if (DEBUG)
        std::cout << "\t" << pReaction->getObjectName() << std::endl;
    }

  setUpReport();
  setUpTask();
}

void MacrophageODE2::setInitialConcentration(std::string name, double value)
{
  if (nameMetabs.find(name) != nameMetabs.end())
    {
      CMetab* m = nameMetabs[name];
      m->setInitialConcentration(value);
      m->refreshInitialValue();
    }
  else
    {
      std::cout << name << "\t does not exist as a metab" << std::endl;
    }
}

void MacrophageODE2::setUpReport()
{
  // create a report with the correct filename and all the species against time.
  CReportDefinitionVector* reports = dataModel->getReportDefinitionList();
  // create a new report definition object
  report = reports->createReportDefinition("Report", "Output for timecourse");
  // set the task type for the report definition to timecourse
  report->setTaskType(CCopasiTask::timeCourse);
  // we don't want a table
  report->setIsTable(false);
  // the entries in the output should be seperated by a ", "
  report->setSeparator(CCopasiReportSeparator(", "));

  // we need a handle to the header and the body
  // the header will display the ids of the metabolites and "time" for
  // the first column
  // the body will contain the actual timecourse data
  ReportItemVector header = report->getHeaderAddr();
  ReportItemVector body = report->getBodyAddr();

  body->push_back(CCopasiObjectName(model->getCN() + ",Reference=Time"));
  body->push_back(CRegisteredObjectName(report->getSeparator().getCN()));
  header->push_back(CCopasiStaticString("time").getCN());
  header->push_back(report->getSeparator().getCN());

  size_t i, iMax = model->getMetabolites().size();

  for (i = 0; i < iMax; ++i)
    {
      CMetab* pMetab = model->getMetabolites()[i];
      assert(pMetab != NULL);

      // we don't want output for FIXED metabolites right now
      if (pMetab->getStatus() != CModelEntity::FIXED)
        {
          // we want the concentration in the output
          // alternatively, we could use "Reference=Amount" to get the
          // particle number

          CCopasiObjectName referenceEqConcentration =
            pMetab->getObject(CCopasiObjectName("Reference=Concentration"))->getCN();

          body->push_back(referenceEqConcentration);
          // after each entry, we need a seperator
          body->push_back(report->getSeparator().getCN());

          // add the corresponding id to the header
          header->push_back(CCopasiStaticString(pMetab->getSBMLId()).getCN());
          // and a seperator
          header->push_back(report->getSeparator().getCN());
        }
    }

  if (iMax > 0)
    {
      // delete the last separator
      // since we don't need one after the last element on each line
      if ((*body->rbegin()) == report->getSeparator().getCN())
        {
          body->erase(--body->end());
        }

      if ((*header->rbegin()) == report->getSeparator().getCN())
        {
          header->erase(--header->end());
        }
    }
}

void MacrophageODE2::setUpTask()
{
  // get the task list
  CCopasiVectorN< CCopasiTask > & TaskList = * dataModel->getTaskList();

  // get the trajectory task object
  trajectoryTask = dynamic_cast<CTrajectoryTask*>(TaskList["Time-Course"]);

  // if there isn't one
  if (trajectoryTask == NULL)
    {
      // create a new one
      trajectoryTask = new CTrajectoryTask();
      // remove any existing trajectory task just to be sure since in
      // theory only the cast might have failed above
      TaskList.remove("Time-Course");

      // add the new time course task to the task list
      TaskList.add(trajectoryTask, true);
    }

  // run a deterministic time course
  trajectoryTask->setMethodType(CCopasiMethod::deterministic);

  // pass a pointer of the model to the problem
  trajectoryTask->getProblem()->setModel(dataModel->getModel());

  // activate the task so that it will be run when the model is saved
  // and passed to CopasiSE
  trajectoryTask->setScheduled(true);

  // get the problem for the task to set some parameters
  CTrajectoryProblem* pProblem =
    dynamic_cast<CTrajectoryProblem*>(trajectoryTask->getProblem());

  // simulate 600 steps
  pProblem->setStepNumber(600);
  // start at time 0
  dataModel->getModel()->setInitialTime(0.0);
  // simulate a duration of 60 time units
  pProblem->setDuration(60);
  // tell the problem to actually generate time series data
  pProblem->setTimeSeriesRequested(true);

  // set some parameters for the LSODA method through the method
  CTrajectoryMethod* pMethod =
    dynamic_cast<CTrajectoryMethod*>(trajectoryTask->getMethod());

  CCopasiParameter* pParameter = pMethod->getParameter("Absolute Tolerance");
  assert(pParameter != NULL);
  pParameter->setValue(1.0e-12);
}

void MacrophageODE2::runTimeCourse()
{
  model->applyInitialValues();

  bool result = true;

  try
    {
      result = trajectoryTask->process(true);
    }
  catch (...)
    {
      std::cout << "Error. Running the time course simulation failed.\n";

      // check if there are additional error messages
      if (CCopasiMessage::size() > 0)
        {
          // print the messages in chronological order
          std::cerr << CCopasiMessage::getAllMessageText(true) << "\n";
        }

      // std::exit(1);
    }

  if (result == false)
    {
      std::cerr << "An error occured while running the time course simulation.\n";

      // check if there are additional error messages
      if (CCopasiMessage::size() > 0)
        {
          // print the messages in chronological order
          std::cerr << CCopasiMessage::getAllMessageText(true) << "\n";
        }

      // std::exit(1);
    }
}

double MacrophageODE2::getConcentration(std::string name)
{

  double d = 0.0;

  if (nameMetabs.find(name) != nameMetabs.end())
    {
      CMetab* m = nameMetabs[name];
      d = m->getConcentration();
    }
  else
    {
      if (DEBUG)
        std::cout << name << "\t does not exist as a metab" << std::endl;
    }

  return d;
}
