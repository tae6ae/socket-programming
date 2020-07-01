# Socket Programming

The project is to write socket programming in the client and the server.
The client connects to the designated server.
The designated uses the following protocol to communicate with client.

![1](https://user-images.githubusercontent.com/49182823/86215586-b6e39980-bbb7-11ea-8a9e-02ca1d565a18.png)

## First Part

- Client implimentation

- Protocol : TCP / IPv6

- Send server IP (IPv4), port number

- After receiving tokens, resend and confirm token values

## Second Part

- Server implimentation (running concurrently)

- Protocol : TCP / IPv4

- Receive multiple client request at the same time

- Receive string tokens from connections
