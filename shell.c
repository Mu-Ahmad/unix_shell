// I wrote this version of unix-shell as an assignment for the course
// of Systems Programming in Fall of 22' by Sir Ariff butt
// visit http://arifbutt.me/ for more

#include "headers/shell.h"

int main()
{
	// 0 is read end, 1 is write end
    int pipefd[2][2], curr_pipe = 0;
    pipe(pipefd[0]);

	while(1)
	{	
		char str[MAXARGLEN];
		int argc = 0, command_count = 0, piped_count = 0;
		char **argv, **commands, **piped_out;
		// print the prompt
		printDir();
		// ignore the empty lines
		if(readcommand(str, "\n$ ")) continue;

		//let's get down to the dirty stuff
		jmp_buf env_buffer;
		setjmp(env_buffer);

		commands = parse_atomic_command((char*)str, &command_count, ";");
		for (int idx=0; idx<command_count; idx++)
		{	
			piped_out = parse_atomic_command(commands[idx], &piped_count, "|");
			for (int j=0; j<piped_count; j++)
			{	
				// var = val
				if (strstr(piped_out[j], " = ")) 
				{
					argv = parse_atomic_command(piped_out[j], &argc, " = ");
					setenv(argv[0], argv[1], 1);
					continue;
				}
				else if (strchr(piped_out[j], '=') && !strstr(piped_out[j], "==") && !strstr(piped_out[j], "!="))
				{
					argv = parse_atomic_command(piped_out[j], &argc, "=");
					setenv(argv[0], argv[1], 1);
					continue;
				}

				argv = parse_atomic_command(piped_out[j], &argc, " ");
				insert_variable(argc, argv);

				if (strcmp(argv[0], "if") == 0)
				{
					char * temp = conditional_structure(&argc, argv);
					if (argv == NULL) continue;
					strcpy(str, temp);
					longjmp(env_buffer, 0);
				}
				int name_len = strlen(argv[argc - 1]);
				int bg = 0;
				if (argv[argc - 1][name_len - 1] == '&')
				{
					bg = 1;
					argv[argc - 1][name_len - 1] = 0;
				}
				
				pipe(pipefd[(curr_pipe+1) % 2]);
				pid_t pid = Fork();
				if (pid == 0)
				{	
					// handling pipe and stuff
					if (piped_count > 1) {
						if (j) //pipe ins (all but first)
						{
							dup2(pipefd[curr_pipe][READHEAD], STDIN_FILENO);
						}

						if (j+1 != piped_count) //pipe outs (all but last)
						{	
							dup2(pipefd[(curr_pipe+1) % 2][WRITEHEAD], STDOUT_FILENO);
							close(pipefd[(curr_pipe + 1) % 2][WRITEHEAD]);
							close(pipefd[(curr_pipe + 1) % 2][READHEAD]);
						}
					} 

					// handling io redirection
					io_redirect(argc, argv);

					if (Execute(argv[0], argc, argv) < 0)
					{	
						unix_error("Command not found : ");
						return 0;
					}

				}

				close(pipefd[curr_pipe][READHEAD]);
				close(pipefd[(curr_pipe + 1) % 2][WRITEHEAD]);
				curr_pipe = (curr_pipe + 1) % 2;
				int status;
				if (!bg && waitpid(pid, &status, 0) < 0) unix_error("waitpid error: ");
				
				if (bg) printf("Started [%d] in bg.", pid);
				
				if (strcmp(argv[0], "cd") == 0) internel_funcs->function(argc, argv);
				else if (strcmp(argv[0], "exit") == 0) exit(0);
				else if (strcmp(argv[0], "jobs") == 0) system("jobs");
				
				for (int i=0; i<argc; i++)
					free(argv[i]), argv[i] = NULL;
				free(argv);
			}
		}

		for (int i=0; i<command_count; i++)
			free(commands[i]), commands[i] = NULL;
		free(commands);
	}
	
	return 0;
}