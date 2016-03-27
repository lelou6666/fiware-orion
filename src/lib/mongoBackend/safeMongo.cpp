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

#include "mongoBackend/safeMongo.h"
#include "ngsi/SubscriptionId.h"
#include "ngsi/RegistrationId.h"
#include "ngsi/StatusCode.h"
#include "logMsg/logMsg.h"

using namespace mongo;

/* ****************************************************************************
*
* getObjectField -
*/
BSONObj getObjectField(const BSONObj& b, const std::string& field, const std::string& caller, int line)
{
  if (b.hasField(field) && b.getField(field).type() == Object)
  {
    return b.getObjectField(field);
  }

  // Detect error
  if (!b.hasField(field))
  {
    LM_E(("Runtime Error (object field '%s' is missing in BSONObj <%s> from caller %s:%d)", field.c_str(), b.toString().c_str(), caller.c_str(), line));
  }
  else
  {
    LM_E(("Runtime Error (field '%s' was supposed to be an object but type=%d in BSONObj <%s> from caller %s:%d)",
          field.c_str(), b.getField(field).type(), b.toString().c_str(), caller.c_str(), line));
  }
  return BSONObj();
}

/* ****************************************************************************
*
* getStringField -
*/
std::string getStringField(const BSONObj& b, const std::string& field, const std::string& caller, int line)
{
  if (b.hasField(field) && b.getField(field).type() == String)
  {
    return b.getStringField(field);
  }

  // Detect error
  if (!b.hasField(field))
  {
    LM_E(("Runtime Error (string field '%s' is missing in BSONObj <%s> from caller %s)", field.c_str(), b.toString().c_str(), caller.c_str(), line));
  }
  else
  {
    LM_E(("Runtime Error (field '%s' was supposed to be a string but type=%d in BSONObj <%s> from caller %s:%d)",
          field.c_str(), b.getField(field).type(), b.toString().c_str(), caller.c_str(), line));
  }
  return "";
}

/* ****************************************************************************
*
* getIntField -
*/
int getIntField(const BSONObj& b, const std::string& field, const std::string& caller, int line)
{
  if (b.hasField(field) && b.getField(field).type() == NumberInt)
  {
    return b.getIntField(field);
  }

  // Detect error
  if (!b.hasField(field))
  {
    LM_E(("Runtime Error (int field '%s' is missing in BSONObj <%s> from caller %s:%d)", field.c_str(), b.toString().c_str(), caller.c_str(), line));
  }
  else
  {
    LM_E(("Runtime Error (field '%s' was supposed to be an int but type=%d in BSONObj <%s> from caller %s:%d)",
          field.c_str(), b.getField(field).type(), b.toString().c_str(), caller.c_str(), line));
  }  
  return -1;
}

/* ****************************************************************************
*
* getLongField -
*/
long long getLongField(const BSONObj& b, const std::string& field, const std::string& caller, int line)
{
  if (b.hasField(field) && (b.getField(field).type() == NumberLong))
  {
    return b.getField(field).Long();
  }

  // Detect error
  if (!b.hasField(field))
  {
    LM_E(("Runtime Error (long field '%s' is missing in BSONObj <%s> from caller %s:%d)", field.c_str(), b.toString().c_str(), caller.c_str(), line));
  }
  else
  {
    LM_E(("Runtime Error (field '%s' was supposed to be a long but type=%d in BSONObj <%s> from caller %s:%d)",
          field.c_str(), b.getField(field).type(), b.toString().c_str(), caller.c_str(), line));
  }  

  return -1;
}

/* ****************************************************************************
*
* getIntOrLongFieldAsLong -
*/
long long getIntOrLongFieldAsLong(const BSONObj& b, const std::string& field, const std::string& caller, int line)
{
  if (b.hasField(field))
  {
    if (b.getField(field).type() == NumberLong)
    {
      return b.getField(field).Long();
    }
    else if (b.getField(field).type() == NumberInt)
    {
      return b.getField(field).Int();
    }
  }

  // Detect error
  if (!b.hasField(field))
  {
    LM_E(("Runtime Error (int/long field '%s' is missing in BSONObj <%s> from caller %s:%d)", field.c_str(), b.toString().c_str(), caller.c_str(), line));
  }
  else
  {
    LM_E(("Runtime Error (field '%s' was supposed to be int or long but type=%d in BSONObj <%s> from caller %s:%d)",
          field.c_str(), b.getField(field).type(), b.toString().c_str(), caller.c_str(), line));
  }  

  return -1;
}

/* ****************************************************************************
*
* getBoolField -
*/
bool getBoolField(const BSONObj& b, const std::string& field, const std::string& caller, int line)
{
  if (b.hasField(field) && b.getField(field).type() == Bool)
  {
    return b.getBoolField(field);
  }

  // Detect error
  if (!b.hasField(field))
  {
    LM_E(("Runtime Error (bool field '%s' is missing in BSONObj <%s>)", field.c_str(), b.toString().c_str()));
  }
  else 
  {
    LM_E(("Runtime Error (field '%s' was supposed to be a bool but type=%d in BSONObj <%s> from caller %s:%d)",
          field.c_str(), b.getField(field).type(), b.toString().c_str()));
  }
  return false;
}

/* ****************************************************************************
*
* getField -
*/
BSONElement getField(const BSONObj& b, const std::string& field, const std::string& caller, int line)
{
  if (b.hasField(field))
  {
    return b.getField(field);
  }

  LM_E(("Runtime Error (field '%s' is missing in BSONObj <%s> from caller %s:%d)", field.c_str(), b.toString().c_str(), caller.c_str(), line));
  return BSONElement();
}

/* ****************************************************************************
*
* moreSafe -
*
* This is a safe version of the more() method for cursors in order to avoid
* exception that may crash the broker. However, if the more() method is returning
* an exception something really bad is happening, it is considered a Fatal Error.
*
* (The name resembles the same relationship between next() and nextSafe() in the
* mongo driver)
*
*/
bool moreSafe(const std::auto_ptr<DBClientCursor>& cursor)
{
  try
  {
    return cursor->more();
  }
  catch (std::exception &e)
  {
    LM_E(("Fatal Error (more() exception: %s)", e.what()));
    return false;
  }
  catch (...)
  {
    LM_E(("Fatal Error (more() exception: generic)"));
    return false;
  }
}

/* ****************************************************************************
*
* nextSafeOrError -
*
*/
bool nextSafeOrError(const std::auto_ptr<DBClientCursor>& cursor, BSONObj* r, std::string* err, const std::string& caller, int line)
{
  try
  {
    *r = cursor->nextSafe();
    return true;
  }
  catch (const std::exception &e)
  {
    char lineString[STRING_SIZE_FOR_INT];
    snprintf(lineString, sizeof(lineString), "%d", line);
    *err = std::string(e.what()) + " at " + caller + ":" + lineString;
    return false;
  }
  catch (...)
  {
    char lineString[STRING_SIZE_FOR_INT];
    snprintf(lineString, sizeof(lineString), "%d", line);
    *err = "generic exception at " + caller + ":" + lineString;
    return false;
  }
}

/* ****************************************************************************
*
* safeGetSubId -
*
*/
bool safeGetSubId(const SubscriptionId& subId, OID* id, StatusCode* sc)
{
  try
  {
    *id = OID(subId.get());
    return true;
  }
  catch (const AssertionException &e)
  {
    // FIXME P3: this check is "defensive", but from a efficiency perspective this should be short-cut at
    // parsing stage. To check it. The check should be for: [0-9a-fA-F]{24}.
    sc->fill(SccContextElementNotFound);
    return false;
  }
  catch (const std::exception &e)
  {
    sc->fill(SccReceiverInternalError, e.what());
    return false;
  }
  catch (...)
  {
    sc->fill(SccReceiverInternalError, "generic exception");
    return false;
  }
}

/* ****************************************************************************
*
* safeGetRegId -
*
*/
bool safeGetRegId(const RegistrationId& regId, OID* id, StatusCode* sc)
{
  try
  {
    *id = OID(regId.get());
    return true;
  }
  catch (const AssertionException &e)
  {
    // FIXME P3: this check is "defensive", but from a efficiency perspective this should be short-cut at
    // parsing stage. To check it. The check should be for: [0-9a-fA-F]{24}.
    sc->fill(SccContextElementNotFound);
    return false;
  }
  catch (const std::exception &e)
  {
    sc->fill(SccReceiverInternalError, e.what());
    return false;
  }
  catch (...)
  {
    sc->fill(SccReceiverInternalError, "generic exception");
    return false;
  }
}
