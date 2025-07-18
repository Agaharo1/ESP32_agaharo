import paho.mqtt.client as mqtt
import requests

# Telegram bot configuration
token = ''  # Replace with your actual token
BROKER = ""
PORT = ""
TOPIC = ""

chat_id = None

# Function to send messages to Telegram
def send_telegram_message(token, chat_id, message):
    url = f"https://api.telegram.org/bot{token}/sendMessage"
    params = {
        "chat_id": chat_id,
        "text": message
    }
    
    response = requests.get(url, params=params)
    if response.status_code == 200:
        print("Message successfully sent to Telegram!")
    else:
        print(f"Error sending message to Telegram: {response.status_code}")
        print(response.text)

# Function to get the Telegram chat_id
def get_chat_id(token):
    url = f"https://api.telegram.org/bot{token}/getUpdates"
    response = requests.get(url)
    
    if response.status_code == 200:
        data = response.json()
        if data["result"]:
            last_message = data["result"][-1]
            return last_message["message"]["chat"]["id"]
        else:
            raise Exception("Send a message to the bot first to obtain the chat_id")
    else:
        raise Exception(f"Error getting updates: {response.status_code}")

# Callback when an MQTT message is received
def on_message(client, userdata, message):
    global chat_id
    received_message = message.payload.decode()
    print(f"Message received on {message.topic}: {received_message}")

    # Send the received message to Telegram
    if chat_id:
        send_telegram_message(token, chat_id, f"Alarm triggered:\nDoor No. 3 {received_message} C°")
    else:
        print("Error: chat_id is not defined. Make sure to get the chat_id first.")

# Callback when connected to the MQTT broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker using WebSockets")
        client.subscribe(TOPIC)
    else:
        print(f"Connection failed. Code: {rc}")

# Configure and connect the MQTT client
def start_mqtt():
    client = mqtt.Client(transport="websockets")
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(BROKER, PORT)
    return client

if __name__ == "__main__":
    try:
        # Get Telegram chat_id
        chat_id = get_chat_id(token)
        print(f"Chat ID obtained: {chat_id}")
        send_telegram_message(token, chat_id, "ESP32 started!")   

        # Start the MQTT client
        client = start_mqtt()

        print("Connecting to MQTT broker...")
        client.loop_forever()

    except Exception as e:
        print(f"Error: {e}")
