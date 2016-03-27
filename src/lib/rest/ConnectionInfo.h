#ifndef CONNECTION_INFO_H
#define CONNECTION_INFO_H

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
* Author: Ken Zangelin
*/
#include <stdint.h>
#include <time.h>
#include <string>
#include <vector>
#include <map>

#include "logMsg/logMsg.h"

#include "common/Format.h"
#include "parse/CompoundValueNode.h"
#include "rest/HttpStatusCode.h"
#include "rest/mhd.h"
#include "rest/Verb.h"
#include "rest/HttpHeaders.h"
#include "ngsi/Request.h"

struct ParseData;



/* ****************************************************************************
*
* ConnectionInfo - 
*/
class ConnectionInfo
{
public:
  ConnectionInfo():
    connection             (NULL),
    verb                   (NOVERB),
    inFormat               (JSON),
    outFormat              (JSON),
    tenant                 (""),
    servicePath            (""),
    payload                (NULL),
    payloadSize            (0),
    callNo                 (1),
    parseDataP             (NULL),
    port                   (0),
    ip                     (""),
    apiVersion             ("v1"),
    inCompoundValue        (false),
    compoundValueP         (NULL),
    compoundValueRoot      (NULL),
    httpStatusCode         (SccOk)
  {
    memset(payloadWord, 0, sizeof(payloadWord));
  }

  ConnectionInfo(Format _outFormat):
    connection             (NULL),
    verb                   (NOVERB),
    inFormat               (JSON),
    outFormat              (_outFormat),
    tenant                 (""),
    servicePath            (""),
    payload                (NULL),
    payloadSize            (0),
    callNo                 (1),
    parseDataP             (NULL),
    port                   (0),
    ip                     (""),
    apiVersion             ("v1"),
    inCompoundValue        (false),
    compoundValueP         (NULL),
    compoundValueRoot      (NULL),
    httpStatusCode         (SccOk)
  {
    memset(payloadWord, 0, sizeof(payloadWord));
  }

  ConnectionInfo(std::string _url, std::string _method, std::string _version, MHD_Connection* _connection = NULL):
    connection             (_connection),
    verb                   (NOVERB),
    inFormat               (JSON),
    outFormat              (JSON),
    url                    (_url),
    method                 (_method),
    version                (_version),
    tenant                 (""),
    servicePath            (""),
    payload                (NULL),
    payloadSize            (0),
    callNo                 (1),
    parseDataP             (NULL),
    port                   (0),
    ip                     (""),
    apiVersion             ("v1"),
    inCompoundValue        (false),
    compoundValueP         (NULL),
    compoundValueRoot      (NULL),
    httpStatusCode         (SccOk)
  {

    memset(payloadWord, 0, sizeof(payloadWord));

    if      (_method == "POST")    verb = POST;
    else if (_method == "PUT")     verb = PUT;
    else if (_method == "GET")     verb = GET;
    else if (_method == "DELETE")  verb = DELETE;
    else if (_method == "PATCH")   verb = PATCH;
    else                           verb = NOVERB;
  }

  ~ConnectionInfo()
  {
    if (compoundValueRoot != NULL)
      delete compoundValueRoot;

    servicePathV.clear();
  }

  MHD_Connection*            connection;
  Verb                       verb;
  Format                     inFormat;
  Format                     outFormat;
  std::string                url;
  std::string                method;
  std::string                version;
  std::string                charset;
  std::string                tenantFromHttpHeader;
  std::string                tenant;
  std::string                servicePath;
  std::vector<std::string>   servicePathV;
  HttpHeaders                httpHeaders;
  char*                      payload;
  int                        payloadSize;
  char                       payloadWord[64];
  std::string                answer;
  int                        callNo;
  ParseData*                 parseDataP;
  unsigned short             port;
  std::string                ip;
  std::string                apiVersion;
  RequestType                requestType;

  std::map<std::string, std::string>   uriParam;
  std::map<std::string, bool>          uriParamOptions;
  std::vector<std::string>             uriParamTypes;

  bool                       inCompoundValue;
  orion::CompoundValueNode*  compoundValueP;    // Points to current node in the tree
  orion::CompoundValueNode*  compoundValueRoot; // Points to the root of the tree
  ::std::vector<orion::CompoundValueNode*> compoundValueVector;

  // Outgoing
  HttpStatusCode            httpStatusCode;
  std::vector<std::string>  httpHeader;
  std::vector<std::string>  httpHeaderValue;

  // Timing
  struct timespec           reqStartTime;
};



/* ****************************************************************************
*
* uriParamOptionsParse - 
*/
extern int uriParamOptionsParse(ConnectionInfo* ciP, const char* value);



/* ****************************************************************************
*
* uriParamTypesParse - parse the URI param 'type' into uriParamTypes vector
*/
extern void uriParamTypesParse(ConnectionInfo* ciP, const char* value);

#endif
