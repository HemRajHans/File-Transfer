#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <unistd.h>
#include <iomanip>
#include <string>
#include <filesystem>

#define PORT 8080
#define BUFFER_SIZE 4096

namespace fs = std::filesystem;

void printProgress(long sent, long total) {
    int width = 50;
    float ratio = (float)sent / total;
    int c = ratio * width;

    std::cout << "\r[";
    for (int i = 0; i < width; ++i)
        std::cout << (i < c ? '=' : ' ');
    std::cout << "] " << int(ratio * 100) << "% (" << sent << "/" << total << " bytes)";
    std::cout.flush();
}

int main() {
    fs::create_directories("toSend");

    std::string server_ip, filename;
    std::cout << "Enter server IP: "; std::cin >> server_ip;
    std::cout << "Enter filename (from toSend folder): "; std::cin >> filename;

    std::string fullPath = "toSend/" + filename;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("Socket creation failed"); return 1; }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed"); return 1;
    }

    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) { std::cerr << "File not found: " << fullPath << "\n"; return 1; }

    // Send filename
    write(sock, filename.c_str(), filename.size() + 1);

    // Send filesize
    file.seekg(0, std::ios::end);
    long filesize = file.tellg();
    file.seekg(0, std::ios::beg);
    write(sock, &filesize, sizeof(filesize));

    // Send file data
    char buffer[BUFFER_SIZE];
    long totalSent = 0;
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        int bytesRead = file.gcount();
        write(sock, buffer, bytesRead);
        totalSent += bytesRead;
        printProgress(totalSent, filesize);
    }

    std::cout << "\nFile sent successfully!\n";

    file.close();
    close(sock);
    return 0;
}
