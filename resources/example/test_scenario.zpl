
name: "Examle scenario"
description: "Test case for writing a zpl script"
repeat: inf
continue_on_error: true

# Env variables
environment:
    api_url: http://127.0.0.1:8000/api/v1
    email: example@gmail.com
    password: Qwerty1234567890

steps:
    - name: "Register"
      description: "User registration"
      delay: 3000
      request:
            method: POST
            url: "${api_url}/auth/register/"
            headers:
                Content-Type: application/json
            body: |
                {
                    "email": "${email}",
                    "username": "GOD",
                    "password": "${password}",
                    "password2": "${password}",
                    "first_name": "John",
                    "last_name": "Malcov",
                    "phone_number": "+79527981865"
                }
      expected_response:
            status_code: 201

    - name: "Login"
      description: "User login"
      delay: 3000
      request:
            method: POST
            url: "${api_url}/auth/login/"
            headers:
                Content-Type: application/json
            body: |
                {
                    "email": "${email}",
                    "password": "${password}"
                }
      expected_response:
            status_code: 200
      variables:
            token: $.access
            refresh_token: $.refresh
            user_id: $.user.id  

    - name: "Profile"
      description: "Get current user profile"
      delay: 3000
      request:
            method: GET
            url: "${api_url}/auth/profile/"
            headers:
                Authorization: Bearer ${token}
      expected_response:
            status_code: 200 

    - name: "Refresh"
      description: "Refresh token"
      delay: 3000
      request:
            method: POST
            url: "${api_url}/auth/token/refresh/"
            headers:
                Content-Type: application/json
            body: |
                {
                    "refresh": "${refresh_token}"
                }
      expected_response:
            status_code: 200
      variables:
            token: $.access
            refresh_token: $.refresh

    - name: "Update"
      description: "Update user profile"
      delay: 3000
      request:
            method: PUT
            url: "${api_url}/auth/profile/"
            headers:
                Content-Type: application/json
                Authorization: Bearer ${token}
            body: |
                {
                    "first_name": "Morgan",
                    "last_name": "Freeman",
                    "phone_number": "+49070782980"
                }
      expected_response:
            status_code: 200

    - name: "Profile2"
      description: "Get current user profile after update"
      delay: 3000
      request:
            method: GET
            url: "${api_url}/auth/profile/"
            headers:
                Authorization: Bearer ${token}
      expected_response:
            status_code: 200 

    - name: "Logout"
      description: "User Logout"
      delay: 3000
      request:
            method: POST
            url: "${api_url}/auth/logout/"
            headers:
                Content-Type: application/json
                Authorization: Bearer ${token}
            body: |
                {
                    "refresh": "${refresh_token}"
                }
      expected_response:
            status_code: 200 