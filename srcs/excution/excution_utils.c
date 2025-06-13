#include "../includes/minishell.h"

void execute_commands(t_all *as, t_command *cmd_list, t_envp *env)
{
	int fd[2];
	int prev_fd;
	pid_t pid;
	int status;
	int num_forked_children;
	int max_children_capacity;
	t_command *count_list;
	int c;

	prev_fd = -1;
	// For dynamic PID storage
	as->child_pids = malloc(8 * sizeof(pid_t));
	if (!as->child_pids)
		exit_program(as, "malloc failed", 1); // ask
	num_forked_children = 0;
	max_children_capacity = 8;
	// (void)as;
	count_list = cmd_list;
	c = 0;
	while (count_list)
	{
		count_list = count_list->next;
		c++;
	}
}