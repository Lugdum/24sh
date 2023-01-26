#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

#define FILE_POS 1

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("USAGE %s: %s <FILE>\n", argv[0], argv[0]);
        return 2;
    }
    int file_fd = open(argv[FILE_POS], O_TRUNC | O_CREAT| O_WRONLY);
    if (file_fd == -1)
        errx(1, "1 - Could not open file for redirection!");

    // Duplicate STDOUT_FILENO
    int fd_dup = dup(STDOUT_FILENO);
    if (fd_dup == -1)
    {
        close(file_fd);
        errx(1, "2 - Could not dup stdout!");
    }

    //Duplicate given file descriptor in stdout
    int new_stdout = dup2(file_fd, STDOUT_FILENO);
    if (new_stdout == -1)
    {
        close(fd_dup);
        close(file_fd);
        errx(1, "3 - Failed to dup stdout");
    }

    //Close given file descriptor, as it is not needed anymore
    if (close(file_fd) == -1)
    {
        close(fd_dup);
        close(new_stdout);
        errx(1, "4 - Failed to close dup stdout");
    }

    //do the print
    printf("This will be in a file\n");
    fflush(stdout);

    //restore stdout
    new_stdout = dup2(fd_dup, STDOUT_FILENO);
    if (new_stdout == -1)
    {
        close(fd_dup);
        errx(1, "5 - Failed to dup stdout!");
    }

    //close stdout duplicqte as stdout's file descriptor was retreived
    if (close(fd_dup) == -1)
        errx(1, "6 - could not close fd");

    //Print again on regular stdout
    printf("this will print on terminal stdout\n");

    return 0;
}
