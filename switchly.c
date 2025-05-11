#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMD_BUFFER 256

// Function to generate the correct partition suffix for NVMe vs non-NVMe
void get_partitions(const char *disk, char *part1, char *part2) {
    if (strstr(disk, "nvme") != NULL) {
        snprintf(part1, CMD_BUFFER, "%sp1", disk);
        snprintf(part2, CMD_BUFFER, "%sp2", disk);
    } else {
        snprintf(part1, CMD_BUFFER, "%s1", disk);
        snprintf(part2, CMD_BUFFER, "%s2", disk);
    }
}

void run_command(const char *cmd) {
    printf("[*] Running: %s\n", cmd);
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "[!] Command failed: %s\n", cmd);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        fprintf(stderr, "Usage: %s <edition> <disk> <username> <userpass> <rootpass> <hostname>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *edition = argv[1];
    const char *disk = argv[2];
    const char *username = argv[3];
    const char *userpass = argv[4];
    const char *rootpass = argv[5];
    const char *hostname = argv[6];

    char part1[CMD_BUFFER], part2[CMD_BUFFER];
    get_partitions(disk, part1, part2);

    // Partition disk
    char cmd[CMD_BUFFER];
    snprintf(cmd, CMD_BUFFER, "wipefs -af %s", disk);
    run_command(cmd);

    snprintf(cmd, CMD_BUFFER, "parted %s -- mklabel gpt", disk);
    run_command(cmd);

    snprintf(cmd, CMD_BUFFER, "parted %s -- mkpart primary fat32 1MiB 512MiB", disk);
    run_command(cmd);

    snprintf(cmd, CMD_BUFFER, "parted %s -- set 1 esp on", disk);
    run_command(cmd);

    snprintf(cmd, CMD_BUFFER, "parted %s -- mkpart primary ext4 512MiB 100%%", disk);
    run_command(cmd);

    snprintf(cmd, CMD_BUFFER, "mkfs.fat -F32 %s", part1);
    run_command(cmd);

    snprintf(cmd, CMD_BUFFER, "mkfs.ext4 %s", part2);
    run_command(cmd);

    run_command("mount /dev/null /mnt");  // Clear previous mount
    snprintf(cmd, CMD_BUFFER, "mount %s /mnt", part2);
    run_command(cmd);

    run_command("mkdir -p /mnt/boot");
    snprintf(cmd, CMD_BUFFER, "mount %s /mnt/boot", part1);
    run_command(cmd);

    // Placeholder for the rest of the installation logic, such as pacstrap, config, user setup, etc.
    printf("[+] Successfully partitioned and mounted. Now ready for system install for edition: %s\n", edition);
    printf("[+] Username: %s\n", username);
    printf("[+] Hostname: %s\n", hostname);

    return EXIT_SUCCESS;
}
