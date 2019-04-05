#include <unistd.h>
#include <stropts.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <door.h>
#include <stdio.h>

void handle(void* cookie, char* args, size_t nargs, door_desc_t* descriptors, uint_t ndescriptors) {
	char* path = "server.door";
        int door = open(path, O_RDONLY);
        if (door == -1) {
                perror("Could not open door");
                exit(1);
        }

        door_desc_t w_descriptor;
        w_descriptor.d_attributes = DOOR_DESCRIPTOR;
        w_descriptor.d_data.d_desc.d_descriptor = door;

	door_return(NULL, 0, &w_descriptor, 1);
}

int main() {
        struct stat buf;

	char* path = "proxy.door";

        int door = door_create(&handle, NULL, 0);
        if (door == -1) {
                perror("Handle cannot be attached to door");
                exit(1);
        }

        if (stat(path, &buf) < 0) {
                int newfd;
                if ((newfd = creat(path, 0444)) < 0) {
                        perror("creat");
                        exit(1);
                }
                close(newfd);
        }

        fdetach(path);

        int attachment = fattach(door, path);
        if (attachment == -1) {
                perror("Could not attach door to proxy.door");
                exit(1);
        }

        printf("%s will remain attached to this process for 1 hour\n", path);

	sleep(3600);

	return 0;
}
