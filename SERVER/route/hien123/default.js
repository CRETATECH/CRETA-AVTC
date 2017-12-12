/*************** Not configure **************/
const user = 'hien123';

var main = require('../../main.js');
var app = main.app;
var io = main.io;
var express = main.express;
main.updateViews('views/' + user);

var mysql = require('mysql');
var db

var mqtt = require('mqtt')
var tcp, ws

var e = require(main.PWD + '/route/black/errorLog.js');

var bodyParser = require('body-parser');
var urlencodedParser = bodyParser.urlencoded({ extended: false })

var events = require('events');
var eEmit = new events.EventEmitter()

var _ = require('lodash');

var fs = require('fs');

var futures = require('futures');
var seq = futures.sequence();

var errLog
var tCtrl = [], tScanInterval;
var check = [];
var devStatus = {};
var ruleList = 'sRules';
var history = 'tcpSlave';
var wsHistory = "wsMaster";
var listDev = 'listDev';
/********************************************/

/* ROUTE UPDATE WITH MAIN SERVER */
var configureRoute = function () {
    app.post('/hien123/setRules', urlencodedParser, function (req, res) {
        var data = req.body.data
        data = JSON.parse(data)
        var OKAY = function () {
            res.send('{"status":"OK"}')
        }
        var ERROR = function () {
            res.send('{"status":"ERROR"}')
        }
        console.log("#POST ---setRules---")
        console.log(data)
        uptCtrl();
        setRules(data, OKAY, ERROR);
    })

    app.post('/hien123/getHistory', urlencodedParser, function (req, res) {
        var data = req.body.data;
        data = JSON.parse(data)
        console.log("#POST ---getHistory---")
        console.log("   " + JSON.stringify(data))
        var history = function (info) {
            var list = info;
            res.send({ DATA: list })
        }
        json = getHistory(data, history)
    })

    app.post('/hien123/getRules', urlencodedParser, function (req, res) {
        var data = req.body.data
        var data = JSON.parse(data)
        console.log("#POST ---getRules---")
        console.log(data)
        var rules = function (info) {
            var list = info
            res.send({ DATA: list });
        }
        json = getRules(data, rules)
    })

    app.post('/hien123/deleteRules', urlencodedParser, function (req, res) {
        var data = req.body.data
        var data = JSON.parse(data)
        console.log("#POST ---deletRules---")
        console.log(data)
        var OKAY = function () {
            res.send('{"status":"OK"}')
        }
        var ERROR = function () {
            res.send('{"status":"ERROR"}')
        }
        json = deleteRules(data, OKAY, ERROR)
    })

    app.get('/hien123/update', urlencodedParser, function (req, res) {
        console.log('content: ' + JSON.stringify(req.headers))
        console.log(req.headers["x-esp8266-version"])
        res.sendFile("/home/creta/application/route/hien123/" + "/" + "espOTA.ino.bin")
        // res.send(req.header.toString())
    })

    app.get('/hien123/error', urlencodedParser, function (req, res) {
        res.render("error.ejs")
    })

}

var configureDB_MQTT = function () {

    var x = 0;

    tcp = mqtt.connect('tcp://cretacam.ddns.net', { port: 1889 })
    tcp.on('connect', function (connack) {
        console.log('#MQTT-TCP CONNECTED!')
        tcp.subscribe('topicTest');
        x++;
    });

    ws = mqtt.connect('ws://cretacam.ddns.net', { port: 1883 });
    ws.on('connect', function (connack) {
        console.log('#MQTT-WS CONNECTED!')
        x++;
    });

    db = mysql.createConnection({
        host: "192.168.1.198",
        user: "creta",
        password: "yoursolution",
        database: "mydb"
    });
    db.connect(function (err) {
        if (err) throw err;
        console.log('#DATABASE CONNECTED!')
        x++;
    });

    function conCheck() {
        if (x == 3) {
            db.query("SELECT * FROM " + listDev, function (err, results) {
                if (err) throw err;
                for (var i = 0; i < results.length; i++) {
                    var user = results[i].PRODUCT + results[i].MACID;
                    tcp.subscribe(user + '/slave');
                    ws.subscribe(results[i].MACID + '/master');
                }
            })
            clearInterval(interval)
        }
    }
    var interval = setInterval(conCheck, 50)

    // Device <-> Server
    tcp.on('message', function (topic, msg) {
        console.log("# TCP:")
        console.log("   " + msg.toString())

        switch (topic.toString()) {
            case 'topicTest':
                var dev = uSplit(msg.toString())
                db.query("SELECT * FROM " + listDev + " WHERE MACID = " + mysql.escape(dev.MACID),
                    function (err, results) {
                        if (err) throw err;
                        if (results[0] == null) {
                            values = [
                                [
                                    dev.PRODUCT,
                                    dev.MACID
                                ]
                            ]

                            db.query("INSERT INTO " + listDev + " (PRODUCT,MACID) VALUES ?", [values],
                                function (err, results) {
                                    if (err) throw err;
                                }
                            )

                            tcp.subscribe(dev.PRODUCT + dev.MACID + '/slave')
                            ws.subscribe(dev.MACID + '/master')
                        }
                    }
                )
                break;

            default:
                var msg = JSON.parse(msg)

                //Send Info to App
                var wsTopic = uSplit(topic.toString()).MACID
                var js = {
                    ID: msg.USER,
                    FUNC: msg.FUNC,
                    ADDR: msg.ADDR,
                    DATA: msg.DATA
                }

                js.ID = uSplit(msg.USER).MACID

                msg.FUNC == "001" ? js.FUNC = "Ctrl" :
                    msg.FUNC == "002" ? js.FUNC = "Data" :
                        msg.FUNC == "003" ? js.FUNC = "Error" :
                            null

                var addr = msg.ADDR.split("")
                var prod = addr[0] + addr[1]
                var ordr = addr[2] + addr[3]
                prod == "01" ? js.ADDR = "RL_" :
                    prod == "02" ? js.ADDR = "TE_" :
                        prod == "04" ? js.ADDR = "WL_" :
                            null
                js.ADDR += ordr

                msg.DATA == '1' ? js.DATA = "100" :
                    js.DATA = msg.DATA

                ws.publish(wsTopic, JSON.stringify(js))
                break;

                //Update History
                var date = new Date();
                var values = [
                    [
                        date,
                        msg.USER,
                        msg.ADDR,
                        msg.FUNC,
                        msg.DATA
                    ]
                ]
                db.query("INSERT INTO " + history + " (TIME,MACID,ADDR,FUNC,DATA) VALUES ?", [values],
                    function (err, results) {
                        if (err) throw err;
                    }
                )
        }
    })

    ws.on('message', function (topic, msg) {

        console.log("# WS:")
        console.log("   " + msg.toString())
        msg = JSON.parse(msg)
        var js = {
            USER: msg.ID,
            FUNC: msg.FUNC,
            ADDR: msg.ADDR,
            DATA: msg.DATA
        }

        msg.FUNC == "Ctrl" ? js.FUNC = "001" :
            msg.FUNC == "Data" ? js.FUNC = "002" :
                msg.FUNC == "Error" ? js.FUNC = "003" :
                    msg.FUNC == "" ? js.FUNC = "101" :
                        null

        var addr = msg.ADDR.split("_")
        var prod = addr[0]
        var ordr = addr[1]
        prod == "RL" ? js.ADDR = "01" :
            prod == "TE" ? js.ADDR = "02" :
                prod == "WL" ? js.ADDR = "04" :
                    null
        js.ADDR += ordr

        msg.DATA == "100" ? js.DATA = "1" :
            null

        db.query("SELECT * FROM " + listDev + " WHERE MACID = " + mysql.escape(msg.ID),
            function (err, results) {
                if (err) throw err;
                js.USER = results[0].PRODUCT + results[0].MACID
                var tcpTopic = js.USER + "/master"
                tcp.publish(tcpTopic, JSON.stringify(js)); 
                               
            }
        )

        //Update History
        var date = new Date();
        var values = [
            [
                date,
                msg.ID,
                msg.ADDR,
                msg.FUNC,
                msg.DATA
            ]
        ]
        db.query("INSERT INTO " + wsHistory + " (TIME,MACID,ADDR,FUNC,DATA) VALUES ?", [values],
            function (err, results) {
                if (err) throw err;
            }
        )

    });
}

var configureSocket = function () {
    io.on('connection', function (socket) {
        //Whenever someone disconnects this piece of code executed
        function errLog(e){
            socket.emit('error', e)
        }
    });
}

/* IF YOU WANT DEVEL ANOTHER ROUTE-STATIC FILE */
var configurePublic = function () {
    //   app.use('/static/' + user, express.static(main.PWD + '/public/' + user));
}

//Time control
var timeControl = function () {
    uptCtrl();
    // uptCheck();
    setInterval(uptCtrl, 1800000)
    setInterval(tScan, 500)
}

//Set Rules;
function setRules(data, OKAY, ERROR) {
    db.query('DELETE FROM ' + ruleList + ' WHERE ADDR = ' + mysql.escape(data.ADDR),
        function (err, result) {
            if (err) throw err;
        }
    )
    var mac = "CSR" + data.MACID;
    var acc = data.ACC;
    var addr = data.ADDR;
    var begin = JSON.stringify(data.BEGIN);
    var mode = JSON.stringify(data.MODE);
    var state = data.STATE;
    var time = JSON.stringify(data.TIME)
    var du = JSON.stringify(data.DU)
    var func = data.FUNC
    var values = [
        [mac, acc, addr, begin, mode, state, time, du, func]
    ]
    var addRules = function (val) {
        console.log('# Adding new rules')
        db.query('INSERT INTO ' + ruleList + ' (MACID,ACC,ADDR,BEGIN,MODE,STATE,TIME,DU,FUNC) VALUES ?',
            [val], function (err, result) {
                if (err) {
                    ERROR();
                    if (err) throw err;
                } else {
                    OKAY();
                    uptCtrl();
                }
            }
        )
    }
    db.query('SELECT * FROM ' + ruleList + ' WHERE MACID = ' + mysql.escape(data.MACID),
        function (err, results) {
            if (err) throw err;
            var findValue = -1
            findValue = _.findIndex(results, function (value) {
                return value.MACID == data.MACID
                    && value.ACC == data.ACC
                    && value.ADDR == data.ADDR
                    && value.BEGIN == data.BEGIN
                    && value.MODE == data.MODE
                    && value.STATE == data.STATE
                    && value.TIME == data.TIME
                    && value.DU == data.DU
                    && value.FUNC == data.FUNC
            })
            if (findValue == -1) {
                addRules(values);
            } else {
                ERROR();
            }
        }
    )
}

// Get Rules
function getRules(data, callback) {
    var list = [];
    data.MACID = "CSR" + data.MACID
    console.log("getRules: " + data.MACID)
    db.query("SELECT * FROM " + ruleList + " WHERE MACID =" + mysql.escape(data.MACID),
        function (err, results) {
            for (var i = 0; i < results.length; i++) {
                var info = results[i];
                var json = {}
                for (var j in info) {
                    json[j] = info[j]
                }
                list.push(json);
            }
            callback(list);
        }
    )
}

// Get History
function getHistory(data, callback) {
    var list = [];
    var macid = data.MACID
    var addr = data.ADDR
    var values = [
        [macid, addr]
    ]
    db.query("SELECT * FROM " + history + " WHERE (MACID,ADDR) = ?", [values],
        function (err, results) {
            for (var i = 0; i < results.length; i++) {
                var re = results[i];
                var key = re.ADDR.split("")[0] + re.ADDR.split("")[1]
                var json = {
                    TIME: re.TIME,
                    MACID: re.MACID,
                    DATA: re.DATA
                };
                switch (key) {
                    case "01":
                        re.DATA == "0" ? json.DATA = "Off" :
                        json.DATA = "On"
                    case "02":
                        break;
                    case "04":
                        re.DATA == "0" ? json.DATA = "LOW" :
                        json.DATA = "HIGH"
                }
                list.push(json);
            }
            callback(list);
        }
    )

}

//Delete rules
function deleteRules(data, OK, ER) {
    values = [
        [
            data.MACID,
            data.ACC,
            data.ADDR,
            data.BEGIN,
            data.MODE,
            data.STATE,
            data.TIME,
            data.DU
        ]
    ]
    db.query("DELETE FROM " + ruleList + " WHERE (MACID,ACC,ADDR,BEGIN,MODE,STATE,TIME,DU) = ?",
        [values], function (err, results) {
            if (err) { ERR() }
            else { OK() }
        }
    )
}

//UPDATE TIME CONTROL ARRAY
function uptCtrl() {
    tCtrl = []
    db.query('SELECT * FROM ' + ruleList, function (err, results) { //Query timeRules
        if (err) throw err;
        var i
        for (i = 0; i < results.length; i++) {
            var re = results[i]
            var BEG = JSON.parse(re.BEGIN)
            var MOD = JSON.parse(re.MODE)
            var TI = JSON.parse(re.TIME)
            var DU = JSON.parse(re.DU)

            var ru = {
                MACID: re.MACID,
                ACC: re.ACC,
                ADDR: re.ADDR,
                MODE: MOD.KIND,
                bDAY: "",
                bTIME: "",
                STATE: re.STATE,
                eTIME: "",
                REP: "",
                FUNC: re.FUNC
            }

            var addr = re.ADDR.split("_")
            var prod = addr[0]
            var ordr = addr[1]
            prod == "RL" ? ru.ADDR = "01" :
                prod == "TE" ? ru.ADDR = "02" :
                    prod == "WL" ? ru.ADDR = "04" :
                        null
            ru.ADDR += ordr

            // "X" DAYS MODE
            if (MOD.KIND == '1') {
                ru.bDAY = new Date(BEG.YEAR, BEG.MONTH - 1, BEG.DATE)
                ru.bDAY = ru.bDAY.getTime()
                ru.bTIME = (parseInt(TI.HOUR) * 60 + parseInt(TI.MINUTES)) * 60000
                ru.eTIME = ru.bTIME + (parseInt(DU.HOUR) * 60 + parseInt(DU.MINUTES)) * 60000
                ru.REP = parseInt(MOD.DATA) * 24 * 60 * 60 * 1000
            }
            tCtrl.push(ru)
        }
        console.log("# Rules:")
        console.log(tCtrl)
    })
}

//TIME SCAN
function tScan() {

    var date = new Date();
    var today = new Date(date.getFullYear(), date.getMonth() + 1, date.getDate())
    today = today.getTime()
    var now = (date.getHours() * 60 + date.getMinutes()) * 60000
    for (var i = 0; i < tCtrl.length; i++) {
        var ru = tCtrl[i]
        switch (ru.STATE) {
            case "100":
                ru.STATE = "1"
                break;
        }
        var js = {
            USER: ru.MACID,
            FUNC: ru.FUNC,
            ADDR: ru.ADDR,
            DATA: ru.STATE
        }
        var topic = js.USER + "/master"

        //Check Mode
        switch (ru.MODE) {
            case "1":
                if ((today - ru.bDAY) % ru.REP == 0) {
                    tCompare();
                }
                break;
            case "2":

                break;
        }

        // Time compare
        function tCompare() {
            if (now < ru.bTIME) {
                check[i] = 0;
            }
            if (now >= ru.bTIME && now < ru.eTIME && check[i] != 1) {
                console.log("# TI_CONTROL 1: " + date)
                console.log("- Topic: " + topic)
                console.log("- Frame: " + JSON.stringify(js))
                tcpPublish(topic, js)
                check[i] = 1
            }
            if (now >= ru.eTIME && check[i] != 2) {
                if (js.DATA == '1') {
                    js.DATA = '0'
                } else { js.DATA = '1' }
                console.log("# TI_CONTROL 2: " + date)
                console.log("- Topic: " + topic)
                console.log("- Frame: " + JSON.stringify(js))
                tcpPublish(topic, js)
                check[i] = 2
            }
        }
    }
}

//TCP Publish
function tcpPublish(topic, js) {
    var x = 0;
    var pubInterval = setInterval(function () {
        tcp.publish(topic, JSON.stringify(js))
        if (x == 1) {
            clearInterval(pubInterval)
        }
        x++;
    }, 500)
}

// Zero-int
function zero(i) {
    if (i.toString().length == 1) {
        str = '0' + i.toString();
    } else {
        str = i.toString()
    }
    return str;
}

// Split device's name
function uSplit(x) {
    var c = x.split("");
    var js = {
        PRODUCT: "",
        MACID: ""
    };

    for (let i = 0; i < c.length; i++) {
        if (c[i] == c[i].toLowerCase()) {
            js.MACID += c[i]
        } else {
            js.PRODUCT += c[i]
        }
    };
    return (js);
}

exports.start = function () {
    configurePublic();
    configureRoute();
    // configureSocket();
    configureDB_MQTT();
    timeControl();
}
