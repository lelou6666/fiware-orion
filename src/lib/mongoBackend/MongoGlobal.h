#ifndef MONGO_GLOBAL_H
#define MONGO_GLOBAL_H

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
* Author: Fermín Galán
*/
#include "logMsg/logMsg.h"

#include "mongo/client/dbclient.h"

#include "ngsi/EntityId.h"
#include "ngsi/ContextRegistrationAttribute.h"
#include "ngsi/ContextAttribute.h"
#include "ngsi/EntityIdVector.h"
#include "ngsi/AttributeList.h"
#include "ngsi/ContextElementResponseVector.h"
#include "ngsi/ConditionValueList.h"
#include "ngsi/Restriction.h"
#include "ngsi/NotifyConditionVector.h"
#include "ngsi10/UpdateContextResponse.h"
#include "ngsi9/RegisterContextRequest.h"
#include "ngsi9/RegisterContextResponse.h"
#include "ngsiNotify/Notifier.h"
#include "rest/uriParamNames.h"

#include "mongoBackend/TriggeredSubscription.h"

using namespace mongo;


/* ****************************************************************************
*
* MongoTreatFunction - callback signature for Mongo callback functions
*/
typedef void (*MongoTreatFunction)(std::string tenant, BSONObj& bobj);



/* ****************************************************************************
*
* mongoMultitenant -
*/
extern bool mongoMultitenant(void);



/* ****************************************************************************
*
* mongoStart - 
*/
extern bool mongoStart
(
  const char* host,
  const char* db,
  const char* rplSet,
  const char* username,
  const char* passwd,
  bool        _multitenant,
  double      timeout,
  int         writeConcern = 1,
  int         poolSize     = 10,
  bool        semTimeStat  = false
);



#ifdef UNIT_TEST
extern void setMongoConnectionForUnitTest(DBClientBase* _connection);
#endif

/*****************************************************************************
*
* getNotifier -
*/
extern Notifier* getNotifier();

/*****************************************************************************
*
* setNotifier -
*/
extern void setNotifier(Notifier* n);


/*****************************************************************************
*
* getMongoConnection -
*
* I would prefer to have per-collection methods, to have a better encapsulation, but
* the Mongo C++ API seems not to work that way
*/
extern DBClientBase* getMongoConnection(void);

/* ****************************************************************************
*
* releaseMongoConnection - 
*/
extern void releaseMongoConnection(DBClientBase* connection);

/*****************************************************************************
*
* setDbPrefix -
*/
extern void setDbPrefix(const std::string& dbPrefix);

/*****************************************************************************
*
* getDbPrefix -
*/
extern const std::string& getDbPrefix(void);

/*****************************************************************************
*
* getOrionDatabases -
*
* Return the list of Orion databases (the ones that start with the dbPrefix + "_").
* Note that the DB belonging to the default service is not included in the
* returned list
*
* Function return value is false in the case of problems accessing database,
* true otherwise.
*
*/
extern bool getOrionDatabases(std::vector<std::string>& dbs);

/*****************************************************************************
*
* tenantFromDb -
*/
extern std::string tenantFromDb(const std::string& database);

/*****************************************************************************
*
* setEntitiesCollectionName -
*/
extern void setEntitiesCollectionName(const std::string& name);

/*****************************************************************************
*
* setRegistrationsCollectionName -
*/
extern void setRegistrationsCollectionName(const std::string& name);

/*****************************************************************************
*
* setSubscribeContextCollectionName -
*/
extern void setSubscribeContextCollectionName(const std::string& name);

/*****************************************************************************
*
* setSubscribeContextAvailabilityCollectionName -
*/
extern void setSubscribeContextAvailabilityCollectionName(const std::string& name);

/*****************************************************************************
*
* composeDatabaseName -
*
*/
extern std::string composeDatabaseName(const std::string& tenant);

/*****************************************************************************
*
* getEntitiesCollectionName -
*/
extern std::string getEntitiesCollectionName(const std::string& tenant);

/*****************************************************************************
*
* getRegistrationsCollectionName -
*/
extern std::string getRegistrationsCollectionName(const std::string& tenant);

/*****************************************************************************
*
* getSubscribeContextCollectionName -
*/
extern std::string getSubscribeContextCollectionName(const std::string& tenant);

/*****************************************************************************
*
* getSubscribeContextAvailabilityCollectionName -
*/
extern std::string getSubscribeContextAvailabilityCollectionName(const std::string& tenant);

/*****************************************************************************
*
* mongoLocationCapable -
*/
extern bool mongoLocationCapable(void);

/*****************************************************************************
*
* ensureLocationIndex -
*/
extern void ensureLocationIndex(const std::string& tenant);

/* ****************************************************************************
*
* matchEntity -
*/
extern bool matchEntity(const EntityId* en1, const EntityId* en2);

/* ****************************************************************************
*
* includedEntity -
*/
extern bool includedEntity(EntityId en, const EntityIdVector& entityIdV);

/* ****************************************************************************
*
* includedAttribute -
*/
extern bool includedAttribute(const ContextRegistrationAttribute& attr, const AttributeList& attrsV);


/* ****************************************************************************
*
* entitiesQuery -
*
*/
extern bool entitiesQuery
(
  const EntityIdVector&            enV,
  const AttributeList&             attrL,
  const Restriction&               res,
  ContextElementResponseVector*    cerV,
  std::string*                     err,
  bool                             includeEmpty,
  const std::string&               tenant,
  const std::vector<std::string>&  servicePath,
  int                              offset         = DEFAULT_PAGINATION_OFFSET_INT,
  int                              limit          = DEFAULT_PAGINATION_LIMIT_INT,
  bool*                            limitReached   = NULL,
  long long*                       countP         = NULL,
  bool*                            badInputP      = NULL,
  const std::string&               sortOrderList  = "",
  bool                             includeCreDate = false,
  bool                             includeModDate = false,
  const std::string&               apiVersion     = "v1"
);

/* ****************************************************************************
*
* pruneContextElements -
*
*/
extern void pruneContextElements(ContextElementResponseVector& oldCerV, ContextElementResponseVector* newCerVP);

/* ****************************************************************************
*
* registrationsQuery -
*
*/
extern bool registrationsQuery
(
  const EntityIdVector&               enV,
  const AttributeList&                attrL,
  ContextRegistrationResponseVector*  crrV,
  std::string*                        err,
  const std::string&                  tenant,
  const std::vector<std::string>&     servicePathV,
  int                                 offset       = DEFAULT_PAGINATION_OFFSET_INT,
  int                                 limit        = DEFAULT_PAGINATION_LIMIT_INT,
  bool                                details      = false,
  long long*                          countP       = NULL
);

/* ****************************************************************************
*
* someEmptyCondValue -
*
*/
extern bool someEmptyCondValue(const BSONObj& sub);

/* ****************************************************************************
*
* condValueAttrMatch -
*
*/
extern bool condValueAttrMatch(const BSONObj& sub, const std::vector<std::string>& modifiedAttrs);

/* ****************************************************************************
*
* subToEntityIdVector -
*
* Extract the entity ID vector from a BSON document (in the format of the csubs
* collection)
*
*/
extern EntityIdVector subToEntityIdVector(const BSONObj& sub);

/* ****************************************************************************
*
* subToAttributeList -
*
* Extract the attribute list from a BSON document (in the format of the csubs
* collection)
*
*/
extern AttributeList subToAttributeList(const BSONObj& attrL);


/* ****************************************************************************
*
* processOnChangeConditionForSubscription -
*
*/
extern bool processOnChangeConditionForSubscription
(
  const EntityIdVector&            enV,
  const AttributeList&             attrV,
  ConditionValueList*              condValues,
  const std::string&               subId,
  const std::string&               notifyUrl,
  Format                           format,
  const std::string&               tenant,
  const std::string&               xauthToken,
  const std::vector<std::string>&  servicePathV,
  const std::string&               qFilter
);

/* ****************************************************************************
*
* processConditionVector -
*
*/
extern BSONArray processConditionVector
(
  NotifyConditionVector*           ncvP,
  const EntityIdVector&            enV,
  const AttributeList&             attrL,
  const std::string&               subId,
  const std::string&               url,
  bool*                            notificationDone,
  Format                           format,
  const std::string&               tenant,
  const std::string&               xauthToken,
  const std::vector<std::string>&  servicePathV,
  const std::string&               qFilter
);

/* ****************************************************************************
*
* processAvailabilitySubscriptions -
*
*/
extern bool processAvailabilitySubscription(
    const EntityIdVector& enV,
    const AttributeList&  attrL,
    const std::string&    subId,
    const std::string&    notifyUrl,
    Format                format,
    const std::string&    tenant
);

/* ****************************************************************************
*
* slashEscape - 
*
* When the 'to' buffer is full, slashEscape returns.
* No warnings, no nothing.
* Make sure 'to' is big enough!
*/
extern void slashEscape(const char* from, char* to, unsigned int toLen);

/* ****************************************************************************
*
* releaseTriggeredSubscriptions -
*
*/
extern void releaseTriggeredSubscriptions(std::map<std::string, TriggeredSubscription*>& subs);


/* ****************************************************************************
*
* fillQueryServicePath -
*
*/
extern BSONObj fillQueryServicePath(const std::vector<std::string>& servicePath);

/* ****************************************************************************
*
* fillContextProviders -
*
*/
extern void fillContextProviders(ContextElementResponse* cer, ContextRegistrationResponseVector& crrV);

/* ****************************************************************************
*
* someContextElementNotFound -
*
*/
extern bool someContextElementNotFound(ContextElementResponse& cer);

/* ****************************************************************************
*
* cprLookupByAttribute -
*
*/
extern void cprLookupByAttribute(EntityId&                          en,
                                 const std::string&                 attrName,
                                 ContextRegistrationResponseVector& crrV,
                                 std::string*                       perEntPa,
                                 Format*                            perEntPaFormat,
                                 std::string*                       perAttrPa,
                                 Format*                            perAttrPaFormat);


/* ****************************************************************************
*
* qStringFilters -
*/
extern bool qStringFilters(const std::string& in, std::vector<BSONObj> &filters, ContextElementResponse* cerP = NULL);

#endif
