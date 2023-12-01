import asyncio
from aiocoap import resource, Message, CHANGED, GET, Context
from influxdb_client_3 import InfluxDBClient3, Point
import json
from config import TOKEN, ORG, HOST, DATABASE, SERVER_HOST

client = InfluxDBClient3(host=HOST, token=TOKEN, org=ORG)

class SensorMeasurement(resource.Resource):
    async def render_post(self, request):
        print("Received data: " + str(request.payload))

        payload = request.payload.decode("utf-8")

        # parse payload
        value = json.loads(payload)

        # sensor name is node_<id>_<measurement_type>
        key = [key for key in value.keys()][1]
        node_id = str(value["id"])
        sensor_name = "node_" + value["id"] + "_" + key

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
        client.write(database=DATABASE, record=value)
        return Message(code=CHANGED, payload=b"Data received")


async def main():
    # Set the server address to localhost on IPv6 and port 5683
    request = Message(code=GET)
    request.set_request_uri("coap://[::1]:5683")

    # Listen for incoming requests
    root = resource.Site()
    root.add_resource(("sensor_data",), SensorMeasurement())
    await Context.create_server_context(root, bind=(SERVER_HOST, 5683))

    try:
        # Run the event loop indefinitely
        print("Starting server")
        await asyncio.Future()
    except KeyboardInterrupt:
        pass
    except Exception as e:
        print("Failed to run server: " + str(e))


if __name__ == "__main__":
    # Run the event loop
    asyncio.get_event_loop().run_until_complete(main())
