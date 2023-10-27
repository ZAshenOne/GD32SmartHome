THINGSCLOUD_MQTT_HOST="bj-2-mqtt.iot-api.com"
mqttClientId="123456"
mqttUsername="5mbgrsd01c71w0fu"
mqttPassword="uVLaOuauBk"

pubTopic="attributes"
subTopic="attributes/push"

station_cfg={}
wifi.setmode(wifi.STATION)
station_cfg.ssid="ZJ"
station_cfg.pwd="12345678"
wifi.sta.config(station_cfg)
wifi.sta.connect()
uart.setup(0,115200,8,uart.PARITY_NONE,uart.STOPBITS_1,0)
mytimer=tmr.create()
mytimer2=tmr.create()
gpio.mode(4,gpio.OUTPUT)
gpio.write(4,gpio.HIGH)
closeCurtainTime="25:25"
openCurtianTime="25:25"
closeCurtainFlag=0
openCurtainFlag=0
s = softuart.setup(9600, 1,2)

mytimer:register(1000,tmr.ALARM_AUTO,
function()
    if wifi.sta.getip()==nil then
        print("连接中...")
    else
        print("ip is"..wifi.sta.getip())
        mytimer:stop()
        create_sub()
        get_time()
    end
end)
mytimer:start()

m=mqtt.Client(mqttClientId,1000,mqttUsername,mqttPassword)

function MQTTSUB(client,topic,data) 
   
    if string.find(topic,subTopic)  then
        t=sjson.decode(data)
        for key,value in pairs(t) do 
            print("收到云:"..key,value) 
            data1={}    
   
            if key=="led" and value==true then
                gpio.write(4,gpio.LOW)
                s:write("led on") 
            elseif key=="led" and value==false then
                gpio.write(4,gpio.HIGH)
                s:write("led of")
            elseif key=="curtain" and value==true then
                s:write("crt on")
            elseif key=="curtain" and value==false then
                s:write("crt of")
            elseif key=="window" and value==true then
                s:write("win on")
            elseif key=="window" and value==false then
                s:write("win of")
            elseif key=="air" and value==true then
                s:write("air on")
            elseif key=="air" and value==false then
                s:write("air of")
            elseif key=="closeCurtainTime" then
                closeCurtainTime=value
                print("closeCurtainTime:"..closeCurtainTime)
            elseif key=="openCurtianTime" then
                openCurtianTime=value
            end   

        end
    end
end

function MQTTPUB(temp,humi,light)
    data={}
    data.temperature=temp
    data.humidity=humi
    data.light=light
    
    ok,json=pcall(sjson.encode,data)
    if ok then
        m:publish(pubTopic,json,1,0)
    end
end

function split(str,reps)    
    local resultStrList = {}
    string.gsub(str,'[^'..reps..']+',function (w)
        table.insert(resultStrList,w)
    end)
    return resultStrList
end

function create_sub()
    m:connect(THINGSCLOUD_MQTT_HOST,1883,false,
    function(client)
        print("connected")
        mytimer2:start()

        s:on("data",8, 
        function(data)
            local data1=split(data,';')   
            local temp=tonumber(data1[2])   
            local humi=tonumber(data1[1])
            local light=tonumber(data1[3])
            print("准备发给云:"..temp..";"..humi..";"..light)
            MQTTPUB(temp,humi,light)    
        end)

        m:subscribe(subTopic,0,
        function(conn)
            print("subscribe success")
            m:on("message",MQTTSUB)
        end)
    end,
    function(client,reason)
        print("connection failed reason:"..reason)
    end
    )
end

function get_time()
    local url2="http://quan.suning.com/getSysTime.do"
    http.get(url2,nil,
    function(code,data)
        if code==200 then
            local decode=sjson.decode(data)
            local time=decode.sysTime2
            local time2=string.sub(time,12,16) 
            print(time2)
            if time2==closeCurtainTime and closeCurtainFlag==0 then
                s:write("crt of")
                closeCurtainFlag=1;
            elseif time2==openCurtianTime and openCurtainFlag==0 then
                s:write("crt on")
                openCurtainFlag=1;
            elseif time2~=closeCurtainTime then
                closeCurtainFlag=0;
            elseif time2~=openCurtainFlag then
                openCurtainFlag=0;
            end
        end
    end)
end

mytimer2:register(5000,tmr.ALARM_AUTO,
function()
   get_time()
end)
