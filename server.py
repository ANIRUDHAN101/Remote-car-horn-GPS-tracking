import paho.mqtt.client as mqtt
from math import cos, asin, sqrt

def distance(lat1, lon1, lat2, lon2):
    p = 0.017453292519943295
    a = 0.5 - cos((lat2 - lat1) * p)/2 + cos(lat1 * p) * cos(lat2 * p) * (1 - cos((lon2 - lon1) * p)) / 2
    return 12742 * asin(sqrt(a))

def on_connect(client, userdata, flags, rc):  # The callback for when the client connects to the broker
    #print("Connected with result code {0}".format(str(rc)))  # Print result of connection attempt
    client.subscribe("Car1\\latitude")  # Subscribe to the topic “digitest/test1”, receive any messages published on it
    client.subscribe("Car1\\longitude")
    client.subscribe("Car2\\latitude")  # Subscribe to the topic “digitest/test1”, receive any messages published on it
    client.subscribe("Car2\\longitude")

def on_message(client, userdata, msg):
    global c1Long
    global c1Lat
    global c2Long
    global c2Lat
    # The callback for when a PUBLISH message is received from the server.
    #print("Message received-> " + msg.topic + " " + str(msg.payload))  # Print a received msg
    if msg.topic == "Car1\\latitude":
        c1Lat = float(msg.payload.decode("utf-8"))
        print(c1Lat)
    if msg.topic == "Car1\\longitude":
        c1Long = float(msg.payload.decode("utf-8"))
        print(c1Long)
    if msg.topic == "Car2\\latitude":
        c2Lat = float(msg.payload.decode("utf-8"))
        print(c2Lat)
    if msg.topic == "Car2\\longitude":
        c2Long = float(msg.payload.decode("utf-8"))
        print(c2Long)
    try:
        d = distance(c1Lat, c2Long, c2Lat, c2Long)*1000
        print(f"distance between 2 gps locations:{d} m")
        
        if(d<=50000):
            print(d)
            client.publish('Car1\\near', payload=d, qos=0, retain=False)
            client.publish('Car2\\near', payload=d, qos=0, retain=False)


    except:
        pass


client = mqtt.Client("digi_mqtt_test")  # Create instance of client with client ID “digi_mqtt_test”
client.on_connect = on_connect  # Define callback function for successful connection
client.on_message = on_message  # Define callback function for receipt of a message
# client.connect("m2m.eclipse.org", 1883, 60)  # Connect to (broker, port, keepalive-time)
client.connect('public.mqtthq.com', 1883, 60)
client.loop_forever()  # Start networking daemon
