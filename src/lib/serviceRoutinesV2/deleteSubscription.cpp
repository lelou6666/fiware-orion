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
* Author: Orion dev team
*/
#include <string>

#include "common/statistics.h"
#include "common/clockFunctions.h"

#include "rest/ConnectionInfo.h"
#include "ngsi/ParseData.h"
#include "rest/OrionError.h"
#include "apiTypesV2/ErrorCode.h"
#include "mongoBackend/mongoUnsubscribeContext.h"
#include "ngsi10/UnsubscribeContextResponse.h"

#include "serviceRoutinesV2/deleteSubscription.h"


/* ****************************************************************************
*
* deleteSubscription -
*
* DELETE /v2/subscriptions/{entityId}
*
* Payload In:  None
* Payload Out: None
*
* URI parameters:
*   -
*/


std::string deleteSubscription
(
    ConnectionInfo*            ciP,
    int                        components,
    std::vector<std::string>&  compV,
    ParseData*                 parseDataP
)
{
  std::string                 answer;
  std::string                 subscriptionId =  compV[2];
  UnsubscribeContextResponse  uncr;

  // 'Fill In' UnsubscribeContextRequest
  parseDataP->uncr.res.subscriptionId.set(subscriptionId);

  TIMED_MONGO(ciP->httpStatusCode = mongoUnsubscribeContext(&parseDataP->uncr.res, &uncr, ciP->tenant));

  if (uncr.statusCode.code != SccOk)
  {
    OrionError oe(uncr.statusCode);

    ciP->httpStatusCode = uncr.statusCode.code;
    oe.reasonPhrase     = uncr.statusCode.reasonPhrase;

    if (uncr.statusCode.code == SccContextElementNotFound)
    {
      oe.details = "The requested subscription has not been found. Check id";
    }

    TIMED_RENDER(answer = oe.render(ciP, ""));

    return answer;
  }

  ciP->httpStatusCode = SccNoContent;

  return "";
}

