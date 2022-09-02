#include "headers/utilities.h"

// builtin commands
int cd(int argc, char **argv);
int pwd(int argc, char **argv);
int exit_shell(int argc, char **argv);
int echo(int argc, char **argv);
int print_history(int argc, char **argv);
int jobs(int argc, char **argv);
int help(int argc, char **argv);


internel_func_t internel_funcs[] = {
    {"cd", cd},
    {"pwd", pwd},
    {"exit", exit_shell},
    {"echo", echo},
    {"!n", print_history},
    {"jobs", jobs},
    {"help", help},
    {NULL, NULL}
};

int cd(int argc, char **argv)
{
    if (argc > 2)
    {
        fprintf(stderr, "cd: Too many arguments\n");
        return 1;
    }

    char *dirpath = NULL;
    if (argc == 1)
    {
        dirpath = getenv("HOME");
    }
    else if (argc == 2)
    {
        dirpath = argv[1];
    }

    if (chdir(dirpath) == -1)
    {
        fprintf(stderr, "cd: %s: No such file or directory\n", argv[1]);
        return 1;
    }

    char *cwd = getcwd(NULL, 0);
    setenv("PWD", cwd, 1);
    free(cwd);
    return 0;
}

int pwd(int argc, char **argv)
{
    if (argc > 1)
    {
        fprintf(stderr, "pwd: Too many arguments\n");
        return 1;
    }

    char *cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        fprintf(stderr, "pwd: Failed to get current working directory\n");
        return 1;
    }

    printf("%s\n", cwd);
    free(cwd);
    return 0;
}

int echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) printf("%s ", argv[i]);

    puts("");
    return 0;
}

int exit_shell(int argc, char **argv)
{
    write_history(HISTORY_FILE);
	exit(0);
}

int jobs(int argc, char **argv)
{
    	system("ps -a");
	return 0;
}

int help(int argc, char **argv)
{
	puts("Deep Sadness and regret, Irritability, Nostalgia\n" 
		 "Impulsive and indulgent behavior, Changes in ambition!\n"
		 "Changes in sexual desire?\n"
		 "Whatever you need help with, sure enough you are in wrong place!");

	return 0;
}

int print_history(int argc, char** argv)
{	
  	HISTORY_STATE *myhist = history_get_history_state ();

    printf ("\n<<< session history >>>\n");
    for (int i = 1; i <= myhist->length; i++) /* output history list */
    { 
  		HIST_ENTRY *entry = history_get(i);
        printf ("%-5d -->  %s\n", i, entry->line);

    }
    puts("-------------------\n");

	return 0;
}

void unix_error(char *msg) /* Unix-style error */ 
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

pid_t Fork()
{	
	pid_t pid = fork();
	if (pid < 0) unix_error("Fork Error");
	return pid;
}

void printDir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\n%s", cwd);
}

int readcommand(char* str, char* prompt)
{
    char* buf;
  
    buf = readline(prompt);

    if (strlen(buf) != 0) {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    } else {
        return 1;
    }
}

void io_redirect(int argc, char ** argv)
{
    int fd_stdin;
    int fd_stdout;
    int last = argc - 1;
    for (int i = 0; i < argc - 1; i++)
    {
        if (argv[i] != NULL)
        {
            if ((strcmp(argv[i], "<") == 0))
            {
                fd_stdin = open(argv[i + 1], O_RDONLY, O_TRUNC, 0666);
                // fd_in = dup(STDIN_FILENO);
                dup2(fd_stdin, STDIN_FILENO);
                argv[i] = NULL;
                argv[i + 1] = NULL;
            }
            else if ((strcmp(argv[i], ">") == 0))
            {
                fd_stdout = open(argv[i + 1], O_RDWR|O_CREAT, S_IRWXU|S_IROTH);
                // fd_out = dup(STDOUT_FILENO);
                dup2(fd_stdout, STDOUT_FILENO);

                argv[i] = NULL;
                argv[i + 1] = NULL;
            }
            argv[last + 1] = NULL;
        }
    }

    char ** temp = malloc(sizeof(char *) * argc);
    for (int i = 0; i < argc ; i++) 
    	temp[i] = NULL;

    int idx = 0;
    for (int i = 0; i < argc ; i++) if (argv[i])
    	temp[idx++] = argv[i];

    for (int i = 0; i < argc ; i++) 
    	argv[i] = temp[i];

    argv[argc] = NULL;
}

void insert_variable(int argc, char** argv)
{
	for (int i=0; i<argc; i++)
	{	
		if (argv[i][0] == '$')
		{
			char* val = getenv(argv[i]+1);
			free(argv[i]);
			argv[i] = malloc(sizeof(strlen(val) + 1) * sizeof(char));

			strcpy(argv[i], val);
		}
	}

	return;
}

char* conditional_structure(int* argc, char** argv)
{	
	char condition[1000] = {'\0'};
	char then_command[1000] = {'\0'};
	char else_command[1000] = {'\0'};
	char fi_command[1000] = {'\0'};
	char* str;
	for (int i=1; i< *argc; i++)
	{
		strcat(condition, argv[i]);
		strcat(condition, " ");
	}

	for (int i=0; i< *argc; i++)
		free(argv[i]), argv[i] = NULL;
	free(argv);

	readcommand(then_command, "then> ");
	readcommand(else_command, "else> ");
	while(1)
	{	
		readcommand(fi_command, "fi> ");
		if (strstr(fi_command, "fi"))
			break;
	}

	int flag = system(condition); // check condition 0 for true 256 for false

	if (!flag)
	{
		char* check = "then\0";
		str = then_command;
		for (int i=0; i<strlen(check); i++)
			if (str[0] != check[i])
			{
				unix_error("parse error from then");
				return NULL;
			} else str++;
	}
	else
	{
		char* check = "else\0";
		str = else_command;
		for (int i=0; i<strlen(check); i++)
			if (str[0] != check[i])
			{
				unix_error("parse error from else");
				return NULL;
			} else str++;
	}

	return str;
}

int interel_execute(char* command, int argc, char** argv)
{
    internel_func_t *internel_func_ptr = internel_funcs;
    int status;
    while (internel_func_ptr->function_index)
    {
        if (strcmp(internel_func_ptr->function_index, command) == 0)
        {
            status = internel_func_ptr->function(argc, argv);
            return 1;
        }
        internel_func_ptr++;
    }
    return 0;
}

int Execute(char* command, int argc, char** argv)
{	
	return execvp(command, argv);
}

