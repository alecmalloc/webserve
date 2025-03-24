# client.py
import requests
import itertools

def chunked_generator(data, chunk_size=10):
    """Generator to yield chunks of data."""
    for i in range(0, len(data), chunk_size):
        yield data[i:i+chunk_size]

def send_chunked_request(url, data):
    """Send a chunked POST request."""
    def chunked_data():
        for chunk in chunked_generator(data):
            yield f"{len(chunk):x}\r\n".encode() + chunk + b"\r\n"
        yield b"0\r\n\r\n"  # Last chunk

    headers = {"Transfer-Encoding": "chunked"}
    response = requests.post(url, headers=headers, data=chunked_data())
    print(f"Response status: {response.status_code}")
    print(f"Response content: {response.text}")

if __name__ == "__main__":
    url = "http://localhost:8080"
    data = b"Hello, this is a chunked request."
    try:
        send_chunked_request(url, data)
    except Exception as e:
        print(f"An error occurred: {e}")