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
* iot_support at tid dot es
*
* Author: Fermin Galan Marquez
*/
#include <string>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"
#include "common/globals.h"
#include "common/Format.h"
#include "common/sem.h"
#include "alarmMgr/alarmMgr.h"
#include "mongoBackend/MongoGlobal.h"
#include "mongoBackend/dbConstants.h"
#include "mongoBackend/mongoSubscribeContext.h"
#include "mongoBackend/connectionOperations.h"
#include "cache/subCache.h"
#include "ngsi10/SubscribeContextRequest.h"
#include "ngsi10/SubscribeContextResponse.h"
#include "ngsi/StatusCode.h"
#include "rest/uriParamNames.h"



/* ****************************************************************************
*
* mongoSubscribeContext - 
*/
HttpStatusCode mongoSubscribeContext
(
  SubscribeContextRequest*             requestP,
  SubscribeContextResponse*            responseP,
  const std::string&                   tenant,
  std::map<std::string, std::string>&  uriParam,
  const std::string&                   xauthToken,
  const std::vector<std::string>&      servicePathV
)
{
    std::string        servicePath           = (servicePathV.size() == 0)? "" : servicePathV[0];    
    bool               reqSemTaken           = false;    

    reqSemTake(__FUNCTION__, "ngsi10 subscribe request", SemWriteOp, &reqSemTaken);

    //
    // Calculate expiration (using the current time and the duration field in the request).
    // If expiration is not present, use a default value
    //
    long long expiration = -1;
    if (requestP->expires > 0)
    {
      expiration = requestP->expires;
    }
    else
    {
      if (requestP->duration.isEmpty())
      {
        requestP->duration.set(DEFAULT_DURATION);
      }

      expiration = getCurrentTime() + requestP->duration.parse();
    }
    LM_T(LmtMongo, ("Subscription expiration: %lu", expiration));

    /* Create the mongoDB subscription document */
    BSONObjBuilder  sub;
    OID             oid;

    oid.init();

    sub.append("_id", oid);
    sub.append(CSUB_EXPIRATION, expiration);
    sub.append(CSUB_REFERENCE,  requestP->reference.get());


    /* Throttling */
    long long throttling = 0;
    if (requestP->throttling.seconds != -1)
    {
      throttling = (long long) requestP->throttling.seconds;
      sub.append(CSUB_THROTTLING, throttling);
    }
    else if (!requestP->throttling.isEmpty())
    {
      throttling = (long long) requestP->throttling.parse();
      sub.append(CSUB_THROTTLING, throttling);
    }

    if (servicePath != "")
    {
      sub.append(CSUB_SERVICE_PATH, servicePath);
    }

    
    /* Build entities array */
    BSONArrayBuilder entities;
    for (unsigned int ix = 0; ix < requestP->entityIdVector.size(); ++ix)
    {
        EntityId* en = requestP->entityIdVector[ix];

        if (en->type == "")
        {
          entities.append(BSON(CSUB_ENTITY_ID << en->id <<
                               CSUB_ENTITY_ISPATTERN << en->isPattern));
        }
        else
        {
          entities.append(BSON(CSUB_ENTITY_ID << en->id <<
                               CSUB_ENTITY_TYPE << en->type <<
                               CSUB_ENTITY_ISPATTERN << en->isPattern));
        }
    }
    sub.append(CSUB_ENTITIES, entities.arr());

    /* Build attributes array */
    BSONArrayBuilder attrs;
    for (unsigned int ix = 0; ix < requestP->attributeList.size(); ++ix) {
        attrs.append(requestP->attributeList[ix]);
    }
    sub.append(CSUB_ATTRS, attrs.arr());

    /* Build conditions array (including side-effect notifications and threads creation) */
    bool notificationDone = false;
    BSONArray conds = processConditionVector(&requestP->notifyConditionVector,
                                             requestP->entityIdVector,
                                             requestP->attributeList, oid.toString(),
                                             requestP->reference.get(),
                                             &notificationDone,
                                             JSON,
                                             tenant,
                                             xauthToken,
                                             servicePathV,
                                             requestP->expression.q);
    sub.append(CSUB_CONDITIONS, conds);

    /* Build expression */
    BSONObjBuilder expression;

    expression << CSUB_EXPR_Q << requestP->expression.q
               << CSUB_EXPR_GEOM << requestP->expression.geometry
               << CSUB_EXPR_COORDS << requestP->expression.coords
               << CSUB_EXPR_GEOREL << requestP->expression.georel;
    sub.append(CSUB_EXPR, expression.obj());

    /* Last notification */
    long long lastNotificationTime = 0;
    if (notificationDone)
    {
      lastNotificationTime = (long long) getCurrentTime();

      sub.append(CSUB_LASTNOTIFICATION, lastNotificationTime);
      sub.append(CSUB_COUNT, (long long) 1);
    }

    /* Adding format to use in notifications */
    sub.append(CSUB_FORMAT, "JSON");

    /* Insert document in database */
    std::string err;
    if (!collectionInsert(getSubscribeContextCollectionName(tenant), sub.obj(), &err))
    {
      reqSemGive(__FUNCTION__, "ngsi10 subscribe request (mongo db exception)", reqSemTaken);
      responseP->subscribeError.errorCode.fill(SccReceiverInternalError, err);
      return SccOk;
    }

    //
    // 3. Create Subscription for the cache
    //
    std::string oidString = oid.toString();

    LM_T(LmtSubCache, ("inserting a new sub in cache (%s)", oidString.c_str()));

    cacheSemTake(__FUNCTION__, "Inserting subscription in cache");
    subCacheItemInsert(tenant.c_str(),
                       servicePath.c_str(),
                       requestP,
                       oidString.c_str(),
                       expiration,
                       throttling,
                       JSON,
                       notificationDone,
                       lastNotificationTime,
                       requestP->expression.q,
                       requestP->expression.geometry,
                       requestP->expression.coords,
                       requestP->expression.georel);

    cacheSemGive(__FUNCTION__, "Inserting subscription in cache");

    reqSemGive(__FUNCTION__, "ngsi10 subscribe request", reqSemTaken);

    /* Fill int the response element */
    responseP->subscribeResponse.duration = requestP->duration;
    responseP->subscribeResponse.subscriptionId.set(oid.toString());
    responseP->subscribeResponse.throttling = requestP->throttling;

    return SccOk;
}
