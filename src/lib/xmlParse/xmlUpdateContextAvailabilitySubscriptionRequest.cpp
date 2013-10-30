/*
*
* Copyright 2013 Telefonica Investigacion y Desarrollo, S.A.U
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
* fermin at tid dot es
*
* Author: Ken Zangelin
*/
#include <stdio.h>
#include <string>

#include "xmlParse/XmlNode.h"

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "common/globals.h"
#include "ngsi/ContextAttribute.h"
#include "ngsi/EntityId.h"
#include "ngsi/Restriction.h"
#include "ngsi/Scope.h"
#include "ngsi9/UpdateContextAvailabilitySubscriptionRequest.h"
#include "xmlParse/XmlNode.h"
#include "xmlParse/xmlParse.h"
#include "xmlParse/xmlUpdateContextAvailabilitySubscriptionRequest.h"



/* ****************************************************************************
*
* entityId - 
*/
static int entityId(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got an entityId"));

  reqData->ucas.entityIdP = new EntityId();
  reqData->ucas.res.entityIdVector.push_back(reqData->ucas.entityIdP);

  std::string es = entityIdParse(UpdateContextAvailabilitySubscription, node, reqData->ucas.entityIdP);

  if (es != "OK")
    reqData->errorString = es;
  
  return 0;
}



/* ****************************************************************************
*
* entityIdId - 
*/
static int entityIdId(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got an entityId:id: '%s'", node->value()));

  if ((reqData->ucas.entityIdP->id != "") && (reqData->ucas.entityIdP->id != node->value()))
    LM_W(("Overwriting entityId:id (was '%s') for '%s'", reqData->ucas.entityIdP->id.c_str(), node->value()));
  reqData->ucas.entityIdP->id = node->value();

  return 0;
}



/* ****************************************************************************
*
* entityIdType - 
*/
static int entityIdType(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got an entityId:type: '%s'", node->value()));

  if ((reqData->ucas.entityIdP->type != "") && (reqData->ucas.entityIdP->type != node->value()))
    LM_W(("Overwriting entityId:type (was '%s') for '%s'", reqData->ucas.entityIdP->type.c_str(), node->value()));
  reqData->ucas.entityIdP->type = node->value();

  return 0;
}



/* ****************************************************************************
*
* entityIdIsPattern - 
*/
static int entityIdIsPattern(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got an entityId:isPattern: '%s'", node->value()));

  if ((reqData->ucas.entityIdP->isPattern != "") && (reqData->ucas.entityIdP->isPattern != node->value()))
    LM_W(("Overwriting entityId:isPattern (was '%s') for '%s'", reqData->ucas.entityIdP->isPattern.c_str(), node->value()));
  reqData->ucas.entityIdP->isPattern = node->value();

  return 0;
}



/* ****************************************************************************
*
* attribute - 
*/
static int attribute(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got an attribute"));

  reqData->ucas.res.attributeList.push_back(node->value());

  return 0;
}



/* ****************************************************************************
*
* restriction - 
*/
static int restriction(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got a restriction"));
  ++reqData->ucas.restrictions;

  return 0;
}



/* ****************************************************************************
*
* attributeExpression - 
*/
static int attributeExpression(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got an attributeExpression: '%s'", node->value()));
  reqData->ucas.res.restriction.attributeExpression.set(node->value());

  return 0;
}



/* ****************************************************************************
*
* operationScope - 
*/
static int operationScope(xml_node<>* node, ParseData* reqData)
{
  Scope* newScopeP = new Scope();

  LM_T(LmtParse, ("Got an operationScope"));
  reqData->ucas.scopeP = newScopeP;
  reqData->ucas.res.restriction.scopeVector.push_back(reqData->ucas.scopeP);

  return 0;
}



/* ****************************************************************************
*
* scopeType - 
*/
static int scopeType(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got a scopeType: '%s'", node->value()));
  reqData->ucas.scopeP->type = node->value();
  return 0;
}



/* ****************************************************************************
*
* scopeValue - 
*/
static int scopeValue(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got a scopeValue: '%s'", node->value()));
  reqData->ucas.scopeP->value = node->value();
  return 0;
}



/* ****************************************************************************
*
* duration - 
*/
static int duration(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got a duration: '%s'", node->value()));
  reqData->ucas.res.duration.set(node->value());
  return 0;
}



/* ****************************************************************************
*
* subscriptionId - 
*/
static int subscriptionId(xml_node<>* node, ParseData* reqData)
{
  LM_T(LmtParse, ("Got a subscriptionId: '%s'", node->value()));
  reqData->ucas.res.subscriptionId.set(node->value());
  return 0;
}



/* ****************************************************************************
*
* ucasInit - 
*/
void ucasInit(ParseData* reqData)
{
  reqData->ucas.scopeP        = NULL;
  reqData->ucas.restrictions  = 0;
  reqData->errorString        = "";
}



/* ****************************************************************************
*
* ucasRelease - 
*/
void ucasRelease(ParseData* reqData)
{
  reqData->ucas.res.release();
}



/* ****************************************************************************
*
* ucasCheck - 
*/
std::string ucasCheck(ParseData* reqData, ConnectionInfo* ciP)
{
   return reqData->ucas.res.check(UpdateContextAvailabilitySubscription, ciP->outFormat, "", reqData->errorString, 0);
}



#define PRINTF printf
/* ****************************************************************************
*
* ucasPresent - 
*/
void ucasPresent(ParseData* reqData)
{
  if (!lmTraceIsSet(LmtDump))
    return;

  PRINTF("\n\n");
  reqData->ucas.res.present("");
}



/* ****************************************************************************
*
* scrParseVector - 
*/
XmlNode ucasParseVector[] = 
{
  { "/updateContextAvailabilitySubscriptionRequest",                                             nullTreat             },

  { "/updateContextAvailabilitySubscriptionRequest/entityIdList",                                nullTreat             },
  { "/updateContextAvailabilitySubscriptionRequest/entityIdList/entityId",                       entityId              },
  { "/updateContextAvailabilitySubscriptionRequest/entityIdList/entityId/id",                    entityIdId            },
  { "/updateContextAvailabilitySubscriptionRequest/entityIdList/entityId/type",                  entityIdType          },
  { "/updateContextAvailabilitySubscriptionRequest/entityIdList/entityId/isPattern",             entityIdIsPattern     },

  { "/updateContextAvailabilitySubscriptionRequest/attributeList",                               nullTreat             },
  { "/updateContextAvailabilitySubscriptionRequest/attributeList/attribute",                     attribute             },

  { "/updateContextAvailabilitySubscriptionRequest/duration",                                    duration              },
  
  { "/updateContextAvailabilitySubscriptionRequest/restriction",                                  restriction          },
  { "/updateContextAvailabilitySubscriptionRequest/restriction/attributeExpression",              attributeExpression  },
  { "/updateContextAvailabilitySubscriptionRequest/restriction/scope",                            nullTreat            },
  { "/updateContextAvailabilitySubscriptionRequest/restriction/scope/operationScope",             operationScope       },
  { "/updateContextAvailabilitySubscriptionRequest/restriction/scope/operationScope/scopeType",   scopeType            },
  { "/updateContextAvailabilitySubscriptionRequest/restriction/scope/operationScope/scopeValue",  scopeValue           },

  { "/updateContextAvailabilitySubscriptionRequest/subscriptionId",                               subscriptionId       },
  
  { "LAST", NULL }
};