import asyncio
from aiocoap import *

async def main():
    protocol = await Context.create_client_context()

    request = Message(code=GET)
    request.set_request_uri('coap://[2001:660:5307:3100:e449:92fa:8265:160d]5683/temperature')

    try:
        response = await protocol.request(request).response
        print(response.payload.decode('utf-8'))
    except Exception as e:
        print('Failed to fetch resource:')
        print(e)

if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
