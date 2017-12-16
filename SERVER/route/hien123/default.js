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
var check = [], devArr = {};
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
        var history = function (js) {
            res.send(js)
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
            for (const i in devArr) {
                tcp.subscribe(uID(i,"App") + '/slave');
                ws.subscribe(i + '/master');
                clearInterval(interval)
            }
        }
    }
    var interval = setInterval(conCheck, 50)

    // Device <-> Server
    tcp.on('message', function (topic, msg) {
        console.log("# TCP:")
        console.log("   " + msg.toString())

        switch (topic.toString()) {
            case 'topicTest':
                var dev = uID(msg.toString(), "Dev")
                // db.query("SELECT * FROM " + listDev + " WHERE ID = " + mysql.escape(dev),
                //     function (err, results) {
                //         if (err) throw err;

                if (dev == null) {
                    values = [
                        [
                            dev.PRODUCT,
                            dev.ID
                        ]
                    ]
                    db.query("INSERT INTO " + listDev + " (PRODUCT,ID) VALUES ?", [values],
                        function (err, results) {
                            if (err) throw err;
                        }
                    )
                    tcp.subscribe(uID(dev, "App") + '/slave')
                    ws.subscribe(dev + '/master')
                }
                break;

            default:
                var msg = JSON.parse(msg)

                //Send Info to App
                var wsTopic = uID(topic.toString(), "Dev")
                var js = {
                    ID: msg.USER,
                    FUNC: msg.FUNC,
                    ADDR: msg.ADDR,
                    DATA: msg.DATA
                }

                js.ID = uID(msg.USER, "Dev")

                msg.FUNC == "001" ? js.FUNC = "Ctrl" :
                    msg.FUNC == "002" ? js.FUNC = "Data" :
                        msg.FUNC == "003" ? js.FUNC = "Error" :
                            msg.FUNC == "101" ? js.FUNC = "Timer" :
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
                upHis(msg,history)                
                break;

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
                    msg.FUNC == "Timer" ? js.FUNC = "101" :
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

        js.USER = uID(msg.ID, "App")
        var tcpTopic = js.USER + "/master"
        tcp.publish(tcpTopic, JSON.stringify(js));

        upHis(msg,wsHistory)

    });
}

var configureSocket = function () {
    io.on('connection', function (socket) {
        //Whenever someone disconnects this piece of code executed
        function errLog(e) {
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
    updArr();
    uptCtrl();
    // uptCheck();
    setInterval(function () {
        updArr();
        var date = new Date();
        var x = date.getHours() + date.getMinutes() + date.getSeconds
        if (x == 0) {
            uptCtrl();
        }
        tScan();
    }, 500)
}

//Set Rules;
function setRules(data, OKAY, ERROR) {
    var id = uID(data.ID, "App");
    var acc = data.ACC;
    var addr = data.ADDR;
    var begin = JSON.stringify(data.BEGIN);
    var mode = JSON.stringify(data.MODE);
    var state = data.STATE;
    var time = JSON.stringify(data.TIME)
    var du = JSON.stringify(data.DU)
    var func = data.FUNC
    var values = [
        [id, acc, addr, begin, mode, state, time, du, func]
    ]

    var addRules = function (val) {
        console.log('# Adding new rules')
        db.query('INSERT INTO ' + ruleList + ' (ID,ACC,ADDR,BEGIN,MODE,STATE,TIME,DU,FUNC) VALUES ?',
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
    db.query('SELECT * FROM ' + ruleList + ' WHERE ID = ' + mysql.escape(data.ID),
        function (err, results) {
            if (err) throw err;
            var findValue = -1
            findValue = _.findIndex(results, function (value) {
                return value.ID == data.ID
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
    data.ID = "CSR" + data.ID
    console.log("getRules: " + data.ID)
    db.query("SELECT * FROM " + ruleList + " WHERE ID =" + mysql.escape(data.ID),
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
    var js = {
        STATUS: "OK",
        DATA: list
    }
    id = uID(data.ID, "App")
    var addr = data.ADDR.split("_")
    var prod = addr[0]
    var ordr = addr[1]
    prod == "RL" ? data.ADDR = "01" :
        prod == "TE" ? data.ADDR = "02" :
            prod == "WL" ? data.ADDR = "04" :
                null
    data.ADDR += ordr
    var values = [
        [id, data.ADDR]
    ]
    db.query("SELECT * FROM " + history + " WHERE (ID,ADDR) = ?", [values],
        function (err, results) {
            if (err) throw err;
            if (results[0] == null) {
                js.STATUS = "ERROR"
                callback(js)
            } else {
                for (var i = 0; i < results.length; i++) {
                    var re = results[i];
                    var json = {
                        TIME:{
                            "HOUR":re.HOUR.toString(),
                            "MINUTES":re.MINUTES.toString(),
                            "SECOND":re.SECOND.toString()
                        },
                        DATE:{
                            "DAY":re.DAY.toString(),
                            "MONTH":re.MONTH.toString(),
                            "YEAR":re.YEAR.toString()
                        },
                        ID:re.ID,
                        ADDR:re.ADDR,
                        FUNC:re.FUNC,
                        DATA:re.DATA
                    }
                    re.ID = uID(re.ID, "Dev")

                    var addr = re.ADDR.split("")
                    var prod = addr[0] + addr[1]
                    var ordr = addr[2] + addr[3]
                    prod == "01" ? re.ADDR = "RL_" :
                        prod == "02" ? re.ADDR = "TE_" :
                            prod == "04" ? re.ADDR = "WL_" :
                                null
                    re.ADDR += ordr

                    re.FUNC == "001" ? re.FUNC = "Ctrl" :
                        re.FUNC == "002" ? re.FUNC = "Data" :
                            re.FUNC == "003" ? re.FUNC = "Error" :
                                re.FUNC == "101" ? re.FUNC = "Timer" :
                                    null

                    re.DATA == '1' ? re.DATA = "100" :
                        null
                    list.push(json);
                }
                callback(js);
            }
        }
    )

}

//Delete rules
function deleteRules(data, OK, ER) {
    values = [
        [
            data.ID,
            data.ACC,
            data.ADDR,
            data.BEGIN,
            data.MODE,
            data.STATE,
            data.TIME,
            data.DU
        ]
    ]
    db.query("DELETE FROM " + ruleList + " WHERE (ID,ACC,ADDR,BEGIN,MODE,STATE,TIME,DU) = ?",
        [values], function (err, results) {
            if (err) { ERR() }
            else { OK() }
        }
    )
}

//UPDATE DEVICE LIST
function updArr() {
    db.query("SELECT * FROM " + listDev, function (err, results) {
        if (err) throw err;
        for (let i = 0; i < results.length; i++) {
            devArr[results[i].ID] = results[i].PRODUCT + results[i].ID
        }
    })
}

//UPDATE TIME CONTROL ARRAY
function uptCtrl() {
    tCtrl = []
    db.query('SELECT * FROM ' + ruleList, function (err, results) { //Query timeRules
        if (err) throw err;
        var date = new Date();
        var today = new Date(date.getFullYear(), date.getMonth(), date.getDate() + 1)
        today = today.getTime()
        // var i
        for (let i = 0; i < results.length; i++) {
            var re = results[i]
            var BEG = JSON.parse(re.BEGIN)
            var MOD = JSON.parse(re.MODE)
            var TI = JSON.parse(re.TIME)
            var DU = JSON.parse(re.DU)
            let ruB = {
                ID: re.ID,
                ADDR: re.ADDR,
                STATE: re.STATE,
                FUNC: re.FUNC,
                CHECK: "0"
            }

            re.FUNC == "Ctrl" ? ruB.FUNC = "001" :
                re.FUNC == "Data" ? ruB.FUNC = "002" :
                    re.FUNC == "Error" ? ruB.FUNC = "003" :
                        re.FUNC == "Timer" ? ruB.FUNC = "101" :
                            null

            var addr = re.ADDR.split("_")
            var prod = addr[0]
            var ordr = addr[1]
            prod == "RL" ? ruB.ADDR = "01" :
                prod == "TE" ? ruB.ADDR = "02" :
                    prod == "WL" ? ruB.ADDR = "04" :
                        null
            ruB.ADDR += ordr

            re.STATE == "100" ? ruB.STATE = "1" :
                null

            let ruE = {
                ID: ruB.ID,
                ADDR: ruB.ADDR,
                STATE: "",
                FUNC: ruB.FUNC,
                CHECK: "0"
            }

            ruB.STATE == "1" ? ruE.STATE = "0" :
                ruE.STATE = "1";

            // "X" DAYS MODE
            if (MOD.KIND == "1") {
                var bDay = new Date(BEG.YEAR, BEG.MONTH - 1, BEG.DATE + 1);
                bDay = bDay.getTime();
                var rep = 86400000 * parseInt(MOD.DATA);
                (today - bDay) % rep == 0 ? x = 1 :
                    x = 0;
            } else if (MOD.KIND == "2") {

            }

            if (x = 1) {
                var beg, end
                TI.HOUR == "0" ? beg = TI.MINUTES :
                    beg = TI.HOUR + zero(TI.MINUTES)
                var hr = parseInt(TI.HOUR) + parseInt(DU.HOUR)
                var min = parseInt(TI.MINUTES) + parseInt(DU.MINUTES)
                if (min > 59) {
                    min = 60 - min;
                    hr++;
                    if (hr > 23) {
                        hr = 24 - hr;
                    }
                }
                hr == "0" ? end = min.toString() :
                    end = hr.toString() + zero(min.toString())
                tCtrl[beg] = ruB
                tCtrl[end] = ruE
            }

        }
        console.log("# Rules:")
        for (let i = 0; i < tCtrl.length; i++) {
            if (tCtrl[i] != null) {
                console.log(i, tCtrl[i])
            }
        }
    })
}

//TIME SCAN
function tScan() {

    var date = new Date();
    var hr = date.getHours()
    var min = date.getMinutes()
    var now;
    hr == 0 ? now = min.toString() :
        now = hr.toString() + min.toString();
    let ru = tCtrl[now]
    let i

    if (ru == null) {
        for (i = parseInt(now); i > -1; i--) {
            if (tCtrl[i] != null) {
                ru = tCtrl[i]
                break;
            };
            i == 0 ? i = 2359 :
                null;
        }
    }

    if (ru.CHECK != "1") {
        timeFire(ru)
        tCtrl[i]["CHECK"] = "1";
    }
}

function timeFire(ru) {
    var date = new Date()
    var topic = ru.ID + "/master"
    var js = {
        USER: ru.ID,
        FUNC: ru.FUNC,
        ADDR: ru.ADDR,
        DATA: ru.STATE
    }
    tcpPublish(topic, js)
    console.log("# TI_CONTROL :", date)
    console.log("- Topic: ", topic)
    console.log("- Frame: ", JSON.stringify(js))
}
// TCP Publish
function tcpPublish(topic, js) {
    // var x = 0;
    var pubInterval = setInterval(function () {
        tcp.publish(topic, JSON.stringify(js))
        // if (x == 1) {
        //     clearInterval(pubInterval)
        // }
        // x++;
        tcp.on("message", function (t, msg) {
            if (t == topic) {
                clearInterval(pubInterval);
            }
        });
    }, 2000)
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
function uID(x, src) {
    var msg = ""
    if (src == "App") {
        msg = devArr[x]
    } else if (src == "Dev") {
        var c = x.split("");
        for (let i = 0; i < c.length; i++) {
            if (c[i] == c[i].toLowerCase()) {
                msg += c[i]
            }
        }
    }
    return (msg);
}

//Update History
function upHis(msg,hist){
    var date = new Date();
    var hr = date.getHours()
    var min = date.getMinutes()
    var sec = date.getSeconds()
    var year = date.getFullYear()
    var month = date.getMonth() + 1
    var day = date.getDate()
    var values = [
        [
            year,month,day,hr,min,sec,
            msg.USER,
            msg.ADDR,
            msg.FUNC,
            msg.DATA
        ]
    ]
    db.query("INSERT INTO " + hist + " (YEAR,MONTH,DAY,HOUR,MINUTES,SECOND,ID,ADDR,FUNC,DATA) VALUES ?", [values],
        function (err, results) {
            if (err) throw err;
        }
    )
}

exports.start = function () {
    configurePublic();
    configureRoute();
    // configureSocket();
    configureDB_MQTT();
    timeControl();
}
