[
    {
        "id": "mqtt_in_qr",
        "type": "mqtt in",
        "z": "c977bb9df497f1bf",
        "name": "QR Data In",
        "topic": "qr_code/data",
        "qos": "0",
        "datatype": "auto",
        "broker": "mqtt_broker",
        "inputs": 0,
        "x": 340,
        "y": 260,
        "wires": [
            [
                "process_qr"
            ]
        ]
    },
    {
        "id": "process_qr",
        "type": "function",
        "z": "c977bb9df497f1bf",
        "name": "Process QR by Type (10s rule)",
        "func": "let records = flow.get('qrRecords') || {};\nlet now = Date.now();\nlet expireMs = 10000; // 10 วินาที\n\nlet typeMap = { 'LPG': 'LPG', 'CBI': 'CBI', 'BKK': 'BKK', 'CHA': 'CHA' };\nlet prefix = msg.payload.substring(0,3);\nlet type = typeMap[prefix] || 'LPG';\n\nif (!records[type]) {\n    records[type] = { value: type, count: 0, lastSeen: 0 };\n}\n\nlet rec = records[type];\n\nif (now - rec.lastSeen >= expireMs) {\n    rec.count += 1;\n    rec.lastSeen = now;\n\n    // ส่ง MQTT ตามชื่อประเภท เช่น qr/LPG\n    node.send([null, { topic: \"qr/\"+type, payload: 1 }]);\n}\n\nflow.set('qrRecords', records);\n\nlet table = Object.values(records).map(r => ({\n    value: r.value,\n    count: r.count,\n    lastSeen: new Date(r.lastSeen).toLocaleTimeString()\n}));\n\nreturn [{ payload: table }, null];",
        "outputs": 2,
        "x": 620,
        "y": 260,
        "wires": [
            [
                "ui_table"
            ],
            [
                "mqtt_dynamic"
            ]
        ]
    },
    {
        "id": "ui_table",
        "type": "ui_table",
        "z": "c977bb9df497f1bf",
        "group": "group_main",
        "name": "QR Counts",
        "order": 1,
        "width": 12,
        "height": 8,
        "columns": [
            {
                "field": "value",
                "title": "Type",
                "formatter": "plaintext",
                "align": "center"
            },
            {
                "field": "count",
                "title": "Count",
                "formatter": "plaintext",
                "align": "center"
            },
            {
                "field": "lastSeen",
                "title": "Last Seen",
                "formatter": "plaintext",
                "align": "left"
            }
        ],
        "outputs": 0,
        "x": 910,
        "y": 260,
        "wires": []
    },
    {
        "id": "mqtt_dynamic",
        "type": "mqtt out",
        "z": "c977bb9df497f1bf",
        "name": "MQTT per Type",
        "topic": "",
        "qos": "",
        "retain": "",
        "broker": "mqtt_broker",
        "x": 910,
        "y": 320,
        "wires": []
    },
    {
        "id": "inject_refresh",
        "type": "inject",
        "z": "c977bb9df497f1bf",
        "name": "Refresh every 2s",
        "props": [],
        "repeat": "2",
        "once": true,
        "topic": "",
        "x": 350,
        "y": 360,
        "wires": [
            [
                "refresh_table"
            ]
        ]
    },
    {
        "id": "refresh_table",
        "type": "function",
        "z": "c977bb9df497f1bf",
        "name": "Build Table",
        "func": "let records = flow.get('qrRecords') || {};\nlet table = Object.values(records).map(r => ({\n    value: r.value,\n    count: r.count,\n    lastSeen: new Date(r.lastSeen).toLocaleTimeString()\n}));\nmsg.payload = table;\nreturn msg;",
        "outputs": 1,
        "x": 620,
        "y": 360,
        "wires": [
            [
                "ui_table"
            ]
        ]
    },
    {
        "id": "btn_clear",
        "type": "ui_button",
        "z": "c977bb9df497f1bf",
        "name": "Clear Data",
        "group": "group_main",
        "order": 2,
        "width": 3,
        "height": 1,
        "label": "Clear",
        "payload": "clear",
        "payloadType": "str",
        "x": 350,
        "y": 460,
        "wires": [
            [
                "clear_records"
            ]
        ]
    },
    {
        "id": "clear_records",
        "type": "function",
        "z": "c977bb9df497f1bf",
        "name": "Clear Records",
        "func": "flow.set('qrRecords', {});\nmsg.payload = [];\nreturn msg;",
        "outputs": 1,
        "x": 620,
        "y": 460,
        "wires": [
            [
                "ui_table"
            ]
        ]
    },
    {
        "id": "mqtt_broker",
        "type": "mqtt-broker",
        "name": "Local MQTT",
        "broker": "localhost",
        "port": "1883",
        "clientid": "",
        "autoConnect": true,
        "usetls": false,
        "protocolVersion": 4,
        "keepalive": 15,
        "cleansession": true,
        "autoUnsubscribe": true,
        "birthTopic": "",
        "birthQos": "0",
        "birthPayload": "",
        "birthMsg": {},
        "closeTopic": "",
        "closePayload": "",
        "closeMsg": {},
        "willTopic": "",
        "willQos": "0",
        "willPayload": "",
        "willMsg": {},
        "userProps": "",
        "sessionExpiry": ""
    },
    {
        "id": "group_main",
        "type": "ui_group",
        "name": "QR Data",
        "tab": "tab_main",
        "order": 1,
        "disp": true,
        "width": "14",
        "collapse": false,
        "className": ""
    },
    {
        "id": "tab_main",
        "type": "ui_tab",
        "name": "Main",
        "icon": "dashboard",
        "order": 1
    },
    {
        "id": "22c958f0db5dc437",
        "type": "global-config",
        "env": [],
        "modules": {
            "node-red-node-ui-table": "0.4.5",
            "node-red-dashboard": "3.6.6"
        }
    }
]