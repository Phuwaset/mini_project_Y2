[
    {
        "id": "00e3e802397beb7e",
        "type": "tab",
        "label": "Flow 1",
        "disabled": false,
        "info": "",
        "env": []
    },
    {
        "id": "48d1995c2c664901",
        "type": "mqtt in",
        "z": "00e3e802397beb7e",
        "name": "QR Data",
        "topic": "qr_code/data",
        "qos": "0",
        "datatype": "auto",
        "broker": "mqtt_broker",
        "nl": false,
        "rap": false,
        "inputs": 0,
        "x": 160,
        "y": 100,
        "wires": [
            [
                "e0ebf8bcc2cbe7d6",
                "d9cec7491664cdcf",
                "4d7984e6b6de803b"
            ]
        ]
    },
    {
        "id": "e0ebf8bcc2cbe7d6",
        "type": "function",
        "z": "00e3e802397beb7e",
        "name": "Store QR Data",
        "func": "// เก็บ QR ลง flow\nlet records = flow.get('qrRecords') || [];\n\n// กำหนดประเภทตาม prefix\nlet typeMap = { 'LPG':1, 'CBI':2, 'BKK':3, 'CHA':4 };\nlet prefix = msg.payload.substring(0,3);\nlet type = typeMap[prefix] || 1;\n\nrecords.unshift({\n    timestamp: new Date().toLocaleString(),\n    value: msg.payload,\n    type: type\n});\n\nflow.set('qrRecords', records);\n\nmsg.payload = records;\nreturn msg;",
        "outputs": 1,
        "x": 400,
        "y": 100,
        "wires": [
            [
                "a195891be61eb4a2"
            ]
        ]
    },
    {
        "id": "a195891be61eb4a2",
        "type": "ui_table",
        "z": "00e3e802397beb7e",
        "group": "group_main",
        "name": "QR Records",
        "order": 1,
        "width": 12,
        "height": 8,
        "columns": [
            {
                "field": "timestamp",
                "title": "Time",
                "formatter": "plaintext",
                "align": "left"
            },
            {
                "field": "value",
                "title": "Value",
                "formatter": "plaintext",
                "align": "left"
            },
            {
                "field": "type",
                "title": "Type",
                "formatter": "plaintext",
                "align": "center"
            }
        ],
        "outputs": 0,
        "x": 650,
        "y": 100,
        "wires": []
    },
    {
        "id": "d9cec7491664cdcf",
        "type": "function",
        "z": "00e3e802397beb7e",
        "name": "Prepare Chart Data",
        "func": "// นับจำนวน QR ตามประเภท\nlet records = flow.get('qrRecords') || [];\nlet counts = {1:0,2:0,3:0,4:0};\nrecords.forEach(r => { if(counts[r.type]!==undefined) counts[r.type]++; });\n\nmsg.payload = [{\n    series: [\"LPG\",\"CBI\",\"BKK\",\"CHA\"],\n    data: [[counts[1],counts[2],counts[3],counts[4]]],\n    labels: [\"QR Types\"]\n}];\n\nreturn msg;",
        "outputs": 1,
        "timeout": "",
        "noerr": 0,
        "initialize": "",
        "finalize": "",
        "libs": [],
        "x": 400,
        "y": 200,
        "wires": [
            []
        ]
    },
    {
        "id": "21e577e0e750e0d5",
        "type": "ui_chart",
        "z": "00e3e802397beb7e",
        "name": "QR Chart",
        "group": "group_main",
        "order": 2,
        "width": "1",
        "height": "1",
        "label": "QR Types Count",
        "chartType": "bar",
        "legend": "true",
        "xformat": "HH:mm:ss",
        "interpolate": "linear",
        "nodata": "No Data",
        "dot": false,
        "ymin": 0,
        "ymax": "",
        "removeOlder": 1,
        "removeOlderPoints": "",
        "removeOlderUnit": "3600",
        "cutout": 0,
        "useOneColor": false,
        "useUTC": false,
        "colors": [
            "#1f77b4",
            "#ff7f0e",
            "#2ca02c",
            "#d62728",
            "#000000",
            "#000000",
            "#000000",
            "#000000",
            "#000000"
        ],
        "outputs": 1,
        "useDifferentColor": false,
        "className": "",
        "x": 660,
        "y": 200,
        "wires": [
            []
        ]
    },
    {
        "id": "efafe76cee8ed5a0",
        "type": "ui_button",
        "z": "00e3e802397beb7e",
        "name": "Clear Data",
        "group": "group_main",
        "order": 3,
        "width": 3,
        "height": 1,
        "label": "Clear",
        "payload": "clear",
        "payloadType": "str",
        "x": 160,
        "y": 300,
        "wires": [
            [
                "cc92348d4de41cd6"
            ]
        ]
    },
    {
        "id": "cc92348d4de41cd6",
        "type": "function",
        "z": "00e3e802397beb7e",
        "name": "Clear QR Records",
        "func": "flow.set('qrRecords', []);\nmsg.payload = [];\nreturn msg;",
        "outputs": 1,
        "x": 400,
        "y": 300,
        "wires": [
            [
                "a195891be61eb4a2"
            ]
        ]
    },
    {
        "id": "4d7984e6b6de803b",
        "type": "debug",
        "z": "00e3e802397beb7e",
        "name": "debug 1",
        "active": true,
        "tosidebar": true,
        "console": false,
        "tostatus": false,
        "complete": "false",
        "statusVal": "",
        "statusType": "auto",
        "x": 660,
        "y": 320,
        "wires": []
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
        "id": "4602432c7fb0bcb1",
        "type": "global-config",
        "env": [],
        "modules": {
            "node-red-node-ui-table": "0.4.5",
            "node-red-dashboard": "3.6.6"
        }
    }
]
