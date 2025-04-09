# # Test all codes
# python server_test.py

# # Test specific code
# python return_codes.py 404
# python return_codes.py 500

# # Test multiple codes
# python return_codes.py 400,401,403

import argparse
import requests

serverUrl = 'http://localhost:8080'

# Configuration - Update these URLs to match your server's error endpoints
TEST_ENDPOINTS = {
    '400': f'{serverUrl}/bad-request',         # Malformed request test
    '401': f'{serverUrl}/protected',           # Unauthorized access test
    '403': f'{serverUrl}/admin',               # Forbidden resource test
    '404': f'{serverUrl}/non-existent-page',   # Not found test
    '500': f'{serverUrl}/error-generator'      # Internal server error test
}

def test_400():
    """Test Bad Request (400) with malformed JSON"""
    try:
        response = requests.post(
            TEST_ENDPOINTS['400'],
            headers={'Content-Type': 'application/json'},
            data='{"malformed": json}'
        )
        return response.status_code == 400
    except Exception as e:
        print(f"400 Test Failed: {str(e)}")
        return False

def test_401():
    """Test Unauthorized (401) with missing credentials"""
    try:
        response = requests.get(TEST_ENDPOINTS['401'])
        return response.status_code == 401
    except Exception as e:
        print(f"401 Test Failed: {str(e)}")
        return False

def test_403():
    """Test Forbidden (403) with insufficient permissions"""
    try:
        response = requests.get(TEST_ENDPOINTS['403'])
        return response.status_code == 403
    except Exception as e:
        print(f"403 Test Failed: {str(e)}")
        return False

def test_404():
    """Test Not Found (404) with invalid URL"""
    try:
        response = requests.get(TEST_ENDPOINTS['404'])
        return response.status_code == 404
    except Exception as e:
        print(f"404 Test Failed: {str(e)}")
        return False

def test_500():
    """Test Internal Server Error (500)"""
    try:
        response = requests.get(TEST_ENDPOINTS['500'])
        return response.status_code == 500
    except Exception as e:
        print(f"500 Test Failed: {str(e)}")
        return False

def run_test(test_name):
    tests = {
        '400': test_400,
        '401': test_401,
        '403': test_403,
        '404': test_404,
        '500': test_500,
        'all': lambda: all([func() for func in [test_400, test_401, test_403, test_404, test_500]])
    }
    
    if test_name not in tests:
        print(f"Invalid test name. Available tests: {', '.join(tests.keys())}")
        return False
    
    result = tests[test_name]()
    status = "SUCCESS" if result else "FAILED"
    print(f"\nTest {test_name.upper()} {status}")
    return result

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Web Server Status Code Tester')
    parser.add_argument('test', nargs='?', default='all',
                        help='Test to run (400,401,403,404,500,all)')
    args = parser.parse_args()
    
    print(f"Testing server for HTTP status codes...")
    run_test(args.test.lower())
