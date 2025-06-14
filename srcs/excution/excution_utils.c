#include "../includes/minishell.h"

void	wait_for_children(t_all *as, int num_children)
{
	int	i;
	int	status;
	int	wpid;

	i = 0;
	while (i < num_children)
	{
		wpid = waitpid(as->child_pids[i], &status, 0);
		if (wpid == as->child_pids[num_children - 1])
		{
			if (WIFEXITED(status))
				as->exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				as->exit_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	free(as->child_pids);
	as->child_pids = NULL;
}

void	handle_builtin_parent(t_command *cmd_list, t_exec_ctx *ctx)
{
	execute_built_ins(cmd_list, ctx->env, ctx->as);
}

static void	handle_fork_and_exec_utils(t_child_ctx *child_ctx,
		t_command *cmd_list, t_exec_ctx *ctx)
{
	child_ctx->as = ctx->as;
	child_ctx->cmd = cmd_list;
	child_ctx->env = ctx->env;
	child_ctx->prev_fd = *(ctx->prev_fd);
	child_ctx->fd = ctx->fd;
	child_process_logic_ctx(child_ctx);
}

void	handle_fork_and_exec(t_command *cmd_list, t_exec_ctx *ctx)
{
	pid_t		pid;
	t_child_ctx	child_ctx;
	pid_t		*temp_pids;

	pid = prepare_pipe_and_fork(ctx->as, ctx->fd, cmd_list->next != NULL);
	if (pid == 0)
		handle_fork_and_exec_utils(&child_ctx, cmd_list, ctx);
	else
	{
		if (*(ctx->num_forked_children) == *(ctx->max_children_capacity))
		{
			*(ctx->max_children_capacity) *= 2;
			temp_pids = realloc(ctx->as->child_pids,
					*(ctx->max_children_capacity) * sizeof(pid_t));
			if (!temp_pids)
			{
				perror("realloc");
				free(ctx->as->child_pids);
				exit(EXIT_FAILURE);
			}
			ctx->as->child_pids = temp_pids;
		}
		ctx->as->child_pids[(*(ctx->num_forked_children))++] = pid;
		parent_process_cleanup(cmd_list, ctx->prev_fd, ctx->fd);
	}
}
