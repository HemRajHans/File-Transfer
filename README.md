# Simple File Transfer

This is a very small and easy program to send a file from one computer (client) to another (server) on a local network.  
It uses plain TCP sockets and works on **Linux** or **Android (Termux)**.

---

## 📦 What It Does

- Client reads a file from the `toSend/` folder and sends it to the server.  
- Server saves incoming files into the `Recived/` folder.  
- The server adds a timestamp to the filename but keeps the extension.  
  Example: `photo.jpg` → `Recived/photo_1731437601.jpg`  
- Shows a simple progress bar while sending and receiving.

---

## 🗂 Files

- `server.cpp` → run this on the **receiver** computer.  
- `client.cpp` → run this on the **sender** computer.

---

## ⚙️ Requirements

- g++ (C++ compiler)  
- Linux or Android (Termux)  
- Both devices on the same local network  
- Port **8080** open and not blocked by firewall  

---

## 🛠 Setup

### On the server (receiver)
```bash
mkdir Recived
