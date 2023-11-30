import asyncio
from aiocoap import resource, Message, CHANGED, GET
import aiocoap
from influxdb_client_3 import InfluxDBClient3, Point
import json


token = "your_token"
org = "your_org"
host = "your_host"
database = "your_database"

client = InfluxDBClient3(host=host, token=token, org=org)

your_host = "your_host"


class SensorMeasurement(resource.Resource):
    async def render_post(self, request):
        print("Received data: " + str(request.payload))

        payload = request.payload.decode("utf-8")

        # parse payload
        value = json.loads(payload)

        # sensor name is node_<id>_<measurement_type> example: {"node_id": "123", "temperature": "25"} -> node_123_temperature
        key = [key for key in value.keys()][1]
        print("Measurement type: " + key)
        node_id = str(value["id"])
        sensor_name = "node_" + value["id"] + "_" + key
        print("Sensor name: " + sensor_name)

        value = float(value[key])

        # create InfluxDB point
        value = (
            Point(sensor_name)
            .field("value", value)
            .tag("sensor", sensor_name)
            .tag("node_id", node_id)
            .tag("measurement_type", key)
        )

        # write data to InfluxDB
        print("Writing data to InfluxDB")
        client.write(database=database, record=value)

        return Message(code=CHANGED, payload=b"Data received")


async def main():
    # Set the server address to localhost on IPv6 and port 5683
    request = Message(code=GET)
    request.set_request_uri("coap://[::1]:5683")

    # Listen for incoming requests
    root = resource.Site()
    root.add_resource(("sensor_data",), SensorMeasurement())
    await aiocoap.Context.create_server_context(root, bind=(your_host, 5683))

    try:
        # Run the event loop indefinitely
        print("Starting server")
        await asyncio.Future()
    except KeyboardInterrupt:
        pass
    except Exception as e:
        print("Failed to run server: " + str(e))
    finally:
        # Clean up resources
        protocol.shutdown()
        context.shutdown()


if __name__ == "__main__":
    # Run the event loop
    asyncio.get_event_loop().run_until_complete(main())
