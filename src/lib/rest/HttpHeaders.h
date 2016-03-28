#ifndef HTTP_HEADERS_H
#define HTTP_HEADERS_H

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
#include <string>



/* ****************************************************************************
*
* HttpHeaders - 
*/
typedef struct HttpHeaders
{
  HttpHeaders()
  {
    gotHeaders          = false;
    servicePathReceived = false;
    contentLength       = 0;
  }

  bool          gotHeaders;
  std::string   userAgent;
  std::string   host;
  std::string   accept;
  std::string   expect;
  std::string   contentType;
  std::string   origin;
  std::string   tenant;
  std::string   servicePath;
  std::string   xauthToken;
  std::string   xforwardedFor;

  bool          servicePathReceived;

  unsigned int  contentLength;
  std::string   connection;
} HttpHeaders;

#endif
