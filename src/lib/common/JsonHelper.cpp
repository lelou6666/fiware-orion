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
* Author: Orion dev team
*/

#include "common/JsonHelper.h"
#include "common/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iomanip>


/* ****************************************************************************
*
* toJsonString -
*/
std::string toJsonString(const std::string& input)
{
  std::ostringstream ss;

  ss << '"';
  for (std::string::const_iterator iter = input.begin(); iter != input.end(); ++iter)
  {
    /* FIXME P3: This function ensures that if the DB holds special characters (which are
     * not supported in JSON according to its specification), they are converted to their escaped
     * representations. The process wouldn't be necessary if the DB couldn't hold such special characters, 
     * but as long as we support NGSIv1, it is better to have the check (e.g. a newline could be 
     * used in an attribute value using XML). Even removing NGSIv1, we have to ensure that the 
     * input parser (rapidjson) doesn't inject not supported JSON characters in the DB (this needs to be
     * investigated in the rapidjson documentation)
     *
     * JSON specification is a bit obscure about the need of escaping / (what they call 'solidus'). The
     * picture at JSON specification (http://www.json.org/) seems suggesting so, but after a careful reading of
     * https://tools.ietf.org/html/rfc4627#section-2.5, we can conclude it is not mandatory. Online checkers
     * such as http://jsonlint.com confirm this. Looking in some online discussions
     * (http://andowebsit.es/blog/noteslog.com/post/the-solidus-issue/ and
     * https://groups.google.com/forum/#!topic/opensocial-and-gadgets-spec/FkLsC-2blbo) it seems that
     * escaping / may have sense in some situations related with JavaScript code, which is not the case of Orion.
     */
    switch (char ch = *iter)
    {
    case '\\': ss << "\\\\"; break;
    case '"': ss << "\\\""; break;    
    case '\b': ss << "\\b"; break;
    case '\f': ss << "\\f"; break;
    case '\n': ss << "\\n"; break;
    case '\r': ss << "\\r"; break;
    case '\t': ss << "\\t"; break;
    default:
      /* Converting the rest of special chars 0-31 to \u00xx. Note that 0x80 - 0xFF are untouched as they
       * correspond to UTF-8 multi-byte characters */
      if (ch >= 0 && ch <= 0x1F)
      {
        static const char intToHex[16] =  { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' } ;

        ss << "\\u00" << intToHex[(ch & 0xF0) >> 4] << intToHex[ch & 0x0F];
      }
      else
      {
        ss << ch;
      }
      break;
    } //end-switch

  } //end-for
  ss << '"';
  return ss.str();
}



/* ****************************************************************************
*
* vectorToJson -
*/
template <>
std::string vectorToJson(std::vector<std::string> &list)
{
  switch (list.size())
  {
  case 0:
    return "[]";

  case 1:
    return "[ " + toJsonString(list[0]) + " ]";

  default:
    std::ostringstream os;
    os << '[';
    os << toJsonString(list[0]);
    for (std::vector<std::string>::size_type i = 1; i != list.size(); ++i)
    {
      os << ',' << toJsonString(list[i]);
    }
    os << ']';
    return os.str();
  }
}


/* ****************************************************************************
*
* JsonHelper -
*/
JsonHelper::JsonHelper(): empty(true)
{
  /* Set format  for floats (it doesn't affect integers) */
  ss << std::fixed << std::setprecision(9);

  ss << '{';
}



/* ****************************************************************************
*
* JsonHelper::addString -
*/
void JsonHelper::addString(const std::string& key, const std::string& value)
{
  if (!empty)
  {
    ss << ',';
  }
  ss << toJsonString(key) << ':' << toJsonString(value);

  empty = false;
}



/* ****************************************************************************
*
* JsonHelper::addRaw -
*/
void JsonHelper::addRaw(const std::string& key, const std::string& value)
{
  if (!empty)
  {
    ss << ',';
  }
  ss << toJsonString(key) << ':' << value;

  empty = false;
}


/* ****************************************************************************
*
* JsonHelper::addNumber -
*/
void JsonHelper::addNumber(const std::string& key, long long value)
{
  if (!empty)
  {
    ss << ',';
  }
  ss << toJsonString(key) << ':' << value;

  empty = false;
}

/* ****************************************************************************
*
* JsonHelper::addFloat -
*/
void JsonHelper::addFloat(const std::string& key, float  value)
{
  if (!empty)
  {
    ss << ',';
  }  
  ss << toJsonString(key) << ':' << value;

  empty = false;
}

/* ****************************************************************************
*
* JsonHelper::addDate -
*/
void JsonHelper::addDate(const std::string& key, long long timestamp)
{
  if (!empty)
  {
    ss << ',';
  }
  ss << toJsonString(key) << ':' << toJsonString(isodate2str(timestamp));

  empty = false;
}



/* ****************************************************************************
*
* JsonHelper::str -
*/
std::string JsonHelper::str()
{
  ss << '}';
  return ss.str();
}
