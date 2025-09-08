[
    {
        "id": "1f3b5f8b5b1c4e2b",
        "type": "tab",
        "label": "ESP32 Car Control",
        "disabled": false,
        "info": ""
    },
    {
        "id": "c2c6a6227a28e2e1",
        "type": "mqtt out",
        "z": "1f3b5f8b5b1c4e2b",
        "name": "MQTT to ESP32",
        "topic": "car/control",
        "qos": "0",
        "retain": "false",
        "respTopic": "",
        "contentType": "",
        "userProps": "",
        "correl": "",
        "expiry": "",
        "broker": "6ac0a08a5c6e35f0",
        "x": 840,
        "y": 160,
        "wires": []
    },
    {
        "id": "f0bc032e2627d358",
        "type": "inject",
        "z": "1f3b5f8b5b1c4e2b",
        "name": "Forward",
        "props": [
            {
                "p": "payload"
            }
        ],
        "repeat": "",
        "crontab": "",
        "once": false,
        "onceDelay": 0.1,
        "topic": "",
        "payload": "forward",
        "payloadType": "str",
        "x": 330,
        "y": 80,
        "wires": [
            [
                "c2c6a6227a28e2e1",
                "b47c9be8ca2a060b"
            ]
        ]
    },
    {
        "id": "fc119c128f90b6b8",
        "type": "inject",
        "z": "1f3b5f8b5b1c4e2b",
        "name": "Backward",
        "props": [
            {
                "p": "payload"
            }
        ],
        "repeat": "",
        "crontab": "",
        "once": false,
        "onceDelay": 0.1,
        "topic": "",
        "payload": "backward",
        "payloadType": "str",
        "x": 330,
        "y": 120,
        "wires": [
            [
                "c2c6a6227a28e2e1",
                "b47c9be8ca2a060b"
            ]
        ]
    },
    {
        "id": "da46e49a02bc9a20",
        "type": "inject",
        "z": "1f3b5f8b5b1c4e2b",
        "name": "Left",
        "props": [
            {
                "p": "payload"
            }
        ],
        "repeat": "",
        "crontab": "",
        "once": false,
        "onceDelay": 0.1,
        "topic": "",
        "payload": "left",
        "payloadType": "str",
        "x": 330,
        "y": 160,
        "wires": [
            [
                "c2c6a6227a28e2e1",
                "b47c9be8ca2a060b"
            ]
        ]
    },
    {
        "id": "097d4f79a4a5f6cf",
        "type": "inject",
        "z": "1f3b5f8b5b1c4e2b",
        "name": "Right",
        "props": [
            {
                "p": "payload"
            }
        ],
        "repeat": "",
        "crontab": "",
        "once": false,
        "onceDelay": 0.1,
        "topic": "",
        "payload": "right",
        "payloadType": "str",
        "x": 330,
        "y": 200,
        "wires": [
            [
                "c2c6a6227a28e2e1",
                "b47c9be8ca2a060b"
            ]
        ]
    },
    {
        "id": "3b2eb85d8f8a76e7",
        "type": "inject",
        "z": "1f3b5f8b5b1c4e2b",
        "name": "Stop",
        "props": [
            {
                "p": "payload"
            }
        ],
        "repeat": "",
        "crontab": "",
        "once": false,
        "onceDelay": 0.1,
        "topic": "",
        "payload": "stop",
        "payloadType": "str",
        "x": 330,
        "y": 240,
        "wires": [
            [
                "c2c6a6227a28e2e1",
                "b47c9be8ca2a060b"
            ]
        ]
    },
    {
        "id": "b47c9be8ca2a060b",
        "type": "debug",
        "z": "1f3b5f8b5b1c4e2b",
        "name": "debug 2",
        "active": true,
        "tosidebar": true,
        "console": false,
        "tostatus": false,
        "complete": "false",
        "statusVal": "",
        "statusType": "auto",
        "x": 780,
        "y": 440,
        "wires": []
    },
    {
        "id": "6ac0a08a5c6e35f0",
        "type": "mqtt-broker",
        "name": "Mosquitto Local",
        "broker": "192.168.1.54",
        "port": "1883",
        "clientid": "",
        "autoConnect": true,
        "usetls": false,
        "protocolVersion": "4",
        "keepalive": "60",
        "cleansession": true,
        "autoUnsubscribe": true,
        "birthTopic": "",
        "birthQos": "0",
        "birthRetain": "false",
        "birthPayload": "",
        "closeTopic": "",
        "closeQos": "0",
        "closeRetain": "false",
        "closePayload": "",
        "willTopic": "",
        "willQos": "0",
        "willRetain": "false",
        "willPayload": ""
    }
]
