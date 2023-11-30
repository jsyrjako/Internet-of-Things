import asyncio
from aiocoap import resource, Message, CHANGED, GET, PUT, Context
import os, time
from influxdb_client_3 import InfluxDBClient3, Point
import json
import pandas

token = "your-token"
org = "your-org"
host = "your-host"
database = "your-database"

client = InfluxDBClient3(host=host, token=token, org=org)



class MyResource(resource.Resource):
    async def render_put(self, request):
        payload = request.payload.decode("utf-8")
        print("Received data: " + payload)

        # parse payload ({"node_id": "103", "temperature": "35.7", "pressure": "1000,9", "light": "35"}) using json
        value = json.loads(payload)
        print("Json parsed: " + str(value))

        value = pandas.read_json(payload, typ="series")
        print("Pandas parsed: " + str(value))

        # add timestamp and sensor name to the payload and create InfluxDB point
        sensor_name = "node_" + value["node_id"]
        value = Point(sensor_name).field("temperature", value["temperature"]).field("pressure", value["pressure"]).field("light", value["light"])
        print("InfluxDB point created: " + str(value))

        # write data to InfluxDB
        print("Writing data to InfluxDB")
        client.write(database=database, record=value)

        return Message(code=CHANGED, payload=b"Data received")

    async def render_post(self, request):
        payload = request.payload.decode("utf-8")
        print(payload)

        # parse payload ({"node_id": "103", "temperature": "35.7", "pressure": "1000,9", "light": "35"}) using json
        value = json.loads(payload)
        print("Json parsed: " + str(value))

        value = pandas.read_json(payload, typ="series")
        print("Pandas parsed: " + str(value))

        # add timestamp and sensor name to the payload and create InfluxDB point
        sensor_name = "node_" + value["node_id"]
        value = Point(sensor_name).field("temperature", value["temperature"]).field("pressure", value["pressure"]).field("light", value["light"])
        print("InfluxDB point created: " + str(value))

        # write data to InfluxDB
        print("Writing data to InfluxDB")
        client.write(database=database, record=value)

        return Message(code=CHANGED, payload=b"Data received")


async def main():
    # Create CoAP client
    context = await Context.create_client_context()

    # Set the server address to localhost on IPv6 and port 5683
    request = Message(code=GET)
    request.set_request_uri("coap://[::1]:5683")

    # Listen for incoming requests
    root = resource.Site()
    root.add_resource(("sensor_data",), MyResource())
    protocol = await context.create_server_context(
        root, bind=("<your-ipv6-address>", 5683)
    )

    try:
        # Run the event loop indefinitely
        print("Starting server")
        await asyncio.Future()
    except KeyboardInterrupt:
        pass
    finally:
        # Clean up resources
        protocol.shutdown()
        context.shutdown()


if __name__ == "__main__":
    # Run the event loop
    asyncio.get_event_loop().run_until_complete(main())
