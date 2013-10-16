/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * Authors:                                                                  *
 *   JvL, Jeroen van Leerdam, Nikhef, j.van.leerdam@nikhef.nl                *
 *                                                                           *
 * Copyright (c) 2013, Nikhef. All rights reserved.                          *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/ 

#include "RooAbsReal.h" 
#include "RooArgList.h" 
#include "RooMsgService.h"
#include "P2VV/RooConvertPolAmp.h" 
#include "Riostream.h"

#include <math.h>
#include <iostream>
using std::endl;

//_____________________________________________________________________________
RooConvertPolAmp::RooConvertPolAmp() :
  RooAbsReal(),
  _type(FracToMag),
  _args("arguments", "arguments", this),
  _vals(0)
{}

//_____________________________________________________________________________
RooConvertPolAmp::RooConvertPolAmp(const char* name, const char* title,
    EvalType type, const RooArgList& arguments) :
  RooAbsReal(name, title),
  _type(type),
  _args("arguments", "arguments", this),
  _vals(0)
{
  // check if arguments are RooAbsReals
  RooFIter coefIter = arguments.fwdIterator();
  RooAbsArg* coef = 0;
  while ((coef = coefIter.next()) != 0) {
    if (dynamic_cast<RooAbsReal*>(coef) == 0) {
      coutE(InputArguments) << "RooConvertPolAmp::RooConvertPolAmp("
          << GetName() << ") omitting coefficient \"" << coef->GetName()
          << "\": not a RooAbsReal" << endl;
      continue;
    }

    _args.add(*coef);
  }

  // check number of arguments
  Int_t numReq(0);
  switch (_type) {
    case FracToMag          : numReq = 1; break;
    case FracToMagSq        : numReq = 1; break;
    case MagToFrac          : numReq = 1; break;
    case MagSqToFrac        : numReq = 1; break;
    case PolToRe            : numReq = 2; break;
    case PolToIm            : numReq = 2; break;
    case PolSqToRe          : numReq = 2; break;
    case PolSqToIm          : numReq = 2; break;
    case MagSqSinToReIm     : numReq = 2; break;
    case CarthToMag         : numReq = 2; break;
    case CarthToMagSq       : numReq = 2; break;
    case CarthFracToMagSq   : numReq = 2; break;
    case CarthToPhase       : numReq = 2; break;
    case ProdCarthPhaseToRe : numReq = 3; break;
    case ProdCarthPhaseToIm : numReq = 3; break;
    case PolSqToReC         : numReq = 3; break;
    case PolSqToImC         : numReq = 3; break;
    case ProdCarthToRe      : numReq = 4; break;
    case ProdCarthToIm      : numReq = 4; break;
    case ProdCarthCToRe     : numReq = 4; break;
    case ProdCarthCToIm     : numReq = 4; break;
    case PolSqToReRelC      : numReq = 4; break;
    case PolSqToImRelC      : numReq = 4; break;
    case FracPolToReC       : numReq = 3; break;
    case FracPolToImC       : numReq = 3; break;
    case FracPolToReRelC    : numReq = 4; break;
    case FracPolToImRelC    : numReq = 4; break;
    case FracCarthToReRelC  : numReq = 3; break;
    case FracCarthToImRelC  : numReq = 3; break;
    case PolSqToReRelMagC   : numReq = 5; break;
    case PolSqToImRelMagC   : numReq = 5; break;
    case MixSqToReRelMagC   : numReq = 5; break;
    case MixSqToImRelMagC   : numReq = 5; break;
  }

  if (_args.getSize() != numReq) {
    coutF(InputArguments) << "RooConvertPolAmp::RooConvertPolAmp("
        << GetName() << "): got " << _args.getSize()
        << " arguments instead of " << numReq << " (type " << _type << ")"
        << endl;
    assert(0);
  }
}

//_____________________________________________________________________________
RooConvertPolAmp::RooConvertPolAmp(
    const RooConvertPolAmp& other, const char* name) :
  RooAbsReal(other, name),
  _type(other._type),
  _args("arguments", this, other._args),
  _vals(0)
{}

//_____________________________________________________________________________
RooConvertPolAmp::~RooConvertPolAmp()
{
  if (_vals != 0) delete[] _vals;
}

//_____________________________________________________________________________
void RooConvertPolAmp::printArgs(ostream& os) const
{
  os << "[ ";
  switch (_type) {
    case FracToMag :
      os << "sqrt(" << _args.at(0)->GetName() << " / (1 - "
         << _args.at(0)->GetName() << "))";
      break;
    case FracToMagSq :
      os << _args.at(0)->GetName() << " / (1 - " << _args.at(0)->GetName()
         << ")";
      break;
    case MagToFrac :
      os << _args.at(0)->GetName() << "^2 / (1 + " << _args.at(0)->GetName()
         << "^2)";
      break;
    case MagSqToFrac :
      os << _args.at(0)->GetName() << " / (1 + " << _args.at(0)->GetName()
         << ")";
      break;
    case PolToRe :
      os << _args.at(0)->GetName() << " * cos(" << _args.at(1)->GetName()
         << ")";
      break;
    case PolToIm :
      os << _args.at(0)->GetName() << " * sin(" << _args.at(1)->GetName()
         << ")";
      break;
    case PolSqToRe :
      os << "sqrt(" << _args.at(0)->GetName() << ") * cos("
         << _args.at(1)->GetName() << ")";
      break;
    case PolSqToIm :
      os << "sqrt(" << _args.at(0)->GetName() << ") * sin("
         << _args.at(1)->GetName() << ")";
      break;
    case MagSqSinToReIm :
      os << "sqrt(" << _args.at(0)->GetName() << ") * "
         << _args.at(1)->GetName();
      break;
    case CarthToMag :
      os << "sqrt(" << _args.at(0)->GetName() << "^2 + "
         << _args.at(1)->GetName() << "^2)";
      break;
    case CarthToMagSq :
      os << _args.at(0)->GetName() << "^2 + " << _args.at(1)->GetName() << "^2";
      break;
    case CarthFracToMagSq :
      os << "(" << _args.at(0)->GetName() << "^2 + " << _args.at(1)->GetName()
         << "^2) / (1 - " << _args.at(0)->GetName() << "^2 - "
         << _args.at(1)->GetName() << "^2)";
      break;
    case CarthToPhase :
      os << "arctan(" << _args.at(1)->GetName() << " / "
         << _args.at(0)->GetName() << ")";
      break;
    case ProdCarthPhaseToRe :
      os << _args.at(0)->GetName() << " * cos(" << _args.at(2)->GetName()
         << ") - " << _args.at(1)->GetName() << " * sin("
         << _args.at(2)->GetName() << ")";
      break;
    case ProdCarthPhaseToIm :
      os << _args.at(0)->GetName() << " * sin(" << _args.at(2)->GetName()
         << ") + " << _args.at(1)->GetName() << " * cos("
         << _args.at(2)->GetName() << ")";
      break;
    case PolSqToReC :
      os << _args.at(2)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << ") * cos(" << _args.at(1)->GetName() << ")";
      break;
    case PolSqToImC :
      os << _args.at(2)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << ") * sin(" << _args.at(1)->GetName() << ")";
      break;
    case ProdCarthToRe :
      os << _args.at(0)->GetName() << " * " << _args.at(2)->GetName() << " - "
         << _args.at(1)->GetName() << " * " << _args.at(3)->GetName();
      break;
    case ProdCarthToIm :
      os << _args.at(0)->GetName() << " * " << _args.at(3)->GetName() << " + "
         << _args.at(1)->GetName() << " * " << _args.at(2)->GetName();
      break;
    case ProdCarthCToRe :
      os << _args.at(0)->GetName() << " * " << _args.at(2)->GetName() << " + "
         << _args.at(1)->GetName() << " * " << _args.at(3)->GetName();
      break;
    case ProdCarthCToIm :
      os << _args.at(0)->GetName() << " * " << _args.at(3)->GetName() << " - "
         << _args.at(1)->GetName() << " * " << _args.at(2)->GetName();
      break;
    case PolSqToReRelC :
      os << _args.at(3)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << ") * cos(" << _args.at(1)->GetName() << " + "
         << _args.at(2)->GetName() << ")";
      break;
    case PolSqToImRelC :
      os << _args.at(3)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << ") * sin(" << _args.at(1)->GetName() << " + "
         << _args.at(2)->GetName() << ")";
      break;
    case FracPolToReC :
      os << _args.at(2)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << " / (1 - " << _args.at(0)->GetName() << ")) * cos("
         << _args.at(1)->GetName() << ")";
      break;
    case FracPolToImC :
      os << _args.at(2)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << " / (1 - " << _args.at(0)->GetName() << ")) * sin("
         << _args.at(1)->GetName() << ")";
      break;
    case FracPolToReRelC :
      os << _args.at(3)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << " / (1 - " << _args.at(0)->GetName() << ")) * cos("
         << _args.at(1)->GetName() << " + " << _args.at(2)->GetName() << ")";
      break;
    case FracPolToImRelC :
      os << _args.at(3)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << " / (1 - " << _args.at(0)->GetName() << ")) * sin("
         << _args.at(1)->GetName() << " + " << _args.at(2)->GetName() << ")";
      break;
    case FracCarthToReRelC :
      os << _args.at(2)->GetName() << " * " << _args.at(0)->GetName()
         << " / sqrt(1 - " << _args.at(0)->GetName() << "^2 - "
         << _args.at(1)->GetName() << "^2)";
      break;
    case FracCarthToImRelC :
      os << _args.at(2)->GetName() << " * " << _args.at(1)->GetName()
         << " / sqrt(1 - " << _args.at(0)->GetName() << "^2 - "
         << _args.at(1)->GetName() << "^2)";
      break;
    case PolSqToReRelMagC :
      os << _args.at(4)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << " * " << _args.at(2)->GetName() << ") * cos("
         << _args.at(1)->GetName() << " * " << _args.at(3)->GetName() << ")";
      break;
    case PolSqToImRelMagC :
      os << _args.at(4)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << " * " << _args.at(2)->GetName() << ") * sin("
         << _args.at(1)->GetName() << " * " << _args.at(3)->GetName() << ")";
      break;
    case MixSqToReRelMagC :
      os << _args.at(4)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << ") * (cos(" << _args.at(1)->GetName() << ") * "
         << _args.at(2)->GetName() << " - sin(" << _args.at(1)->GetName()
         << ") * " << _args.at(3)->GetName() << ")";
      break;
    case MixSqToImRelMagC :
      os << _args.at(4)->GetName() << " * sqrt(" << _args.at(0)->GetName()
         << ") * (cos(" << _args.at(1)->GetName() << ") * "
         << _args.at(3)->GetName() << " + sin(" << _args.at(1)->GetName()
         << ") * " << _args.at(2)->GetName() << ")";
      break;
  }
  os << " ]";
}

//_____________________________________________________________________________
Double_t RooConvertPolAmp::evaluate() const
{
  // initialize argument values pointer
  if (_vals == 0) _vals = new Double_t[_args.getSize()];

  // get values of arguments
  for (Int_t argIt = 0; argIt < _args.getSize(); ++argIt)
    _vals[argIt] = static_cast<RooAbsReal*>(_args.at(argIt))->getVal();

  // return function value
  switch (_type) {
    case FracToMag :
      return sqrt(_vals[0] / (1. - _vals[0]));
    case FracToMagSq :
      return _vals[0] / (1. - _vals[0]);
    case MagToFrac :
      return _vals[0] * _vals[0] / (1. + _vals[0] * _vals[0]);
    case MagSqToFrac :
      return _vals[0] / (1. + _vals[0]);
    case PolToRe :
      return _vals[0] * cos(_vals[1]);
    case PolToIm :
      return _vals[0] * sin(_vals[1]);
    case PolSqToRe :
      return sqrt(_vals[0]) * cos(_vals[1]);
    case PolSqToIm :
      return sqrt(_vals[0]) * sin(_vals[1]);
    case MagSqSinToReIm :
      return sqrt(_vals[0]) * _vals[1];
    case CarthToMag :
      return sqrt(_vals[0] * _vals[0] + _vals[1] * _vals[1]);
    case CarthToMagSq :
      return _vals[0] * _vals[0] + _vals[1] * _vals[1];
    case CarthFracToMagSq :
      return (_vals[0] * _vals[0] + _vals[1] * _vals[1])
             / (1. - _vals[0] * _vals[0] - _vals[1] * _vals[1]);
    case CarthToPhase :
      return atan2(_vals[1], _vals[0]);
    case ProdCarthPhaseToRe :
      return _vals[0] * cos(_vals[2]) - _vals[1] * sin(_vals[2]);
    case ProdCarthPhaseToIm :
      return _vals[0] * sin(_vals[2]) + _vals[1] * cos(_vals[2]);
    case PolSqToReC :
      return _vals[2] * sqrt(_vals[0]) * cos(_vals[1]);
    case PolSqToImC :
      return _vals[2] * sqrt(_vals[0]) * sin(_vals[1]);
    case ProdCarthToRe :
      return _vals[0] * _vals[2] - _vals[1] * _vals[3];
    case ProdCarthToIm :
      return _vals[0] * _vals[3] + _vals[1] * _vals[2];
    case ProdCarthCToRe :
      return _vals[0] * _vals[2] + _vals[1] * _vals[3];
    case ProdCarthCToIm :
      return _vals[0] * _vals[3] - _vals[1] * _vals[2];
    case PolSqToReRelC :
      return _vals[3] * sqrt(_vals[0]) * cos(_vals[1] + _vals[2]);
    case PolSqToImRelC :
      return _vals[3] * sqrt(_vals[0]) * sin(_vals[1] + _vals[2]);
    case FracPolToReC :
      return _vals[2] * sqrt(_vals[0] / (1. - _vals[0])) * cos(_vals[1]);
    case FracPolToImC :
      return _vals[2] * sqrt(_vals[0] / (1. - _vals[0])) * sin(_vals[1]);
    case FracPolToReRelC :
      return _vals[3] * sqrt(_vals[0] / (1. - _vals[0]))
             * cos(_vals[1] + _vals[2]);
    case FracPolToImRelC :
      return _vals[3] * sqrt(_vals[0] / (1. - _vals[0]))
             * sin(_vals[1] + _vals[2]);
    case FracCarthToReRelC :
      return _vals[2] * _vals[0]
             / sqrt(1. - _vals[0] * _vals[0] - _vals[1] * _vals[1]);
    case FracCarthToImRelC :
      return _vals[2] * _vals[1]
             / sqrt(1. - _vals[0] * _vals[0] - _vals[1] * _vals[1]);
    case PolSqToReRelMagC :
      return _vals[4] * sqrt(_vals[0] * _vals[2]) * cos(_vals[1] * _vals[3]);
    case PolSqToImRelMagC :
      return _vals[4] * sqrt(_vals[0] * _vals[2]) * sin(_vals[1] * _vals[3]);
    case MixSqToReRelMagC :
      return _vals[4] * sqrt(_vals[0])
             * (cos(_vals[1]) * _vals[2] - sin(_vals[1]) * _vals[3]);
    case MixSqToImRelMagC :
      return _vals[4] * sqrt(_vals[0])
             * (cos(_vals[1]) * _vals[3] + sin(_vals[1]) * _vals[2]);
  }

  return 0.;  // we should never get here
}
