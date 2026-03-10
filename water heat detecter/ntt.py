import asyncio
from bleak import BleakScanner, BleakClient

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
TEMP_CHAR_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

async def notification_handler(sender, data):
    print(f"Temperature data: {data.decode('utf-8')}")

async def main():
    print("Scanning for Smart Water Bottle...")
    devices = await BleakScanner.discover()
    
    target_device = None
    for d in devices:
        if d.name == "SmartWaterBottle":
            target_device = d
            break
    
    if target_device:
        print(f"Found device: {target_device.name}")
        async with BleakClient(target_device.address) as client:
            print("Connected")
            await client.start_notify(TEMP_CHAR_UUID, notification_handler)
            await asyncio.sleep(60)  # Listen for 60 seconds
            await client.stop_notify(TEMP_CHAR_UUID)
    else:
        print("Device not found")

asyncio.run(main())