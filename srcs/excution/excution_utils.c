#include "../includes/minishell.h"

static int	count_commands(t_command *cmd_list)
{
	int	count;

	count = 0;
	while (cmd_list)
	{
		count++;
		cmd_list = cmd_list->next;
	}
	return (count);
}

static void	wait_for_children(t_all *as, int num_children)
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

// ...existing code...

static void	handle_builtin_parent(t_command *cmd_list, t_exec_ctx *ctx)
{
	execute_built_ins(cmd_list, ctx->env, ctx->as);
	ctx->as->exit_status = 0;
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

static void	handle_fork_and_exec(t_command *cmd_list, t_exec_ctx *ctx)
{
	pid_t			pid;
	t_child_ctx		child_ctx;
	pid_t			*temp_pids;

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

static void	handle_command(t_command *cmd_list, t_exec_ctx *ctx)
{
	if (built_in(cmd_list) && ctx->c == 1 && (!cmd_list->infile
			&& !cmd_list->outfile && !cmd_list->heredoc))
	{
		handle_builtin_parent(cmd_list, ctx);
	}
	else
	{
		handle_fork_and_exec(cmd_list, ctx);
	}
}
// ...existing code...

static void	loop_commands(t_command *cmd_list, t_exec_ctx *ctx)
{
	while (cmd_list)
	{
		if (cmd_list->executable == 1)
			handle_command(cmd_list, ctx);
		cmd_list = cmd_list->next;
	}
}

void	execute_commands(t_all *as, t_command *cmd_list, t_envp *env)
{
	int			fd[2];
	int			prev_fd;
	int			num_forked_children;
	int			max_children_capacity;
	t_exec_ctx	ctx;

	prev_fd = -1;
	as->child_pids = malloc(8 * sizeof(pid_t));
	if (!as->child_pids)
		exit_program(as, "malloc failed", 1);
	num_forked_children = 0;
	max_children_capacity = 8;
	ctx.c = count_commands(cmd_list);
	ctx.as = as;
	ctx.env = env;
	ctx.prev_fd = &prev_fd;
	ctx.fd = fd;
	ctx.num_forked_children = &num_forked_children;
	ctx.max_children_capacity = &max_children_capacity;
	loop_commands(cmd_list, &ctx);
	wait_for_children(as, num_forked_children);
}
