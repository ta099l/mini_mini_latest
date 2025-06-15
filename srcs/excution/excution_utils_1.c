#include "../includes/minishell.h"

// Helper for exec logic
static void	exec_command(t_all *as, t_command *cmd, t_envp *env, int flag)
{
	char	*path;

	if (!cmd->args)
		exit_fork(as, NULL, 0);
	if (ft_strchr(cmd->args[0], '/'))
	{
		path = cmd->args[0];
		flag = 1;
	}
	else
		path = find_path(env, cmd->args[0]);
	if (!path)
		exit_fork(as, "command not found", 127);
	restore_signals();
	if (ft_strncmp(cmd->args[0], "make", 4) == 0)
	{
		free(cmd->args[0]);
		cmd->args[0] = ft_strdup(path);
	}
	execve(path, cmd->args, env->tmp_envp);
	perror("execve");
	if (!flag)
		free(path);
	exit_fork(as, NULL, 126);
}

void	child_process_logic_ctx(t_child_ctx *ctx)
{
	int	flag;

	flag = 0;
	redirect_io(ctx->as, ctx->cmd, ctx->prev_fd, ctx->fd);
	if (ctx->prev_fd != -1)
		close(ctx->prev_fd);
	if (ctx->cmd->next != NULL)
	{
		close(ctx->fd[0]);
		close(ctx->fd[1]);
	}
	if (built_in(ctx->cmd))
	{
		execute_built_ins(ctx->cmd, ctx->env, ctx->as);
		exit_fork(ctx->as, NULL, 0);
	}
	else
		exec_command(ctx->as, ctx->cmd, ctx->env, flag);
}

void	parent_process_cleanup(t_command *cmd, int *prev_fd, int fd[2])
{
	if (*prev_fd != -1)
		close(*prev_fd);
	if (cmd->next != NULL)
	{
		close(fd[1]);
		*prev_fd = fd[0];
	}
	else
		*prev_fd = -1;
}

int	prepare_pipe_and_fork(t_all *as, int fd[2], int has_next)
{
	pid_t	pid;

	if (has_next && pipe(fd) == -1)
	{
		exit_fork(as, "pipe", 1);
	}
	pid = fork();
	if (pid == -1)
	{
		close(fd[0]);
		close(fd[1]);
		exit_fork(as, "fork", 1);
	}
	return (pid);
}
