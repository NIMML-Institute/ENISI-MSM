// Begin CVS Header 
//   $Source: /fs/turing/cvs/copasi_dev/copasi/bindings/swig/CMoiety.i,v $ 
//   $Revision: 1.7 $ 
//   $Name:  $ 
//   $Author: shoops $ 
//   $Date: 2009/01/07 18:51:30 $ 
// End CVS Header 

// Copyright (C) 2008 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc., EML Research, gGmbH, University of Heidelberg, 
// and The University of Manchester. 
// All rights reserved. 

// Copyright (C) 2001 - 2007 by Pedro Mendes, Virginia Tech Intellectual 
// Properties, Inc. and EML Research, gGmbH. 
// All rights reserved. 

// Copyright © 2005 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

%{

#include "model/CMoiety.h"  
  
%}

%nodefaultctor CMoiety;
%nodefaultdtor CMoiety;

%include "model/CMoiety.h"

/**
 * Actually CMoiety now has a method to refresh the dependent number and return
 * it, it is called dependentNumber().
 * getDependentNumber will only get the number without refresh.
%extend CMoiety
{
  
  C_FLOAT64 getDependentNumber() const
  {
    self->refreshDependentNumber();
    return self->getDependentNumber();
  }  

}
 */  




