#include <iostream>
#include <fstream>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <iomanip>
#include <string>
#include <filesystem>

#define PORT 8080
#define BUFFER_SIZE 4096

namespace fs = std::filesystem;

void printProgress(long received, long total) {
    int width = 50;
    float ratio = (float)received / total;
    int c = ratio * width;

    std::cout << "\r[";
    for (int i = 0; i < width; ++i)
        std::cout << (i < c ? '=' : ' ');
    std::cout << "] " << int(ratio * 100) << "% (" << received << "/" << total << " bytes)";
    std::cout.flush();
}

int main() {
    // Ensure output folder exists
    fs::create_directories("Recived");

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("Socket failed"); return 1; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("Bind failed"); return 1; }
    if (listen(server_fd, 1) < 0) { perror("Listen failed"); return 1; }

    std::cout << "Server listening on port " << PORT << "...\n";

    sockaddr_in clientAddr{};
    socklen_t clientLen = sizeof(clientAddr);
    int client_fd = accept(server_fd, (sockaddr*)&clientAddr, &clientLen);
    if (client_fd < 0) { perror("Accept failed"); return 1; }

    std::cout << "Client connected!\n";

    // Receive filename
    char filename[256];
    read(client_fd, filename, sizeof(filename));

    // Receive filesize
    long filesize;
    read(client_fd, &filesize, sizeof(filesize));

    // Generate timestamped filename (keep extension)
    time_t now = time(nullptr);
    std::string origName(filename);
    size_t dot = origName.find_last_of('.');
    std::string base = (dot == std::string::npos) ? origName : origName.substr(0, dot);
    std::string ext  = (dot == std::string::npos) ? "" : origName.substr(dot);
    std::string newFilename = "Recived/" + base + "_" + std::to_string(now) + ext;

    std::ofstream outfile(newFilename, std::ios::binary);
    if (!outfile.is_open()) { std::cerr << "Cannot open output file\n"; return 1; }

    char buffer[BUFFER_SIZE];
    long total = 0;
    while (total < filesize) {
        int bytesRead = read(client_fd, buffer, sizeof(buffer));
        if (bytesRead <= 0) break;
        outfile.write(buffer, bytesRead);
        total += bytesRead;
        printProgress(total, filesize);
    }

    outfile.close();
    std::cout << "\nFile received: " << newFilename << " (" << total << " bytes)\n";

    close(client_fd);
    close(server_fd);
    return 0;
}
