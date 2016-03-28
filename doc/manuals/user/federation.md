# Context Broker Federation

This section described "push" federation (in the sense notifyContext
sent by one Orion instance are processed by other Orion instance).
However, the [registring Context Providers and request
forwarding]()
functionality can be used to implement a kind of "pull" federation (in
which one Orion instance fowards a query/update to another Orion
instance). Note that an importand difference between two approaches is
that in the "push" mode all the Orion instances update its local state,
while in the "push" approach all the intermediate Orion instances acts
as "proxy" without storing the data locally.

Apart from processing updateContext and registerContext (usually issued
by a client application) Orion Context Broker can process
notifyContextRequest and notifyContextAvailabilityRequest with the same
semantics. This opens the door to interesting federation scenarios (one
example is the [FIWARE Lab context management platform](https://forge.fiware.org/plugins/mediawiki/wiki/fiware/index.php/FIWARE_Lab_Context_Management_Platform)).

![](Federation.png "Federation.png")

Consider the following setup: three context broker instances running in
the same machine (of course, this is not a requirement but makes things
simpler to test this feature), in ports 1030, 1031 and 1032 respectively
and using different databases (named A, B and C to be brief). Let's
start each instance (run each command in a separate terminal):

    contextBroker -fg -port 1030 -db orion1030
    contextBroker -fg -port 1031 -db orion1031
    contextBroker -fg -port 1032 -db orion1032

Next, let's send a subscribeContext to A (to make B subscribe to updates
made in A). Note that the URL used in the reference has to be
"/v1/notifyContext":

```
(curl localhost:1030/v1/subscribeContext -s -S --header 'Content-Type: application/json' \
    --header 'Accept: application/json' -d @- | python -mjson.tool) <<EOF
{
    "entities": [
        {
            "type": "Room",
            "isPattern": "false",
            "id": "Room1"
        }
    ],
    "reference": "http://localhost:1031/v1/notifyContext",
    "duration": "P1M",
    "notifyConditions": [
        {
            "type": "ONCHANGE",
            "condValues": [
                "temperature"
            ]
        }
    ],
    "throttling": "PT5S"
}
EOF
```

Next, let's send a subscribeContext to B (to make C subscribe to updates
made in B). The subscription is basically the same, only the port in the
curl line and reference elements are different.

```
(curl localhost:1031/v1/subscribeContext -s -S --header 'Content-Type: application/json' \
    --header 'Accept: application/json' -d @- | python -mjson.tool) <<EOF
{
    "entities": [
        {
            "type": "Room",
            "isPattern": "false",
            "id": "Room1"
        }
    ],
    "reference": "http://localhost:1032/v1/notifyContext",
    "duration": "P1M",
    "notifyConditions": [
        {
            "type": "ONCHANGE",
            "condValues": [
                "temperature"
            ]
        }
    ],
    "throttling": "PT5S"
}
EOF
```

Now, let's create an entity in context broker A.

```
(curl localhost:1030/v1/updateContext -s -S --header 'Content-Type: application/json' \
    --header 'Accept: application/json' -d @- | python -mjson.tool) <<EOF
{
    "contextElements": [
        {
            "type": "Room",
            "isPattern": "false",
            "id": "Room1",
            "attributes": [
                {
                    "name": "temperature",
                    "type": "float",
                    "value": "23"
                }
            ]
        }
    ],
    "updateAction": "APPEND"
}
EOF
```

Given the subscriptions in place, a notifyContextRequest is
automatically sent from A to B. That event at B causes in sequence a
notifyContextRequest to be sent to C. So, at the end, the creation of an
entity in A causes the creation of the same entity (with the same
attribute values) in C. You can check it by doing a queryContext to C:

```
(curl localhost:1032/v1/queryContext -s -S --header 'Content-Type: application/json' \
    --header 'Accept: application/json' -d @- | python -mjson.tool) <<EOF
{
    "entities": [
        {
            "type": "Room",
            "isPattern": "false",
            "id": "Room1"
        }
    ]
}
EOF
```

which response is:

```
{
    "contextResponses": [
        {
            "contextElement": {
                "attributes": [
                    {
                        "name": "temperature",
                        "type": "float",
                        "value": "23"
                    }
                ],
                "id": "Room1",
                "isPattern": "false",
                "type": "Room"
            },
            "statusCode": {
                "code": "200",
                "reasonPhrase": "OK"
            }
        }
    ]
}
```

In the current context broker version, the semantics of
nofityContextRequest are the same that [updateContext
APPEND  or, if the context element already
exists, the semantics of updateContext UPDATE](walkthrough_apiv1.md#update-context-elements).
Thus, federation doesn't provide exact mirroring: an updateContext DELETE to
one context broker will not produce the same effect in the federated context broker.

This mechanism works similarly with registerContext and
subscribeContextAvailability. In this case, the URL for the reference
element is "/v1/registry/notifyContextAvailability".
