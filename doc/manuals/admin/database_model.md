#<a name="top"></a>Data model

* [Introduction](#introduction)
* [entities collection](#entities-collection)
* [registrations collection](#registrations-collection)
* [csubs collection](#csubs-collection)
* [casubs collection](#casubs-collection)

## Introduction

Normally you don't need to access MongoDB directly as Orion Contex
Broker uses it transparently. However, for some operations (e.g. backup,
fault recovery, etc.) it is useful to know how the database is
structured. This section provides that information.

In the case you need to access database directly, be very careful when manipulation it, as some actions could be irreversible ([doing a
backup](database_admin.md#backing-up-and-restoring-database) at the beginning
it's a good idea).

Orion Context Broker uses four collections in the database, described in
the following subsections.

[Top](#top)

## entities collection

The *entities* collection stores information about NGSI entities. Each
document in the collection corresponds to an entity.

Fields:

-   **\_id** stores the EntityID, including the ID itself and type.
    Given that we use \_id for this, we ensure that EntityIDs
    are unique. The JSON document for this field includes:
    -   **id**: entity NGSI ID
    -   **type**: entity NGSI type
    -   **servicePath**: related with [the service
        path](../user/service_path.md) functionality.
-   **attrs** is an keymap of the different attributes that have been
    created for that entity. The key is generated with the attribute
    name (changing "." for "=", as "." is not a valid character in
    MongoDB document keys), appending '\_\_<id>' in the case of having
    and ID. For example, the attribute with name "my.attr" with ID "id1"
    will use the following key: "my=attr\_\_id2". Each one of the
    elements in the map has the following information:
    -   **type**: the attribute type
    -   **value**: the attribute value (for those attribute that has
        received at least one update). Up to version 0.10.1, this value
        is always a string, but in 0.11.0 this value can be also a JSON
        object or JSON vector to represent an structured value (see
        section about [structured attribute values in user
        manual](../user/structured_attribute_valued.md)).
    -   **md** (optional): custom metadata. This is a vector of metadata
        objects, each one with a **name**, **type** and **value**.
    -   **creDate**: the timestamp corresponding to attribute creation
        (as a consequence of append).
    -   **modDate**: the timestamp corresponding to last
        attribute update. It matches creDate if the attribute has not be
        modified after creation.
-   **attrNames**: an array of string. Its elements are the names of the
    attributes of the entity (without IDs). In this case, the "." to "="
    replacement is not done.
-   **creDate**: the timestamp corresponding
    to entity creation date (as a consequence of append).
-   **modDate**: the timestamp corresponding to last entity update. Note
    that it uses to be the same that a modDate corresponding to at least
    one of the attributes (not always: it will not be the same if the
    last update was a DELETE operation). It matches creDate if the
    entity has not be modified after creation.
-   **location** (optional): geographic location of the entity, composed
    of the following fields:
    -   **attrName**: the attribute name that identifies the geographic
        location in the attrs array
    -   **coords**: a GJSON of type "Point", containing the longitude
        and latitude, in that order.
        -   *Note*: you may wonder why the coordinates are stored in the
            coords duple as longitude-latitude and in the opposite order
            in the [geo-location
            API](../user/geolocation.md).
            This is due to the internal [MongoDB geolocation
            functionality](http://docs.mongodb.org/manual/tutorial/query-a-2dsphere-index/),
            which uses longitude-latitude order. However, other systems
            closer to users (e.g. Google Maps) use latitude-longitude
            format, so we have used the latter for the API.

Example document:

```
 {
   "_id":
       "id": "E1",
       "type": "T1",
       "servicePath": "/"
   },
   "attrs": {
       "A1": {
           "type": "TA1",
           "value": "282",
           "creDate" : 1389376081,
           "modDate" : 1389376120,
           "md" : [
              { 
                 "name" : "customMD1",
                 "type" : "string",
                 "value" : "AKAKA"
              },
              {
                 "name" : "customMD2",
                 "type" : "integer",
                 "value" : "23232"
              }
           ]
       },
       "A2_ID101": {
           "type": "TA2",
           "value": "176",
           "creDate" : 1389376244,
           "modDate" : 1389376244
       },
       "position": {
           "type": "location",
           "value": "40.418889, -3.691944",
           "creDate" : 1389376244,
           "modDate" : 1389376244
       }
   },
   "attrNames": [ "A1", "A2", "position" ],
   "creDate": 1389376081,
   "modDate": 1389376244,
   "location": {
       "attrName": "position",
       "coords": {
           "type": "Point",
           "coordinates": [ -3.691944, 40.418889 ]
       }
   }
 }
```
[Top](#top)

## registrations collection

The *registrations* collection stores information about NGSI9
registrations. Each document in the collection corresponds to a
registration.

Fields:

-   **\_id** is the registration ID (the value that is provided to the
    user to update the registration). Given that we use \_id for this,
    we ensure that registration IDs are unique and that queries by
    registration IDs will be very fast (as there is an automatic default
    index in \_id).
-   **servicePath**: related with [the service
    path](../user/service_path.md) functionality.
-   **fwdRegId**: the ID corresponding to the registration in the
    forward Context Broker. Used only in "ConfMan mode", (see
    [programmers
    guide](../deprecated.md#configuration-manager-role)).
-   **expiration**: this is the timestamp for which the
    registration expires. The expiration is calculated using the
    duration parameter included in the registerContext operation
    (basically, sum "now" and duration) and will be recalculated when a
    registerContext for updating (i.e. using a not null registration ID
    in the request) is received (see [programmers
    guide](../user/duration.md)).
-   **contextRegistration**: is an array whose elements contain the
    following information:
    -   **entities**: an array containing a list of
        entities (mandatory). The JSON for each entity contains **id**,
        **type** and **isPattern**.
    -   **attrs**: an array containing a list of attributes (optional).
        The JSON for each attribute contains **name**, **type** and
        **isDomain**.
    -   **providingApplication**: the URL of the providing application
        for this registration (mandatory)

Example document:

```
 {
   "_id": ObjectId("5149f60cf0075f2fabca43da"),
   "fwdRegId": "5149f60cf0075f241bca22f1",
   "expiration": 1360232760,
   "contextRegistration": [
       {
           "entities": [
               {
                   "id": "E1",
                   "type": "T1",
                   "isPattern": "false"
               },
               {
                   "id": "E2",
                   "type": "T2",
                   "isPattern": "false"
               }
           ],
           "attrs": [
               {
                   "name": "A1",
                   "type": "TA1",
                   "isDomain": "false"
               },
               {
                   "name": "A2",
                  "type": "TA2",
                   "isDomain": "true"
               }
           ],
           "providingApplication": "http://foo.bar/notif"
      },
  ]
 }
```
[Top](#top)

## csubs collection

The *csubs* collection stores information about NGSI10 subscriptions.
Each document in the collection corresponds to a subscription.

Fields:

-   **\_id** is the subscription ID (the value that is provided to the
    user to update and cancel the subscription). Given that we use \_id
    for this, we ensure that subscription IDs are unique and that
    queries by subscription IDs are very fast (as there is an automatic
    default index in \_id).
-   **servicePath**: related with [the service
    path](../user/service_path.md) functionality.
-   **expiration**: this is the timestamp on which the
    subscription expires. This is calculated using the duration
    parameter included in the subscribeContext operation (basically, sum
    "now" and duration) and will be recalculated when an
    updateContextSubscription is received (see [programmers
    guide](../user/duration.md)). For permanent subscriptions (allowed in NGSIv2)
    it is set to -1.
-   **lastNotification**: the time when last notification was sent. This
    is updated each time a notification is sent, to avoid
    violating throttling.
-   **throttling**: minimum interval between notifications.
-   **reference**: the URL for notifications
-   **entities**: an array of entities (mandatory). The JSON for each
    entity contains **id**, **type** and **isPattern**.
-   **attrs**: an array of attribute names (strings) (optional).
-   **conditions**: a list of conditions that trigger notifications.
-   **expression**: an expression used to evaluate if notifications has
    to be sent or not when updates come. It may be composed of the following
    fields: q, georel, geometry and/or coords (optional)
-   **count**: the number of notifications sent associated to
    the subscription.
-   **format**: the format to use to send notification, either "XML" or "JSON".
    However, note that XML has been deprecated in Orion 0.23.0 and that this field eventually will be removed.

Example document:

```
{
        "_id" : ObjectId("5697d4d123acbf5e794ab031"),
        "expiration" : NumberLong(1459864800),
        "reference" : "http://localhost:1234",
        "servicePath" : "/",
        "entities" : [
                {
                        "id" : ".*",
                        "type" : "Room",
                        "isPattern" : "true"
                }
        ],
        "attrs" : [
                "humidity",
                "temperature"
        ],
        "conditions" : [
                {
                        "type" : "ONCHANGE",
                        "value" : [
                                "temperature "
                        ]
                }
        ],
        "expression" : {
                "q" : "temperature>40",
                "geometry" : "",
                "coords" : "",
                "georel" : ""
        },
        "format" : "JSON"
}
```
[Top](#top)

## casubs collection

The *casubs* collection stores information about NGSI9 subscriptions.
Each document in the collection corresponds to a subscription.

Fields:

-   **\_id** is the subscription ID (the value that is provided to the
    user to update and cancel the subscription). Given that we use \_id
    for this, we ensure that subscription IDs are unique and that
    queries by subscription IDs are very fast (as there is an automatic
    default index in \_id).
-   **expiration**: this is the timestamp on which the subscription
    will expire. It is calculated using the duration parameter included
    in the subscribeContextAvailability operation (basically, sum "now"
    and duration) and will be recalculated when an
    updateContextAvailabilitySubscription is received (see [programmers
    guide](../user/duration.md)).
-   **reference**: the URL to send notifications
-   **entities**: an array of entities (mandatory). The JSON for each
    entity contains **id**, **type** and **isPattern**.
-   **attrs**: an array of attribute names (strings) (optional).
-   **lastNotification**: timestamp corresponding to the last
    notification sent associated to a given subscription.
-   **count**: the number of notifications sent associated to
    the subscription.
-   **format**: the format to use to send notification, either "XML" or "JSON".
    However, note that XML has been deprecated in Orion 0.23.0 and that this field eventually will be removed.

Example document:

```
 {
   "_id": ObjectId("51756c2220be8dc1b5f415ff"),
   "expiration": 1360236300,
   "reference": "`[`http://notify.me`](http://notify.me)`",
   "entities": [
       {
           "id": "E5",
           "type": "T5",
           "isPattern": "false"
       },
       {
           "id": "E6",
           "type": "T6",
           "isPattern": "false"
       }
   ],
   "attrs": [
       "A1",
       "A2"
   ],
   "lastNotification" : 1381132312,
   "count": 42,
   "format": "JSON"
 }
```
[Top](#top)
