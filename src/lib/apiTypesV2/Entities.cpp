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

#include "logMsg/traceLevels.h"
#include "ngsi10/QueryContextResponse.h"
#include "apiTypesV2/Entities.h"



/* ****************************************************************************
*
* Entities::Entities - 
*/
Entities::Entities()
{
  errorCode.fill("OK", "");
}



/* ****************************************************************************
*
* Entities::~Entities - 
*/
Entities::~Entities()
{
  release();
}



/* ****************************************************************************
*
* Entities::render - 
*
* If no error reported in errorCode, render the vector of entities.
* Otherwise, render the errorCode.
*/
std::string Entities::render(ConnectionInfo* ciP, RequestType requestType)
{
  if ((errorCode.description == "") && ((errorCode.error == "OK") || (errorCode.error == "")))
  {
    return vec.render(ciP, requestType, false);
  }

  return errorCode.toJson(true);
} 



/* ****************************************************************************
*
* Entities::check - 
*
* NOTE
*   The 'check' method is normally only used to check that incoming payload is correct.
*   For now (at least), the Entities type is only used as outgoing payload ...
*/
std::string Entities::check(ConnectionInfo* ciP, RequestType requestType)
{
  return vec.check(ciP, requestType);
}



/* ****************************************************************************
*
* Entities::present - 
*/
void Entities::present(const std::string& indent)
{
  LM_T(LmtPresent, ("%s%d Entities:", 
		    indent.c_str(), 
		    vec.size()));
  vec.present(indent + "  ");
}



/* ****************************************************************************
*
* Entities::release - 
*/
void Entities::release(void)
{
  vec.release();
}



/* ****************************************************************************
*
* Entities::fill - 
*
* NOTE
*   The errorCode field from qcrsP is not used at all if errorCode::code equals SccOk. 
*   This means that e.g. the "Count:" in errorCode::details (from v1 logic) will not be
*   present in the Entities for v2 (that number is in the HTTP header X-Total-Count for v2).
*   Other values for "details" are lost as well, if errorCode::code equals SccOk.
*/
void Entities::fill(QueryContextResponse* qcrsP)
{
  if (qcrsP->errorCode.code == SccContextElementNotFound)
  {
    //
    // If no entities are found, we respond with a 200 OK
    // and an empty vector of entities ( [] )
    //

    errorCode.fill("OK", "");
    return;
  }
  else if (qcrsP->errorCode.code != SccOk)
  {
    //
    // If any other error - use the error for the response
    //

    errorCode.fill(qcrsP->errorCode);
    return;
  }

  for (unsigned int ix = 0; ix < qcrsP->contextElementResponseVector.size(); ++ix)
  {
    ContextElement* ceP = &qcrsP->contextElementResponseVector[ix]->contextElement;
    Entity*         eP  = new Entity();

    eP->id        = ceP->entityId.id;
    eP->type      = ceP->entityId.type;
    eP->isPattern = ceP->entityId.isPattern;

    eP->attributeVector.fill(&ceP->contextAttributeVector);
    vec.push_back(eP);
  }
}
