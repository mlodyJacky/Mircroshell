#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <fcntl.h>
void red ()
{
  printf("\033[1;31m");
}

void yellow()
{
  printf("\033[1;33m");
}
void white()
{
  printf("\033[0;37m");
}

void zielony()
{
    printf("\033[0;32m");
}


void PWD()
{
        char *buf;
        buf=(char *)malloc(100*sizeof(char));
        getcwd(buf,100);
        printf("%s",buf);
}

void HELP()
{
    printf("Twórcą programu jest Tymon Pluciński\nFunkcje programu: \n");
    printf("exit - zakończenie działania programu\npwd - wypisanie aktualnej scieżki roboczej\ncd - zmiana aktualnej biblioteki\necho - wypisanie podanego tekstu\ntouch - utworzenie pliku\nrm - usuniecie pliku\nmkdir - utworzenie biblioteki\nrmdir - usuniecie biblioteki\nhistory - wyswietlenie historii polecen\nwhoami - wyswietlenie loginu uzytkownika");
}
void MKDIR(char komendawpisana[])
{
    strtok(komendawpisana, " ");
    char* nazwa = strtok(NULL, " ");
    mkdir(nazwa, 0777);
}
void RMDIR(char komendawpisana[])
{
    strtok(komendawpisana, " ");
    char* nazwa = strtok(NULL, " ");
    if (rmdir(nazwa) != 0)
    {
        red();
        printf("Błąd podczas usuwania katalogu");
        white();
    }
}
void RM(char komendawpisana[])
{
    strtok(komendawpisana, " ");
    char* nazwa = strtok(NULL, " ");
    if (unlink(nazwa) != 0)
    {
        red();
        printf("Błąd podczas usuwania pliku");
        white();
    }
}

void TOUCH(char komendawpisana[])
{
    strtok(komendawpisana, " ");
    char* nazwa = strtok(NULL, " ");
    struct stat fileStat;
    if(stat(nazwa,&fileStat) == 0)
    {
        struct utimbuf new_times;
        new_times.actime = fileStat.st_atime;
        new_times.modtime = time(NULL);
        utime(nazwa, &new_times);
    }
    else
    {
        int fd = open(nazwa, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
        if (fd == -1)
        {
            red();
            printf("Błąd podczas tworzenia pliku");
            white();
            return;
        }
        close(fd);
    }
}

void FORKEXEC(char* komendawpisana)
{
    char* args[100];
    int i = 0;
    args[i] = strtok(komendawpisana, " ");
    while (args[i])
    {
        i++;
        args[i] = strtok(NULL, " ");
    }
    int status = fork();
    if (status == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            red();
            printf("Bledna komenda. Wpisz help: %s\n", args[0]);
            exit(EXIT_FAILURE);
            white();
        }
    }
    else
    {
        wait(NULL);
    }
}



char history[10][10];
int licznikhis = 0;

void dodajdohis(char* komendawpisana)
{
    if (licznikhis < 10)
    {
        strcpy(history[licznikhis], komendawpisana);
        licznikhis++;
    }
    else
    {
        int i = 0;
        for(i=0; i < 10 - 1; i++)
        {
            strcpy(history[i], history[i+1]);
        }
        strcpy(history[10 - 1], komendawpisana);
    }
}

void pokazhis()
{
    int i=0;
    for(i=0; i < licznikhis; i++)
    {
        printf("%s", history[i]);
    }
}
void WHOAMI()
{
    char *login = getlogin();
    printf("%s: ",login);
}
void get_hostname(char* nazwa, size_t dlugosc)
{
    struct utsname info;
    uname(&info);
    strncpy(nazwa, info.nodename, dlugosc);
}

int main() 
{

    int wyjdz=0;
    char komendawpisana[100];
    char poprzednia[100];

    
    white();

    while(wyjdz==0)
    {
        char maszyna[256];
        get_hostname(maszyna, sizeof(maszyna));
        char *buf;
        buf=(char *)malloc(100*sizeof(char));
        getcwd(buf,100);
        char *login = getlogin();
        yellow();
        printf("\n%s",login);
        white();
        printf("@");
        zielony();
        printf("%s",maszyna);
        white();
        printf(":");
        white();
        printf("%s $ ",buf);
        fgets( komendawpisana,sizeof(komendawpisana), stdin );
        dodajdohis(komendawpisana);
        komendawpisana[strcspn(komendawpisana, "\n")] = 0;
        if(strcmp(komendawpisana, "help")==0) HELP();
        else if(strcmp(komendawpisana,"pwd")==0) PWD();
        else if(strcmp(komendawpisana,"exit")==0) wyjdz=1;
        else if (strcmp(komendawpisana, "history") == 0)pokazhis();
        else if (strncmp(komendawpisana, "echo ", 5) == 0)
        {
            printf("%s", komendawpisana + 5);
        }
        else if(strncmp(komendawpisana, "mkdir ",6)==0)
        {
            MKDIR(komendawpisana);
        }
        else if(strncmp(komendawpisana,"rmdir ",6)==0)
        {
            RMDIR(komendawpisana);
        }
        else if(strncmp(komendawpisana,"rm ",3)==0)
        {
            RM(komendawpisana);
        }
        else if (strncmp(komendawpisana, "touch ",6)==0)
        {
            TOUCH(komendawpisana);
        
        }
        else if (strncmp(komendawpisana, "cd -", 4) == 0)
        {
            chdir(poprzednia);
        }
        else if (strncmp(komendawpisana, "cd ~", 4) == 0)
        {
        
            char* home_dir = getenv("HOME");
            getcwd(poprzednia,100);
            chdir(home_dir);
        }
        else if (strncmp(komendawpisana, "cd ", 3) == 0)
        {
            getcwd(poprzednia,100);
            chdir(komendawpisana + 3);
        }
        else if (strncmp(komendawpisana, "cd", 2) == 0)
        {
            char* home_dir = getenv("HOME");
            getcwd(poprzednia,100);
            chdir(home_dir);
        }

        else
        {
            FORKEXEC(komendawpisana);
        }   
        
    }
    printf("Do zobaczenia :)\n");
    return 0; 
} 
