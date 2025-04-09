import requests
import time

def generate_chunks():
    chunks = [b"chunk1", b"chunk2", b"chunk3", b"BLABLABLABLABLABLABLA", b"chunk5"]
    for chunk in chunks:
        time.sleep(0.5)  # Simulate delay between chunks
        yield chunk


try:
    response = requests.post(
        'http://localhost:8080/post',
        data=generate_chunks(),
        headers={'Content-Type': 'application/octet-stream'}
    )

    print(f"{response.status_code} {response.reason} {response.text}")
except Exception as e:
    print(f"An error occured: {e}")
