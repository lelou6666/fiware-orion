/*
*
* Copyright 2015 Telefonica Investigacion y Desarrollo, S.A.U
*
* This file is part of Orion Context Broker.
*
* Orion Context Broker is free software: you can redistribute it and/or
* modify it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Orion Context Broker is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero
* General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Orion Context Broker. If not, see http://www.gnu.org/licenses/.
*
* For those usages not covered by this license please contact with
* iot_support at tid dot es
*
* Author: Ken Zangelin
*/
#include <string>
#include <vector>

#include "common/statistics.h"
#include "common/clockFunctions.h"
#include "common/errorMessages.h"
#include "parse/forbiddenChars.h"
#include "rest/ConnectionInfo.h"
#include "ngsi/ParseData.h"
#include "apiTypesV2/Entities.h"
#include "rest/EntityTypeInfo.h"
#include "serviceRoutinesV2/patchEntity.h"
#include "serviceRoutines/postUpdateContext.h"
#include "rest/OrionError.h"



/* ****************************************************************************
*
* patchEntity - 
*
* PATCH /v2/entities
*
* Payload In:  Entity
* Payload Out: None
*
* URI parameters:
*   - 
*
* 01. Fill in UpdateContextRequest
* 02. Call standard op postUpdateContext
* 03. Check output from mongoBackend - any errors?
* 04. Prepare HTTP headers
* 05. Cleanup and return result
*/
std::string patchEntity
(
  ConnectionInfo*            ciP,
  int                        components,
  std::vector<std::string>&  compV,
  ParseData*                 parseDataP
)
{
  std::string  answer = "";
  Entity*      eP     = &parseDataP->ent.res;

  eP->id = compV[2];
  eP->type = ciP->uriParam["type"];

  if (forbiddenIdChars(ciP->apiVersion, eP->id.c_str() , NULL))
  {
    OrionError oe(SccBadRequest, "invalid character in URI");
    return oe.render(ciP, "");
  }

  // 01. Fill in UpdateContextRequest
  parseDataP->upcr.res.fill(eP, "UPDATE");
  

  // 02. Call standard op postUpdateContext
  postUpdateContext(ciP, components, compV, parseDataP);

  // 03. Check output from mongoBackend - any errors?
  if (parseDataP->upcrs.res.contextElementResponseVector.size() == 1)
  {
    if (parseDataP->upcrs.res.contextElementResponseVector[0]->statusCode.code != SccOk)
    {
      ciP->httpStatusCode = parseDataP->upcrs.res.contextElementResponseVector[0]->statusCode.code;

      if (parseDataP->upcrs.res.contextElementResponseVector[0]->statusCode.code == SccContextElementNotFound)
      {
        OrionError orionError(SccContextElementNotFound, "No context element found");

        TIMED_RENDER(answer = orionError.render(ciP, ""));
      } 
      else if (parseDataP->upcrs.res.contextElementResponseVector[0]->statusCode.code == SccConflict)
      {
        OrionError orionError(SccConflict, MORE_MATCHING_ENT);

        TIMED_RENDER(answer = orionError.render(ciP, ""));
      }
    }
  }


  // 04. Prepare HTTP headers
  if ((ciP->httpStatusCode == SccOk) || (ciP->httpStatusCode == SccNone))
  {
    ciP->httpStatusCode = SccNoContent;
  }
  else if (ciP->httpStatusCode == SccInvalidParameter)
  {
    OrionError orionError(SccContextElementNotFound, "No context element found");

    ciP->httpStatusCode = SccContextElementNotFound;

    TIMED_RENDER(answer = orionError.render(ciP, ""));
  }


  // 05. Cleanup and return result
  eP->release();

  return answer;
}
