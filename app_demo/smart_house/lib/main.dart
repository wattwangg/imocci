import 'dart:async';
import 'dart:io';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() {
  runApp(MyApp());
}

const String discovery_service = "_googlecast._tcp";

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        primarySwatch: Colors.blue,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      home: MyHomePage(title: '庭院温度'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key key, this.title}) : super(key: key);
  final String title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  double _counter = 0.0;
  bool _netOK = false;
  var _switch_state = [false, false, false, false];
  var pin_map = [14, 12, 13, 4];
  String esp_IP = "";
  bool auto_mode = false;

  TextEditingController _ssid = TextEditingController();
  TextEditingController _pass = TextEditingController();

  @override
  void initState() {
    super.initState();
    /*new Timer.periodic(new Duration(seconds: 30), (Timer timer) {
      _gettemprature();
    });*/
    new Timer.periodic(new Duration(seconds: 1), (Timer timer) {
      _getSwitchState();
    });
    _getESP();
  }

  void _swithchange(String name) async {
    String url = "http://$esp_IP/?cmd=";
    String cmd = "";
    for (int i = 0; i < 4; i++) {
      if (int.parse(name) - 1 == i) {
        _switch_state[i] = !_switch_state[i];
        cmd += "${pin_map[i]},";
        cmd += _switch_state[i] ? '1' : '0';
      }
    }
    setState(() {});
    var client = http.Client();
    http.Response response;
    try {
      response = await client.get(url + cmd);
    } catch (e) {
      url = "http://192.168.4.1/?cmd=";
      response = await client.get(url + cmd);
    }
    print(response.statusCode);
    if (response.statusCode != 200) {
      return;
    }
    // var _content = response.body;
    setState(() {});
  }

  void _getSwitchState() async {
    if (true) {
      var client = http.Client();
      http.Response response;
      try {
        response = await client.get("http://$esp_IP/state");
      } catch (e) {
        response = await client.get("http://192.168.4.1/state");
      }
      print(
          "==========================get esp state===================================");
      print(response.statusCode);
      if (response.statusCode != 200) {
        return;
      }
      var _content = response.body;
      setState(() {
        Map<String, dynamic> data = jsonDecode(_content);
        for (int i = 0; i < 4; i++) {
          _switch_state[i] = data["switch_${i + 1}_val"] == 1 ? true : false;
        }
        auto_mode = data["mode"] == 1 ? true : false;
      });
    }
  }

  void _gettemprature() async {
    var client = http.Client();
    http.Response response =
        await client.get("http://www.imooci.com/gettemperature");
    print("=============================================================");
    print(response.statusCode);
    if (response.statusCode != 200) {
      _netOK = false;
      return;
    }
    _netOK = true;

    var _content = response.body;
    setState(() {
      Map<String, dynamic> data = jsonDecode(_content);
      _counter = data['temperature'];
    });
  }

  Color getTempratureBg() {
    if (_counter < 15) {
      return Colors.blue;
    }
    if (_counter < 25) {
      return Colors.green;
    }
    return Colors.red;
  }

  void _getESP() {
    var DESTINATION_ADDRESS = InternetAddress("255.255.255.255");
    RawDatagramSocket.bind(InternetAddress.anyIPv4, 8889)
        .then((RawDatagramSocket udpSocket) {
      udpSocket.broadcastEnabled = true;
      udpSocket.listen((e) {
        Datagram dg = udpSocket.receive();
        if (dg != null) {
          String message = new String.fromCharCodes(dg.data).trim();
          print("===============");
          print("received $message");
          Map<String, dynamic> msgData = json.decode(message);
          if (msgData.containsKey("esp8266") && msgData.containsKey("mark")) {
            print("esp8266 IP: ${msgData['esp8266']}");
            esp_IP = msgData['esp8266'];
          }
        }
      });
      new Timer.periodic(new Duration(seconds: 1), (Timer t) {
        List<int> data = utf8.encode('hiesp');
        udpSocket.send(data, DESTINATION_ADDRESS, 8888);
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.start,
          children: <Widget>[
            Container(
              height: 5,
            ),
            Row(
              children: [
                Container(
                  width: 30,
                ),
                Text(
                  '当前室外温度(℃)：',
                ),
              ],
            ),
            Container(
              height: 30,
            ),
            ClipOval(
              child: Container(
                width: 100,
                height: 100,
                color: getTempratureBg(),
                alignment: Alignment.center,
                child: Text(
                  _netOK ? '$_counter' : 'Err',
                  style: TextStyle(color: Colors.white, fontSize: 30),
                ),
              ),
            ),
            Container(
              height: 30,
            ),
            Container(
              margin: EdgeInsets.only(left: 30, right: 30),
              child: Column(
                children: [
                  TextField(
                    decoration: InputDecoration(
                      hintText: '网络',
                      hintStyle: TextStyle(
                        fontSize: 12,
                        color: Colors.grey[300],
                      ),
                    ),
                    autofocus: false,
                    controller: this._ssid,
                    onChanged: (value) {
                      // this._ssid.text = value;
                    },
                  ),
                  SizedBox(height: 10),
                  TextField(
                    //obscureText: true,
                    decoration: InputDecoration(
                      hintText: '密码',
                      hintStyle: TextStyle(
                        fontSize: 12,
                        color: Colors.grey[300],
                      ),
                    ),
                    controller: this._pass,
                    autofocus: false,
                    onChanged: (value) {
                      // this._pass.text = value;
                    },
                  ),
                  SizedBox(height: 10),
                  Container(
                    width: double.infinity,
                    height: 40,
                    child: RaisedButton(
                      child: Text('修改连接路由'),
                      onPressed: () async {
                        print(this._ssid.text);
                        print(this._pass.text);
                        print("======change wifi=====");
                        var client = http.Client();
                        http.Response response;
                        String cmd = "ssid=${_ssid.text}&psw=${_pass.text}";
                        try {
                          response =
                              await client.get("http://$esp_IP/wifi?" + cmd);
                        } catch (e) {
                          response = await client
                              .get("http://192.168.4.1/wifi?" + cmd);
                        }
                        print(response.statusCode);
                        if (response.statusCode != 200) {
                          return;
                        }
                      },
                    ),
                  ),
                  SizedBox(height: 10),
                ],
              ),
            ),
            GestureDetector(
                onTap: () async {
                  print("======change mode=====");
                  var client = http.Client();
                  http.Response response;
                  String cmd = auto_mode == false ? "1" : "0";
                  try {
                    response =
                        await client.get("http://$esp_IP/mode?mode=" + cmd);
                  } catch (e) {
                    response =
                        await client.get("http://192.168.4.1/mode?mode=" + cmd);
                  }
                  print(response.statusCode);
                  if (response.statusCode != 200) {
                    return;
                  }
                  auto_mode = !auto_mode;
                  setState(() {});
                },
                child: Container(
                  width: MediaQuery.of(context).size.width * 0.9,
                  child: Row(
                    // mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text("运行模式："),
                      Text(auto_mode ? "自动" : "手动"),
                      Container(),
                      Icon(Icons.settings)
                    ],
                  ),
                )),
            Container(
                width: MediaQuery.of(context).size.width * 0.9,
                child: Column(
                  children: [
                    Row(
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      crossAxisAlignment: CrossAxisAlignment.end,
                      children: [
                        RaisedButton(
                          child: Text('开关1'),
                          color: _switch_state[0] ? Colors.green : Colors.red,
                          onPressed: () {
                            this._swithchange("1");
                            // ...
                          },
                        ),
                        RaisedButton(
                          child: Text('开关2'),
                          color: _switch_state[1] ? Colors.green : Colors.red,
                          onPressed: () {
                            this._swithchange("2");
                            // ...
                          },
                        )
                      ],
                    ),
                    Row(
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      crossAxisAlignment: CrossAxisAlignment.end,
                      children: [
                        RaisedButton(
                          child: Text('开关3'),
                          color: _switch_state[2] ? Colors.green : Colors.red,
                          onPressed: () {
                            this._swithchange("3");
                            // ...
                          },
                        ),
                        RaisedButton(
                          child: Text('开关4'),
                          color: _switch_state[3] ? Colors.green : Colors.red,
                          onPressed: () {
                            this._swithchange("4");
                            // ...
                          },
                        )
                      ],
                    ),
                  ],
                ))
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _gettemprature,
        tooltip: 'Reflush',
        child: Icon(Icons.refresh_outlined),
      ),
    );
  }
}
