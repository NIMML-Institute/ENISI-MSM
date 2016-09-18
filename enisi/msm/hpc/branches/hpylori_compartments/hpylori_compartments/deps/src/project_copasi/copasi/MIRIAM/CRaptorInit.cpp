// Copyright (C) 2010 - 2015 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Copyright (C) 2008 - 2009 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., EML Research, gGmbH, University of Heidelberg,
// and The University of Manchester.
// All rights reserved.

#include <raptor.h>
#include <string.h>
#include <stdlib.h>

#include "copasi.h"

#include "CRaptorInit.h"

// static
bool CRaptorInit::Initialized = false;

CRaptorInit::CRaptorInit()
{
  if (!Initialized)
    {
      raptor_init();
      Initialized = true;
      atexit(&raptor_finish);
    }
}

CRaptorInit::~CRaptorInit()
{}

// static
bool CRaptorInit::isLocalURI(raptor_uri * pURI)
{
  raptor_uri * pTmp =
    raptor_new_uri_for_retrieval(pURI);

  bool isLocal =
    (strcmp("/", (char *) raptor_uri_as_string(pTmp)) == 0);

  pRaptorFreeUri(pTmp);

  return isLocal;
}
