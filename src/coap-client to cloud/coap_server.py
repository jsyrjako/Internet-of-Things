import asyncio
from aiocoap import resource, Message, CHANGED, GET, PUT, Context
import os


class MyResource(resource.Resource):
    async def render_put(self, request):
        payload = request.payload.decode("utf-8")
        print(payload)
        # with open("data.txt", "a") as file:
        #     file.write(payload + os.linesep)

        return Message(code=CHANGED, payload=b"Updated resource")

    async def render_post(self, request):
        payload = request.payload.decode("utf-8")
        print(payload)
        # with open("data.txt", "a") as file:
        #     file.write(payload + os.linesep)

        return Message(code=CHANGED, payload=b"Updated resource")



async def main():
    # Create CoAP client
    context = await Context.create_client_context()

    # Set the server address to localhost on IPv6 and port 5683
    request = Message(code=GET)
    request.set_request_uri('coap://[::1]:5683')

    # Listen for incoming requests
    root = resource.Site()
    root.add_resource(("sensor_data",), MyResource())
    protocol = await context.create_server_context(root, bind=('<change to your own ipv6 address>', 5683))

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


if __name__ == '__main__':
    # Run the event loop
    asyncio.get_event_loop().run_until_complete(main())
