# OS Project 2

## Setup
1. Install the dependances. (To be written.)
1. Make the executables.
    ```bash
      make
    ```

## Usage
1. Run the server. Note the server is set to delay 3s for each incoming message if there are too many messages received at the same time.
    ```bash
      ./server
    ```
1. Run the client on a different terminal with `<massage>` as the argument. This action sends `<message>` to the server.
    ```bash
      ./client <message>
    ```